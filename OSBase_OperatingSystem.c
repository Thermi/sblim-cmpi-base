/*
 * OSBase_OperatingSystem.c
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
 *               C. Eric Wu <cwu@us.ibm.com>               
 *
 * Description:
 * This shared library provides resource access functionality for the class
 * Linux_OperatingSystem. 
 * It is independent from any specific CIM technology.
*/

/* ---------------------------------------------------------------------------*/

#define _GNU_SOURCE

#include "OSBase_Common.h"
#include "OSBase_OperatingSystem.h"
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <langinfo.h>

/* ---------------------------------------------------------------------------*/
// private declarations

char * CIM_OS_DISTRO = NULL;

/* ---------------------------------------------------------------------------*/

/* initialization routine */
void _init() {
}

/* deinitialization routine */
void _fini() { 
  free ( CIM_OS_DISTRO );
}

/* ---------------------------------------------------------------------------*/

/* returns a structure, which contains necessary information about the currently
 * running operating system
 */
int get_operatingsystem_data( struct cim_operatingsystem ** sptr ){

  FILE * fmeminfo = NULL ;

  _OSBASE_TRACE(3,("--- get_operatingsystem_data() called"));

  (*sptr) = calloc(1,sizeof(struct cim_operatingsystem));

  (*sptr)->version = get_os_distro();
  (*sptr)->osType = 36;

  (*sptr)->numOfProcesses = get_os_numOfProcesses();
  (*sptr)->numOfUsers     = get_os_numOfUsers();
  (*sptr)->maxNumOfProc   = get_os_maxNumOfProc();
  (*sptr)->maxProcMemSize = get_os_maxProcMemSize();

  /* get values for memory properties :
   * TotalVisibleMemorySize, FreePhysicalMemory, SizeStoredInPagingFiles,
   * FreeSpaceInPagingFiles
   */
  if ( (fmeminfo=fopen("/proc/meminfo","r")) != NULL ) {
    fscanf(fmeminfo,
	   "%*s %*s %*s %*s %*s %*s %*s %lld %*s %lld %*s %*s %*s %*s %lld %*s %lld",
	   &((*sptr)->totalPhysMem),&((*sptr)->freePhysMem),
	   &((*sptr)->totalSwapMem),&((*sptr)->freeSwapMem) );
    fclose(fmeminfo);
  }
  (*sptr)->totalPhysMem = (*sptr)->totalPhysMem/1024;
  (*sptr)->freePhysMem = (*sptr)->freePhysMem/1024;
  (*sptr)->totalSwapMem = (*sptr)->totalSwapMem/1024;
  (*sptr)->freeSwapMem = (*sptr)->freeSwapMem/1024;
  /* TotalVirtualMemorySize */
  (*sptr)->totalVirtMem = (*sptr)->totalPhysMem + (*sptr)->totalSwapMem;
  /* FreeVirtualMemory */
  (*sptr)->freeVirtMem = (*sptr)->freePhysMem + (*sptr)->freeSwapMem;

  //  fprintf( stderr,"%lli : %lli: %lli: %lli\n",(*sptr)->totalPhysMem,(*sptr)->freePhysMem,(*sptr)->totalSwapMem,(*sptr)->freeSwapMem);

  /* CurrentTimeZone */
  (*sptr)->curTimeZone = get_os_timezone();

  /* LocalDateTime */
  (*sptr)->localDate = get_os_localdatetime();

  /* InstallDate */
  (*sptr)->installDate = get_os_installdate();

  /* LastBootUp */
  (*sptr)->lastBootUp = get_os_lastbootup();

  /* CodeSet */
  (*sptr)->codeSet = get_os_codeSet();

  /* LanguageEdition */
  (*sptr)->langEd = get_os_langEd();

  /* DefaultPageSize */
  (*sptr)->defPageSize = getpagesize();

  _OSBASE_TRACE(3,("--- get_operatingsystem_data() exited"));
  return 0;
}


