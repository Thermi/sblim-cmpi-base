#!/bin/sh

className="Linux_UnixProcess.instance";

case $1 in
    -rm) rm $className;
	 exit 0;
esac;

ps --no-headers -eo pid,ppid,tty,pri > $className;


# nice,uid,gid,pmem,pcpu,cputime,comm,session,state,args
