#!/bin/sh

# cmpic.sh
#
# Copyright (c) 2002, International Business Machines
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
# Description: Provider Skeleton Generator
# This shell script creates the skeleton for a native C provider using 
# the Common Manageability Programming Interface ( CMPI ) .
#
# ./createc provider_name [inst|-] [assoc|-] [method|-] [event|-]
# you have to specify at least one interface type you want to implement
#
#


case $1 in
"--help") echo './cmpic.sh provider_name [inst|-] [assoc|-] [method|-] [event|-]';exit;;
esac

case $2 in
"-")
    inst=0;;
inst)
    inst=1;;
esac

case $3 in
"-")
    assoc=0;;
assoc)
    assoc=1;;
esac

case $4 in
"-")
    method=0;;
method)
    method=1;;
esac

case $5 in
"-")
    event=0;;
event)
    event=1;;
esac


echo '/*
 * '$1'.c
 *
 * Copyright (c) 2002, International Business Machines
 *
 * THIS FILE IS PROVIDED UNDER THE TERMS OF THE COMMON PUBLIC LICENSE 
 * ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE 
 * CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
 *
 * You can obtain a current copy of the Common Public License from
 * http://oss.software.ibm.com/developerworks/opensource/license-cpl.html
 *
 * Author:       
 * Contributors:
 *
 * Interface Type : Common Manageability Programming Interface ( CMPI )
 *
 * Description: 
 * 
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmpidt.h"
#include "cmpift.h"
#include "cmpimacs.h"

//#include "cimibase.h"

unsigned char CMPI_true=1;
unsigned char CMPI_false=0;

CMPIBroker * _broker;

/* ---------------------------------------------------------------------------*/
/* private declarations                                                       */
#ifdef DEBUG
    int _debug = 1;
#else
    int _debug = 0;
#endif

static char * _ClassName     = "'$1'";' >> $1.c


inst_private_def() {
echo '
/* private methods                                                            */
/*                                                                            */

/* - method to create a CMPIObjectPath of this class                          */
static CMPIObjectPath * _makePath( CMPIObjectPath * cop, CMPIStatus * rc);

/* - method to create a CMPIInstance of this class                            */
static CMPIInstance * _makeInst( CMPIObjectPath * cop, CMPIStatus * rc);
/* ---------------------------------------------------------------------------*/

' >> $1.c 
}

assoc_private_def() {
echo 'static char * _RefLeft       = "";
static char * _RefRight      = "";
static char * _RefLeftClass  = "";
static char * _RefRightClass = "";

/* ---------------------------------------------------------------------------*/

' >> $1.c
}

case $inst in
0) case $assoc in
    1) assoc_private_def $*;;
   esac;;
1) case $assoc in
    0) inst_private_def $*;;
    1) assoc_private_def $*;;
   esac;;
esac

inst() {
echo '/* ---------------------------------------------------------------------------*/
/*                      Instance Provider Interface                           */
/* ---------------------------------------------------------------------------*/


CMPIStatus '$1'Cleanup( CMPIInstanceMI * mi, 
  CMPIContext * ctx) {
  CMReturn(CMPI_RC_OK);
}

CMPIStatus '$1'EnumInstanceNames( CMPIInstanceMI * mi, 
  CMPIContext * ctx, 
  CMPIResult * rslt, 
  CMPIObjectPath * ref) {
  CMPIObjectPath * op = NULL;  
  CMPIStatus       rc = {CMPI_RC_OK, NULL};
  
  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI EnumInstanceNames()\n", _ClassName );' >> $1.c

case $assoc in
0) echo '  
  op = _makePath( ref, &rc );

  if( op == NULL ) { 
    if( _debug ) {
      if( rc.msg != NULL ) 
	{ fprintf(stderr,"rc.msg: %s\n",CMGetCharPtr(rc.msg)); }
    }
    CMReturn(CMPI_RC_ERR_FAILED); 
  }

  CMReturnObjectPath( rslt, op );' >> $1.c
esac

echo '
  CMReturnDone( rslt );
  CMReturn( CMPI_RC_OK );
}

