#!/bin/sh

# semantictest_OSBaseStage2_Processor.sh
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
#              Stage 2 - Processor :
#              Processor, CSProcessor
#
# Prerequisites: 
# To run this script successful, check the following requirements:
#
# 1) CIMOM on your specified host is running
# 2) the classes are loaded into the specified namespace
# 3) wbemcli is installed on your local system, where you run this 
#    script
# 4) function test (script functiontest.sh) and semantic test for
#    Stage 1 completed successfully 
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
_TEST_HOSTNAME=localhost:5988
_PROTOCOL=http://
_TEST_HOST=${_PROTOCOL}${_TEST_HOSTNAME}${_TEST_NAMESPACE}


#**********************************************************************#
#   ---------------------------------------------------------------    #
#                define the interface types to test                    #
#   ---------------------------------------------------------------    #
#   set the value of the following variables to                        #
#   0 ... ignore execution                                             #
#   1 ... perform execution                                            # 
#**********************************************************************#

# -------------- Instance Interface specific tests ------------------- #

# enumInstances()
#declare -i _TEST_ENUM_INST=0;
declare -i _TEST_ENUM_INST=1;
# enumInstanceNames()
#declare -i _TEST_ENUM_INSTNAMES=0;
declare -i _TEST_ENUM_INSTNAMES=1;
# getInstance()
#declare -i _TEST_GET_INST=0;
declare -i _TEST_GET_INST=1;
# setInstance()
#declare -i _TEST_SET_INST=0;
declare -i _TEST_SET_INST=1;
# createInstance()
#declare -i _TEST_CREATE_INST=0;
declare -i _TEST_CREATE_INST=1;
# deleteInstance()
#declare -i _TEST_DELETE_INST=0;
declare -i _TEST_DELETE_INST=1;
# execQuery()
#declare -i _TEST_EXEC_QUERY=0;
declare -i _TEST_EXEC_QUERY=1;

# -------------- Association Interface specific tests ---------------- #

# associators()
#declare -i _TEST_ASSOC=0;
declare -i _TEST_ASSOC=1;
# associatorNames()
#declare -i _TEST_ASSOC_NAMES=0;
declare -i _TEST_ASSOC_NAMES=1;
# references()
#declare -i _TEST_REF=0;
declare -i _TEST_REF=1;
# referenceNames()
#declare -i _TEST_REF_NAMES=0;
declare -i _TEST_REF_NAMES=1;



#**********************************************************************#
#   ---------------------------------------------------------------    #
#           define the class / association names to test               #
#   ---------------------------------------------------------------    #
#**********************************************************************#

# Definition of the names of the classes, which have to be tested by
# this script
declare -a _TEST_CLASSNAMES[]
_TEST_CLASSNAMES=([0]=cim_processor [1]=cim_computersystem)

# Definition of the names of the associations, which have to be tested 
# by this script
declare -a _TEST_ASSOCNAMES[]
_TEST_ASSOCNAMES=([0]=linux_csprocessor)

# **********************************************************************



# **********************************************************************
# !!!!!!!!!         DO NOT CHANGE CODE BELOW THIS LINE         !!!!!!!!!
# **********************************************************************



# **********************************************************************
# Test Scenario : Semantic Test for Processor
#
# 1) class Processor 
# 2) association CSProcessor
#
# **********************************************************************

# exit, if CIMOM is not running
wbemein $_TEST_HOST${_TEST_CLASSNAMES[0]} 2>&1 | grep Exception >/dev/null \
&& echo 'CIMOM NOT RUNNING ?' && exit 1;

# **********************************************************************

typeset -i _InstCount=0;



# **********************************************************************
#
# Step 1) CIM_Processor
#
# **********************************************************************

