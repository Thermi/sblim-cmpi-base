/*
 * cmpiOSBase_ComputerSystemProvider.c
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

#include "cmpidt.h"
#include "cmpift.h"
#include "cmpimacs.h"

#include "OSBase_Common.h"
#include "cmpiOSBase_Common.h"
#include "cmpiOSBase_ComputerSystem.h"

unsigned char CMPI_true  = 1;
unsigned char CMPI_false = 0;

CMPIBroker * _broker;

/* ---------------------------------------------------------------------------*/
/* private declarations                                                       */

#ifdef DEBUG
    int _debug = 1;
#else
    int _debug = 0;
#endif

static char * _FILENAME = "cmpiOSBase_ComputerSystemProvider.c";

/* ---------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------*/
/*                      Instance Provider Interface                           */
/* ---------------------------------------------------------------------------*/


CMPIStatus OSBase_ComputerSystemProviderCleanup( CMPIInstanceMI * mi, 
           CMPIContext * ctx) { 
  if( _debug )
    fprintf( stderr, "--- %s : %s CMPI Cleanup()\n", _FILENAME, _ClassName );
  CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_ComputerSystemProviderEnumInstanceNames( CMPIInstanceMI * mi, 
           CMPIContext * ctx, 
           CMPIResult * rslt, 
           CMPIObjectPath * ref) { 
  CMPIObjectPath * op = NULL;
  CMPIStatus       rc = {CMPI_RC_OK, NULL};
  
  if( _debug )
    fprintf( stderr, "--- %s : %s CMPI EnumInstanceNames()\n", _FILENAME, _ClassName );

  op = _makePath( _broker, ctx, ref, &rc );

  if( op == NULL ) {
    if( _debug ) {
      if( rc.msg != NULL )
	{ fprintf(stderr,"rc.msg: %s\n",CMGetCharPtr(rc.msg)); }
    }
    return rc;
  }

  CMReturnObjectPath( rslt, op );
  CMReturnDone( rslt );
  return rc;
}

CMPIStatus OSBase_ComputerSystemProviderEnumInstances( CMPIInstanceMI * mi, 
           CMPIContext * ctx, 
           CMPIResult * rslt, 
           CMPIObjectPath * ref, 
           char ** properties) { 
  CMPIInstance * ci = NULL;
  CMPIStatus     rc = {CMPI_RC_OK, NULL};

  if( _debug )
    fprintf( stderr, "--- %s : %s CMPI EnumInstances()\n", _FILENAME, _ClassName );

  ci = _makeInst( _broker, ctx, ref, &rc );

  if( ci == NULL ) {
    if( _debug ) {
      if( rc.msg != NULL )
	{ fprintf(stderr,"rc.msg: %s\n",CMGetCharPtr(rc.msg)); }
    }
    return rc;
  }

  CMReturnInstance( rslt, ci );
  CMReturnDone( rslt );
  return rc;
}

CMPIStatus OSBase_ComputerSystemProviderGetInstance( CMPIInstanceMI * mi, 
           CMPIContext * ctx, 
           CMPIResult * rslt, 
           CMPIObjectPath * cop, 
           char ** properties) {
  CMPIInstance * ci = NULL;
  CMPIStatus     rc = {CMPI_RC_OK, NULL};

  if( _debug )
    fprintf( stderr, "--- %s : %s CMPI GetInstance()\n", _FILENAME, _ClassName );

  _check_system_key_value_pairs( _broker, cop, "CreationClassName", "Name", &rc );
  if( rc.rc != CMPI_RC_OK ) { return rc; }

  ci = _makeInst( _broker, ctx, cop, &rc );

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

CMPIStatus OSBase_ComputerSystemProviderCreateInstance( CMPIInstanceMI * mi, 
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

CMPIStatus OSBase_ComputerSystemProviderSetInstance( CMPIInstanceMI * mi, 
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

CMPIStatus OSBase_ComputerSystemProviderDeleteInstance( CMPIInstanceMI * mi, 
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

CMPIStatus OSBase_ComputerSystemProviderExecQuery( CMPIInstanceMI * mi, 
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


CMPIStatus OSBase_ComputerSystemProviderMethodCleanup( CMPIMethodMI * mi, 
           CMPIContext * ctx) {
  if( _debug )
    fprintf( stderr, "--- %s : %s CMPI MethodCleanup()\n", _FILENAME, _ClassName );
  CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_ComputerSystemProviderInvokeMethod( CMPIMethodMI * mi,
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
  else {
    CMSetStatusWithChars( _broker, &rc,
			  CMPI_RC_ERR_NOT_FOUND, methodName );
  }
 
  return rc;
}

/* ---------------------------------------------------------------------------*/
/*                              Provider Factory                              */
/* ---------------------------------------------------------------------------*/

CMInstanceMIStub( OSBase_ComputerSystemProvider, 
                  OSBase_ComputerSystemProvider, 
                  _broker, 
                  CMNoHook);

CMMethodMIStub( OSBase_ComputerSystemProvider, 
		OSBase_ComputerSystemProvider, 
                _broker, 
                CMNoHook);


/* ---------------------------------------------------------------------------*/
/*              end of cmpiOSBase_ComputerSystemProvider                      */
/* ---------------------------------------------------------------------------*/

