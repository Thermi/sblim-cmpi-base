#!/bin/sh

# functiontest.sh
#
# (C) Copyright IBM Corp. 2002
#
# THIS FILE IS PROVIDED UNDER THE TERMS OF THE COMMON PUBLIC LICENSE 
#  ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE 
# CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
#
# You can obtain a current copy of the Common Public License from
# http://oss.software.ibm.com/developerworks/opensource/license-cpl.html
#
# Author:       Heidi Neumann <heidineu@de.ibm.com>
# Contributors:
# 
# Description: Script to test the OS Base Management Instrumentation
#
# Prerequisites: 
# To run this script successful, check the following requirements:
#
# 1) CIMOM on your specified host is running
# 2) the classes are loaded into the specified namespace
# 3) wbemcli is installed on your local system, where you run this 
#    script
#
# **********************************************************************


#**********************************************************************#
#   ---------------------------------------------------------------    #
#      define the target host, where the Providers are installed       #
#   ---------------------------------------------------------------    #
#   define the access path to the CIMOM                                #
#   Syntax: http://<host>:<port><namespace>:                           #
#**********************************************************************#

_TEST_NAMESPACE=/root/cimv2:
_TEST_HOSTPREFIX=localhost:5988${_TEST_NAMESPACE}
_TEST_HOST=http://${_TEST_HOSTPREFIX}


#**********************************************************************#
#   ---------------------------------------------------------------    #
#                define the interface types to test                    #
#   ---------------------------------------------------------------    #
#   set the value of the following variables to                        #
#   0 ... ignore execution                                             #
#   1 ... perform execution                                            # 
#**********************************************************************#

# ----------------------- Class specific tests ----------------------- #

# enumInstances()
declare -i _TEST_CLASS_ENUM_INST=0;
# enumInstanceNames()
declare -i _TEST_CLASS_ENUM_INSTNAMES=0;
# getInstance()
declare -i _TEST_CLASS_GET_INST=0;


# -------------------- Association specific tests -------------------- #

# enumInstances()
declare -i _TEST_ASSOC_ENUM_INST=0;
# enumInstanceNames()
declare -i _TEST_ASSOC_ENUM_INSTNAMES=0;
# getInstance()
declare -i _TEST_ASSOC_GET_INST=0;

# associatorNames()
declare -i _TEST_ASSOC_ASSOCIATOR_INSTNAMES=0;
# associators()
declare -i _TEST_ASSOC_ASSOCIATOR_INST=1;
# referenceNames()
declare -i _TEST_ASSOC_REFERENCE_INSTNAMES=0;
# references()
declare -i _TEST_ASSOC_REFERENCE_INST=0;


# **********************************************************************


#**********************************************************************#
#   ---------------------------------------------------------------    #
#           define the class / association names to test               #
#   ---------------------------------------------------------------    #
#**********************************************************************#


# _TEST_COUNTClass contains the number of class names in the field 
# _TEST_CLASSNAMES[]
declare -i _TEST_COUNTClass=7

# Definition of the names of the classes, which have to be tested by
# this script
declare -a _TEST_CLASSNAMES[${_TEST_COUNTClass}]
_TEST_CLASSNAMES=([0]=cim_computersystem [1]=cim_operatingsystem \
[2]=cim_process [3]=cim_processor [4]=cim_networkport \
[5]=cim_ipprotocolendpoint [6]=cim_filesystem)


# _TEST_COUNTAssoc contains the number of class names in the field 
# _TEST_ASSOCNAMES[]
declare -i _TEST_COUNTAssoc=6

# Definition of the names of the associations, which have to be tested 
# by this script
declare -a _TEST_ASSOCNAMES[${_TEST_COUNTAssoc}]
_TEST_ASSOCNAMES=([0]=cim_runningos [1]=cim_osprocess \
[2]=cim_hostedfilesystem [3]=cim_bootosfromfs [4]=cim_systemdevice \
[5]=cim_portimplementsendpoint)


# **********************************************************************



# **********************************************************************
# Test Scenario : Function Test
#
# 1) execute enumInstances() for each class / association name
# 2) 2.1) execute enumInstanceNames() for each class / association name
#    2.2) execute getInstance() for each instance of class / 
#         association name
# 3) execute Association Provider Calls for each class instance (all
#    associator providers are called)
#    3.1) execute enumInstanceNames() for each class name
#    3.2) execute associatorNames() for each instance
#    3.3) execute associators() for each instance
#    3.4) execute referenceNames() for each instance
#    3.5) execute references() for each instance
#
# **********************************************************************

# exit, if CIMOM is not running
wbemein $_TEST_HOST${_TEST_CLASSNAMES[0]} 2>&1 | grep Exception >/dev/null \
&& echo 'CIMOM NOT RUNNING ?' && exit 1;