CMPIStatus '$1'EnumInstances( CMPIInstanceMI * mi, 
  CMPIContext * ctx, 
  CMPIResult * rslt, 
  CMPIObjectPath * ref, 
  char ** properties) {
  CMPIInstance * ci = NULL;  
  CMPIStatus     rc = {CMPI_RC_OK, NULL};

  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI EnumInstances()\n", _ClassName );' >> $1.c

case $assoc in
0) echo '
  ci = _makeInst( ref, &rc );

  if( ci == NULL ) { 
    if( _debug ) {
      if( rc.msg != NULL ) 
	{ fprintf(stderr,"rc.msg: %s\n",CMGetCharPtr(rc.msg)); }
    }
    CMReturn(CMPI_RC_ERR_FAILED); 
  }

  CMReturnInstance( rslt, ci );' >> $1.c
esac

echo '
  CMReturnDone( rslt );
  CMReturn( CMPI_RC_OK );
}

CMPIStatus '$1'GetInstance( CMPIInstanceMI * mi, 
  CMPIContext * ctx, 
  CMPIResult * rslt, 
  CMPIObjectPath * cop, 
  char ** properties) {
  CMPIInstance * ci = NULL;
  CMPIStatus     rc = {CMPI_RC_OK, NULL};

  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI GetInstance()\n", _ClassName );' >> $1.c

case $assoc in
0) echo '
  ci = _makeInst( cop, &rc );

  if( ci == NULL ) { 
    if( _debug ) {
      if( rc.msg != NULL ) 
	{ fprintf(stderr,"rc.msg: %s\n",CMGetCharPtr(rc.msg)); }
    }
    CMReturn(CMPI_RC_ERR_NOT_FOUND); 
  }

  CMReturnInstance( rslt, ci );' >> $1.c
esac

echo '
  CMReturnDone(rslt);
  CMReturn(CMPI_RC_OK);
}

CMPIStatus '$1'CreateInstance( CMPIInstanceMI * mi, 
  CMPIContext * ctx, 
  CMPIResult * rslt, 
  CMPIObjectPath * cop, 
  CMPIInstance * ci) {
  CMPIStatus rc = {CMPI_RC_OK, NULL};
  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI CreateInstance()\n", _ClassName ); 
  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "NOT_SUPPORTED" ); 
  return rc;
}

CMPIStatus '$1'SetInstance( CMPIInstanceMI * mi, 
  CMPIContext * ctx, 
  CMPIResult * rslt, 
  CMPIObjectPath * cop,
  CMPIInstance * ci, 
  char **properties) {
  CMPIStatus rc = {CMPI_RC_OK, NULL};
  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI SetInstance()\n", _ClassName ); 
  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "NOT_SUPPORTED" ); 
  return rc;
}

CMPIStatus '$1'DeleteInstance( CMPIInstanceMI * mi, 
  CMPIContext * ctx, 
  CMPIResult * rslt, 
  CMPIObjectPath * cop) {
  CMPIStatus rc = {CMPI_RC_OK, NULL}; 
  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI DeleteInstance()\n", _ClassName ); 
  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "NOT_SUPPORTED" ); 
  return rc;
}

CMPIStatus '$1'ExecQuery( CMPIInstanceMI * mi, 
  CMPIContext * ctx, 
  CMPIResult * rslt, 
  CMPIObjectPath * ref, 
  char * lang, 
  char * query) {
  CMPIStatus rc = {CMPI_RC_OK, NULL};
  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI ExecQuery()\n", _ClassName ); 
  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "NOT_SUPPORTED" ); 
  return rc;
}

' >> $1.c
}