char * get_os_distro() {
  char ** hdout   = NULL;
  char *  ptr     = NULL;
  int     strl    = 0;
  int     i       = 0;
  int     rc      = 0;

  if( !CIM_OS_DISTRO ) {
    
    _OSBASE_TRACE(4,("--- get_os_distro() called : init"));

    rc = runcommand( "cat /etc/`ls /etc/ | grep release`" , NULL , &hdout , NULL );
    if( rc == 0 ) {
      while ( hdout[i]) {
	strl = strl+strlen(hdout[i])+1;
	ptr = strchr(hdout[i], '\n');
	*ptr = '\0';
	i++;
      }	
      i=0;
      CIM_OS_DISTRO = calloc(1,strl);
      strcpy( CIM_OS_DISTRO , hdout[0] );
      i++;
      while ( hdout[i]) {
	strcat( CIM_OS_DISTRO , " " );
	strcat( CIM_OS_DISTRO , hdout[i] );
	i++;
      }
    }
    else {
      CIM_OS_DISTRO = calloc(1,6);
      strcpy( CIM_OS_DISTRO , "Linux" );
    }
    freeresultbuf(hdout);
    _OSBASE_TRACE(4,("--- get_os_distro() : CIM_OS_DISTRO initialized with %s",CIM_OS_DISTRO));
  }

  _OSBASE_TRACE(4,("--- get_os_distro() exited : %s",CIM_OS_DISTRO));
  return CIM_OS_DISTRO;
}


char * get_kernel_version() {
  char ** hdout = NULL ;
  char *  str   = NULL;
  int     rc    = 0;

  _OSBASE_TRACE(4,("--- get_kernel_version() called"));

  rc = runcommand( "uname -r" , NULL , &hdout , NULL );
  if( rc == 0 ) {
    str = (char *) malloc((strlen(hdout[0])+1));
    strcpy( str, hdout[0]);
  }
  else {
    str = (char *) malloc(10);
    strcpy( str , "not found");
  }
  freeresultbuf(hdout);

  _OSBASE_TRACE(4,("--- get_kernel_version() exited : %s",str));
  return str;
}


char * get_os_installdate() {
  struct tm    date;
  char      ** hdout = NULL;
  char      *  dstr  = NULL;
  char      *  str   = NULL;
  char      *  ptr   = NULL;
  int          rc    = 0;

  _OSBASE_TRACE(4,("--- get_os_installdate() called"));

  get_os_distro();

  if( strstr( CIM_OS_DISTRO, "Red Hat" ) != NULL ) {
    rc = runcommand( "rpm -qi redhat-release | grep Install" , NULL , &hdout , NULL );
    if( rc == 0 ) {
      str = strstr( hdout[0], ": ");
      str+=2;
      for( ptr = str ; (*ptr)!=' ' ; ptr++ ) {
	if( *(ptr+1)==' ' ) { ptr ++; }
      }
      dstr = (char *) malloc( (strlen(str)-strlen(ptr)+1));
      strncpy( dstr , str, strlen(str)-strlen(ptr)-1);
      strptime(dstr, "%A %d %B %Y %H:%M:%S %p %Z", &date);
      str = (char*)malloc(26);
      strftime(str,26,"%Y%m%d%H%M%S.000000",&date);
      _cat_timezone(str, get_os_timezone());
      if(dstr) free(dstr);
    }
    freeresultbuf(hdout);
  }

  _OSBASE_TRACE(4,("--- get_os_installdate() exited : %s",str));
  return str;
}


char * get_os_lastbootup() {
  char          * uptime = NULL;
  struct tm       uptm;
  unsigned long   up     = 0;

  _OSBASE_TRACE(4,("--- get_os_lastbootup() called"));

  up = _get_os_boottime();
  if( up == 0 ) { 
    _OSBASE_TRACE(4,("--- get_os_lastbootup() failed : was not able to set last boot time"));
    return NULL;
  }
  if( gmtime_r( &up, &uptm ) != NULL ) {
    uptime = (char*)malloc(26);
    strftime(uptime,26,"%Y%m%d%H%M%S.000000",&uptm);
    _cat_timezone(uptime, get_os_timezone());
  }

  _OSBASE_TRACE(4,("--- get_os_lastbootup() exited : %s",uptime));
  return uptime;
}


char * get_os_localdatetime() {
  char          * tm  = NULL;
  long            sec = 0;
  struct tm       cttm;
  struct timeval  tv;
  struct timezone tz;

  _OSBASE_TRACE(4,("--- get_os_localdatetime() called"));

  if( gettimeofday( &tv, &tz) == 0 ) {
    sec = tv.tv_sec + (tz.tz_minuteswest*-1*60);
    if( gmtime_r( &sec , &cttm) != NULL ) {
      tm = (char*)malloc(26);
      strftime(tm,26,"%Y%m%d%H%M%S.000000",&cttm);
      _cat_timezone(tm, get_os_timezone());
    }
  }

  _OSBASE_TRACE(4,("--- get_os_localdatetime() exited : %s",tm));
  return tm;
}


