#!/bin/sh

files=`find /etc/ -type f -maxdepth 1 -name *release* 2>/dev/null | head -1`;
value=`cat $files`;
echo $value;
