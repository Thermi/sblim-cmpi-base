/*
 * cmpiOSBase_ProcessorProvider.c
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

#include "OSBase_Common.h"
#include "cmpiOSBase_Common.h"
#include "cmpiOSBase_Processor.h"

static CMPIBroker * _broker;

/* ---------------------------------------------------------------------------*/
/* private declarations                                                       */


static char * _FILENAME = "cmpiOSBase_ProcessorProvider.c";

/* ---------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------*/
/*                      Instance Provider Interface                           */
/* ---------------------------------------------------------------------------*/


CMPIStatus OSBase_ProcessorProviderCleanup( CMPIInstanceMI * mi, 
           CMPIContext * ctx) { 
  if( _debug )
    fprintf( stderr, "--- %s : %s CMPI Cleanup()\n", _FILENAME, _ClassName );
  CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_ProcessorProviderEnumInstanceNames( CMPIInstanceMI * mi, 
           CMPIContext * ctx, 
           CMPIResult * rslt, 
           CMPIObjectPath * ref) { 
  CMPIObjectPath       * op    = NULL;  
  CMPIStatus             rc    = {CMPI_RC_OK, NULL};
  struct processorlist * lptr  = NULL ;
  struct processorlist * rm    = NULL ;
  int                    cmdrc = 0;
  
  if( _debug )
    fprintf( stderr, "--- %s : %s CMPI EnumInstanceNames()\n", _FILENAME, _ClassName );
  
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
      op = _makePath_Processor( _broker, ctx, ref,lptr->sptr, &rc );
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
  return rc;
}

CMPIStatus OSBase_ProcessorProviderEnumInstances( CMPIInstanceMI * mi, 
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
    fprintf( stderr, "--- %s : %s CMPI EnumInstances()\n", _FILENAME, _ClassName );
  
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
      ci = _makeInst_Processor( _broker, ctx, ref, lptr->sptr, &rc );
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
  return rc;
}

CMPIStatus OSBase_ProcessorProviderGetInstance( CMPIInstanceMI * mi, 
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
    fprintf( stderr, "--- %s : %s CMPI GetInstance()\n", _FILENAME, _ClassName );

  _check_system_key_value_pairs( _broker, cop, "SystemCreationClassName", "SystemName", &rc );
  if( rc.rc != CMPI_RC_OK ) { return rc; }

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

  ci = _makeInst_Processor( _broker, ctx, cop, sptr, &rc );
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
  return rc;
}

CMPIStatus OSBase_ProcessorProviderCreateInstance( CMPIInstanceMI * mi, 
           CMPIContext * ctx, 
           CMPIResult * rslt, 
           CMPIObjectPath * cop, 
           CMPIInstance * ci) {
  CMPIStatus rc = {CMPI_RC_OK, NULL};

  if( _debug )
    fprintf( stderr, "--- %s : %s CMPI CreateInstance()\n", _FILENAME, _ClassName );

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED" ); 
  return rc;
}

CMPIStatus OSBase_ProcessorProviderSetInstance( CMPIInstanceMI * mi, 
           CMPIContext * ctx, 
           CMPIResult * rslt, 
           CMPIObjectPath * cop,
           CMPIInstance * ci, 
           char **properties) {
  CMPIStatus rc = {CMPI_RC_OK, NULL};

  if( _debug )
    fprintf( stderr, "--- %s : %s CMPI SetInstance()\n", _FILENAME, _ClassName );

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED" ); 
  return rc;
}

CMPIStatus OSBase_ProcessorProviderDeleteInstance( CMPIInstanceMI * mi, 
           CMPIContext * ctx, 
           CMPIResult * rslt, 
           CMPIObjectPath * cop) {
  CMPIStatus rc = {CMPI_RC_OK, NULL}; 

  if( _debug )
    fprintf( stderr, "--- %s : %s CMPI DeleteInstance()\n", _FILENAME, _ClassName );

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED" ); 
  return rc;
}

CMPIStatus OSBase_ProcessorProviderExecQuery( CMPIInstanceMI * mi, 
           CMPIContext * ctx, 
           CMPIResult * rslt, 
           CMPIObjectPath * ref, 
           char * lang, 
           char * query) {
  CMPIStatus rc = {CMPI_RC_OK, NULL};

  if( _debug )
    fprintf( stderr, "--- %s : %s CMPI ExecQuery()\n", _FILENAME, _ClassName );

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED" ); 
  return rc;
}


/* ---------------------------------------------------------------------------*/
/*                        Method Provider Interface                           */
/* ---------------------------------------------------------------------------*/


CMPIStatus OSBase_ProcessorProviderMethodCleanup( CMPIMethodMI * mi, 
           CMPIContext * ctx) {
  if( _debug )
    fprintf( stderr, "--- %s : %s CMPI MethodCleanup()\n", _FILENAME, _ClassName );
  CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_ProcessorProviderInvokeMethod( CMPIMethodMI * mi,
           CMPIContext * ctx,
           CMPIResult * rslt,
           CMPIObjectPath * ref,
           char * methodName,
           CMPIArgs * in,
           CMPIArgs * out) {
  CMPIString * class = NULL; 
  CMPIStatus   rc    = {CMPI_RC_OK, NULL};

  if( _debug )
    fprintf( stderr, "--- %s : %s CMPI InvokeMethod()\n", _FILENAME, _ClassName );

  class = CMGetClassName(ref, &rc);

  if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 && 
      strcasecmp("SetPowerState",methodName) == 0 ) {   
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_SUPPORTED, methodName ); 
  }
  else if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 && 
	   strcasecmp("Reset",methodName) == 0 ) {   
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_SUPPORTED, methodName ); 
  }
  else if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 && 
	   strcasecmp("EnableDevice",methodName) == 0 ) {   
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_SUPPORTED, methodName ); 
  }
  else if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 && 
	   strcasecmp("OnlineDevice",methodName) == 0 ) {   
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_SUPPORTED, methodName ); 
  }
  else if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 && 
	   strcasecmp("QuiesceDevice",methodName) == 0 ) {   
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_SUPPORTED, methodName ); 
  }
  else if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 && 
	   strcasecmp("SaveProperties",methodName) == 0 ) {   
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_SUPPORTED, methodName ); 
  }
  else if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 && 
	   strcasecmp("RestoreProperties",methodName) == 0 ) {  
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_SUPPORTED, methodName ); 
  }
  else {
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_FOUND, methodName ); 
  }
 
  return rc;
}


/* ---------------------------------------------------------------------------*/
/*                              Provider Factory                              */
/* ---------------------------------------------------------------------------*/

CMInstanceMIStub( OSBase_ProcessorProvider, 
                  OSBase_ProcessorProvider, 
                  _broker, 
                  CMNoHook);

CMMethodMIStub( OSBase_ProcessorProvider,
                OSBase_ProcessorProvider, 
                _broker, 
                CMNoHook);


/* ---------------------------------------------------------------------------*/
/*          end of cmpiOSBase_ProcessorProvider                      */
/* ---------------------------------------------------------------------------*/

