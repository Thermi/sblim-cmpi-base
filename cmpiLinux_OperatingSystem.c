/*
 * Linux_OperatingSystem.c
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
 * This is the C Provider implementation for the CIM class 
 * Linux_OperatingSystem. The enumeration of this class returns one  
 * single instance - the current running OS instance. Not the
 * number of all installed instances ! This decision was made, to 
 * avoid the effort of searching for more installed, but currently 
 * not running OS. 
 * The original CIM definition ignores the effort to find out, which
 * OSs are installed on the System and the value this information has. 
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmpidt.h"
#include "cmpift.h"
#include "cmpimacs.h"

#include "cmpiprovsup.h"
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

static char * _ClassName = "Linux_OperatingSystem";

/* private methods                                                            */
/*                                                                            */

/* - method to create a CMPIObjectPath of this class                          */
static CMPIObjectPath * _makePath( CMPIObjectPath * cop, CMPIStatus * rc);

/* - method to create a CMPIInstance of this class                            */
static CMPIInstance * _makeInst( CMPIObjectPath * cop, CMPIStatus * rc);
static CMPIInstance * _makeOS( CMPIObjectPath * cop, 
			       struct cim_operatingsystem * sptr, 
			       CMPIStatus * rc);
/* ---------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------*/
/* --- CPU utilization with event support                                     */
/* ---------------------------------------------------------------------------*/

typedef struct _CpuSample {
  unsigned long cpu;
  unsigned long total;
} CpuSample;
static int getcpu(CpuSample * cps);


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

static int checkProviderThread();
//static void startProviderThread( NPIHandle *);
static void stopProviderThread();
static void * statusLurker(void *);

#endif

/* ---------------------------------------------------------------------------*/

/* ---------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------*/
/*                      Instance Provider Interface                           */
/* ---------------------------------------------------------------------------*/


CMPIStatus Linux_OperatingSystemCleanup( CMPIInstanceMI * mi, 
					 CMPIContext * ctx) {
#ifndef NOEVENTS
  pthread_mutex_destroy(&mutex);
#endif
  CMReturn(CMPI_RC_OK);
}

