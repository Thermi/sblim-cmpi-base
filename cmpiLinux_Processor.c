/*
 * Linux_Processor.c
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
 * Interface Type : Common Manageability Programming Interface ( CMPI )
 *
 * Description: 
 * 
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "cmpidt.h"
#include "cmpift.h"
#include "cmpimacs.h"

#include "mlogsup.h"
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

static char * _ClassName = "Linux_Processor";

/* private methods                                                            */
/*                                                                            */

/* - method to create a CMPIObjectPath of this class                          */
static CMPIObjectPath * _makePath( CMPIObjectPath * cop, 
				  struct cim_processor * sptr,
				  CMPIStatus * rc);

/* - method to create a CMPIInstance of this class                            */
static CMPIInstance * _makeInst( CMPIObjectPath * cop, 
				struct cim_processor * sptr,
				CMPIStatus * rc);
/* ---------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------*/
/*                      Instance Provider Interface                           */
/* ---------------------------------------------------------------------------*/


CMPIStatus Linux_ProcessorCleanup( CMPIInstanceMI * mi, 
				   CMPIContext * ctx) {
  CMReturn(CMPI_RC_OK);
}

CMPIStatus Linux_ProcessorEnumInstanceNames( CMPIInstanceMI * mi, 
					     CMPIContext * ctx, 
					     CMPIResult * rslt, 
					     CMPIObjectPath * ref) {
  CMPIObjectPath       * op    = NULL;  
  CMPIStatus             rc    = {CMPI_RC_OK, NULL};
  struct processorlist * lptr  = NULL ;
  struct processorlist * rm    = NULL ;
  int                    cmdrc = 0;
  
  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI EnumInstanceNames()\n", _ClassName );
  
  cmdrc = enum_all_processor( &lptr );
  if( cmdrc != 0 ) {
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_FAILED, "Could not list processors." ); 
    return rc;
  }
  
  rm = lptr;
  // iterate process list
  if( lptr->sptr ) {
    for ( ; lptr && rc.rc == CMPI_RC_OK ; lptr = lptr->next) {
      // method call to create the CIMInstance object
      op = _makePath( ref,lptr->sptr, &rc );
      if( op == NULL || rc.rc != CMPI_RC_OK ) { 
	if( _debug ) {
	  if( rc.msg != NULL ) 
	    { fprintf(stderr,"rc.msg: %s\n",CMGetCharPtr(rc.msg)); }
	}
	CMSetStatusWithChars( _broker, &rc,
			      CMPI_RC_ERR_FAILED, "Transformation from internal structure to CIM ObjectPath failed." ); 
	if(rm) free_processorlist(rm);
	return rc; 
      }
      else { CMReturnObjectPath( rslt, op ); }
    }
    if(rm) free_processorlist(rm);
  }
  
  CMReturnDone( rslt );
  CMReturn( CMPI_RC_OK );
}

CMPIStatus Linux_ProcessorEnumInstances( CMPIInstanceMI * mi, 
					 CMPIContext * ctx, 
					 CMPIResult * rslt, 
					 CMPIObjectPath * ref, 
					 char ** properties) {
  CMPIInstance         * ci    = NULL;  
  CMPIStatus             rc    = {CMPI_RC_OK, NULL};
  struct processorlist * lptr  = NULL ;
  struct processorlist * rm    = NULL ;
  int                    cmdrc = 0;

  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI EnumInstances()\n", _ClassName ); 
  
  cmdrc = enum_all_processor( &lptr );
  if( cmdrc != 0 ) {
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_FAILED, "Could not list processors." ); 
    return rc;
  }

  rm = lptr;
  // iterate process list
  if( lptr->sptr != NULL ) {
    for ( ; lptr && rc.rc == CMPI_RC_OK ; lptr = lptr->next) {
      // method call to create the CIMInstance object
      ci = _makeInst( ref, lptr->sptr, &rc );
      if( ci == NULL || rc.rc != CMPI_RC_OK ) { 
	if( _debug ) {
	  if( rc.msg != NULL ) 
	    { fprintf(stderr,"rc.msg: %s\n",CMGetCharPtr(rc.msg)); }
	}
	CMSetStatusWithChars( _broker, &rc,
			      CMPI_RC_ERR_FAILED, "Transformation from internal structure to CIM Instance failed." ); 
	if(rm) free_processorlist(rm);
	return rc; 
      }
      else { CMReturnInstance( rslt, ci ); }
    }
    if(rm) free_processorlist(rm);
  }

  CMReturnDone( rslt );
  CMReturn( CMPI_RC_OK );
}


CMPIStatus Linux_ProcessorGetInstance( CMPIInstanceMI * mi, 
				       CMPIContext * ctx, 
				       CMPIResult * rslt, 
				       CMPIObjectPath * cop, 
				       char ** properties) {
  
  CMPIInstance         * ci    = NULL;
  CMPIString           * name  = NULL;
  struct cim_processor * sptr  = NULL;
  CMPIStatus             rc    = {CMPI_RC_OK, NULL};
  int                    cmdrc = 0;

  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI GetInstance()\n", _ClassName ); 

  name = CMGetKey( cop, "DeviceID", &rc).value.string;
  if( CMGetCharPtr(name) == NULL ) {    
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_FAILED, "Could not get Processor ID." ); 
    return rc;
  }

  cmdrc = get_processor_data( CMGetCharPtr(name) , &sptr );
  if( cmdrc != 0 || sptr == NULL ) {    
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_FOUND, "Processor ID does not exist." ); 
    return rc;
  }

  ci = _makeInst( cop, sptr, &rc );
  if(sptr) free_processor(sptr);

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