assoc() {
echo '/* ---------------------------------------------------------------------------*/
/*                    Associator Provider Interface                           */
/* ---------------------------------------------------------------------------*/


CMPIStatus '$1'AssociationCleanup( CMPIAssociationMI * mi,
  CMPIContext * ctx) {
  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI AssociationCleanup()\n", _ClassName ); 
  CMReturn(CMPI_RC_OK);
}

/* 
 * The intention of associations is to show the relations between different
 * classes and their instances. Therefore an association has two properties. 
 * Each one representing a reference to a certain instance of the specified
 * class. We can say, that an association has a left and the right "end". 
 * 
 * '$1' : 
 *    < role >   -> < class >
 *    "_RefLeft"  -> "_RefLeftClass" 
 *    "_RefRight" -> "_RefRightClass"
 *
 */

/*
 * general API information
 *
 * cop :
 *    Specifies the CMPIObjectPath to the CMPIInstance of the known end of 
 *    the association. Its absolutelly necessary to define this anchor point,
 *    from where the evaluation of the association starts.
 * assocPath : 
 *    If not NULL, it contains the name of the association the caller is 
 *    looking for. The provider should only return values, when the assocPath 
 *    contains the name(s) of the association(s) he is responsible for !
 * role :
 *    The caller can specify that the source instance ( cop ) has to play 
 *    a certain <role> in the association. The <role> is specified by the
 *    association definition (see above). That means, role has to contain
 *    the same value as the <role> the source instance plays in this assoc.
 *    If this requirement is not true, the provider returns nothing.
 */

/*
 * specification of associators() and associatorNames()
 *
 * These methods return CMPIInstance ( in the case of associators() ) or 
 * CMPIObjectPath ( in the case of associatorNames() ) object(s) of the 
 * opposite end of the association.
 *
 * resultRole :
 *    The caller can specify that the target instance(s) has/have to play 
 *    a certain <role> in the association. The <role> is specified by the
 *    association definition (see above). That means, resultRole has to 
 *    contain the same value as the <role> the target instance(s) plays
 *    in this assoc. If this requirement is not true, the provider returns 
 *    nothing.
 * resultClass :
 *    The caller can specify that the target instance(s) has/have to be 
 *    instances of a certain <class>. The <class> is specified by the
 *    association definition (see above). That means, resultClass has to 
 *    contain the same value as the <class> of the target instance(s).
 *    If this requirement is not true, the provider returns nothing.
 */

CMPIStatus '$1'Associators( CMPIAssociationMI * mi,
  CMPIContext * ctx,
  CMPIResult * rslt,
  CMPIObjectPath * cop,
  char * assocClass,
  char * resultClass,
  char * role,
  char * resultRole,
  char ** propertyList ) {
  CMPIStatus       rc = {CMPI_RC_OK, NULL};
  CMPIObjectPath * op = NULL;

  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI Associators()\n", _ClassName ); 
    
  if( assocClass ) {
    op = CMNewObjectPath( _broker, _ClassName,
			    CMGetCharPtr(CMGetNameSpace(cop,&rc)), &rc );
  }

  if( ( assocClass==NULL ) || ( CMClassPathIsA(_broker,op,assocClass,&rc) == 1 ) ) {
  }

  CMReturnDone( rslt );
  CMReturn(CMPI_RC_OK);
}

CMPIStatus '$1'AssociatorNames( CMPIAssociationMI * mi,
  CMPIContext * ctx,
  CMPIResult * rslt,
  CMPIObjectPath * cop,
  char * assocClass,
  char * resultClass,
  char * role,
  char * resultRole) {
  CMPIStatus       rc = {CMPI_RC_OK, NULL};
  CMPIObjectPath * op = NULL;

  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI AssociatorNames()\n", _ClassName ); 
    
  if( assocClass ) {
    op = CMNewObjectPath( _broker, _ClassName,
			    CMGetCharPtr(CMGetNameSpace(cop,&rc)), &rc );
  }

  if( ( assocClass==NULL ) || ( CMClassPathIsA(_broker,op,assocClass,&rc) == 1 ) ) {
  }

  CMReturnDone( rslt );
  CMReturn(CMPI_RC_OK);
}


/*
 * specification of references() and referenceNames()
 *
 * These methods return CMPIInstance ( in the case of references() ) or 
 * CMPIObjectPath ( in the case of referenceNames() ) object(s) of th 
 * association itself.
 */

CMPIStatus '$1'References( CMPIAssociationMI * mi,
  CMPIContext * ctx,
  CMPIResult * rslt,
  CMPIObjectPath * cop,
  char * assocClass,
  char * role,
  char ** propertyList ) {
  CMPIStatus       rc = {CMPI_RC_OK, NULL};
  CMPIObjectPath * op = NULL;

  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI References()\n", _ClassName ); 
    
  if( assocClass ) {
    op = CMNewObjectPath( _broker, _ClassName,
			    CMGetCharPtr(CMGetNameSpace(cop,&rc)), &rc );
  }

  if( ( assocClass==NULL ) || ( CMClassPathIsA(_broker,op,assocClass,&rc) == 1 ) ) {
  }

  CMReturnDone( rslt );
  CMReturn(CMPI_RC_OK);
}

CMPIStatus '$1'ReferenceNames( CMPIAssociationMI * mi,
  CMPIContext * ctx,
  CMPIResult * rslt,
  CMPIObjectPath * cop,
  char * assocClass,
  char * role) {
  CMPIStatus       rc = {CMPI_RC_OK, NULL};
  CMPIObjectPath * op = NULL;

  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI ReferenceNames()\n", _ClassName ); 
    
  if( assocClass ) {
    op = CMNewObjectPath( _broker, _ClassName,
			    CMGetCharPtr(CMGetNameSpace(cop,&rc)), &rc );
  }

  if( ( assocClass==NULL ) || ( CMClassPathIsA(_broker,op,assocClass,&rc) == 1 ) ) {
  }

  CMReturnDone( rslt );
  CMReturn(CMPI_RC_OK);
}

' >> $1.c
}