CMPIStatus Linux_OperatingSystemEnumInstanceNames( CMPIInstanceMI * mi, 
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

CMPIStatus Linux_OperatingSystemEnumInstances( CMPIInstanceMI * mi, 
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


CMPIStatus Linux_OperatingSystemGetInstance( CMPIInstanceMI * mi, 
					     CMPIContext * ctx, 
					     CMPIResult * rslt, 
					     CMPIObjectPath * cop, 
					     char ** properties) {
  CMPIInstance * ci = NULL;
  CMPIStatus     rc = {CMPI_RC_OK, NULL};

  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI GetInstance()\n", _ClassName ); 

  _check_system_key_value_pairs( _broker, cop, "CSCreationClassName", "CSName", &rc );
  if( rc.rc != CMPI_RC_OK ) { return rc; }
  _check_system_key_value_pairs( _broker, cop, "CreationClassName", "Name", &rc );
  if( rc.rc != CMPI_RC_OK ) { return rc; }

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

CMPIStatus Linux_OperatingSystemCreateInstance( CMPIInstanceMI * mi, 
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

CMPIStatus Linux_OperatingSystemSetInstance( CMPIInstanceMI * mi, 
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

CMPIStatus Linux_OperatingSystemDeleteInstance( CMPIInstanceMI * mi, 
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

CMPIStatus Linux_OperatingSystemExecQuery( CMPIInstanceMI * mi, 
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
/*                       Method Provider Interface                            */
/* ---------------------------------------------------------------------------*/

static char * _copy_buf( char ** hdbuf );

CMPIStatus Linux_OperatingSystemMethodCleanup( CMPIMethodMI * mi, 
					       CMPIContext * ctx) {
  CMReturn(CMPI_RC_OK);
}

CMPIStatus Linux_OperatingSystemInvokeMethod( CMPIMethodMI * mi,
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
    fprintf( stderr, "--- %s.c : CMPI InvokeMethod()\n", _ClassName );
  
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
    valrc.uint32 = 99;
    CMReturnData( rslt, &valrc, CMPI_uint32); 
  }
  else  if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 && 
	    strcasecmp(methodName, "Shutdown") == 0 ) {
    valrc.uint32 = 99;
    CMReturnData( rslt, &valrc, CMPI_uint32); 
  }
  else {
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_SUPPORTED, methodName ); 
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
/*                      Indication Provider Interface                         */
/* ---------------------------------------------------------------------------*/


#ifndef NOEVENTS

CMPIStatus Linux_OperatingSystemIndicationCleanup( CMPIIndicationMI * mi, 
						   CMPIContext * ctx) {
  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI IndicationCleanup()\n", _ClassName ); 
  CMReturn(CMPI_RC_OK);
}

CMPIStatus Linux_OperatingSystemAuthorizeFilter( CMPIIndicationMI * mi, 
						 CMPIContext * ctx, 
						 CMPIResult * rslt,
						 CMPISelectExp * filter, 
						 char * indType, 
						 CMPIObjectPath * classPath,
						 char * owner) {
  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI AuthorizeFilter()\n", _ClassName ); 
  /* we don't object */
   CMReturn(CMPI_RC_OK);
}

CMPIStatus Linux_OperatingSystemMustPoll( CMPIIndicationMI * mi, 
					  CMPIContext * ctx, 
					  CMPIResult * rslt,
					  CMPISelectExp * filter, 
					  char * indType, 
					  CMPIObjectPath * classPath) {
  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI MustPoll()\n", _ClassName ); 
  /* no polling */
  CMReturnData(rslt,(CMPIValue*)&(CMPI_false),CMPI_boolean);
  CMReturnDone(rslt);
  CMReturn(CMPI_RC_OK);
}

CMPIStatus Linux_OperatingSystemActivateFilter( CMPIIndicationMI * mi, 
						CMPIContext * ctx, 
						CMPIResult * rslt,
						CMPISelectExp * filter, 
						char * indType, 
						CMPIObjectPath * classPath,
						CMPIBoolean firstActivation) {
  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI ActivateFilter()\n", _ClassName ); 
  /* we are already running */
  if (checkProviderThread()) { CMReturn(CMPI_RC_OK); }
  if (strcasecmp(type,"cim_instmodification")==0)
    { startProviderThread(nh); }
  CMReturn(CMPI_RC_OK);
}

CMPIStatus Linux_OperatingSystemDeActivateFilter( CMPIIndicationMI * mi, 
						  CMPIContext * ctx, 
						  CMPIResult * rslt,
						  CMPISelectExp * filter, 
						  char * indType, 
						  CMPIObjectPath *classPath,
						  CMPIBoolean lastActivation) {
  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI DeActivateFilter()\n", _ClassName ); 

  if (last && checkProviderThread()) { stopProviderThread(); }
  CMReturn(CMPI_RC_OK);
}

#endif


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

  get_os_name();
  if( !CIM_OS_NAME ) { 
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "no OS name found" );
    goto exit;
  }

  op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(ref,rc)), 
			_ClassName, rc );
  if( CMIsNullObject(op) ) { 
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." ); 
    goto exit; 
  }

  CMAddKey(op, "CSCreationClassName", CSCreationClassName, CMPI_chars);  
  CMAddKey(op, "CSName", CIM_HOST_NAME, CMPI_chars);
  CMAddKey(op, "CreationClassName", _ClassName, CMPI_chars);  
  CMAddKey(op, "Name", CIM_OS_NAME, CMPI_chars);
  
 exit:
  return op; 
}


/* ----------- method to create a CMPIInstance of this class ----------------*/

static CMPIInstance * _makeInst( CMPIObjectPath * ref,
				 CMPIStatus * rc) {
  CMPIInstance               * ci   = NULL;
  struct cim_operatingsystem * sptr = NULL;
  int                          frc  = 0;   

  frc = get_operatingsystem_data(&sptr);
  if (frc==0)
    ci = _makeOS( ref, sptr, rc );
  else {
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "Could not get OS Data." );
  }

  free_os_data(sptr);
  return ci;
}

static int getcpu(CpuSample * cps)
{
  unsigned long user, system, nice, idle;
  FILE * fcpuinfo = fopen("/proc/stat","r");
  if (cps && fcpuinfo) {
    fscanf(fcpuinfo,"%*s %ld %ld %ld %ld",&user,&system,&nice,&idle);
    fclose(fcpuinfo);
    cps->total=user+system+nice+idle;
    cps->cpu=cps->total - idle;
    return 0;
  } 
  else
    return -1;
}

static CMPIInstance * _makeOS( CMPIObjectPath * ref,
			       struct cim_operatingsystem * sptr, 
			       CMPIStatus * rc) {
  CMPIObjectPath * op = NULL;
  CMPIInstance   * ci = NULL; 
  CMPIDateTime   * dt = NULL;
  CpuSample        cs; 
  unsigned long long totalSwap = 0;
  unsigned short     pctcpu    = 0;
#ifndef CIM26COMPAT
  CMPIArray      * opstat    = NULL;
  unsigned short   status    = 2; /* Enabled */
  unsigned short   opstatval = 2; /* 2 ... OK ; 4 ... Stressed */
#endif
#ifndef NOEVENTS
  int                i         = 0;
#endif

  /* tool method call to get the unique name of the system */
  get_system_name(); 
  if( !CIM_HOST_NAME ) {   
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "no host name found" );
    goto exit;
  }

  get_os_name();
  if( !CIM_OS_NAME ) { 
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "no OS name found" );
    goto exit;
  }

  get_os_distro();
  if( !CIM_OS_DISTRO ) { 
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "no distro name found" );
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

  /* calculate cpu percentage */
