/*
 * cmpiOSBase_OperatingSystem.c
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
 * Interface Type : Common Magabeablity Programming Interface ( CMPI )
 *
 * Description:
 * This is the factory implementation for creating instances of CIM
 * class Linux_OperatingSystem.
 *
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmpidt.h"
#include "cmpimacs.h"

#include "cmpiOSBase_Common.h"
#include "OSBase_Common.h"

#include "cmpiOSBase_OperatingSystem.h"
#include "OSBase_OperatingSystem.h"


/* ---------------------------------------------------------------------------*/
/* private declarations                                                       */

/* ---------------------------------------------------------------------------*/

static CMPIInstance * _makeOS( CMPIBroker * _broker,
                               CMPIObjectPath * ref,
			       const char ** properties,
                               struct cim_operatingsystem * sptr,
                               CMPIStatus * rc);

/* ---------------------------------------------------------------------------*/
/* --- CPU utilization with event support                                     */
/* ---------------------------------------------------------------------------*/

typedef struct _CpuSample {
  unsigned long cpu;
  unsigned long total;
} CpuSample;

static int getcpu(CpuSample * cps);

/* ---------------------------------------------------------------------------*/

#ifndef NOEVENTS
int checkProviderThread();
void stopProviderThread();
void * statusLurker(void *);
#endif

/* ---------------------------------------------------------------------------*/

/* ---------------------------------------------------------------------------*/
/*                            Factory functions                               */
/* ---------------------------------------------------------------------------*/

/* ---------- method to create a CMPIObjectPath of this class ----------------*/

