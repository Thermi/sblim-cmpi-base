/*
 * cmpiOSBase_UnixProcessProvider.c
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
 * Contributors: Viktor Mihajlovski <mihajlov@de.ibm.com>
 *               Eric C. Wu <cwu@us.ibm.com>
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

#include "OSBase_Common.h"
#include "cmpiOSBase_Common.h"
#include "cmpiOSBase_UnixProcess.h"

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

static char * _FILENAME = "cmpiOSBase_UnixProcessProvider.c";

/* ---------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------*/
/*                      Instance Provider Interface                           */
/* ---------------------------------------------------------------------------*/


CMPIStatus OSBase_UnixProcessProviderCleanup( CMPIInstanceMI * mi, 
           CMPIContext * ctx) { 
  if( _debug )
    fprintf( stderr, "--- %s : %s CMPI Cleanup()\n", _FILENAME, _ClassName );
  CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_UnixProcessProviderEnumInstanceNames( CMPIInstanceMI * mi, 
           CMPIContext * ctx, 
           CMPIResult * rslt, 
           CMPIObjectPath * ref) { 
  CMPIObjectPath     * op    = NULL;  
  CMPIStatus           rc    = {CMPI_RC_OK, NULL};
  struct processlist * lptr  = NULL;
  struct processlist * rm    = NULL;
  int                  cmdrc = 0;
  
  if( _debug )
    fprintf( stderr, "--- %s : %s CMPI EnumInstanceNames()\n", _FILENAME, _ClassName );

  cmdrc = enum_all_process( &lptr );
  if( cmdrc != 0 ) {
    CMSetStatusWithChars( _broker, &rc,
			  CMPI_RC_ERR_FAILED, "Could not list active processes." ); 
    return rc;
  }
  
  rm = lptr;
  // iterate process list
  if( lptr ) {
    for ( ; lptr && rc.rc == CMPI_RC_OK ; lptr = lptr->next) {
      // method call to create the CMPIInstance object
      op = _makePath( _broker, ctx, ref, lptr->p, &rc );
      if( op == NULL || rc.rc != CMPI_RC_OK ) { 
	if( _debug ) {
	  if( rc.msg != NULL ) 
	    { fprintf(stderr,"rc.msg: %s\n",CMGetCharPtr(rc.msg)); }
	}
	CMSetStatusWithChars( _broker, &rc,
			      CMPI_RC_ERR_FAILED, "Transformation from internal structure to CIM ObjectPath failed." ); 
	if(rm) free_processlist(rm);
	return rc; 
      }
      else { CMReturnObjectPath( rslt, op ); }
    }  
    if(rm) free_processlist(rm);
  }
  
  CMReturnDone( rslt );
  return rc;
}

CMPIStatus OSBase_UnixProcessProviderEnumInstances( CMPIInstanceMI * mi, 
           CMPIContext * ctx, 
           CMPIResult * rslt, 
           CMPIObjectPath * ref, 
           char ** properties) { 
  CMPIInstance       * ci    = NULL;
  CMPIStatus           rc    = {CMPI_RC_OK, NULL};
  struct processlist * lptr  = NULL;
  struct processlist * rm    = NULL;
  int                  cmdrc = 0;

  if( _debug )
    fprintf( stderr, "--- %s : %s CMPI EnumInstances()\n", _FILENAME, _ClassName );
  
  cmdrc = enum_all_process( &lptr );
  if( cmdrc != 0 ) {
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_FAILED, "Could not list active processes." ); 
    return rc;
  }

  rm = lptr;
  // iterate process list
  if( lptr ) {
    for ( ; lptr && rc.rc == CMPI_RC_OK ; lptr = lptr->next) {
      // method call to create the CMPIInstance object
      ci = _makeInst( _broker, ctx, ref, lptr->p, &rc );
      if( ci == NULL || rc.rc != CMPI_RC_OK ) { 
	if( _debug ) {
	  if( rc.msg != NULL ) 
	    { fprintf(stderr,"rc.msg: %s\n",CMGetCharPtr(rc.msg)); }
	}
	CMSetStatusWithChars( _broker, &rc,
			      CMPI_RC_ERR_FAILED, "Transformation from internal structure to CIM Instance failed." ); 
	if(rm) free_processlist(rm);
	return rc; 
      }
      else { CMReturnInstance( rslt, ci ); }
    }  
    if(rm) free_processlist(rm);
  }

  CMReturnDone( rslt );
  return rc;
}