# **********************************************************************
# !!!!!!!!!         DO NOT CHANGE CODE BELOW THIS LINE         !!!!!!!!!
# **********************************************************************


declare -i _TEST_COUNT=0;
declare -i _TEST_GET_INST=0;
declare -a _TEST_NAMES[];


# **********************************************************************
# Step 1)
# execute enumInstances() for each class / association name
#
# **********************************************************************

# iterate through field of _TEST_NAMES[], and execute enumInstances()
# for each entry

function _enumInstances() {
  declare -i c=0;
  echo '';
  while((c<_TEST_COUNT))
  do
    echo 'execute enumInstances() on '$_TEST_HOST' to '${_TEST_NAMES[c]}'';
    _TEST_RC=`wbemei $_TEST_HOST${_TEST_NAMES[c]}`|grep "*";
    # ERROR OUTPUT
    echo ${_TEST_RC};
    c=${c}+1;
  done
  unset _TEST_NAMES;
  _TEST_COUNT=0;
}


# iterate through field of _TEST_CLASSNAMES[], and execute 
# enumInstances() for each entry

if ((${_TEST_CLASS_ENUM_INST}==1))
then 
  _TEST_COUNT=${_TEST_COUNTClass};
  _TEST_NAMES=(${_TEST_CLASSNAMES[@]});
  _enumInstances;
fi;

# iterate through field of _TEST_ASSOCNAMES[], and execute 
# enumInstances() for each entry

if ((${_TEST_ASSOC_ENUM_INST}==1))
then 
  _TEST_COUNT=${_TEST_COUNTAssoc};
  _TEST_NAMES=(${_TEST_ASSOCNAMES[@]});
  _enumInstances;
fi;



# **********************************************************************


# **********************************************************************
# Step 2)
#      2.1) execute enumInstanceNames() for each class / association
#           name
#      2.2) execute getInstance() for each instance of class / 
#           association name
#
# **********************************************************************


