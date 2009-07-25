/*
 * sysman_pid.c
 *
 * (C) Copyright IBM Corp. 2009
 *
 * THIS FILE IS PROVIDED UNDER THE TERMS OF THE ECLIPSE PUBLIC LICENSE
 * ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE
 * CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
 *
 * You can obtain a current copy of the Eclipse Public License from
 * http://www.opensource.org/licenses/eclipse-1.0.php
 *
 * Author:       C. Eric Wu <cwu@us.ibm.com>
 * Contributors: 
 *
 * Description:
 * This kernel module is the actuator for controlling existing processes
 *
 * version 0.52: added the MODULE_LICENSE() for kernel 2.4.10 and later
 * version 0.51: fixed the hang bug when an invalid pid was given
 * version 0.5:  adopted by sblim-base
 */
#ifndef __KERNEL__
#define __KERNEL__
#endif
#ifndef MODULE
#define MODULE
#endif

#include <linux/config.h>
#ifdef CONFIG_SMP
#define __SMP__
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/resource.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/sysctl.h>
#include <asm/uaccess.h>
#include <sys/errno.h>
#include <stdio.h>
#include <dlfcn.h>
#include <linux/dirent.h>

#include "sysman.h"

int sysman_pid = SYSMAN_PID;
MODULE_PARM(sysman_pid, "i");
MODULE_PARM_DESC(sysman_pid, "process pid under system management control");

#define MODULE_VERSION	"0.51"
#define MODULE_NAME	"sysman"
#define STRINGLENGTH	256
#define BUFFERSIZE	4096
#define NOFIELDS	7

struct pidrlimit_t {
	struct semaphore sem;			/* semaphore */
	int sysman_pid;				/* pid */
	char strbuf[STRINGLENGTH + 1];		/* string buffer */
	struct rlimit rlim[RLIM_NLIMITS];	/* rlimit */
	int  intValue;
};

struct sysdata_t {
	char filename[STRINGLENGTH + 1];	/* config file name */
	int count;				/* number of valid accounts */
	char *strbufp;				/* string buffer */
};

static struct proc_dir_entry *proc_sysman_root, *pidrlimit_file;

struct pidrlimit_t pidrlimit_data;
struct sysdata_t system_data;

static int strtoint(char *s, int *start) { 
	int i, n, sign, j=0;

	i = *start;
	if (i < 0 || s == 0 || i >= strlen(s)) return -1;
	for (; s[i] == ' ' || s[i] == '\t' || s[i] == ','; i++);
	sign = (s[i] == '-') ? -1 : 1;
	if (s[i] == '+' || s[i] == '-') i++;
	for (n = 0; s[i] <= '9' && s[i] >= '0'; i++) {
		n = 10 * n + (s[i] - '0');
		j++;
       	}
	if (j == 0) {
		*start = i+1;
		return -1;
	} else
		*start = i;
	return sign*n;
}

static int proc_read_pidrlimit(char *page, char **start, off_t off, int count, 
	int *eof, void *data)
{
	int len = 0;
	struct task_struct *p;
	struct pidrlimit_t *p_data = (struct pidrlimit_t *)data;

	MOD_INC_USE_COUNT;
//	if (down_interruptible(&(p_data->sem))) return -ERESTARTSYS;
	len += sprintf(page+len, "%d", p_data->sysman_pid);

	read_lock(&tasklist_lock);
	for_each_task(p) {
		if (p->pid == p_data->sysman_pid) {
		        len += sprintf(page+len, "\t%ld\t%ld\t%ld\n", 
				p->rlim[RLIMIT_NPROC].rlim_cur, 
				p->rlim[RLIMIT_NOFILE].rlim_cur,
				p->rlim[RLIMIT_STACK].rlim_cur);
			break;
		}
	}
	read_unlock(&tasklist_lock);
//	up(&(p_data->sem));
	MOD_DEC_USE_COUNT;

	return len;
}

