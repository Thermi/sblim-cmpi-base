/*
 * sysman.h
 *
 * Copyright (c) 2002, International Business Machines
 *
 * THIS FILE IS PROVIDED UNDER THE TERMS OF THE COMMON PUBLIC LICENSE
 * ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE
 * CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
 *
 * You can obtain a current copy of the Common Public License from
 * http://oss.software.ibm.com/developerworks/opensource/license-cpl.html
 *
 * Author:       C. Eric Wu <cwu@us.ibm.com>
 * Contributors: 
 *
 * Description:
 * This kernel module is the actuator for controlling existing processes
 *
*/

#ifndef _SYSMAN_H_
#define _SYSMAN_H_

#ifndef SYSMAN_MAJOR
#define SYSMAN_MAJOR 0 /* dynamic major */
#endif

#ifndef SYSMAN_NR_DEVS
#define SYSMAN_NR_DEVS 1
#endif

#ifndef SYSMAN_PID
#define SYSMAN_PID 1
#endif

#ifdef CONFIG_DEVFS_FS /* only if enabled, to avoid errors in 2.0 */
#include <linux/devfs_fs_kernel.h>
#else
  typedef void * devfs_handle_t;  /* avoid #ifdef inside the structure */
#endif

#ifndef DEVFS_FL_DEFAULT
extern inline void devfs_unregister(devfs_handle_t de) {}
#endif

extern devfs_handle_t sysman_devfs_dir;

typedef struct sysman_dev {
  void **data;
  struct sysman_dev *next;  /* next listitem */
  int quantum;              /* the current quantum size */
  int qset;                 /* the current array size */
  unsigned long size;
  devfs_handle_t handle;    /* only used if devfs is there */
  unsigned int access_key;  /* used by sculluid and scullpriv */
  struct semaphore sem;     /* mutual exclusion semaphore     */
  int sysman_pid;           /* process id for getrlimit/setrlimit */
} sysman_dev;

/*
 * Macros to help debugging
 */
#undef PDEBUG             /* undef it, just in case */
#ifdef SYSMAN_DEBUG
#  ifdef __KERNEL__
     /* This one if debugging is on, and kernel space */
#    define PDEBUG(fmt, args...) printk( KERN_ALERT "sysman: " fmt, ## args)
#  else
     /* This one for user space */
#    define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
#  endif
#else
#  define PDEBUG(fmt, args...) /* not debugging: nothing */
#endif

#undef PDEBUGG
#define PDEBUGG(fmt, args...) /* nothing: it's a placeholder */

#ifndef LINUX_VERSION_CODE
#  include <linux/version.h>
#endif

#ifndef KERNEL_VERSION /* pre-2.1.90 didn't have it */
#  define KERNEL_VERSION(vers,rel,seq) ( ((vers)<<16) | ((rel)<<8) | (seq) )
#endif

/* only allow 2.2.y and 2.4.z */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,2,0) /* not < 2.2 */
#  error "This kernel is too old: not supported by this file"
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0) /* not > 2.4, by now */
#  error "This kernel is too recent: not supported by this file"
#endif
#if (LINUX_VERSION_CODE & 0xff00) == 3 /* not 2.3 */
#  error "Please don't use linux-2.3, use 2.4 instead"
#endif

/* remember about the current version */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,3,0)
#  define LINUX_22
#else
#  define LINUX_24
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,10)
#  define LINUX_2_4_10_ORLATER
#endif
/* we can't support versioning in pre-2.4 because we #define some functions */
#if !defined(LINUX_24) && defined(CONFIG_MODVERSIONS)
#  error "This sysdep.h can't support CONFIG_MODVERSIONS"
#  error "and old kernels at the same time."
#  error "Either use 2.4 or avoid using versioning"
#endif

#ifdef VM_READ /* a typical flag defined by mm.h */
#  include <linux/vmalloc.h>
#endif

#include <linux/sched.h>

/* Modularization issues */
#define REGISTER_SYMTAB(tab) /* nothing */

#ifdef __USE_OLD_SYMTAB__
#  define __MODULE_STRING(s)         /* nothing */
#  define MODULE_PARM(v,t)           /* nothing */
#  define MODULE_PARM_DESC(v,t)      /* nothing */
#  define MODULE_AUTHOR(n)           /* nothing */
#  define MODULE_DESCRIPTION(d)      /* nothing */
#  define MODULE_SUPPORTED_DEVICE(n) /* nothing */
#endif