#ifdef NOEVENTS
  if(getcpu(&cs) == 0) { pctcpu = 100*cs.cpu/cs.total; }
#else
  if (!checkProviderThread()) {
    if (getcpu(&cs)==0) { pctcpu = cs.cpu*100/cs.total; } 
  }
  else {
    pctcpu=0;
    if (!pthread_mutex_lock(&mutex)) {
      for (i=0;i<histlen;i++)
	pctcpu+=histcpu[i];
      pthread_mutex_unlock(&mutex);
    }
    pctcpu/=histlen;
  }
#endif 

  CMSetProperty( ci, "CSCreationClassName", CSCreationClassName, CMPI_chars ); 
  CMSetProperty( ci, "CSName", CIM_HOST_NAME, CMPI_chars );
  CMSetProperty( ci, "CreationClassName", _ClassName, CMPI_chars ); 
  CMSetProperty( ci, "Name", CIM_OS_NAME, CMPI_chars );

  CMSetProperty( ci, "Status", "NULL", CMPI_chars);
  CMSetProperty( ci, "Caption", "Operating System", CMPI_chars);
  CMSetProperty( ci, "Description", "A class derived from OperatingSystem to represents the running Linux OS.", CMPI_chars);

  CMSetProperty( ci, "OSType", (CMPIValue*)&(sptr->osType), CMPI_uint16);
  CMSetProperty( ci, "OtherTypeDescription", "NULL", CMPI_chars);

  CMSetProperty( ci, "Version", sptr->version, CMPI_chars);

  CMSetProperty( ci, "NumberOfUsers", (CMPIValue*)&(sptr->numOfUsers), CMPI_uint32);
  CMSetProperty( ci, "NumberOfProcesses", (CMPIValue*)&(sptr->numOfProcesses), CMPI_uint32);
  CMSetProperty( ci, "MaxNumberOfProcesses", (CMPIValue*)&(sptr->maxNumOfProc), CMPI_uint32);
  CMSetProperty( ci, "MaxProcessMemorySize", (CMPIValue*)&(sptr->maxProcMemSize), CMPI_uint64);
  CMSetProperty( ci, "PctTotalCPUTime", (CMPIValue*)&pctcpu, CMPI_uint8);

  CMSetProperty( ci, "Distributed",(CMPIValue*)&CMPI_false, CMPI_boolean);
  CMSetProperty( ci, "TotalSwapSpaceSize",(CMPIValue*)&totalSwap, CMPI_uint64);

  CMSetProperty( ci, "TotalVirtualMemorySize", (CMPIValue*)&(sptr->totalVirtMem), CMPI_uint64);
  CMSetProperty( ci, "FreeVirtualMemory", (CMPIValue*)&(sptr->freeVirtMem), CMPI_uint64);
  CMSetProperty( ci, "TotalVisibleMemorySize", (CMPIValue*)&(sptr->totalPhysMem), CMPI_uint64);
  CMSetProperty( ci, "FreePhysicalMemory", (CMPIValue*)&(sptr->freePhysMem), CMPI_uint64);
  CMSetProperty( ci, "SizeStoredInPagingFiles", (CMPIValue*)&(sptr->totalSwapMem), CMPI_uint64);
  CMSetProperty( ci, "FreeSpaceInPagingFiles", (CMPIValue*)&(sptr->freeSwapMem), CMPI_uint64);
 
  CMSetProperty( ci, "CurrentTimeZone", (CMPIValue*)&(sptr->curTimeZone), CMPI_sint16); 
  if( sptr->localDate != NULL ) {
    dt = CMNewDateTimeFromChars(_broker,sptr->localDate,rc);
    CMSetProperty( ci, "LocalDateTime", (CMPIValue*)&(dt), CMPI_dateTime);
  }
  if( sptr->installDate != NULL ) {
    dt = CMNewDateTimeFromChars(_broker,sptr->installDate,rc);
    CMSetProperty( ci, "InstallDate", (CMPIValue*)&(dt), CMPI_dateTime);
  }
  if( sptr->lastBootUp != NULL ) {
    dt = CMNewDateTimeFromChars(_broker,sptr->lastBootUp,rc);
    CMSetProperty( ci, "LastBootUpTime", (CMPIValue*)&(dt), CMPI_dateTime);
  }
  
  /* 2.7 */

