/*
 * OSBase_OperatingSystem.c
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

/* ---------------------------------------------------------------------------*/
// private declarations

#ifdef DEBUG
    int _debug = 1;
#else
    int _debug = 0;
#endif

static char* _FILENAME = "OSBase_OperatingSystem.c";

/* ---------------------------------------------------------------------------*/

char * CIM_OS_DISTRO = NULL;

/* ---------------------------------------------------------------------------*/


/* returns a structure, which contains necessary information about the currently
 * running operating system
 */
int get_operatingsystem_data( struct cim_operatingsystem ** sptr ){

  struct tm * cttm     = NULL;
  time_t      ct       = 0;
  FILE      * fmeminfo = NULL ;

  if( _debug ) { fprintf(stderr, "--- %s : get_operatingsystem_data()\n",_FILENAME); }

  (*sptr) = (struct cim_operatingsystem *)malloc(sizeof(struct cim_operatingsystem));
  memset(*sptr, 0, sizeof(struct cim_operatingsystem));

  if( !CIM_OS_DISTRO ) { get_os_distro(); }
  (*sptr)->version = CIM_OS_DISTRO;
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
  if( time(&ct) != 0 ) {
    cttm = gmtime( &ct );
    (*sptr)->localDate = (char*)malloc(26+sizeof(char));
    strftime((*sptr)->localDate,26,"%Y%m%d%H%M%S.000000",cttm);
    _cat_timezone((*sptr)->localDate, (*sptr)->curTimeZone);
  }

  /* InstallDate */
  (*sptr)->installDate = get_os_installdate();
  _cat_timezone((*sptr)->installDate, (*sptr)->curTimeZone);

  /* LastBootUp */
  (*sptr)->lastBootUp = get_os_lastbootup();
  _cat_timezone((*sptr)->lastBootUp, (*sptr)->curTimeZone);

  return 0;
}

char * get_os_distro() {
  char ** hdout   = NULL;
  char *  ptr     = NULL;
  int     strl    = 0;
  int     i       = 0;
  int     rc      = 0;

  if( _debug && !CIM_OS_DISTRO )
    { fprintf(stderr, "--- %s : get_os_distro()\n",_FILENAME); }

  /* check if Red Hat */
  /*
    if( !CIM_OS_DISTRO ) {
    rc = runcommand( "rpm -q redhat-release" , NULL , &hdout , NULL );
    if( rc == 0 && ( strstr(hdout[0],"not installed") == NULL) ) {
      CIM_OS_DISTRO = (char *) malloc( strlen(hdout[0])*sizeof(char));
      ptr = strchr(hdout[0], '\n');
      *ptr = '\0';
      strcpy( CIM_OS_DISTRO , hdout[0] );
      freeresultbuf(hdout);
    }
  }
  */
  /* check if SuSE */
  /*
  if( !CIM_OS_DISTRO ) {
#ifdef INTEL
    rc = runcommand( "rpm -q SuSE-release" , NULL , &hdout , NULL );
    if( rc == 0 && ( strstr(hdout[0],"not installed") == NULL) ) {
      CIM_OS_DISTRO = (char *) malloc( strlen(hdout[0])*sizeof(char));
      ptr = strchr(hdout[0], '\n');
      *ptr = '\0';
      strcpy( CIM_OS_DISTRO , hdout[0] );
      freeresultbuf(hdout);
    }
#elif S390
    rc = runcommand( "cat /etc/SuSE-release" , NULL , &hdout , NULL );
    if( rc == 0 ) {
      CIM_OS_DISTRO = (char *) malloc( strlen(hdout[0]+strlen(hdout[1]))*sizeof(char));
      ptr = strchr(hdout[0], '\n');
      *ptr = '\0';
      strcpy( CIM_OS_DISTRO , hdout[0] );
      strcat( CIM_OS_DISTRO , hdout[1] );
      ptr = strchr(CIM_OS_DISTRO, '\n');
      *ptr = '\0';
      freeresultbuf(hdout);
    }
#endif
  }
  */

  if( !CIM_OS_DISTRO ) {
    rc = runcommand( "cat /etc/`ls /etc/ | grep release`" , NULL , &hdout , NULL );
    if( rc == 0 ) {
      while ( hdout[i]) {
	strl = strl+strlen(hdout[i])+1;
	ptr = strchr(hdout[i], '\n');
	*ptr = '\0';
	i++;
      }	
      i=0;
      CIM_OS_DISTRO = (char *) malloc( strl*sizeof(char));
      strcpy( CIM_OS_DISTRO , hdout[0] );
      i++;
      while ( hdout[i]) {
	strcat( CIM_OS_DISTRO , " " );
	strcat( CIM_OS_DISTRO , hdout[i] );
	i++;
      }
      freeresultbuf(hdout);
    }
    else {
      CIM_OS_DISTRO = (char *) malloc( 6*sizeof(char));
      strcpy( CIM_OS_DISTRO , "Linux" );
    }
  }

  return CIM_OS_DISTRO;
}