/*
 * In version 2.2 (up to 2.2.19, at least), the macro for request_module()
 * when no kmod is there is wrong. It's a "do {} while 0" but it shouldbe int
 */
#ifdef LINUX_22
#  ifndef CONFIG_KMOD
#    undef request_module
#    define request_module(name) -ENOSYS
#  endif
#endif

#ifndef module_init
#  define module_init(x)        int init_module(void) { return x(); }
#  define module_exit(x)        void cleanup_module(void) { x(); }
#endif

#ifndef SET_MODULE_OWNER
#  define SET_MODULE_OWNER(structure) /* nothing */
#endif

/*
 * "select" changed in 2.1.23. The implementation is twin, but this
 * header is new
 *
 */
#include <linux/poll.h>
#define INODE_FROM_F(filp) ((filp)->f_dentry->d_inode)

/* Other changes in the fops are solved using wrappers */

/*
 * Wait queues changed with 2.3
 */
#ifndef DECLARE_WAIT_QUEUE_HEAD
#  define DECLARE_WAIT_QUEUE_HEAD(head) struct wait_queue *head = NULL
   typedef  struct wait_queue *wait_queue_head_t;
#  define init_waitqueue_head(head) (*(head)) = NULL

/* offer wake_up_sync as an alias for wake_up */
#  define wake_up_sync(head) wake_up(head)
#  define wake_up_interruptible_sync(head) wake_up_interruptible(head)

/* Pretend we have add_wait_queue_exclusive */
#  define add_wait_queue_exclusive(q,entry) add_wait_queue ((q), (entry))

#endif /* no DECLARE_WAIT_QUEUE_HEAD */

/*
 * 2.3 added tasklets
 */
#ifdef LINUX_24
#  define HAVE_TASKLETS
#endif

/* FIXME: implement the other versions of wake_up etc */

/*
 * access to user space: use the 2.2 functions,
 */
#include <asm/uaccess.h>
#include <asm/io.h>
#define verify_area_20(t,a,sz) (0) /* == success */
#define   PUT_USER   put_user
#define __PUT_USER __put_user
#define   GET_USER   get_user
#define __GET_USER __get_user

/*
 * Allocation issues
 */
#ifdef GFP_USER /* only if mm.h has been included */
#  ifndef LINUX_24
#    define __GFP_HIGHMEM  0  /* was not there */
#    define GFP_HIGHUSER   0   /* idem */
#  endif

#  ifndef LINUX_24
#    define get_zeroed_page get_free_page
#  endif
#endif

/* ioremap */
#if defined(LINUX_20) && defined(_LINUX_MM_H)
#  define ioremap_nocache ioremap
#  ifndef __i386__
   /* This simple approach works for non-PC platforms. */
#    define ioremap vremap
#    define iounmap vfree
#  else /* the PC has <expletive> ISA; 2.2 and 2.4 remap it, 2.0 needs not */
extern inline void *ioremap(unsigned long phys_addr, unsigned long size)
{
    if (phys_addr >= 0xA0000 && phys_addr + size <= 0x100000)
        return (void *)phys_addr;
    return vremap(phys_addr, size);
}

extern inline void iounmap(void *addr)
{
    if ((unsigned long)addr >= 0xA0000
            && (unsigned long)addr < 0x100000)
        return;
    vfree(addr);
}
#  endif
#endif

/* Also, define check_mem_region etc */
#ifndef LINUX_24
#  define check_mem_region(a,b)     0 /* success */
#  define request_mem_region(a,b,c) /* nothing */
#  define release_mem_region(a,b)   /* nothing */
#endif

#define INCRCOUNT(p)  ((p)->use_count++)
#define DECRCOUNT(p)  ((p)->use_count--)
#define CURRCOUNT(p)  ((p)->use_count)

/*
 * 2.2 didn't have create_proc_{read|info}_entry yet.
 * And it looks like there are no other "interesting" entry point, as
 * the rest is somehow esotique (mknod, symlink, ...)
 */
#ifdef LINUX_22
#  ifdef PROC_SUPER_MAGIC  /* Only if procfs is being used */
extern inline struct proc_dir_entry *create_proc_read_entry(const char *name,
                          mode_t mode, struct proc_dir_entry *base, 
                          read_proc_t *read_proc, void * data)
{
    struct proc_dir_entry *res=create_proc_entry(name,mode,base);
    if (res) {
        res->read_proc=read_proc;
        res->data=data;
    }
    return res;
}

