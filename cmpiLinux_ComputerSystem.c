/*
 * Linux_ComputerSystem.c
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
 * Author:       Heidi Neumann <heidineu@de.ibm.com>
 * Contributors:
 *
 * Interface Type : Common Magabeablity Programming Interface ( CMPI )
 *
 * Description: 
 * This is the C Provider implementation for the CIM class 
 * Linux_ComputerSystem. The enumeration of this class 
 * returns one single instance, representing the Computer System
 * container. All other classes depend on this instance. This
 * becomes visible in the key elements of other classes.
 *
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmpidt.h"
#include "cmpift.h"
#include "cmpimacs.h"

#include "cimibase.h"

unsigned char CMPI_true=1;
unsigned char CMPI_false=0;

static CMPIBroker * _broker;

/* ---------------------------------------------------------------------------*/
/* private declarations                                                       */
#ifdef DEBUG
    int _debug = 1;
#else
    int _debug = 0;
#endif

static char * _ClassName = "Linux_ComputerSystem";

/* private methods                                                            */
/*                                                                            */

/* - method to create a CMPIObjectPath of this class                          */
static CMPIObjectPath * _makePath( CMPIObjectPath * cop, CMPIStatus * rc);

/* - method to create a CMPIInstance of this class                            */
static CMPIInstance * _makeInst( CMPIObjectPath * cop, CMPIStatus * rc);
/* ---------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------*/
/*                      Instance Provider Interface                           */
/* ---------------------------------------------------------------------------*/


CMPIStatus Linux_ComputerSystemCleanup( CMPIInstanceMI * mi, 
					CMPIContext * ctx) {
  CMReturn(CMPI_RC_OK);
}

CMPIStatus Linux_ComputerSystemEnumInstanceNames( CMPIInstanceMI * mi, 
						  CMPIContext * ctx,
						  CMPIResult * rslt,
						  CMPIObjectPath * ref) {
  CMPIObjectPath * op = NULL;  
  CMPIStatus       rc = {CMPI_RC_OK, NULL};
  
  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI EnumInstanceNames()\n", _ClassName );
  
  op = _makePath( ref, &rc );

  if( op == NULL ) { 
    if( _debug ) {
      if( rc.msg != NULL ) 
	{ fprintf(stderr,"rc.msg: %s\n",CMGetCharPtr(rc.msg)); }
    }
    return rc;
  }

  CMReturnObjectPath( rslt, op );
  CMReturnDone( rslt );
  CMReturn( CMPI_RC_OK );
}

CMPIStatus Linux_ComputerSystemEnumInstances( CMPIInstanceMI * mi, 
					      CMPIContext * ctx, 
					      CMPIResult * rslt,
					      CMPIObjectPath * ref, 
					      char ** properties) {
  CMPIInstance * ci = NULL;  
  CMPIStatus     rc = {CMPI_RC_OK, NULL};

  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI EnumInstances()\n", _ClassName ); 

  ci = _makeInst( ref, &rc );

  if( ci == NULL ) { 
    if( _debug ) {
      if( rc.msg != NULL ) 
	{ fprintf(stderr,"rc.msg: %s\n",CMGetCharPtr(rc.msg)); }
    }
    return rc;
  }

  CMReturnInstance( rslt, ci );
  CMReturnDone( rslt );
  CMReturn( CMPI_RC_OK );
}


CMPIStatus Linux_ComputerSystemGetInstance( CMPIInstanceMI * mi, 
					    CMPIContext * ctx, 
					    CMPIResult * rslt,
					    CMPIObjectPath * cop, 
					    char ** properties) {
  CMPIInstance * ci = NULL;
  CMPIStatus     rc = {CMPI_RC_OK, NULL};

  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI GetInstance()\n", _ClassName ); 

  ci = _makeInst( cop, &rc );

  if( ci == NULL ) { 
    if( _debug ) {
      if( rc.msg != NULL ) 
	{ fprintf(stderr,"rc.msg: %s\n",CMGetCharPtr(rc.msg)); }
    }
    return rc;
  }

  CMReturnInstance( rslt, ci );
  CMReturnDone(rslt);
  CMReturn(CMPI_RC_OK);
}


