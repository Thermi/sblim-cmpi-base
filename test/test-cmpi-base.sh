#!/bin/sh

# test sblim-cmpi-base package

declare -a CLASSNAMES[];
CLASSNAMES=([0]=Linux_ComputerSystem [1]=Linux_OperatingSystem \
[2]=Linux_UnixProcess [3]=Linux_Processor \
[4]=Linux_RunningOS [5]=Linux_OSProcess [6]=Linux_CSProcessor)

declare -i max=6;
declare -i i=0;

while(($i<=$max))
do
  ./run.sh ${CLASSNAMES[$i]} || exit 1;
  i=$i+1;
done