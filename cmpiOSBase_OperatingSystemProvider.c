/*
 * cmpiOSBase_OperatingSystemProvider.c
 *
 * (C) Copyright IBM Corp. 2002
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
#include "cmpiOSBase_OperatingSystem.h"

static CMPIBroker * _broker;

/* ---------------------------------------------------------------------------*/
/* private declarations                                                       */


/* ---------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------*/
/* --- CPU utilization with event support                                     */
/* ---------------------------------------------------------------------------*/

#ifndef NOEVENTS

#include <pthread.h>
#include <unistd.h>

#define histlen 5
static int histindex=0;
static int histcpu[histlen];

static pthread_t tid;
static int threadActive=0;
static int threadCancelPending=0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


#endif

/* ---------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------*/
/*                      Instance Provider Interface                           */
/* ---------------------------------------------------------------------------*/


CMPIStatus OSBase_OperatingSystemProviderCleanup( CMPIInstanceMI * mi, 
           CMPIContext * ctx) { 
  _OSBASE_TRACE(1,("--- %s CMPI Cleanup() called",_ClassName));

#ifndef NOEVENTS
  pthread_mutex_destroy(&mutex);
#endif

  _OSBASE_TRACE(1,("--- %s CMPI Cleanup() exited",_ClassName));
  CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_OperatingSystemProviderEnumInstanceNames( CMPIInstanceMI * mi, 
           CMPIContext * ctx, 
           CMPIResult * rslt, 
           CMPIObjectPath * ref) { 
  CMPIObjectPath * op = NULL;
  CMPIStatus       rc = {CMPI_RC_OK, NULL};
  
  _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() called",_ClassName));

  op = _makePath_OperatingSystem( _broker, ctx, ref, &rc );

  if( op == NULL ) {
    if( rc.msg != NULL )
      { _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() failed : %s",_ClassName,CMGetCharPtr(rc.msg))); }
    else
      { _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() failed",_ClassName)); }
    return rc;
  }

  CMReturnObjectPath( rslt, op );
  CMReturnDone( rslt );

  _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() exited",_ClassName));
  return rc;
}

CMPIStatus OSBase_OperatingSystemProviderEnumInstances( CMPIInstanceMI * mi, 
           CMPIContext * ctx, 
           CMPIResult * rslt, 
           CMPIObjectPath * ref, 
           char ** properties) { 
  CMPIInstance * ci = NULL;
  CMPIStatus     rc = {CMPI_RC_OK, NULL};

  _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() called",_ClassName));

  ci = _makeInst_OperatingSystem( _broker, ctx, ref, &rc );

  if( ci == NULL ) {
    if( rc.msg != NULL )
      { _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() failed : %s",_ClassName,CMGetCharPtr(rc.msg))); }
    else
      { _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() failed",_ClassName)); }
    return rc;
  }

  CMReturnInstance( rslt, ci );
  CMReturnDone( rslt );
  _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() exited",_ClassName));
  return rc;
}

CMPIStatus OSBase_OperatingSystemProviderGetInstance( CMPIInstanceMI * mi, 
           CMPIContext * ctx, 
           CMPIResult * rslt, 
           CMPIObjectPath * cop, 
           char ** properties) {
  CMPIInstance * ci = NULL;
  CMPIStatus     rc = {CMPI_RC_OK, NULL};

  _OSBASE_TRACE(1,("--- %s CMPI GetInstance() called",_ClassName));

  _check_system_key_value_pairs( _broker, cop, "CSCreationClassName", "CSName", &rc );
  if( rc.rc != CMPI_RC_OK ) { 
    _OSBASE_TRACE(1,("--- %s CMPI GetInstance() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
    return rc; 
  }
  _check_system_key_value_pairs( _broker, cop, "CreationClassName", "Name", &rc );
  if( rc.rc != CMPI_RC_OK ) { 
    _OSBASE_TRACE(1,("--- %s CMPI GetInstance() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
    return rc; 
  }

  ci = _makeInst_OperatingSystem( _broker, ctx, cop, &rc );

  if( ci == NULL ) {
    if( rc.msg != NULL )
      { _OSBASE_TRACE(1,("--- %s CMPI GetInstance() failed : %s",_ClassName,CMGetCharPtr(rc.msg))); }
    else
      { _OSBASE_TRACE(1,("--- %s CMPI GetInstance() failed",_ClassName)); }
    return rc;
  }

  CMReturnInstance( rslt, ci );
  CMReturnDone(rslt);

  _OSBASE_TRACE(1,("--- %s CMPI GetInstance() exited",_ClassName));
  return rc;
}

CMPIStatus OSBase_OperatingSystemProviderCreateInstance( CMPIInstanceMI * mi, 
           CMPIContext * ctx, 
           CMPIResult * rslt, 
           CMPIObjectPath * cop, 
           CMPIInstance * ci) {
  CMPIStatus rc = {CMPI_RC_OK, NULL};

  _OSBASE_TRACE(1,("--- %s CMPI CreateInstance() called",_ClassName));

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED" ); 

  _OSBASE_TRACE(1,("--- %s CMPI CreateInstance() exited",_ClassName));
  return rc;
}

CMPIStatus OSBase_OperatingSystemProviderSetInstance( CMPIInstanceMI * mi, 
           CMPIContext * ctx, 
           CMPIResult * rslt, 
           CMPIObjectPath * cop,
           CMPIInstance * ci, 
           char **properties) {
  CMPIStatus rc = {CMPI_RC_OK, NULL};

  _OSBASE_TRACE(1,("--- %s CMPI SetInstance() called",_ClassName));

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED" ); 

  _OSBASE_TRACE(1,("--- %s CMPI SetInstance() exited",_ClassName));
  return rc;
}

CMPIStatus OSBase_OperatingSystemProviderDeleteInstance( CMPIInstanceMI * mi, 
           CMPIContext * ctx, 
           CMPIResult * rslt, 
           CMPIObjectPath * cop) {
  CMPIStatus rc = {CMPI_RC_OK, NULL}; 

  _OSBASE_TRACE(1,("--- %s CMPI DeleteInstance() called",_ClassName));

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED" ); 

  _OSBASE_TRACE(1,("--- %s CMPI DeleteInstance() exited",_ClassName));
  return rc;
}

CMPIStatus OSBase_OperatingSystemProviderExecQuery( CMPIInstanceMI * mi, 
           CMPIContext * ctx, 
           CMPIResult * rslt, 
           CMPIObjectPath * ref, 
           char * lang, 
           char * query) {
  CMPIStatus rc = {CMPI_RC_OK, NULL};

  _OSBASE_TRACE(1,("--- %s CMPI ExecQuery() called",_ClassName));

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED" ); 

  _OSBASE_TRACE(1,("--- %s CMPI ExecQuery() exited",_ClassName));
  return rc;
}


/* ---------------------------------------------------------------------------*/
/*                        Method Provider Interface                           */
/* ---------------------------------------------------------------------------*/

static char * _copy_buf( char ** hdbuf );

CMPIStatus OSBase_OperatingSystemProviderMethodCleanup( CMPIMethodMI * mi, 
           CMPIContext * ctx) {
  _OSBASE_TRACE(1,("--- %s CMPI MethodCleanup() called",_ClassName));
  _OSBASE_TRACE(1,("--- %s CMPI MethodCleanup() exited",_ClassName));
  CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_OperatingSystemProviderInvokeMethod( CMPIMethodMI * mi,
           CMPIContext * ctx,
           CMPIResult * rslt,
           CMPIObjectPath * ref,
           char * methodName,
           CMPIArgs * in,
           CMPIArgs * out) {
  CMPIData      incmd;
  CMPIValue     valrc;
  CMPIString  * class = NULL;
  CMPIStatus    rc    = {CMPI_RC_OK, NULL};
  char        * cmd   = NULL;
  char       ** hdout = NULL;
  char       ** hderr = NULL;
  char        * buf   = NULL;
  int           cmdrc = 0;

  _OSBASE_TRACE(1,("--- %s CMPI InvokeMethod() called",_ClassName));

  class = CMGetClassName(ref, &rc);

  /* "execCmd" */
  if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 &&
      strcasecmp(methodName, "execCmd") == 0 ) {

    valrc.uint8 = 0;

    incmd = CMGetArg( in, "cmd", &rc);

    cmd = (char*)malloc( (strlen(CMGetCharPtr(incmd.value.string))+1)*sizeof(char) );
    strcpy(cmd, CMGetCharPtr(incmd.value.string) );

    cmdrc = runcommand( cmd, NULL, &hdout, &hderr );

    /* command execution failed */
    if( cmdrc != 0 ) {
      valrc.uint8 = 2;
      _OSBASE_TRACE(1,("--- %s CMPI InvokeMethod() failed : runcommand() returned with %i",_ClassName,cmdrc));
      goto exitExecCmd;
    }

    /* command execution output on hderr */
    if( hderr[0] != NULL ) {
      valrc.uint8 = 1;
      buf = _copy_buf( hderr );
      if( buf != NULL ) {
	rc = CMAddArg( out, "out", buf, CMPI_chars);
	if( rc.rc != CMPI_RC_OK ) {
	  if( rc.msg != NULL ) {
	    _OSBASE_TRACE(1,("--- %s CMPI InvokeMethod() failed : stderr CMAddArg() %s",_ClassName,CMGetCharPtr(rc.msg)));
	  }
	  else {
	    _OSBASE_TRACE(1,("--- %s CMPI InvokeMethod() failed : stderr CMAddArg()",_ClassName));
	  }
	}
      }
      goto exitExecCmd;
    }

    if( hdout[0] != NULL ) {
      buf = _copy_buf( hdout );
      if( buf != NULL ) {
	rc = CMAddArg( out, "out", buf, CMPI_chars);
	if( rc.rc != CMPI_RC_OK ) {
	  if( rc.msg != NULL ) {
	    _OSBASE_TRACE(1,("--- %s CMPI InvokeMethod() failed : stdout CMAddArg() %s",_ClassName,CMGetCharPtr(rc.msg)));
	  }
	  else {
	    _OSBASE_TRACE(1,("--- %s CMPI InvokeMethod() failed : stdout CMAddArg()",_ClassName));
	  }
	}
      }
      goto exitExecCmd;
    }

  exitExecCmd:
    freeresultbuf(hdout);
    freeresultbuf(hderr);
    if(cmd) free(cmd);
    if(buf) free(buf);
    _OSBASE_TRACE(1,("--- %s CMPI InvokeMethod() %s exited",_ClassName,methodName));
    CMReturnData( rslt, &valrc, CMPI_uint8);
    CMReturnDone( rslt );
  }
  else if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 &&
	   strcasecmp(methodName, "Reboot") == 0 ) {
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_SUPPORTED, methodName ); 
  }
  else  if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 &&
	    strcasecmp(methodName, "Shutdown") == 0 ) {
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_SUPPORTED, methodName ); 
  }
  else {
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_FOUND, methodName ); 
  }
 
  _OSBASE_TRACE(1,("--- %s CMPI InvokeMethod() exited",_ClassName));
  return rc;
}


static char * _copy_buf( char ** hdbuf ) {
  char * buf      = NULL;
  int    i        = 0;
  int    buf_size = 0;

  /* allocate memory for return message of command */
  while ( hdbuf[i] ) {
    buf_size = buf_size + strlen(hdbuf[i]) +1 ;
    i++;	
  }	
  if(buf_size<1) { buf_size = 1; }

  buf = (char *) malloc ( buf_size*sizeof(char) );
  memset( buf, 0, buf_size*sizeof(char) );

  /* write output of hdbuf to buf */
  i=0;
  while ( hdbuf[i] ) {
    strcat(buf,hdbuf[i]);
    i++;
  }
  if(buf_size == 1) { strcpy(buf,"\n"); }

  return buf;
}


/* ---------------------------------------------------------------------------*/
/*                       Indication Provider Interface                        */
/* ---------------------------------------------------------------------------*/


/* compile option -DNOEVENTS allows to make the Indication support optional   */

#ifndef NOEVENTS

CMPIStatus OSBase_OperatingSystemProviderIndicationCleanup( CMPIIndicationMI * mi, 
           CMPIContext * ctx) {
  _OSBASE_TRACE(1,("--- %s CMPI IndicationCleanup() called",_ClassName));
  _OSBASE_TRACE(1,("--- %s CMPI IndicationCleanup() exited",_ClassName));
  CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_OperatingSystemProviderAuthorizeFilter( CMPIIndicationMI * mi, 
           CMPIContext * ctx, 
           CMPIResult * rslt,
           CMPISelectExp * filter, 
           char * indType, 
           CMPIObjectPath * classPath,
           char * owner) {
  _OSBASE_TRACE(1,("--- %s CMPI AuthorizeFilter() called",_ClassName));
  /* we don't object */
  _OSBASE_TRACE(1,("--- %s CMPI AuthorizeFilter() exited",_ClassName));
  CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_OperatingSystemProviderMustPoll( CMPIIndicationMI * mi, 
           CMPIContext * ctx, 
           CMPIResult * rslt,
           CMPISelectExp * filter, 
           char * indType, 
           CMPIObjectPath * classPath) {
  _OSBASE_TRACE(1,("--- %s CMPI MustPoll() called",_ClassName));
  /* no polling */
  CMReturnData(rslt,(CMPIValue*)&(CMPI_false),CMPI_boolean);
  CMReturnDone(rslt);
  _OSBASE_TRACE(1,("--- %s CMPI MustPoll() exited",_ClassName));
  CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_OperatingSystemProviderActivateFilter( CMPIIndicationMI * mi, 
           CMPIContext * ctx, 
           CMPIResult * rslt,
           CMPISelectExp * filter, 
           char * indType, 
           CMPIObjectPath * classPath,
           CMPIBoolean firstActivation) {
  _OSBASE_TRACE(1,("--- %s CMPI ActivateFilter() called",_ClassName));
  /* we are already running */
  if (checkProviderThread()) { CMReturn(CMPI_RC_OK); }
  if (strcasecmp(type,"cim_instmodification")==0)
    { startProviderThread(nh); }

  _OSBASE_TRACE(1,("--- %s CMPI ActivateFilter() exited",_ClassName));
  CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_OperatingSystemProviderDeActivateFilter( CMPIIndicationMI * mi, 
           CMPIContext * ctx, 
           CMPIResult * rslt,
           CMPISelectExp * filter, 
           char * indType, 
           CMPIObjectPath *classPath,
           CMPIBoolean lastActivation) {
  _OSBASE_TRACE(1,("--- %s CMPI DeActivateFilter() called",_ClassName));
  if (last && checkProviderThread()) { stopProviderThread(); }
  _OSBASE_TRACE(1,("--- %s CMPI DeActivateFilter() exited",_ClassName));
  CMReturn(CMPI_RC_OK);
}

#endif


/* ---------------------------------------------------------------------------*/
/*                              Provider Factory                              */
/* ---------------------------------------------------------------------------*/

CMInstanceMIStub( OSBase_OperatingSystemProvider, 
                  OSBase_OperatingSystemProvider, 
                  _broker, 
                  CMNoHook);

CMMethodMIStub( OSBase_OperatingSystemProvider,
                OSBase_OperatingSystemProvider, 
                _broker, 
                CMNoHook);

#ifndef NOEVENTS

CMIndicationMIStub( OSBase_OperatingSystemProvider,
                    OSBase_OperatingSystemProvider, 
                    _broker, 
                    CMNoHook);

#endif


/* ---------------------------------------------------------------------------*/
/*             end of cmpiOSBase_OperatingSystemProvider                      */
/* ---------------------------------------------------------------------------*/