static int proc_write_pidrlimit(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	struct rlimit nproc_rlim, nofile_rlim, stack_rlim, *rlimp=0, *rlimq=0, *rlims=0;
	struct task_struct *p;
	int len, find = 0, i = 0;
	struct pidrlimit_t *p_data = (struct pidrlimit_t *)data;

	MOD_INC_USE_COUNT;

	if(count > STRINGLENGTH) len = STRINGLENGTH;
	else len = count;

//	if (down_interruptible(&(p_data->sem))) return -ERESTARTSYS;
	if(copy_from_user(p_data->strbuf, buffer, len)) {
		MOD_DEC_USE_COUNT;
		return -EFAULT;
	}
	p_data->strbuf[len] = '\0';
	p_data->intValue = strtoint(p_data->strbuf, &i);
        if (p_data->intValue <= 0) {
		len = 0;
		goto fail;
	}
	read_lock(&tasklist_lock);
	for_each_task(p) {
		if (p->pid == p_data->intValue) {
			rlimp = p->rlim + RLIMIT_NPROC;
			rlimq = p->rlim + RLIMIT_NOFILE;
			rlims = p->rlim + RLIMIT_STACK;
			nproc_rlim.rlim_cur = p->rlim[RLIMIT_NPROC].rlim_cur;
			nproc_rlim.rlim_max = p->rlim[RLIMIT_NPROC].rlim_max;
			nofile_rlim.rlim_cur = p->rlim[RLIMIT_NOFILE].rlim_cur;
			nofile_rlim.rlim_max = p->rlim[RLIMIT_NOFILE].rlim_max;
			stack_rlim.rlim_cur = p->rlim[RLIMIT_STACK].rlim_cur;
			stack_rlim.rlim_max = p->rlim[RLIMIT_STACK].rlim_max;
			find = 1;
			break;
		}
	}
	read_unlock(&tasklist_lock);
	if (!find) goto fail;
	p_data->sysman_pid = p_data->intValue;

	p_data->intValue = strtoint(p_data->strbuf, &i);
        if (p_data->intValue >= 0) {
		if ((p_data->intValue > nproc_rlim.rlim_max) && 
			!capable(CAP_SYS_RESOURCE))
			p_data->intValue = 0;
		else 
			nproc_rlim.rlim_cur = nproc_rlim.rlim_max = p_data->intValue;
	}

	p_data->intValue = strtoint(p_data->strbuf, &i);
	if (p_data->intValue >= 0) {
		if ((p_data->intValue > nofile_rlim.rlim_max
		&& !capable(CAP_SYS_RESOURCE)) || p_data->intValue > NR_OPEN)
			p_data->intValue = 0;
		else
			nofile_rlim.rlim_cur = nofile_rlim.rlim_max = p_data->intValue;
	}

	p_data->intValue = strtoint(p_data->strbuf, &i);
	if (p_data->intValue >= 0) {
		if ((p_data->intValue > stack_rlim.rlim_max) &&
			!capable(CAP_SYS_RESOURCE))
			p_data->intValue = 0;
		else
			stack_rlim.rlim_cur = stack_rlim.rlim_max = p_data->intValue;
	}

	write_lock_irq(&tasklist_lock);
	*rlimp = nproc_rlim;
	*rlimq = nofile_rlim;
	*rlims = stack_rlim;
	write_unlock_irq(&tasklist_lock);

fail:
//  	up(&(p_data->sem));

	MOD_DEC_USE_COUNT;
	return len;
}

static int __init sysman_init(void)
{
	int rv = 0;

	/* create directory */
	proc_sysman_root = proc_mkdir("sysman", NULL);
	if(proc_sysman_root == NULL) {
		rv = -ENOMEM;
		goto outrv;
	}
	
	proc_sysman_root->owner = THIS_MODULE;
	
	/* create pidrlimit file using same callback functions */
	pidrlimit_file = create_proc_entry("pid_rlimit", 0644, proc_sysman_root);
	if(pidrlimit_file == NULL) {
		rv = -ENOMEM;
		goto no_pidrlimit;
	}

	pidrlimit_data.intValue = 0;
	pidrlimit_data.sysman_pid = sysman_pid;

	pidrlimit_file->data = &pidrlimit_data;
	pidrlimit_file->read_proc = proc_read_pidrlimit;
	pidrlimit_file->write_proc = proc_write_pidrlimit;
	pidrlimit_file->owner = THIS_MODULE;
		
	PDEBUG("%s %s initialised\n", MODULE_NAME, MODULE_VERSION);
	return 0;

no_pidrlimit:
	remove_proc_entry(MODULE_NAME, NULL);
outrv:
	return rv;
}

static void __exit sysman_cleanup(void)
{

	remove_proc_entry("pid_rlimit", proc_sysman_root);
	remove_proc_entry(MODULE_NAME, NULL);

	PDEBUG("%s %s removed\n", MODULE_NAME, MODULE_VERSION);
}


module_init(sysman_init);
module_exit(sysman_cleanup);

MODULE_AUTHOR("C. Eric Wu");
MODULE_DESCRIPTION("sysman module to access per-process RLIMIT parameters");
#ifdef LINUX_2_4_10_ORLATER
MODULE_LICENSE("CPL");
#endif

EXPORT_NO_SYMBOLS;


