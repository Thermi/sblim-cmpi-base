#!/bin/sh

HW=`uname -i`;

if echo $HW | grep "i386" >/dev/null 
    then echo "INTEL"
elif echo $HW | grep "s390" >/dev/null 
    then echo "S390"
elif echo $HW | grep "ppc" >/dev/null 
    then echo "PPC"
elif echo $HW | grep "x86_64" >/dev/null 
    then echo "X86_64"
elif echo $HW | grep "ia64" >/dev/null 
    then echo "IA64"
else
    echo "GENERIC"
fi