char * get_kernel_version() {
  char ** hdout = NULL ;
  char *  str   = NULL;
  int     rc    = 0;

  if( _debug ) { fprintf(stderr, "--- %s : get_kernel_version()\n",_FILENAME); }

  rc = runcommand( "uname -r" , NULL , &hdout , NULL );
  if( rc == 0 ) {
    str = (char *) malloc((strlen(hdout[0])+1)*sizeof(char));
    strcpy( str, hdout[0]);
    freeresultbuf(hdout);
  }
  else {
    str = (char *) malloc(10*sizeof(char));
    strcpy( str , "not found");
  }
  return str;
}

char * get_os_installdate() {
  struct tm    date;
  char      ** hdout = NULL;
  char      *  dstr  = NULL;
  char      *  str   = NULL;
  char      *  ptr   = NULL;
  int          rc    = 0;

  if( _debug ) { fprintf(stderr, "--- %s : get_os_installdate()\n",_FILENAME); }

  get_os_distro();

  if( strstr( CIM_OS_DISTRO, "redhat" ) != NULL ) {
    rc = runcommand( "rpm -qi redhat-release | grep Install" , NULL , &hdout , NULL );
    if( rc == 0 ) {
      str = strstr( hdout[0], ": ");
      str+=2;
      for( ptr = str ; (*ptr)!=' ' ; ptr++ ) {
	if( *(ptr+1)==' ' ) { ptr ++; }
      }
      dstr = (char *) malloc( (strlen(str)-strlen(ptr)+1)*sizeof(char));
      strncpy( dstr , str, strlen(str)-strlen(ptr)-1);
      strptime(dstr, "%A %d %B %Y %H:%M:%S %p %Z", &date);
      str = (char*)malloc(26*sizeof(char));
      strftime(str,26,"%Y%m%d%H%M%S.000000",&date);
      if(dstr) free(dstr);
      freeresultbuf(hdout);
    }
  }
  return str;
}


char * get_os_lastbootup() {
  char          *  uptime = NULL;
  struct tm     *  uptm   = NULL;
  unsigned long    up     = 0;

  if( _debug ) { fprintf(stderr, "--- %s : get_os_lastbootup()\n",_FILENAME); }

  up = _get_os_boottime();
  if( up == 0 ) return NULL;
  uptm = gmtime( &up );
  uptime = (char*)malloc(26+sizeof(char));
  strftime(uptime,26,"%Y%m%d%H%M%S.000000",uptm);
  return uptime;
}

unsigned long get_os_numOfProcesses() {
  char          ** hdout = NULL ;
  unsigned long    np    = 0;
  int              rc    = 0;

  if( _debug ) { fprintf(stderr, "--- %s : get_os_numOfProcesses()\n",_FILENAME); }

  rc = runcommand( "ps --no-headers -eo pid | wc -l" , NULL , &hdout , NULL );
  if( rc == 0 ) {
    np = atol(hdout[0]);
    freeresultbuf(hdout);
  }
  return np;
}

unsigned long get_os_numOfUsers() {
  char          ** hdout = NULL ;
  unsigned long    np    = 0;
  int              rc    = 0;

  if( _debug ) { fprintf(stderr, "--- %s : get_os_numOfUsers()\n",_FILENAME); }

  rc = runcommand( "who -u | wc -l" , NULL , &hdout , NULL );
  if( rc == 0 ) {
    np = atol(hdout[0]);
    freeresultbuf(hdout);
  }
  return np;
}

unsigned long get_os_maxNumOfProc() {
  FILE          * ffilemax = NULL;
  unsigned long   max      = 0;

  if( _debug ) { fprintf(stderr, "--- %s : get_os_maxNumOfProc()\n",_FILENAME); }

  if ( (ffilemax=fopen("/proc/sys/fs/file-max","r")) != NULL ) {
    fscanf(ffilemax,"%ld",&max);
    fclose(ffilemax);
  }
  return max;
}

unsigned long long get_os_maxProcMemSize() {
  struct rlimit      rlim;
  unsigned long long max = 0;
  int                rc  = 0;

  if( _debug ) { fprintf(stderr, "--- %s : get_os_maxProcMemSize()\n",_FILENAME); }

  rc = getrlimit(RLIMIT_DATA,&rlim);
  if( rc == 0 ) { max = rlim.rlim_max; }
  return max;
}


/* ---------------------------------------------------------------------------*/

void free_os_data( struct cim_operatingsystem * sptr ) {
    if(sptr==NULL) return;
    if(sptr->installDate) free(sptr->installDate);
    if(sptr->lastBootUp) free(sptr->lastBootUp);
    if(sptr->localDate) free(sptr->localDate);
    free(sptr);
}

/* ---------------------------------------------------------------------------*/
/*                    end of OSBase_OperatingSystem.c                         */
/* ---------------------------------------------------------------------------*/

