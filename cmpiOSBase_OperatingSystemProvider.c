/*
 * cmpiOSBase_OperatingSystemProvider.c
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

static char * _FILENAME = "cmpiOSBase_OperatingSystemProvider.c";

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
  if( _debug )
    fprintf( stderr, "--- %s : %s CMPI Cleanup()\n", _FILENAME, _ClassName );
#ifndef NOEVENTS
  pthread_mutex_destroy(&mutex);
#endif
  CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_OperatingSystemProviderEnumInstanceNames( CMPIInstanceMI * mi, 
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

CMPIStatus OSBase_OperatingSystemProviderEnumInstances( CMPIInstanceMI * mi, 
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

CMPIStatus OSBase_OperatingSystemProviderGetInstance( CMPIInstanceMI * mi, 
           CMPIContext * ctx, 
           CMPIResult * rslt, 
           CMPIObjectPath * cop, 
           char ** properties) {
  CMPIInstance * ci = NULL;
  CMPIStatus     rc = {CMPI_RC_OK, NULL};

  if( _debug )
    fprintf( stderr, "--- %s : %s CMPI GetInstance()\n", _FILENAME, _ClassName );

  _check_system_key_value_pairs( _broker, cop, "CSCreationClassName", "CSName", &rc );
  if( rc.rc != CMPI_RC_OK ) { return rc; }
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

CMPIStatus OSBase_OperatingSystemProviderCreateInstance( CMPIInstanceMI * mi, 
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

CMPIStatus OSBase_OperatingSystemProviderSetInstance( CMPIInstanceMI * mi, 
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

CMPIStatus OSBase_OperatingSystemProviderDeleteInstance( CMPIInstanceMI * mi, 
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

CMPIStatus OSBase_OperatingSystemProviderExecQuery( CMPIInstanceMI * mi, 
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

static char * _copy_buf( char ** hdbuf );

CMPIStatus OSBase_OperatingSystemProviderMethodCleanup( CMPIMethodMI * mi, 
           CMPIContext * ctx) {
  if( _debug )
    fprintf( stderr, "--- %s : %s CMPI MethodCleanup()\n", _FILENAME, _ClassName );
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

  if( _debug )
    fprintf( stderr, "--- %s : %s CMPI InvokeMethod()\n", _FILENAME, _ClassName );


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
      valrc.uint8 = 1;
      goto exitExecCmd;
    }

    /* command execution output on hderr */
    if( hderr[0] != NULL ) {
      valrc.uint8 = 2;
      buf = _copy_buf( hderr );
      if( buf != NULL ) {
	rc = CMAddArg( out, "out", buf, CMPI_chars);
	if( rc.rc != CMPI_RC_OK ) {
	  if( _debug ) {
	    if( rc.msg != NULL )
	      { fprintf(stderr,"rc.msg: %s\n",CMGetCharPtr(rc.msg)); }
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
	  if( _debug ) {
	    if( rc.msg != NULL )
	      { fprintf(stderr,"rc.msg: %s\n",CMGetCharPtr(rc.msg)); }
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
    CMReturnData( rslt, &valrc, CMPI_uint8);
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
  if( _debug )
    fprintf( stderr, "--- %s : %s CMPI IndicationCleanup()\n", _FILENAME, _ClassName );
  CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_OperatingSystemProviderAuthorizeFilter( CMPIIndicationMI * mi, 
           CMPIContext * ctx, 
           CMPIResult * rslt,
           CMPISelectExp * filter, 
           char * indType, 
           CMPIObjectPath * classPath,
           char * owner) {
  if( _debug )
    fprintf( stderr, "--- %s : %s CMPI AuthorizeFilter()\n", _FILENAME, _ClassName );
  /* we don't object */
  CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_OperatingSystemProviderMustPoll( CMPIIndicationMI * mi, 
           CMPIContext * ctx, 
           CMPIResult * rslt,
           CMPISelectExp * filter, 
           char * indType, 
           CMPIObjectPath * classPath) {
  if( _debug )
    fprintf( stderr, "--- %s : %s CMPI MustPoll()\n", _FILENAME, _ClassName );
  /* no polling */
  CMReturnData(rslt,(CMPIValue*)&(CMPI_false),CMPI_boolean);
  CMReturnDone(rslt);
  CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_OperatingSystemProviderActivateFilter( CMPIIndicationMI * mi, 
           CMPIContext * ctx, 
           CMPIResult * rslt,
           CMPISelectExp * filter, 
           char * indType, 
           CMPIObjectPath * classPath,
           CMPIBoolean firstActivation) {
  if( _debug )
    fprintf( stderr, "--- %s : %s CMPI ActivateFilter()\n", _FILENAME, _ClassName );
  /* we are already running */
  if (checkProviderThread()) { CMReturn(CMPI_RC_OK); }
  if (strcasecmp(type,"cim_instmodification")==0)
    { startProviderThread(nh); }
  CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_OperatingSystemProviderDeActivateFilter( CMPIIndicationMI * mi, 
           CMPIContext * ctx, 
           CMPIResult * rslt,
           CMPISelectExp * filter, 
           char * indType, 
           CMPIObjectPath *classPath,
           CMPIBoolean lastActivation) {
  if( _debug )
    fprintf( stderr, "--- %s : %s CMPI DeActivateFilter()\n", _FILENAME, _ClassName );
  if (last && checkProviderThread()) { stopProviderThread(); }
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
/*          end of cmpiOSBase_OperatingSystemProvider                      */
/* ---------------------------------------------------------------------------*/