function _test_Processor() {

echo '--- start Test for class '${_TEST_CLASSNAMES[0]};

# ------------------------ enumInstances() --------------------------- #
if(( ${_TEST_ENUM_INST}==1 ))
then
  echo '* enumInstances()';
  InstNames=`wbemei $_TEST_HOST${_TEST_CLASSNAMES[0]}`;
  _InstCount=`echo [-n "${InstNames}"] |wc -l`;
  if((${_InstCount}!=`cat /proc/cpuinfo | grep processor | wc -l`))
  then echo 'FAILED - enumInstances() - number of returned Instances is not equal to the number of listed CPUs in /proc/cpuinfo';
  else echo 'OK - enumInstances() - number of returned Instances was '${_InstCount}; 
  fi
fi

# ---------------------- enumInstanceNames() ------------------------- #
if(( (${_TEST_ENUM_INSTNAMES}==1) || (${_TEST_GET_INST}==1) ||\
    (${_TEST_CREATE_INST}==1) || (${_TEST_DELETE_INST}==1) ))
then
  echo '* enumInstanceNames()';
  InstNames=`wbemein $_TEST_HOST${_TEST_CLASSNAMES[0]}`;
  _InstCount=`echo [-n "${InstNames}"]|wc -l`;
  if((${_InstCount}!=`cat /proc/cpuinfo | grep processor | wc -l`))
  then echo 'FAILED - enumInstanceNames() - number of returned Instances is not equal to the number of listed CPUs in /proc/cpuinfo';
  else echo 'OK - enumInstanceNames() - number of returned InstanceNames was '${_InstCount}; 
  fi
      
  # --- prepare field of InstanceIDs --- #
  declare -a _InstID[${_InstCount}];
  echo ${InstNames}>_TEST_OUT; # fill file _TEST_OUT with value of
  read <_TEST_OUT -a _InstID;  # fill field _InstID[] with the values
  rm _TEST_OUT;  # delete file _TEST_OUT
  InstOP=${_InstID[0]}; 

  # ------------------------- getInstance() ------------------------- #
  if(( ${_TEST_GET_INST}==1 ))
  then
    echo '* getInstance()';
    # iterate through list of ObjectPathes and execute getInstances()
    # for each entry
    typeset -i d=0
    while((d<_InstCount))
    do
      InstOP=${_InstID[d]};
      wbemgi ${_PROTOCOL}${InstOP} 2>&1 | grep 'Processor ID does not exist' \
      >/dev/null \
      && echo 'FAILED - getInstance() number '${d}' - returns with Processor ID does not exist - '${InstOP}\
      || echo 'OK - getInstance() number '${d}
      d=${d}+1;
    done
  fi

  # ----------------------- createInstance() ------------------------ #
  if(( ${_TEST_CREATE_INST}==1 ))
  then
    echo '* createInstance()';
    wbemci ${_PROTOCOL}${InstOP} systemcreationclassname="value",systemname="value",creationclassname="value",deviceid="value"\
    2>&1 | grep NOT_SUPPORTED >/dev/null \
    && echo 'OK - createInstance() - returns with NOT_SUPPORTED'\
    || echo 'FAILED - createInstance() - returns NOT with NOT_SUPPORTED'
  fi

  # ---------------------- deleteInstance() ------------------------ #
  if(( ${_TEST_DELETE_INST}==1 ))
  then
    echo '* deleteInstance()';
    wbemdi ${_PROTOCOL}${InstOP} 2>&1 | grep NOT_SUPPORTED >/dev/null \
    && echo 'OK - deleteInstance() - returns with NOT_SUPPORTED'\
    || echo 'FAILED - deleteInstance() - returns NOT with NOT_SUPPORTED'
  fi

  # delete entries in field _InstID[]
  unset _InstID;
fi

# ------------------------- setInstance() -------------------------- #
if(( ${_TEST_SET_INST}==1 ))
then 
  echo '* setInstance() - currently not implemented by this test script';
fi

# --------------------------- execQuery() -------------------------- #
if(( ${_TEST_EXEC_QUERY}==1 ))
then
  echo '* execQuery() - currently not implemented by this test script';
fi

echo '--- finished Test for class '${_TEST_CLASSNAMES[0]};
echo '';

}



# **********************************************************************
#
# Step 2) CSProcessor
#
# **********************************************************************