#    ifndef create_proc_info_entry /* added in 2.2.18 */
typedef int (get_info_t)(char *, char **, off_t, int, int);
extern inline struct proc_dir_entry *create_proc_info_entry(const char *name,
        mode_t mode, struct proc_dir_entry *base, get_info_t *get_info)
{
        struct proc_dir_entry *res=create_proc_entry(name,mode,base);
        if (res) res->get_info=get_info;
        return res;
}
#    endif  /* no create_proc_info_entry */
#  endif
#endif

/* 2.2 lacks the set_ functions */
#ifndef LINUX_24
#define set_mb() do { var = value; mb(); } while (0)
#define set_wmb() do { var = value; wmb(); } while (0)
#endif /* ! LINUX_24 */

/*
 * fasync changed in 2.2.
 */
#ifdef LINUX_20
/*  typedef struct inode *fasync_file; */
#  define fasync_file struct inode *
#else
  typedef int fasync_file;
#endif

/* kill_fasync had less arguments, and a different indirection in the first */
#ifndef LINUX_24
#  define kill_fasync(ptrptr,sig,band)  kill_fasync(*(ptrptr),(sig))
#endif

#ifdef LINUX_PCI_H /* only if PCI stuff is being used */
#  ifdef LINUX_20
#    include "pci-compat.h" /* a whole set of replacement functions */
#  else
#    define  pci_release_device(d) /* placeholder, used in 2.0 to free stuff */
#  endif
#endif



/*
 * Some task state stuff
 */

#ifndef set_current_state
#  define set_current_state(s) current->state = (s);
#endif

/*
 * Schedule_task was a late 2.4 addition.
 */
#ifndef LINUX_24
extern inline int schedule_task(struct tq_struct *task)
{
        queue_task(task, &tq_scheduler);
        return 1;
}
#endif

#ifdef _LINUX_DELAY_H /* only if linux/delay.h is included */
#  ifndef mdelay /* linux-2.0 */
#    ifndef MAX_UDELAY_MS
#      define MAX_UDELAY_MS   5
#    endif
#    define mdelay(n) (\
        (__builtin_constant_p(n) && (n)<=MAX_UDELAY_MS) ? udelay((n)*1000) : \
        ({unsigned long msec=(n); while (msec--) udelay(1000);}))
#  endif /* mdelay */
#endif /* _LINUX_DELAY_H */


/*
 * No del_timer_sync before 2.4
 */
#ifndef LINUX_24
#  define del_timer_sync(timer) del_timer(timer)  /* and hope */
#endif

/*
 * Various changes in mmap and friends.
 */

#ifndef NOPAGE_SIGBUS
#  define NOPAGE_SIGBUS  NULL  /* return value of the nopage memory method */
#  define NOPAGE_OOM     NULL  /* No real equivalent in older kernels */
#endif

#ifndef VM_RESERVED            /* Added 2.4.0-test10 */
#  define VM_RESERVED 0
#endif

#ifdef LINUX_24 /* use "vm_pgoff" to get an offset */
#define VMA_OFFSET(vma)  ((vma)->vm_pgoff << PAGE_SHIFT)
#else /* use "vm_offset" */
#define VMA_OFFSET(vma)  ((vma)->vm_offset)
#endif

#ifdef MAP_NR
#define virt_to_page(page) (mem_map + MAP_NR(page))
#endif

#ifndef get_page
#  define get_page(p) atomic_inc(&(p)->count)
#endif


/*
 * I/O memory was not managed by ealier kernels, define them as success
 */

#if 0 /* FIXME: what is the right way to do request_mem_region? */
#ifndef LINUX_24
#  define check_mem_region(start, len)          0
#  define request_mem_region(start, len, name)  0
#  define release_mem_region(start, len)        0

   /*
    * Also, request_ and release_ region used to return void. Return 0 instead
    */
#  define request_region(s, l, n)  ({request_region((s),(l),(n));0;})
#  define release_region(s, l)     ({release_region((s),(l));0;})

#endif /* not LINUX_24 */
#endif

/*
 * Block layer stuff.
 */
#ifndef LINUX_24