#ifndef CIM26COMPAT
  opstat = CMNewArray(_broker,1,CMPI_uint16A,rc);
  if( rc->rc != CMPI_RC_OK ) {
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "CMNewArray(_broker,1,CMPI_uint16,rc)" );
    ci = NULL;
    goto exit;
  }
  if( pctcpu > 90 ) { opstatval = 4;  /* 4 ... Stressed */ }
  CMSetArrayElementAt(opstat,0,(CMPIValue*)&(opstatval),CMPI_uint16);  
  if( rc->rc != CMPI_RC_OK ) {
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "CMSetArrayElementAt(opstat,0,(CMPIValue*)&(opstatval),CMPI_uint16)" );
    ci = NULL;
    goto exit;
  }
  CMSetProperty( ci, "OperationalStatus", (CMPIValue*)&(opstat), CMPI_uint16A);

  CMSetProperty( ci, "ElementName", CIM_OS_DISTRO, CMPI_chars);
  CMSetProperty( ci, "EnabledStatus", (CMPIValue*)&(status), CMPI_uint16);
  CMSetProperty( ci, "OtherEnabledStatus", "NULL", CMPI_chars);
  CMSetProperty( ci, "RequestedStatus", (CMPIValue*)&(status), CMPI_uint16);
  CMSetProperty( ci, "EnabledDefault", (CMPIValue*)&(status), CMPI_uint16);
#endif    

 exit:
  return ci;
}


#ifndef NOEVENTS


static int checkProviderThread() {
  int status;
  if (pthread_mutex_lock(&mutex)) {
    fprintf(stderr,"Linux_OperatingSystem: couldn't obtain mutex lock");
    return -1;
  }
  status = threadActive;
  pthread_mutex_unlock(&mutex);
  return status;
}


static void startProviderThread(NPIHandle* nh)
{
  NPIHandle* tnh;
  if (pthread_mutex_lock(&mutex)) {
    fprintf(stderr,"Linux_OperatingSystem: couldn't obtain mutex lock");
    return;
  }
  if (threadActive==0) {
    fprintf(stderr,"Linux_OperatingSystem: starting status thread\n");
    tnh=CIMOMPrepareAttach(nh);
    if (errorCheck(tnh))
      CIMOMCancelAttach(tnh);
    else if (pthread_create(&tid,NULL,&statusLurker,tnh)==0)
      threadActive=1;
  }
  pthread_mutex_unlock(&mutex);
}