CMPIStatus OSBase_UnixProcessProviderGetInstance( CMPIInstanceMI * mi, 
           CMPIContext * ctx, 
           CMPIResult * rslt, 
           CMPIObjectPath * cop, 
           char ** properties) {  
  CMPIInstance       * ci    = NULL;
  CMPIStatus           rc    = {CMPI_RC_OK, NULL};
  struct cim_process * sptr  = NULL;
  CMPIString         * name  = NULL;
  int                  cmdrc = 0;

  if( _debug )
    fprintf( stderr, "--- %s : %s CMPI GetInstance()\n", _FILENAME, _ClassName );

  _check_system_key_value_pairs( _broker, cop, "CSCreationClassName", "CSName", &rc );
  if( rc.rc != CMPI_RC_OK ) { return rc; }
  _check_system_key_value_pairs( _broker, cop, "OSCreationClassName", "OSName", &rc );
  if( rc.rc != CMPI_RC_OK ) { return rc; }

  name = CMGetKey( cop, "Handle", &rc).value.string;
  if( name == NULL ) {    
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_FAILED, "Could not get Process ID." ); 
    return rc;
  }

  cmdrc = get_process_data( CMGetCharPtr(name) , &sptr );
  if( cmdrc != 0 || sptr == NULL ) {    
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_FOUND, "Process ID does not exist." ); 
    return rc;
  }

  ci = _makeInst( _broker, ctx, cop, sptr, &rc );
  if(sptr) free_process(sptr);

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

CMPIStatus OSBase_UnixProcessProviderCreateInstance( CMPIInstanceMI * mi, 
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

CMPIStatus OSBase_UnixProcessProviderSetInstance( CMPIInstanceMI * mi, 
           CMPIContext * ctx, 
           CMPIResult * rslt, 
           CMPIObjectPath * cop,
           CMPIInstance * ci, 
           char **properties) {
  CMPIStatus           rc    = {CMPI_RC_OK, NULL};
#ifdef MODULE_SYSMAN
  struct cim_process * sptr  = NULL;
  CMPIString         * pid   = NULL;
  CMPIData             data;
  struct _SysMan       sm;
  int                  cmdrc = 0;
#endif

  if( _debug )
    fprintf( stderr, "--- %s : %s CMPI SetInstance()\n", _FILENAME, _ClassName );

#ifdef MODULE_SYSMAN
 
  if (!moduleInstalled)
    goto exit;
  
  pid = CMGetKey( cop, "Handle", &rc).value.string;
  if( pid == NULL ) {    
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_FAILED, "Could not get ProcessID." );
    return rc;
  }

  /* get data of new process instance */
  cmdrc = get_process_data( CMGetCharPtr(pid) , &sptr );
  if( sptr == NULL ) {    
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_FOUND, "Process ID does not exist." );
    return rc;
  }

  ci = _makeInst( _broker, ctx, cop, sptr, &rc );
  if(sptr) free_process(sptr);

  if( ci == NULL ) { 
    if( _debug ) {
      if( rc.msg != NULL ) 
	{ fprintf(stderr,"rc.msg: %s\n",CMGetCharPtr(rc.msg)); }
    }
    CMReturn(CMPI_RC_ERR_FAILED);
  }

  /* get data of old process instance */
  cmdrc = _sysman_data( CMGetCharPtr(pid) , &sm );
  if( cmdrc != 0 ) goto exit;

  /* set new values for process */

  data = CMGetProperty( ci, "MaxNumberOfOpenFiles", &rc);
  if ( data.value.uint32 != sm.maxChildProc ) {
    sprintf(pathBuffer, "%s\t-1\t%ld", CMGetCharPtr(pid), data.value.uint32 );
    set_system_parameter(dirPath, fileName, pathBuffer);
  }

  data = CMGetProperty( ci, "MaxNumberOfChildProcesses", &rc);
  if ( data.value.uint32 != sm.maxChildProc ) {
    sprintf(pathBuffer, "%s\t-1\t%ld", CMGetCharPtr(pid), data.value.uint32 );
    set_system_parameter(dirPath, fileName, pathBuffer);
  }

  data = CMGetProperty( ci, "MaxRealStack", &rc);
  if ( data.value.uint32 != sm.maxChildProc ) {
    sprintf(pathBuffer, "%s\t-1\t%ld", CMGetCharPtr(pid), data.value.uint32 );
    set_system_parameter(dirPath, fileName, pathBuffer);
  }

 exit:
#else

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED" ); 
#endif

  return rc;
}

CMPIStatus OSBase_UnixProcessProviderDeleteInstance( CMPIInstanceMI * mi, 
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

CMPIStatus OSBase_UnixProcessProviderExecQuery( CMPIInstanceMI * mi, 
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


CMPIStatus OSBase_UnixProcessProviderMethodCleanup( CMPIMethodMI * mi, 
           CMPIContext * ctx) {
  if( _debug )
    fprintf( stderr, "--- %s : %s CMPI MethodCleanup()\n", _FILENAME, _ClassName );
  CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_UnixProcessProviderInvokeMethod( CMPIMethodMI * mi,
           CMPIContext * ctx,
           CMPIResult * rslt,
           CMPIObjectPath * ref,
           char * methodName,
           CMPIArgs * in,
           CMPIArgs * out) {
  CMPIData         pid;
  CMPIValue        valrc;
  CMPIString     * class = NULL;
  CMPIStatus       rc    = {CMPI_RC_OK, NULL};
  char           * cmd   = NULL;
  char          ** hdout = NULL;
  char          ** hderr = NULL;
  int              cmdrc = 0;

  if( _debug )
    fprintf( stderr, "--- %s : %s CMPI InvokeMethod()\n", _FILENAME, _ClassName );
  
  class = CMGetClassName(ref, &rc);

  /* "terminate" */
  if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 && 
      strcasecmp(methodName, "terminate") == 0 ) {

    valrc.uint8 = 0;
    pid = CMGetKey( ref, "Handle", &rc);

    cmd = (char*)malloc( (strlen(CMGetCharPtr(pid.value.string))+9)*sizeof(char) );
    strcpy(cmd, "kill -9 ");
    strcat(cmd, CMGetCharPtr(pid.value.string));

    cmdrc = runcommand( cmd, NULL, &hdout, &hderr );

    /* something went wrong */
    if( cmdrc != 0 ||  hderr[0] != NULL ) {
      /* command execution failed */
      valrc.uint8 = 1;
      if( hderr != NULL )
	{ fprintf(stderr,"--- %s.c : hderr[0] : %s\n", _ClassName, hderr[0]); }
    }

    freeresultbuf(hdout);
    freeresultbuf(hderr); 
    CMReturnData( rslt, &valrc, CMPI_uint8);
    CMReturnDone( rslt );
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

CMInstanceMIStub( OSBase_UnixProcessProvider, 
                  OSBase_UnixProcessProvider, 
                  _broker, 
                  CMNoHook);

CMMethodMIStub( OSBase_UnixProcessProvider,
                OSBase_UnixProcessProvider, 
                _broker, 
                CMNoHook);


/* ---------------------------------------------------------------------------*/
/*          end of cmpiOSBase_UnixProcessProvider                      */
/* ---------------------------------------------------------------------------*/