CMPIObjectPath * _makePath_OperatingSystem( CMPIBroker * _broker,
	         CMPIContext * ctx,  
                 CMPIObjectPath * ref,
	         CMPIStatus * rc) {
  CMPIObjectPath * op = NULL;
 
  _OSBASE_TRACE(2,("--- _makePath_OperatingSystem() called"));

  /* the sblim-cmpi-base package offers some tool methods to get common
   * system data 
  */
  if( !get_system_name() ) {   
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "no host name found" );
    _OSBASE_TRACE(2,("--- _makePath_OperatingSystem() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit;
  }

  if( !get_os_name() ) {
    CMSetStatusWithChars( _broker, rc,
			  CMPI_RC_ERR_FAILED, "no OS name found" );
    _OSBASE_TRACE(2,("--- _makePath_OperatingSystem() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit;
  }

  op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(ref,rc)), 
			_ClassName, rc );
  if( CMIsNullObject(op) ) { 
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." ); 
    _OSBASE_TRACE(2,("--- _makePath_OperatingSystem() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit; 
  }

  CMAddKey(op, "CSCreationClassName", CSCreationClassName, CMPI_chars);
  CMAddKey(op, "CSName", get_system_name(), CMPI_chars);
  CMAddKey(op, "CreationClassName", _ClassName, CMPI_chars);
  CMAddKey(op, "Name", get_os_name(), CMPI_chars);

 exit:
  _OSBASE_TRACE(2,("--- _makePath_OperatingSystem() exited"));
  return op;                
}

/* ----------- method to create a CMPIInstance of this class ----------------*/


CMPIInstance * _makeInst_OperatingSystem( CMPIBroker * _broker,
	       CMPIContext * ctx, 
               CMPIObjectPath * ref,
	       const char ** properties,
               CMPIStatus * rc) {
  CMPIInstance               * ci   = NULL;
  struct cim_operatingsystem * sptr = NULL;
  int                          frc  = 0;

  _OSBASE_TRACE(2,("--- _makeInst_OperatingSystem() called"));

  frc = get_operatingsystem_data(&sptr);
  if (frc==0)
    ci = _makeOS( _broker, ref, properties, sptr, rc );
  else {
    CMSetStatusWithChars( _broker, rc,
			  CMPI_RC_ERR_FAILED, "Could not get OS Data." );
    _OSBASE_TRACE(2,("--- _makeInst_OperatingSystem() failed : %s",CMGetCharPtr(rc->msg)));
  }

  free_os_data(sptr);

  _OSBASE_TRACE(2,("--- _makeInst_OperatingSystem() exited"));
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

static CMPIInstance * _makeOS( CMPIBroker * _broker,
                               CMPIObjectPath * ref,
			       const char ** properties,
                               struct cim_operatingsystem * sptr,
                               CMPIStatus * rc) {
  CMPIObjectPath *   op        = NULL;
  CMPIInstance   *   ci        = NULL;
  CMPIDateTime   *   dt        = NULL;
  CpuSample          cs;
  const char     **  keys      = NULL;
  int                keyCount  = 0;
  unsigned long long totalSwap = 0;
  unsigned short     pctcpu    = 0;
#ifndef CIM26COMPAT
  CMPIArray      *   opstat    = NULL;
  unsigned short     status    = 2; /* Enabled */
  unsigned short     opstatval = 2; /* 2 ... OK ; 4 ... Stressed */
#endif
#ifndef NOEVENTS
  int                i         = 0;
#endif

  _OSBASE_TRACE(2,("--- _makeOS() called"));

  /* the sblim-cmpi-base package offers some tool methods to get common
   * system data 
  */
  if( !get_system_name() ) {   
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "no host name found" );
    _OSBASE_TRACE(2,("--- _makeOS() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit;
  }

  if( !get_os_name() ) {
    CMSetStatusWithChars( _broker, rc,
			  CMPI_RC_ERR_FAILED, "no OS name found" );
    _OSBASE_TRACE(2,("--- _makeOS() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit;
  }

  op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(ref,rc)),
			_ClassName, rc );
  if( CMIsNullObject(op) ) {
    CMSetStatusWithChars( _broker, rc,
			  CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." );
    _OSBASE_TRACE(2,("--- _makeOS() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit;
  }

  ci = CMNewInstance( _broker, op, rc);
  if( CMIsNullObject(ci) ) {
    CMSetStatusWithChars( _broker, rc,
			  CMPI_RC_ERR_FAILED, "Create CMPIInstance failed." );
    _OSBASE_TRACE(2,("--- _makeOS() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit;
  }

  /* set property filter */
  keys = calloc(5,sizeof(char*));
  keys[0] = strdup("CSCreationClassName");
  keys[1] = strdup("CSName");
  keys[2] = strdup("CreationClassName");
  keys[3] = strdup("Name");
  CMSetPropertyFilter(ci,properties,keys);
  for( ;keys[keyCount]!=NULL;keyCount++) { free((char*)keys[keyCount]); }
  free(keys);

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
  CMSetProperty( ci, "CSName", get_system_name(), CMPI_chars );
  CMSetProperty( ci, "CreationClassName", _ClassName, CMPI_chars );
  CMSetProperty( ci, "Name", get_os_name(), CMPI_chars );

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
  CMSetProperty( ci, "PctTotalCPUTime", (CMPIValue*)&pctcpu, CMPI_uint16);

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
    if(dt == NULL) {
      CMSetStatusWithChars( _broker, rc,
			    CMPI_RC_ERR_FAILED, 
			    "CMNewDateTimeFromChars for property LocalDateTime failed." );
      _OSBASE_TRACE(2,("--- _makeOS() failed : %s",CMGetCharPtr(rc->msg)));
      goto exit;
    }
    else { CMSetProperty( ci, "LocalDateTime", (CMPIValue*)&(dt), CMPI_dateTime); }
  }

  if( sptr->installDate != NULL ) {
    dt = CMNewDateTimeFromChars(_broker,sptr->installDate,rc);
    if(dt == NULL) {
      CMSetStatusWithChars( _broker, rc,
			    CMPI_RC_ERR_FAILED, 
			    "CMNewDateTimeFromChars for property InstallDate failed." );
      _OSBASE_TRACE(2,("--- _makeOS() failed : %s",CMGetCharPtr(rc->msg)));
      goto exit;
    }
    else { CMSetProperty( ci, "InstallDate", (CMPIValue*)&(dt), CMPI_dateTime); }
  }

  if( sptr->lastBootUp != NULL ) {
    dt = CMNewDateTimeFromChars(_broker,sptr->lastBootUp,rc);    
    if(dt == NULL) {
      CMSetStatusWithChars( _broker, rc,
			    CMPI_RC_ERR_FAILED, 
			    "CMNewDateTimeFromChars for property LastBootUpTime failed." );
      _OSBASE_TRACE(2,("--- _makeOS() failed : %s",CMGetCharPtr(rc->msg)));
      goto exit;
    }
    else { CMSetProperty( ci, "LastBootUpTime", (CMPIValue*)&(dt), CMPI_dateTime); }
  }

  CMSetProperty( ci, "CodeSet", sptr->codeSet, CMPI_chars);
  CMSetProperty( ci, "LanguageEdition", sptr->langEd, CMPI_chars);

  CMSetProperty( ci, "DefaultPageSize", (CMPIValue*)&(sptr->defPageSize), CMPI_uint32);


  /* 2.7 */
#ifndef CIM26COMPAT
  opstat = CMNewArray(_broker,1,CMPI_uint16,rc);
  if( opstat == NULL ) {
    CMSetStatusWithChars( _broker, rc,
			  CMPI_RC_ERR_FAILED, "CMNewArray(_broker,1,CMPI_uint16,rc)" );
    _OSBASE_TRACE(2,("--- _makeOS() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit;
  }
  else {
    if( pctcpu > 90 ) { opstatval = 4;  /* 4 ... Stressed */ }
    CMSetArrayElementAt(opstat,0,(CMPIValue*)&(opstatval),CMPI_uint16);
    CMSetProperty( ci, "OperationalStatus", (CMPIValue*)&(opstat), CMPI_uint16A);
  }

  CMSetProperty( ci, "ElementName", get_os_distro(), CMPI_chars);  
  CMSetProperty( ci, "EnabledState", (CMPIValue*)&(status), CMPI_uint16);
  CMSetProperty( ci, "OtherEnabledState", "NULL", CMPI_chars);
  CMSetProperty( ci, "RequestedState", (CMPIValue*)&(status), CMPI_uint16);
  CMSetProperty( ci, "EnabledDefault", (CMPIValue*)&(status), CMPI_uint16);
#endif

 exit:
  _OSBASE_TRACE(2,("--- _makeOS() exited"));
  return ci;
}



#ifndef NOEVENTS


int checkProviderThread() {
  int status;
  if (pthread_mutex_lock(&mutex)) {
    fprintf(stderr,"Linux_OperatingSystem: couldn't obtain mutex lock");
    return -1;
  }
  status = threadActive;
  pthread_mutex_unlock(&mutex);
  return status;
}


void startProviderThread(NPIHandle* nh)
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

void stopProviderThread()
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


void * statusLurker(void * args)
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
/*                   end of cmpiOSBase_OperatingSystem.c                      */
/* ---------------------------------------------------------------------------*/