unsigned long get_os_numOfProcesses() {
  FILE          * fhd   = NULL;
  char          * value = NULL;
  char          * ptr   = NULL;
  unsigned long   np    = 0;

  _OSBASE_TRACE(4,("--- get_os_numOfProcesses() called"));

  if ( (fhd=fopen("/proc/loadavg","r")) != NULL ) {
    value = calloc(1,256);
    fscanf(fhd, 
	   "%*s %*s %*s %s",
	   value);
    fclose(fhd);
    ptr = strchr(value,'/');
    ptr++;
    np = atol(ptr);
    if(value) { free(value); }
  }

  _OSBASE_TRACE(4,("--- get_os_numOfProcesses() exited : %lu",np));
  return np;
}

unsigned long get_os_numOfUsers() {
  char          ** hdout = NULL ;
  unsigned long    np    = 0;
  int              rc    = 0;

  _OSBASE_TRACE(4,("--- get_os_numOfUsers() called"));

  rc = runcommand( "who -u | wc -l" , NULL , &hdout , NULL );
  if( rc == 0 ) {
    np = atol(hdout[0]);
  }
  freeresultbuf(hdout);

  _OSBASE_TRACE(4,("--- get_os_numOfUsers() exited : %lu",np));
  return np;
}

unsigned long get_os_maxNumOfProc() {
  FILE          * ffilemax = NULL;
  unsigned long   max      = 0;

  _OSBASE_TRACE(4,("--- get_os_maxNumOfProc() called"));

  if ( (ffilemax=fopen("/proc/sys/fs/file-max","r")) != NULL ) {
    fscanf(ffilemax,"%ld",&max);
    fclose(ffilemax);
  }

  _OSBASE_TRACE(4,("--- get_os_maxNumOfProc() exited : %lu",max));
  return max;
}

unsigned long long get_os_maxProcMemSize() {
  struct rlimit      rlim;
  unsigned long long max = 0;
  int                rc  = 0;

  _OSBASE_TRACE(4,("--- get_os_maxProcMemSize() called"));

  rc = getrlimit(RLIMIT_DATA,&rlim);
  if( rc == 0 ) { max = rlim.rlim_max; }

  _OSBASE_TRACE(4,("--- get_os_maxProcMemSize() exited : %Ld",max));
  return max;
}

char * get_os_codeSet() {
  char * codeSet = NULL;

  _OSBASE_TRACE(4,("--- get_os_codeSet() called"));

  codeSet = nl_langinfo(CODESET);

  _OSBASE_TRACE(4,("--- get_os_codeSet() exited : %s",codeSet));
  return codeSet;
}

char * get_os_langEd() {
  char ** hdout  = NULL;
  char *  langEd = NULL;
  char *  var    = NULL;
  char *  str    = NULL;
  char *  ptr    = NULL;
  int     rc     = 0;

  _OSBASE_TRACE(4,("--- get_os_langEd() called"));

  var = getenv("LANG");
  if(!var) { 
    rc = runcommand( "locale | grep LC_CTYPE" , NULL , &hdout , NULL );
    if( rc == 0 ) { var = hdout[0]; }
  }
  if( var != NULL ) {
    if( (ptr=strchr(var,'"')) ) {
      var = ptr+1;
    }
    str = strchr(var,'.');
    langEd = calloc(1,strlen(var)-strlen(str)+1);
    strncpy(langEd,var,strlen(var)-strlen(str));
    if( (str = strchr(langEd,'_')) ) { *str = '-'; }
  }
  freeresultbuf(hdout);

  _OSBASE_TRACE(4,("--- get_os_langEd() exited : %s",langEd));
  return langEd;
}


/* ---------------------------------------------------------------------------*/

void free_os_data( struct cim_operatingsystem * sptr ) {
    if(sptr==NULL) return;
    if(sptr->installDate) free(sptr->installDate);
    if(sptr->lastBootUp) free(sptr->lastBootUp);
    if(sptr->localDate) free(sptr->localDate);
    if(sptr->codeSet) free(sptr->codeSet);
    if(sptr->langEd) free(sptr->langEd);
    free(sptr);
}

/* ---------------------------------------------------------------------------*/
/*                    end of OSBase_OperatingSystem.c                         */
/* ---------------------------------------------------------------------------*/