/* BLK_DEFAULT_QUEUE for use with these macros only!!!! */
#define BLK_DEFAULT_QUEUE(major) blk_dev[(major)].request_fn
#define blk_init_queue(where,request_fn) where = request_fn;
#define blk_cleanup_queue(where) where = NULL;

/* No QUEUE_EMPTY in older kernels */
#ifndef QUEUE_EMPTY  /* Driver can redefine it too */
#  define QUEUE_EMPTY (CURRENT != NULL)
#endif

#ifdef RO_IOCTLS
static inline int blk_ioctl(kdev_t dev, unsigned int cmd, unsigned long arg)
{
    int err;

    switch (cmd) {
      case BLKRAGET: /* return the readahead value */
        if (!arg)  return -EINVAL;
        err = ! access_ok(VERIFY_WRITE, arg, sizeof(long));
        if (err) return -EFAULT;
        PUT_USER(read_ahead[MAJOR(dev)],(long *) arg);
        return 0;

      case BLKRASET: /* set the readahead value */
        if (!capable(CAP_SYS_ADMIN)) return -EACCES;
        if (arg > 0xff) return -EINVAL; /* limit it */
        read_ahead[MAJOR(dev)] = arg;
        return 0;

      case BLKFLSBUF: /* flush */
        if (! capable(CAP_SYS_ADMIN)) return -EACCES; /* only root */
        fsync_dev(dev);
        invalidate_buffers(dev);
        return 0;

        RO_IOCTLS(dev, arg);
    }
    return -ENOTTY;
}
#endif  /* RO_IOCTLS */

#ifdef LINUX_EXTENDED_PARTITION /* defined in genhd.h */
static inline void register_disk(struct gendisk *gdev, kdev_t dev,
                unsigned minors, struct file_operations *ops, long size)
{
    if (! gdev)
        return;
    resetup_one_dev(gdev, MINOR(dev) >> gdev->minor_shift);
}
#endif /* LINUX_EXTENDED_PARTITION */


#else  /* it is Linux 2.4 */
#define HAVE_BLKPG_H
#endif /* LINUX_24 */



#ifdef LINUX_20 /* physical and virtual addresses had the same value */
#  define __pa(a) (a)
#  define __va(a) (a)
#endif

/*
 * Network driver compatibility
 */

/*
 * 2.0 dev_kfree_skb had an extra arg.  The following is a little dangerous
 * in that it assumes that FREE_WRITE is always wanted.  Very few 2.0 drivers
 * use FREE_READ, but the number is *not* zero...
 *
 * Also: implement the non-checking versions of a couple skb functions -
 * but they still check in 2.0.
 */
#ifdef LINUX_20
#  define dev_kfree_skb(skb) dev_kfree_skb((skb), FREE_WRITE);

#  define __skb_push(skb, len) skb_push((skb), (len))
#  define __skb_put(skb, len)  skb_put((skb), (len))
#endif

/*
 * Softnet changes in 2.4
 */
#ifndef LINUX_24
#  ifdef _LINUX_NETDEVICE_H /* only if netdevice.h was included */
#  define netif_start_queue(dev) clear_bit(0, (void *) &(dev)->tbusy);
#  define netif_stop_queue(dev)  set_bit(0, (void *) &(dev)->tbusy);

static inline void netif_wake_queue(struct device *dev)
{
    clear_bit(0, (void *) &(dev)->tbusy);
    mark_bh(NET_BH);
}

/* struct device became struct net_device */
#  define net_device device
#  endif /* netdevice.h */
#endif /* ! LINUX_24 */

/*
 * Memory barrier stuff, define what's missing from older kernel versions
 */
#ifdef switch_to /* this is always a macro, defined in <asm/sysstem.h> */

#  ifndef set_mb
#    define set_mb(var, value) do {(var) = (value); mb();}  while 0
#  endif
#  ifndef set_rmb
#    define set_rmb(var, value) do {(var) = (value); rmb();}  while 0
#  endif
#  ifndef set_wmb
#    define set_wmb(var, value) do {(var) = (value); wmb();}  while 0
#  endif

/* The hw barriers are defined as sw barriers. A correct thing if this
   specific kernel/platform is supported but has no specific instruction */
#  ifndef mb
#    define mb barrier
#  endif
#  ifndef rmb
#    define rmb barrier
#  endif
#  ifndef wmb
#    define wmb barrier
#  endif

#endif /* switch to (i.e. <asm/system.h>) */


#endif /* _SYSMAN_H_ */