function _test_CSProcessor() {

echo '--- start Test for association CSProcessor';

typeset -i _CPUCount=`cat /proc/cpuinfo | grep processor | wc -l`;
typeset -i _Count=0;

# ------------------------ enumInstances() --------------------------- #
if(( ${_TEST_ENUM_INST}==1 ))
then
  echo '* enumInstances()';
  InstNames=`wbemei $_TEST_HOST${_TEST_ASSOCNAMES[0]}`;
  _InstCount=`echo [-n "${InstNames}"]|wc -l`*2;

  # --- prepare field of InstanceIDs --- #
  declare -a _InstID[${_InstCount}];
  echo ${InstNames}>_TEST_OUT; # fill file _TEST_OUT with value of
  read <_TEST_OUT -a _InstID;  # fill field _InstID[] with the values
  rm _TEST_OUT;  # delete file _TEST_OUT

  typeset -i d=0
  while((d<_InstCount))
  do
    InstOP=${_InstID[d]};
    echo $InstOP|grep -i csprocessor >/dev/null && _Count=${_Count}+1;
    d=${d}+2;
  done

  if((${_Count}!=${_CPUCount}))
  then echo 'FAILED - enumInstances() - number of returned Instances is not equal to the number of listed CPUs in /proc/cpuinfo';
  else echo 'OK - enumInstances() - number of returned Instances was '${_Count}; 
  fi

  # delete entries in field _InstID[]
  unset _InstID;
  _Count=0;
fi

# ---------------------- enumInstanceNames() ------------------------- #
if(( (${_TEST_ENUM_INSTNAMES}==1) || (${_TEST_GET_INST}==1) ||\
    (${_TEST_CREATE_INST}==1) || (${_TEST_DELETE_INST}==1) ))
then
  echo '* enumInstanceNames()';
  InstNames=`wbemein $_TEST_HOST${_TEST_ASSOCNAMES[0]}`;
  _InstCount=`echo [-n "${InstNames}"]|wc -l`;

  # --- prepare field of InstanceIDs --- #
  declare -a _InstID[${_InstCount}];
  echo ${InstNames}>_TEST_OUT; # fill file _TEST_OUT with value of
  read <_TEST_OUT -a _InstID;  # fill field _InstID[] with the values
  rm _TEST_OUT;  # delete file _TEST_OUT

  typeset -i d=0
  while((d<_InstCount))
  do
    InstOP=${_InstID[d]};
    echo $InstOP|grep -i csprocessor >/dev/null && _Count=${_Count}+1;
    d=${d}+1;
  done

  if((${_Count}!=${_CPUCount}))
  then echo 'FAILED - enumInstanceNames() - number of returned Instances is not equal to the number of listed CPUs in /proc/cpuinfo';
  else echo 'OK - enumInstanceNames() - number of returned InstanceNames was '${_Count}; 
  fi

  # iterate through list of ObjectPathes and execute ... for each entry
  typeset -i d=0;
  typeset -i _Valid=0;
  while((d<_InstCount))
  do
    InstOP=${_InstID[d]};  
    echo $InstOP|grep -i csprocessor >/dev/null && _Valid=1;
    # ------------------------- getInstance() ------------------------- #
    if(( ${_TEST_GET_INST}==1 && ${_Valid}==1 ))
    then 
      echo '* getInstance()';
      echo $InstOP|grep -i csprocessor >/dev/null && 
      (wbemgi ${_PROTOCOL}${InstOP} 2>&1 | grep '*' >/dev/null \
       && echo 'FAILED - getInstance() - '${InstOP}\
       || echo 'OK - getInstance()')
    fi

    # ----------------------- createInstance() ------------------------ #
    if(( ${_TEST_CREATE_INST}==1 && ${_Valid}==1 ))
    then
	echo '* createInstance() - currently not implemented by this test script';
    fi

    # ---------------------- deleteInstance() ------------------------ #
    if(( ${_TEST_DELETE_INST}==1 && ${_Valid}==1 ))
    then
	echo '* deleteInstance()';
	echo $InstOP|grep -i csprocessor >/dev/null && 
	wbemdi ${_PROTOCOL}${InstOP} 2>&1 | grep NOT_SUPPORTED >/dev/null \
        && echo 'OK - deleteInstance() - returns with NOT_SUPPORTED'\
        || echo 'FAILED - deleteInstance() - returns NOT with NOT_SUPPORTED'
    fi
    d=${d}+1;
    _Valid=0;
  done

  # delete entries in field _InstID[]
  unset _InstID;
fi

# ------------------------- setInstance() -------------------------- #
if(( ${_TEST_SET_INST}==1 ))
then 
  echo '* setInstance() - currently not implemented by this test script';
fi

# --------------------------- execQuery() -------------------------- #
if(( ${_TEST_EXEC_QUERY}==1 ))
then
  echo '* execQuery() - currently not implemented by this test script';
fi


# -------------------- Associator Interface() ------------------------ #

if(( (${_TEST_ASSOC}==1) || (${_TEST_ASSOC_NAMES}==1) || \
    (${_TEST_REF}==1) || (${_TEST_REF_NAMES}==1) ))
then
  # ------------------ get Input data for calls ---------------------- #
  # left = ComputerSystem (GroupComponent)
  LeftInstNames=`wbemein $_TEST_HOST${_TEST_CLASSNAMES[1]}`;
  # right = Processor (PartComponent)
  RightInstNames=`wbemein $_TEST_HOST${_TEST_CLASSNAMES[0]}`;
  # --- prepare field of InstanceIDs --- #
  declare -a _InstID[];
  echo ${RightInstNames}>_TEST_OUT; # fill file _TEST_OUT with value of
  read <_TEST_OUT -a _InstID;  # fill field _InstID[] with the values
  rm _TEST_OUT;  # delete file _TEST_OUT
  InstOP=${_InstID[0]};

  # ------------------ associators() --------------------------------- #
  if(( ${_TEST_ASSOC}==1 ))
  then 
    echo '* associators()';
    # test return value with left objectpath as input
    wbemai -ac ${_TEST_ASSOCNAMES[0]} ${_PROTOCOL}${LeftInstNames} 2>&1 \
    | grep -i processor. >/dev/null \
    && echo 'OK - associators() -  Input: '${_TEST_CLASSNAMES[1]}\
    ' - returned Instance was class or subclass of type '${_TEST_CLASSNAMES[0]} \
    || echo 'FAILED - associators() - Input: '${_TEST_CLASSNAMES[1]}\
    ' - returned Instance was not class or subclass of type '${_TEST_CLASSNAMES[0]}

    # test return value with right objectpath as input
    wbemai -ac ${_TEST_ASSOCNAMES[0]} ${_PROTOCOL}${InstOP} 2>&1 \
    | grep -i computersystem. >/dev/null \
    && echo 'OK - associators() -  Input: '${_TEST_CLASSNAMES[0]}\
    ' - returned Instance was class or subclass of type '${_TEST_CLASSNAMES[1]} \
    || echo 'FAILED - associators() - Input: '${_TEST_CLASSNAMES[0]}\
    ' - returned Instance was not class or subclass of type '${_TEST_CLASSNAMES[1]}
  fi

  # ------------------ associatorNames() ----------------------------- #
  if(( ${_TEST_ASSOC_NAMES}==1 ))
  then
    echo '* associatorNames()';
    # test return value with left objectpath as input
    wbemain -ac ${_TEST_ASSOCNAMES[0]} ${_PROTOCOL}${LeftInstNames} 2>&1 \
    | grep -i processor. >/dev/null \
    && echo 'OK - associatorNames() -  Input: '${_TEST_CLASSNAMES[1]}\
    ' - returned Instance was class or subclass of type '${_TEST_CLASSNAMES[0]} \
    || echo 'FAILED - associatorNames() - Input: '${_TEST_CLASSNAMES[1]}\
    ' - returned Instance was not class or subclass of type '${_TEST_CLASSNAMES[0]}

    # test return value with right objectpath as input
    wbemain -ac ${_TEST_ASSOCNAMES[0]} ${_PROTOCOL}${InstOP} 2>&1 \
    | grep -i computersystem. >/dev/null \
    && echo 'OK - associatorNames() -  Input: '${_TEST_CLASSNAMES[0]}\
    ' - returned Instance was class or subclass of type '${_TEST_CLASSNAMES[1]} \
    || echo 'FAILED - associatorNames() - Input: '${_TEST_CLASSNAMES[0]}\
    ' - returned Instance was not class or subclass of type '${_TEST_CLASSNAMES[1]}
  fi

  # ------------------- references() --------------------------------- #
  if(( ${_TEST_REF}==1 ))
  then 
    echo '* references()';
    # test return value with left objectpath as input
    wbemri -arc ${_TEST_ASSOCNAMES[0]} ${_PROTOCOL}${LeftInstNames} 2>&1 \
    | grep -i csprocessor. >/dev/null \
    && echo 'OK - references() -  Input: '${_TEST_CLASSNAMES[1]}\
    ' - returned InstanceName was class or subclass of type '${_TEST_ASSOCNAMES[0]} \
    || echo 'FAILED - references() - Input: '${_TEST_CLASSNAMES[1]}\
    ' - returned InstanceName was not class or subclass of type '${_TEST_ASSOCNAMES[0]}

    # test return value with right objectpath as input
    wbemri -arc ${_TEST_ASSOCNAMES[0]} ${_PROTOCOL}${InstOP} 2>&1 \
    | grep -i csprocessor. >/dev/null \
    && echo 'OK - references() -  Input: '${_TEST_CLASSNAMES[0]}\
    ' - returned InstanceName was class or subclass of type '${_TEST_ASSOCNAMES[0]} \
    || echo 'FAILED - references() - Input: '${_TEST_CLASSNAMES[0]}\
    ' - returned InstanceName was not class or subclass of type '${_TEST_ASSOCNAMES[0]}
  fi

  # ------------------ referenceNames() ------------------------------ #
  if(( ${_TEST_REF_NAMES}==1 ))
  then 
    echo '* referenceNames()';
    # test return value with left objectpath as input
    wbemrin -arc ${_TEST_ASSOCNAMES[0]} ${_PROTOCOL}${LeftInstNames} 2>&1 \
    | grep -i csprocessor. >/dev/null \
    && echo 'OK - referenceNames() -  Input: '${_TEST_CLASSNAMES[1]}\
    ' - returned InstanceName was class or subclass of type '${_TEST_ASSOCNAMES[0]} \
    || echo 'FAILED - referenceNames() - Input: '${_TEST_CLASSNAMES[1]}\
    ' - returned InstanceName was not class or subclass of type '${_TEST_ASSOCNAMES[0]}

    # test return value with right objectpath as input
    wbemrin -arc ${_TEST_ASSOCNAMES[0]} ${_PROTOCOL}${InstOP} 2>&1 \
    | grep -i csprocessor. >/dev/null \
    && echo 'OK - referenceNames() -  Input: '${_TEST_CLASSNAMES[0]}\
    ' - returned InstanceName was class or subclass of type '${_TEST_ASSOCNAMES[0]} \
    || echo 'FAILED - referenceNames() - Input: '${_TEST_CLASSNAMES[0]}\
    ' - returned InstanceName was not class or subclass of type '${_TEST_ASSOCNAMES[0]}
  fi

  # delete entries in field _InstID[]
  unset _InstID;
fi
# -------------- end of Associator Interface() ----------------------- #

echo '--- finished Test for association CSProcessor';
echo '';

}


# **********************************************************************
# You can choose the tests which have to be performed.
# **********************************************************************

_test_Processor;
_test_CSProcessor;


# **********************************************************************
# script finished
# **********************************************************************

echo '';
echo '***************************************************************';
echo 'Semantic Test for Processor finished - please review the Output';
echo '***************************************************************';
echo '';
exit 0;

#-----------------------------------------------------------------------
# end of semantictest_OSBaseStage2_Processor.sh
#-----------------------------------------------------------------------