static void stopProviderThread()
{
  if (pthread_mutex_lock(&mutex)) {
    fprintf(stderr,"Linux_OperatingSystem: couldn't obtain mutex lock");
    return;
  }
  if (threadActive) {
    fprintf(stderr,"Linux_OperatingSystem: stopping status thread\n");
    threadActive=0;
    threadCancelPending+=1;
  }
  pthread_mutex_unlock(&mutex);
}


static void * statusLurker(void * args)
{
  int rc=0;
  int cancelstatus=0;
  NPIHandle* nh=args;
  struct cim_operatingsystem * refptr=NULL, * actptr=NULL;
  CIMInstance ciprev,cisrc,cimod;
  CIMObjectPath cop;
  CIMClass cc;
  CpuSample cs1, cs2;
  
  CIMOMAttachThread(nh);
  fprintf(stderr,"Linux_OperatingSystem: status thread starting\n");
  getcpu(&cs1);
  while(!cancelstatus) { 
    fprintf(stderr,"Linux_OperatingSystem: status thread blip\n");
    
  /* Get Initial Operating System */
    if (refptr==NULL)
     rc=get_operatingsystem_data(&refptr);
    sleep(60);
    if (!pthread_mutex_lock(&mutex) && !getcpu(&cs2)) {
      histcpu[histindex]=100*(cs2.cpu-cs1.cpu)/(cs2.total-cs1.total);
      histindex=(histindex+1)%histlen;
      pthread_mutex_unlock(&mutex);
      cs1=cs2;
    }
    cop=CIMObjectPathNew(nh,"cim_instmodification");
    CIMObjectPathSetNameSpace(nh,cop,"root/cimv2");
    cc=CIMOMGetClass(nh,cop,0);
    if (errorCheck(nh)) {
      fprintf(stderr,"Linux_OperatingSystem: couldn't get indication class\n");
      stopProviderThread();
    } else {         
      /* lookout for modifications */
      rc=get_operatingsystem_data(&actptr);
      if (1 /* calc change state*/) {
	fprintf(stderr,"Linux_OperatingSystem: beginning the indication.\n");
	cimod=CIMClassNewInstance(nh,cc);
	ciprev=_makeOS(nh,refptr,"root/cimv2");
	cisrc=_makeOS(nh,actptr,"root/cimv2");
	CIMOMDeliverInstanceEvent(nh,"root/cimv2",cimod,cisrc,ciprev);
	if (errorCheck(nh)) {
	  fprintf(stderr,"Linux_OperatingSystem: indication not delivered\n");
	  /* we do not exit here - could be a transient problem */
	  errorReset(nh);
	}
	free_os_data(refptr);
	refptr=actptr;
      }
    }
    if (pthread_mutex_lock(&mutex)) {
      fprintf(stderr,"Linux_OperatingSystem: couldn't obtain mutex lock");    
      break;
    }
    if (threadCancelPending) {
      cancelstatus=1;
      threadCancelPending-=1;
    }
    pthread_mutex_unlock(&mutex);
  }
  fprintf(stderr,"Linux_OperatingSystem: status thread terminating\n");
  CIMOMDetachThread(nh);
  return NULL;
}

#endif


/* ---------------------------------------------------------------------------*/
/*                              Provider Factory                              */
/* ---------------------------------------------------------------------------*/


CMInstanceMIStub( Linux_OperatingSystem,
		  Linux_OperatingSystem,
		  _broker,
		  CMNoHook);

CMMethodMIStub( Linux_OperatingSystem,
		Linux_OperatingSystem,
		_broker,
		CMNoHook);

#ifndef NOEVENTS
CMIndicationMIStub( Linux_OperatingSystem,
		    Linux_OperatingSystem,
		    _broker,
		    CMNoHook);
#endif

/* ---------------------------------------------------------------------------*/
/*                      end of Linux_OperatingSystem                          */
/* ---------------------------------------------------------------------------*/

