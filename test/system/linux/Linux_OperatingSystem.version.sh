#!/bin/sh

files=`find /etc/ -name *release* 2>/dev/null`;
value=`cat $files`;
echo $value;