#!/bin/sh

# test sblim-cmpi-base package

#******************************************************************************#

export SBLIM_TESTSUITE_RUN=1;

#******************************************************************************#

declare -a CLASSNAMES[];
CLASSNAMES=([0]=Linux_ComputerSystem [1]=Linux_OperatingSystem \
[2]=Linux_UnixProcess [3]=Linux_Processor \
[4]=Linux_RunningOS [5]=Linux_OSProcess [6]=Linux_CSProcessor \
[7]=Linux_OperatingSystemStatisticalData [8]=Linux_OperatingSystemStatistics)

declare -i max=8;
declare -i i=0;

while(($i<=$max))
do
  . run.sh ${CLASSNAMES[$i]} $1 $2 $3 $4 || exit 1;
  i=$i+1;
done