function _enumInstanceNames_getInstance() {
  declare -i c=0;
  echo '';
  while((c<_TEST_COUNT))
  do
    echo '';

    # 2.1) enumInstanceNames()
    echo 'execute enumInstanceNames() on '$_TEST_HOST' to '${_TEST_NAMES[c]}'';
     # execute enumInstanceNames() and save output in $InstNames
    InstNames=`wbemein $_TEST_HOST${_TEST_NAMES[c]}`;
    # ERROR OUTPUT
    echo $InstNames|grep "*";


    # 2.2) getInstance()

    if ((${_TEST_GET_INST}==1))
    then 
      # initialize _InstCount -> contains the number of returned instances
      typeset -i _InstCount=`echo [-n "${InstNames}"]|wc -l`;
      echo 'Number of returned Instances: '${_InstCount};
      
      # each entry of the field _TEST_InstID[] contains the ObjectPath
      # of an Instance
      declare -a _TEST_InstID[${_InstCount}];
      # fill file _TEST_OUT with value of ${InstNames}
      echo ${InstNames}>_TEST_OUT;
      # fill field _TEST_InstID[] with the values of _TEST_OUT
      read <_TEST_OUT -a _TEST_InstID;
      # delete file _TEST_OUT
      rm _TEST_OUT;
      
      # iterate through list of ObjectPathes, and execute getInstances()
      # for each entry
      typeset -i d=0
      while((d<_InstCount))
      do
	InstOP=${_TEST_InstID[d]};
	echo '... getInstance() of '${InstOP#$_TEST_HOSTPREFIX};
	_TEST_RC=`wbemgi $_TEST_HOST${InstOP#$_TEST_HOSTPREFIX}`|grep "*";
        # ERROR OUTPUT
	echo ${_TEST_RC};
	d=${d}+1;
      done
      # delete entries in field _TEST_InstID[]
      unset _TEST_InstID;
    fi
    c=${c}+1;
  done
  unset _TEST_NAMES;
  _TEST_COUNT=0;
  _TEST_GET_INST=0;
}



# iterate through field of _TEST_CLASSNAMES[], and execute 
# enumInstanceNames() for each entry

if ((((${_TEST_CLASS_ENUM_INSTNAMES}==1))||((${_TEST_CLASS_GET_INST}==1))))
then 
  _TEST_COUNT=${_TEST_COUNTClass};
  _TEST_NAMES=(${_TEST_CLASSNAMES[@]});
  _TEST_GET_INST=${_TEST_CLASS_GET_INST};
  _enumInstanceNames_getInstance;
fi


# iterate through field of _TEST_ASSOCNAMES[], and execute 
# enumInstanceNames() for each entry

if ((((${_TEST_ASSOC_ENUM_INSTNAMES}==1))||((${_TEST_ASSOC_GET_INST}==1))))
then 
  _TEST_COUNT=${_TEST_COUNTAssoc};
  _TEST_NAMES=(${_TEST_ASSOCNAMES[@]});
  _TEST_GET_INST=${_TEST_ASSOC_GET_INST};
  _enumInstanceNames_getInstance;
fi



# **********************************************************************


# **********************************************************************
# Step 3) execute Association Provider Calls for each class instance 
#         (all associator providers are called)
#      3.1) execute enumInstanceNames() for each class name
#      3.2) execute associatorNames() for each instance 
#      3.3) execute associators() for each instance 
#      3.4) execute referenceNames() for each instance 
#      3.5) execute references() for each instance 
#
# **********************************************************************


# iterate through field of _TEST_NAMES[], and execute enumInstanceNames()
# for each entry

function _associatorInterface() {
  declare -i c=0;
  echo '';
  while((c<_TEST_COUNT))
  do
    echo '';

    # 3.1) enumInstanceNames()
    echo 'execute enumInstanceNames() on '$_TEST_HOST' to '${_TEST_NAMES[c]}'';
     # execute enumInstanceNames() and save output in $InstNames
    InstNames=`wbemein $_TEST_HOST${_TEST_NAMES[c]}`;
    # ERROR OUTPUT
    echo $InstNames|grep "*";

    # initialize _InstCount -> contains the number of returned instances
    typeset -i _InstCount=`echo [-n "${InstNames}"]|wc -l`;
    echo 'Number of returned Instances: '${_InstCount};
      
    # each entry of the field _TEST_InstID[] contains the ObjectPath
    # of an Instance
    declare -a _TEST_InstID[${_InstCount}];
    # fill file _TEST_OUT with value of ${InstNames}
    echo ${InstNames}>_TEST_OUT;
    # fill field _TEST_InstID[] with the values of _TEST_OUT
    read <_TEST_OUT -a _TEST_InstID;
    # delete file _TEST_OUT
    rm _TEST_OUT;
      
    # iterate through list of ObjectPathes, and execute for each entry
    typeset -i d=0
    while((d<_InstCount))
    do
      InstOP=${_TEST_InstID[d]};
      
      if ((${_TEST_ASSOC_ASSOCIATOR_INSTNAMES}==1))
	then 
          # 3.2) associatorNames()
	  echo '... associatorNames() of '${InstOP#$_TEST_HOSTPREFIX};
	  _TEST_RC=`wbemain $_TEST_HOST${InstOP#$_TEST_HOSTPREFIX}`;
      fi
      if ((${_TEST_ASSOC_ASSOCIATOR_INST}==1))
	then 
          # 3.3) associators()
	  echo '... associators() of '${InstOP#$_TEST_HOSTPREFIX};
	  _TEST_RC=`wbemai $_TEST_HOST${InstOP#$_TEST_HOSTPREFIX}`;
	  echo $_TEST_RC;
      fi
      if ((${_TEST_ASSOC_REFERENCE_INSTNAMES}==1))
	then
          # 3.5) referenceNames()
	  echo '... referenceNames() of '${InstOP#$_TEST_HOSTPREFIX};
	  _TEST_RC=`wbemrin $_TEST_HOST${InstOP#$_TEST_HOSTPREFIX}`;
      fi
      if ((${_TEST_ASSOC_REFERENCE_INST}==1))
	then 
          # 3.5) references()
	  echo '... references() of '${InstOP#$_TEST_HOSTPREFIX};
	  _TEST_RC=`wbemri $_TEST_HOST${InstOP#$_TEST_HOSTPREFIX}`;
      fi

      # ERROR OUTPUT
      echo $_TEST_RC|grep "*";
      d=${d}+1;
    done
    # delete entries in field _TEST_InstID[]
    unset _TEST_InstID;
    c=${c}+1;
  done
  unset _TEST_NAMES;
  _TEST_COUNT=0;
  _TEST_GET_INST=0;
}


if (( ((${_TEST_ASSOC_ASSOCIATOR_INSTNAMES}==1))||((${_TEST_ASSOC_ASSOCIATOR_INST}==1))||\
((${_TEST_ASSOC_REFERENCE_INSTNAMES}==1))||((${_TEST_ASSOC_REFERENCE_INST}==1)) ))
then 
  _TEST_COUNT=${_TEST_COUNTClass};
  _TEST_NAMES=(${_TEST_CLASSNAMES[@]});
  _associatorInterface;
fi


# **********************************************************************


# **********************************************************************
# script finished
# **********************************************************************

echo '';
echo '*************************************************';
echo 'Function Test finished - please review the Output';
echo '*************************************************';
echo '';
exit 0;

#-----------------------------------------------------------------------
# end of testbase.sh
#-----------------------------------------------------------------------