method() {
echo '
/* ---------------------------------------------------------------------------*/
/*                        Method Provider Interface                           */
/* ---------------------------------------------------------------------------*/


CMPIStatus '$1'MethodCleanup( CMPIMethodMI * mi, 
  CMPIContext * ctx) {
  CMReturn(CMPI_RC_OK);
}

CMPIStatus '$1'InvokeMethod( CMPIMethodMI * mi,
  CMPIContext * ctx,
  CMPIResult * rslt,
  CMPIObjectPath * ref,
  char * methodName,
  CMPIArgs * in,
  CMPIArgs * out) {
  CMPIString * class = NULL; 
  CMPIStatus   rc    = {CMPI_RC_OK, NULL};
  CMPIValue    valrc;  

  if( _debug )
    fprintf( stderr, "--- %sMethodProvider.c : CMPI InvokeMethod()\n", _ClassName );

  if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 && 
      strcasecmp("insert_method_name",methodName) == 0 ) {  
    //valrc.uint32 = 99;
    //CMReturnData( rslt, &valrc, CMPI_uint32); 
  }
  else {
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_SUPPORTED, methodName ); 
  }
 
  return rc;
}

' >> $1.c
}


event() {
echo '/* ---------------------------------------------------------------------------*/
/*                       Indication Provider Interface                        */
/* ---------------------------------------------------------------------------*/


/* compile option -DNOEVENTS allows to make the Indication support optional   */

#ifndef NOEVENTS

CMPIStatus '$1'IndicationCleanup( CMPIIndicationMI * mi, 
  CMPIContext * ctx) {
  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI IndicationCleanup()\n", _ClassName ); 
  CMReturn(CMPI_RC_OK);
}

CMPIStatus '$1'AuthorizeFilter( CMPIIndicationMI * mi, 
  CMPIContext * ctx, 
  CMPIResult * rslt,
  CMPISelectExp * filter, 
  char * indType, 
  CMPIObjectPath * classPath,
  char * owner) {
  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI AuthorizeFilter()\n", _ClassName ); 
  CMReturn(CMPI_RC_OK);
}

CMPIStatus '$1'MustPoll( CMPIIndicationMI * mi, 
  CMPIContext * ctx, 
  CMPIResult * rslt,
  CMPISelectExp * filter, 
  char * indType, 
  CMPIObjectPath * classPath) {
  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI MustPoll()\n", _ClassName ); 
  CMReturnDone(rslt);
  CMReturn(CMPI_RC_OK);
}

CMPIStatus '$1'ActivateFilter( CMPIIndicationMI * mi, 
  CMPIContext * ctx, 
  CMPIResult * rslt,
  CMPISelectExp * filter, 
  char * indType, 
  CMPIObjectPath * classPath,
  CMPIBoolean firstActivation) {
  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI ActivateFilter()\n", _ClassName ); 
  CMReturn(CMPI_RC_OK);
}

CMPIStatus '$1'DeActivateFilter( CMPIIndicationMI * mi, 
  CMPIContext * ctx, 
  CMPIResult * rslt,
  CMPISelectExp * filter, 
  char * indType, 
  CMPIObjectPath *classPath,
  CMPIBoolean lastActivation) {
  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI DeActivateFilter()\n", _ClassName ); 
  CMReturn(CMPI_RC_OK);
}

#endif

' >> $1.c
}

case $inst in
1) inst $*;;
esac
case $assoc in
1) assoc $*;;
esac
case $method in
1) method $*;;
esac
case $event in
1) event $*;;
esac

echo '
/* ---------------------------------------------------------------------------*/
/*                              private methods                               */
/* ---------------------------------------------------------------------------*/
' >> $1.c