CMPIStatus Linux_ProcessorCreateInstance( CMPIInstanceMI * mi, 
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

CMPIStatus Linux_ProcessorSetInstance( CMPIInstanceMI * mi, 
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

CMPIStatus Linux_ProcessorDeleteInstance( CMPIInstanceMI * mi, 
					  CMPIContext * ctx, 
					  CMPIResult * rslt, 
					  CMPIObjectPath * cop) {  
  CMPIStatus rc = {CMPI_RC_OK, NULL};
  
  if(_debug)
    fprintf(stderr,"--- %s.c : CMPI DeleteInstance()\n", _ClassName); 

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "NOT_SUPPORTED" ); 
  return rc;
}

CMPIStatus Linux_ProcessorExecQuery( CMPIInstanceMI * mi, 
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


CMPIStatus Linux_ProcessorMethodCleanup( CMPIMethodMI * mi, 
					 CMPIContext * ctx) {
  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI MethodCleanup()\n", _ClassName ); 
  CMReturn(CMPI_RC_OK);
}

CMPIStatus Linux_ProcessorInvokeMethod( CMPIMethodMI * mi,
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
  else if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 && 
	   strcasecmp("Reset",methodName) == 0 ) { 
    valrc.uint32 = 99;
    CMReturnData( rslt, &valrc, CMPI_uint32); 
  }
  else if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 && 
	   strcasecmp("EnableDevice",methodName) == 0 ) { 
    valrc.uint32 = 99;
    CMReturnData( rslt, &valrc, CMPI_uint32); 
  }
  else if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 && 
	   strcasecmp("OnlineDevice",methodName) == 0 ) { 
    valrc.uint32 = 99;
    CMReturnData( rslt, &valrc, CMPI_uint32); 
  }
  else if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 && 
	   strcasecmp("QuiesceDevice",methodName) == 0 ) { 
    valrc.uint32 = 99;
    CMReturnData( rslt, &valrc, CMPI_uint32); 
  }
  else if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 && 
	   strcasecmp("SaveProperties",methodName) == 0 ) { 
    valrc.uint32 = 99;
    CMReturnData( rslt, &valrc, CMPI_uint32); 
  }
  else if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 && 
	   strcasecmp("RestoreProperties",methodName) == 0 ) { 
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
				   struct cim_processor * sptr,
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

  CMAddKey(op, "SystemCreationClassName", CSCreationClassName, CMPI_chars);  
  CMAddKey(op, "SystemName", CIM_HOST_NAME, CMPI_chars);
  CMAddKey(op, "CreationClassName", _ClassName, CMPI_chars);  
  CMAddKey(op, "DeviceID", sptr->id, CMPI_chars);
  
 exit:
  return op; 
}


/* ----------- method to create a CMPIInstance of this class ----------------*/

static CMPIInstance * _makeInst( CMPIObjectPath * ref,
				 struct cim_processor * sptr,
				 CMPIStatus * rc) {
  CMPIObjectPath * op     = NULL;
  CMPIInstance   * ci     = NULL;
#ifndef CIM26COMPAT
  int              status = 2; /* Enabled */
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
  
  CMSetProperty( ci, "SystemCreationClassName",CSCreationClassName , CMPI_chars ); 
  CMSetProperty( ci, "SystemName", CIM_HOST_NAME, CMPI_chars );
  CMSetProperty( ci, "CreationClassName", _ClassName, CMPI_chars ); 
  CMSetProperty( ci, "DeviceID", sptr->id, CMPI_chars );

  CMSetProperty( ci, "Caption", "Linux Processor", CMPI_chars);
  CMSetProperty( ci, "Description", "This class represents instances of available processors.", CMPI_chars);
  CMSetProperty( ci, "Status", "NULL", CMPI_chars);

  CMSetProperty( ci, "Role", "Central Processor", CMPI_chars);  
  CMSetProperty( ci, "CPUStatus", (CMPIValue*)&(sptr->stat), CMPI_uint16);
  CMSetProperty( ci, "LoadPercentage", (CMPIValue*)&(sptr->loadPct), CMPI_uint16);
  CMSetProperty( ci, "Stepping", sptr->step, CMPI_chars);
  CMSetProperty( ci, "Family", (CMPIValue*)&(sptr->family), CMPI_uint16);
  CMSetProperty( ci, "OtherFamilyDescription", "NULL", CMPI_chars);

  CMSetProperty( ci, "MaxClockSpeed", (CMPIValue*)&(sptr->maxClockSpeed), CMPI_uint32);
  CMSetProperty( ci, "CurrentClockSpeed", (CMPIValue*)&(sptr->curClockSpeed), CMPI_uint32);

  CMSetProperty( ci, "Name", sptr->id, CMPI_chars );

  /* 2.7 */
#ifndef CIM26COMPAT
  CMSetProperty( ci, "ElementName", sptr->name, CMPI_chars);  
  CMSetProperty( ci, "EnabledStatus", (CMPIValue*)&(status), CMPI_uint16);
  CMSetProperty( ci, "OtherEnabledStatus", "NULL", CMPI_chars);
  CMSetProperty( ci, "RequestedStatus", (CMPIValue*)&(status), CMPI_uint16);
  CMSetProperty( ci, "EnabledDefault", (CMPIValue*)&(status), CMPI_uint16);
#endif
    
 exit:
  return ci;
}


/* ---------------------------------------------------------------------------*/
/*                              Provider Factory                              */
/* ---------------------------------------------------------------------------*/

CMInstanceMIStub( Linux_Processor,
		  Linux_Processor,
		  _broker,
		  CMNoHook);

CMMethodMIStub( Linux_Processor,
		Linux_Processor,
		_broker,
		CMNoHook);

/* ---------------------------------------------------------------------------*/
/*                        end of Linux_Processor                              */
/* ---------------------------------------------------------------------------*/

