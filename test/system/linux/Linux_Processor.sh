#!/bin/sh

className="Linux_Processor.instance";

case $1 in
    -rm) rm $className;
	 exit 0;
esac;

cat /proc/cpuinfo | grep ^processor | sed -e s/processor// | sed -e s/:// > $className;