CMPIStatus Linux_ComputerSystemCreateInstance( CMPIInstanceMI * mi, 
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

CMPIStatus Linux_ComputerSystemSetInstance( CMPIInstanceMI * mi, 
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

CMPIStatus Linux_ComputerSystemDeleteInstance( CMPIInstanceMI * mi, 
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

CMPIStatus Linux_ComputerSystemExecQuery( CMPIInstanceMI * mi, 
					  CMPIContext * ctx, 
					  CMPIResult * rslt,
					  CMPIObjectPath * cop, 
					  char * lang, 
					  char * query) {
  CMPIStatus rc = {CMPI_RC_OK, NULL};

  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI ExecQuery()\n", _ClassName ); 

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "NOT_SUPPORTED" ); 
  return rc;
}


/* ---------------------------------------------------------------------------*/
/*                        Method Provider Interface                           */
/* ---------------------------------------------------------------------------*/


CMPIStatus Linux_ComputerSystemMethodCleanup( CMPIMethodMI * mi, 
					      CMPIContext * ctx) {
  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI MethodCleanup()\n", _ClassName ); 
  CMReturn(CMPI_RC_OK);
}

CMPIStatus Linux_ComputerSystemInvokeMethod( CMPIMethodMI * mi,
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
    fprintf( stderr, "--- %s.c : CMPI InvokeMethod()\n", _ClassName ); 

  class = CMGetClassName(ref, &rc);

  if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 && 
      strcasecmp("SetPowerState",methodName) == 0 ) { 
    valrc.uint32 = 99;
    CMReturnData( rslt, &valrc, CMPI_uint32); 
  }
  else {
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_SUPPORTED, methodName ); 
  }

  return rc;
}

/* ---------------------------------------------------------------------------*/
/*                              private methods                               */
/* ---------------------------------------------------------------------------*/


/* ---------- method to create a CMPIObjectPath of this class ----------------*/

static CMPIObjectPath * _makePath( CMPIObjectPath * ref,
				   CMPIStatus * rc) {
  CMPIObjectPath * op = NULL;
 
  /* tool method call to get the unique name of the system */
  get_system_name(); 
  if( !CIM_HOST_NAME ) {   
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "no host name found" );
    goto exit;
  }

  op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(ref,rc)), 
			_ClassName, rc );
  if( CMIsNullObject(op) ) { 
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." ); 
    goto exit; 
  }

  CMAddKey(op, "CreationClassName", _ClassName, CMPI_chars);  
  CMAddKey(op, "Name", CIM_HOST_NAME, CMPI_chars);
    
 exit:
  return op;                
}

/* ----------- method to create a CMPIInstance of this class ----------------*/

static CMPIInstance * _makeInst( CMPIObjectPath * ref,
				 CMPIStatus * rc) {
  CMPIObjectPath * op    = NULL;
  CMPIInstance   * ci    = NULL;
  char * owner           = NULL;
  char * contact         = NULL;
#ifndef CIM26COMPAT
  CMPIArray      * dedic = NULL;
  int    status          = 2;       /* Enabled */
  int    dedicated       = 0;       /* Not Dedicated */
#endif

  /* tool method call to get the unique name of the system */
  get_system_name(); 
  if( !CIM_HOST_NAME ) {   
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "no host name found" );
    goto exit;
  }

  op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(ref,rc)), 
			_ClassName, rc );
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
    
  CMSetProperty( ci, "CreationClassName", _ClassName, CMPI_chars ); 
  CMSetProperty( ci, "Name", CIM_HOST_NAME, CMPI_chars );
  CMSetProperty( ci, "Status", "NULL", CMPI_chars);
  CMSetProperty( ci, "NameFormat", "IP", CMPI_chars);
  CMSetProperty( ci, "Caption", "Computer System", CMPI_chars);
  CMSetProperty( ci, "Description", "A class derived from ComputerSystem that represents the single node container of the Linux OS.", CMPI_chars);

  if( (owner=get_cs_primownername()) != NULL ) { 
      CMSetProperty( ci, "PrimaryOwnerName", owner, CMPI_chars); 
      free(owner);
  }
  if( (contact=get_cs_primownercontact()) != NULL) { 
    CMSetProperty( ci, "PrimaryOwnerContact", contact, CMPI_chars); 
    free(contact);
  }
  
  /* 2.7 */ 
#ifndef CIM26COMPAT
  CMSetProperty( ci, "ElementName", CIM_HOST_NAME, CMPI_chars);
  CMSetProperty( ci, "EnabledStatus", (CMPIValue*)&(status), CMPI_uint16);
  CMSetProperty( ci, "OtherEnabledStatus", "NULL", CMPI_chars);
  CMSetProperty( ci, "RequestedStatus", (CMPIValue*)&(status), CMPI_uint16);
  CMSetProperty( ci, "EnabledDefault", (CMPIValue*)&(status), CMPI_uint16);

  dedic = CMNewArray(_broker,1,CMPI_uint16A,rc);
  CMSetArrayElementAt(dedic,0,(CMPIValue*)&(dedicated),CMPI_uint16);
  CMSetProperty( ci, "Dedicated", (CMPIValue*)&(dedic), CMPI_uint16A);
#endif

 exit:
  return ci;
}


/* ---------------------------------------------------------------------------*/
/*                              Provider Factory                              */
/* ---------------------------------------------------------------------------*/

CMInstanceMIStub( Linux_ComputerSystem,
		  Linux_ComputerSystem,
		  _broker,
		  CMNoHook);

CMMethodMIStub( Linux_ComputerSystem,
		Linux_ComputerSystem,
		_broker,
		CMNoHook);

/* ---------------------------------------------------------------------------*/
/*                        end of Linux_ComputerSystem                         */
/* ---------------------------------------------------------------------------*/

