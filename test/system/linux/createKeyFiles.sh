#!/bin/sh


#***********************************************************************#
#                          ComputerSystem                               #
#-----------------------------------------------------------------------#

# CreationClassName

echo 'Linux_ComputerSystem' > ComputerSystem.keys

# Name

echo `hostname` | grep `dnsdomainname` >/dev/null\
&& echo `hostname` >> ComputerSystem.keys \
|| echo `hostname`'.'`dnsdomainname` >> ComputerSystem.keys


#-----------------------------------------------------------------------#



#***********************************************************************#
#                         OperatingSystem                               #
#-----------------------------------------------------------------------#

# CreationClassName

echo 'Linux_OperatingSystem' > OperatingSystem.keys

# Name


echo `hostname` | grep `dnsdomainname` >/dev/null\
&& echo `hostname` >> OperatingSystem.keys \
|| echo `hostname`'.'`dnsdomainname` >> OperatingSystem.keys


#-----------------------------------------------------------------------#