inst_private() {
echo '
/* ---------- method to create a CMPIObjectPath of this class ----------------*/

static CMPIObjectPath * _makePath( CMPIObjectPath * ref,
				   CMPIStatus * rc) {
  CMPIObjectPath * op = NULL;
 
  /* tool method call to get the unique name of the system */
  if( !CIM_HOST_NAME ) { 
    get_system_name(); 
    if( !CIM_HOST_NAME ) {   
      CMSetStatusWithChars( _broker, rc, 
			    CMPI_RC_ERR_FAILED, "no host name found" );
      goto exit;
    }
  }

  op = CMNewObjectPath( _broker, _ClassName,
			CMGetCharPtr(CMGetNameSpace(ref,rc)), rc );
  if( CMIsNullObject(op) ) { 
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." ); 
    goto exit; 
  }

  //  CMAddKey(op, "CreationClassName", _ClassName, CMPI_chars);  
  //  CMAddKey(op, "Name", CIM_HOST_NAME, CMPI_chars);
    
 exit:
  return op;                
}

/* ----------- method to create a CMPIInstance of this class ----------------*/

static CMPIInstance * _makeInst( CMPIObjectPath * ref,
				 CMPIStatus * rc) {
  CMPIObjectPath * op = NULL;
  CMPIInstance   * ci = NULL;
  int    status       = 2;       /* Enabled */

  /* tool method call to get the unique name of the system */
  if( !CIM_HOST_NAME ) { 
    get_system_name(); 
    if( !CIM_HOST_NAME ) {   
      CMSetStatusWithChars( _broker, rc, 
			    CMPI_RC_ERR_FAILED, "no host name found" );
      goto exit;
    }
  }

  op = CMNewObjectPath( _broker, _ClassName,
			CMGetCharPtr(CMGetNameSpace(ref,rc)), rc );
  if( CMIsNullObject(op) ) { 
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." ); 
    goto exit; 
  }

  ci = CMNewInstance( _broker, op, rc);
  if( CMIsNullObject(ci) ) { 
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "Create CMPIInstance failed." ); 
    goto exit; 
  }
    
  /*
  CMSetProperty( ci, "CreationClassName", _ClassName, CMPI_chars ); 
  CMSetProperty( ci, "Name", " ", CMPI_chars );
  CMSetProperty( ci, "Status", "NULL", CMPI_chars);
  CMSetProperty( ci, "Caption", "'$1'", CMPI_chars);
  CMSetProperty( ci, "Description", " ", CMPI_chars);
  */
  /* 2.7 */ 
  //  CMSetProperty( ci, "ElementName", "", CMPI_chars);
  CMSetProperty( ci, "EnabledStatus", (CMPIValue*)&(status), CMPI_uint16);
  CMSetProperty( ci, "OtherEnabledStatus", "NULL", CMPI_chars);
  CMSetProperty( ci, "RequestedStatus", (CMPIValue*)&(status), CMPI_uint16);
  CMSetProperty( ci, "EnabledDefault", (CMPIValue*)&(status), CMPI_uint16);

 exit:
  return ci;
}

' >> $1.c
}

assoc_private() {
echo '
' >> $1.c
}


case $inst in
0) case $assoc in
    1) assoc_private $*;;
   esac;;
1) case $assoc in
    0) inst_private $*;;
    1) assoc_private $*;;
   esac;;
esac

echo '
/* ---------------------------------------------------------------------------*/
/*                              Provider Factory                              */
/* ---------------------------------------------------------------------------*/
' >> $1.c

inst_factory() {
echo 'CMInstanceMIStub( '$1', '$1', broker, CMNoHook);
' >> $1.c
}

assoc_factory() {
echo 'CMAssociationMIStub( '$1', '$1', broker, CMNoHook);
' >> $1.c
}

method_factory() {
echo 'CMMethodMIStub( '$1', '$1', broker, CMNoHook);
' >> $1.c
}

event_factory() {
echo 'CMIndicationMIStub( '$1', '$1', broker, CMNoHook);
' >> $1.c
}


case $inst in
1) inst_factory $*;;
esac
case $assoc in
1) assoc_factory $*;;
esac
case $method in
1) method_factory $*;;
esac
case $event in
1) event_factory $*;;
esac


echo '/* ---------------------------------------------------------------------------*/
/*                    end of '$1'                      */
/* ---------------------------------------------------------------------------*/
' >> $1.c
