/*
 * CimBase.c
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
 * This shared library provides functionality for OS Base Instrumentation,
 * and all other SBLIM packages.
*/

//------------------------------------------------------------------------------

#define _GNU_SOURCE

#ifdef DEBUG
    int _debug = 1;
#else
    int _debug = 0;
#endif

#include "cimibase.h"

#include "mlogsup.h"

#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>  
#include <dirent.h>
#include <sys/stat.h>    
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>


//------------------------------------------------------------------------------

char * CSCreationClassName = "Linux_ComputerSystem";
char * OSCreationClassName = "Linux_OperatingSystem";

char * CIM_HOST_NAME = NULL;
char * CIM_OS_NAME   = NULL;
char * CIM_OS_DISTRO = NULL;
int    CIM_OS_TIMEZONE = 999;
unsigned long CIM_OS_BOOTTIME = 0;

static char * _FILENAME = "CimBase.c";

// -----------------------------------------------------------------------------
// class Linux_ComputerSystem
// -----------------------------------------------------------------------------

/* initializes the variable CIM_HOST_NAME
 * contains the full qualified IP hostname of the system, e.g. host.domain
 */

char * get_system_name() {
  char *  host   = NULL;
  char *  domain = NULL;
  char *  ptr    = NULL;
  char ** hdout  = NULL;
  int     rc     = 0;

  if( !CIM_HOST_NAME ) {

    if( _debug ) { fprintf(stderr, "--- %s : get_system_name()\n",_FILENAME); }

    host = (char *)malloc(255*sizeof(char));
    if ( gethostname(host, 255 ) == -1 ) { return NULL ; }
    /* if host does not contain a '.' we can suppose, that the domain is not
     * available in the current value. but we try to get the full qualified
     * hostname.
     */
    if( strchr( host , '.') == NULL ) {
      /* get domain name */
      rc=runcommand("/bin/dnsdomainname",NULL,&hdout,NULL);
      if (rc == 0 && hdout != NULL) {
	if (hdout[0]) { 
	  domain = strdup(hdout[0]);
	  ptr = strchr(domain, '\n');
	  *ptr = '\0';
	}
	freeresultbuf(hdout);
      }
    }

    /* initializes CIM_HOST_NAME */
    if( strlen(host) ) {
      if( !domain ) {
	CIM_HOST_NAME = (char *)malloc( (strlen(host)+1)*sizeof(char) );
	strcpy( CIM_HOST_NAME, host);
      }
      else {
	CIM_HOST_NAME = (char *)malloc( (strlen(host)+strlen(domain)+2)*sizeof(char) );
	strcpy( CIM_HOST_NAME, host);
	strcat( CIM_HOST_NAME, ".");
	strcat( CIM_HOST_NAME, domain );
      }
    }

    if(host) free(host);
    if(domain) free(domain);

    if( _debug ) { fprintf(stderr,"CIM_HOST_NAME initialized with %s\n",CIM_HOST_NAME); }
  }
  return CIM_HOST_NAME;
}

/* returns the primary owner of the system ... always root ;-) */
char * get_cs_primownername() {
  char * ptr = NULL;
  if( _debug ) { fprintf(stderr, "--- %s : get_cs_primownername()\n",_FILENAME); }
  ptr = (char*)malloc(5*sizeof(char));
  strcpy(ptr,"root");
  return ptr;
}

/* returns the contact address of the primary owner of the system ... 
 * always root@host.domain
 */
char * get_cs_primownercontact() {
  char * ptr = NULL;
  char * own = NULL;
  if( _debug ) { fprintf(stderr, "--- %s : get_cs_primownercontact()\n",_FILENAME); }
  if( (own = get_cs_primownername()) != NULL ) {
    ptr = (char*)malloc( (strlen(own)+strlen(CIM_HOST_NAME)+2)*sizeof(char));
    strcpy( ptr, own);
    strcat( ptr,"@");
    strcat( ptr, CIM_HOST_NAME);
    if(own) free(own);
    return ptr;
  }
  return NULL;
}

// -----------------------------------------------------------------------------
// end of class Linux_ComputerSystem
// -----------------------------------------------------------------------------



// -----------------------------------------------------------------------------
// class Linux_OperatingSystem
// -----------------------------------------------------------------------------

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

char * get_os_name(){

  if( _debug && !CIM_OS_NAME ) 
    { fprintf(stderr, "--- %s : get_os_name()\n",_FILENAME); }

  if( !CIM_OS_NAME ) { 
    get_system_name();
    if( CIM_HOST_NAME ) {
      CIM_OS_NAME = (char*) malloc (strlen(CIM_HOST_NAME)+1);
      strcpy( CIM_OS_NAME, CIM_HOST_NAME );
    }

    if( _debug ) { fprintf(stderr,"CIM_OS_NAME initialized with %s\n",CIM_OS_NAME); }
  }
  return CIM_OS_NAME;
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

signed short get_os_timezone() {
  char ** hdout = NULL;
  int     rc    = 0;

  if( _debug ) { fprintf(stderr, "--- %s : get_os_timezone()\n",_FILENAME); }   
    
  if( CIM_OS_TIMEZONE == 999 ) {
    rc = runcommand( "date +%z" , NULL , &hdout , NULL );
    if( rc == 0 ) {
      CIM_OS_TIMEZONE = atoi(hdout[0]);
      freeresultbuf(hdout);
    }
    else { 
      fprintf(stderr, "--- %s : command date +z not successful returned - Timezone set to 0\n",_FILENAME);
      CIM_OS_TIMEZONE = 0;
    }
  }
  return CIM_OS_TIMEZONE;
}

/* add timezone to datetime 'str'of format %Y%m%d%H%M%S.000000
 * out : yyyyMMddHHmmss.SSSSSSsutc, e.g. 20030112152345.000000+100
 */
void _cat_timezone( char * str , signed short zone ) {
  char * tz = NULL;

  tz = (char *)malloc(5*sizeof(char));
  sprintf(tz, "%i", zone);
  if( str != NULL ) {  
    if(zone > 0) {
      strcat(str,"+");
    }
    strcat(str,tz);
  }
  if(tz) free(tz);
}

unsigned long _get_os_boottime() {
  char ** hdout = NULL;
  int     rc    = 0;

  if( _debug ) { fprintf(stderr, "--- %s : get_os_boottime()\n",_FILENAME); }   

  if( CIM_OS_BOOTTIME == 0 ) {
    rc = runcommand( "cat /proc/stat | grep btime" , NULL , &hdout , NULL );
    if( rc == 0 ) {
      CIM_OS_BOOTTIME = atol(hdout[0]+6);
      freeresultbuf(hdout);
    }
    else { 
      fprintf(stderr, "--- %s : was not able to get boottime - set to 0\n",_FILENAME);
    }
  }
  return CIM_OS_BOOTTIME;
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

void free_os_data( struct cim_operatingsystem * sptr ) {
    if(sptr==NULL) return;
    if(sptr->installDate) free(sptr->installDate);
    if(sptr->lastBootUp) free(sptr->lastBootUp);
    if(sptr->localDate) free(sptr->localDate);
    free(sptr);
}

// -----------------------------------------------------------------------------
// end of class Linux_OperatingSystem
// -----------------------------------------------------------------------------



// -----------------------------------------------------------------------------
// class Linux_Process
// -----------------------------------------------------------------------------

static int _process_data( char * pid, struct cim_process ** sptr );
static char * _get_process_execpath( char * id, char * cmd );

/* returns a NULL terminated list of pointers; each listentry contains a pointer
 * to a structure, which contains necessary information about the currently
 * running processes
 */
int enum_all_process( struct processlist ** lptr ){
  struct processlist *  lptrhelp = NULL;
  char               ** hdout    = NULL;
  char               *  ptr      = NULL;
  int                   rc       = 0;
  int                   i        = 0;
  
  if( _debug ) { fprintf(stderr, "--- %s : enum_all_process()\n",_FILENAME); }  
  
  rc = runcommand( "ps --no-headers -eo pid,ppid,tty,pri,nice,uid,gid,pmem,pcpu,cputime,comm,session,state,args" , NULL, &hdout, NULL );

  if( rc == 0 ) {
    lptrhelp = (struct processlist *) malloc (sizeof(struct processlist));
    memset(lptrhelp, 0, sizeof(struct processlist));
    *lptr = lptrhelp;
    
    while( hdout[i] ) {
      if ( lptrhelp->p != NULL) { 
	lptrhelp->next = (struct processlist *) malloc (sizeof(struct processlist));
	memset(lptrhelp->next, 0, sizeof(struct processlist));
	lptrhelp = lptrhelp->next;
      }    
      ptr = strchr(hdout[i], '\n');
      *ptr = '\0';
      rc = _process_data( hdout[i] , &(lptrhelp->p) );
      i++;
    }
  }
  freeresultbuf( hdout );
  return rc;
}

int get_process_data( char * pid , struct cim_process ** sptr ) {
  char ** hdout = NULL;
  char *  cmd   = NULL;
  char *  str   = NULL;
  char *  ptr   = NULL;
  DIR  *  dpid  = NULL;
  int     i     = 0;
  int     rc    = 0;

  if( _debug ) { fprintf(stderr, "--- %s : get_process()\n",_FILENAME); }   

  cmd = (char*)malloc((strlen(pid)+7)*sizeof(char));
  strcpy(cmd, "/proc/");
  strcat(cmd, pid);
  dpid=opendir(cmd);
  if(cmd) free(cmd);

  if ( dpid != NULL ) {
    cmd = (char*)malloc((strlen(pid)+94)*sizeof(char));
    strcpy(cmd, "ps --no-headers -eo pid,ppid,tty,pri,nice,uid,gid,pmem,pcpu,cputime,comm,session,state | grep ");
    strcat(cmd, pid);
    rc = runcommand( cmd , NULL, &hdout, NULL );
    while( hdout[i] ) {
      ptr = strchr(hdout[i], '\n');
      *ptr = '\0';
      str = hdout[i];
      while( *str == ' ' ) { str = str+1; }
      if( strncmp( str,pid,strlen(pid)) == 0 ) {
	rc = _process_data( hdout[i], sptr );
	break;
      }
      i++;
    }
    if(cmd) free(cmd);
    freeresultbuf( hdout );
    closedir(dpid);
  }
  else { 
    if( _debug ) { fprintf(stderr, "--- get_process_data() --- PID %s not valid\n", pid); }
    return -1 ;
  }
  return rc;
}

static int _process_data( char * phd , struct cim_process ** sptr ){
  char               ** parr   = NULL;
  char               ** hdout  = NULL;
  char               *  d      = NULL;
  char               *  end    = NULL;
  char               *  cmd    = NULL;
  char               *  ptr    = NULL;
  FILE               *  fpstat = NULL;
  struct tm          *  tmdate = NULL;
  unsigned long long    kmtime = 0;        // Kernel Mode Time
  unsigned long long    umtime = 0;        // User Mode Time
  unsigned long         ctime  = 0;        // Start Time
  unsigned long         uptime = 0;
  int                   i      = 0;
  int                   j      = 0;
  int                   rc     = 0;

  if( _debug ) { fprintf(stderr, "--- %s : _process_data()\n",_FILENAME); }   

  (*sptr) = (struct cim_process *) malloc (sizeof(struct cim_process));
  memset((*sptr), 0, sizeof(struct cim_process));

  parr = calloc(1000,sizeof(char*));    
  end = phd + strlen(phd);
  while(phd < end) {
      d=strchr(phd,' ');
      if (d!=NULL)
	  *d=0;
      if (strlen(phd) > 0)
	  parr[i++]=strdup(phd);
      phd+=strlen(phd)+1;
  }
  
  /* Process related */
  (*sptr)->pid = strdup(parr[0]);

  (*sptr)->ppid = strdup(parr[1]);
  (*sptr)->ptty = strdup(parr[2]);
  (*sptr)->prio = atol(parr[3]);
  (*sptr)->nice = atol(parr[4]);

  (*sptr)->uid  = atoll(parr[5]);
  (*sptr)->gid  = atoll(parr[6]);

  (*sptr)->pmem = atoll(parr[7]);
  (*sptr)->pcpu = atol(parr[8]);

  (*sptr)->pcmd = strdup(parr[10]);
  (*sptr)->sid = atoll(parr[11]);

  /* state of Linux processes :
     D   uninterruptible sleep (usually IO) = Blocked (4)
     R   runnable (on run queue)            = Running (3)
     S   sleeping                           = Suspended Ready (6)
     T   traced or stopped                  = Stopped (8)
     Z   a defunct ("zombie") process       = Terminated (7)
  */
  /* Mapping between ExecutionState and integer value */
  /* 0 ... Unknown
   * 1 ... Other
   * 2 ... Ready
   * 3 ... Running
   * 4 ... Blocked
   * 5 ... Suspended Blocked
   * 6 ... Suspended Ready
   * 7 ... Terminated
   * 8 ... Stopped
   * 9 ... Growing
   */
  if(strcmp(parr[12],"R")==0)
    (*sptr)->state = 3 ;
  else if( strcmp(parr[12],"D")==0)
    (*sptr)->state = 4 ;
  else if( strcmp(parr[12],"S")==0)
    (*sptr)->state = 6 ;
  else if( strcmp(parr[12],"Z")==0)
    (*sptr)->state = 7 ;
  else if( strcmp(parr[12],"T")==0)
    (*sptr)->state = 8 ;

  /* values for array of Parameters */
  if( parr[13] == NULL ) {
    cmd = (char*)malloc((strlen((*sptr)->pid)+28)*sizeof(char));
    memset(cmd, 0, (strlen((*sptr)->pid)+28)*sizeof(char));
    strcpy(cmd, "ps -p");
    strcat(cmd, (*sptr)->pid);
    strcat(cmd, " --no-headers -o args");
    rc = runcommand( cmd, NULL, &hdout, NULL );
    ptr = strchr(hdout[0], '\n');
    *ptr = '\0';
    if( rc == 0 ) {
      (*sptr)->args = line_to_array( hdout[0], ' ' );
    }
    if(cmd) free(cmd);
    freeresultbuf(hdout);
  }
  else {
    i=13;
    j=0;
    (*sptr)->args = calloc(100,sizeof(char*));  
    while( parr[i] != NULL ) {
      (*sptr)->args[j]=strdup(parr[i]);
      //     fprintf(stderr,"(*sptr)->args[%i] : %s\n",j,(*sptr)->args[j]);
      j++;
    i++;
    }
  }

  freeresultbuf(parr);
  
  /* ModulePath */
  //  (*sptr)->path = _get_process_execpath((*sptr)->pcmd);
  (*sptr)->path = _get_process_execpath((*sptr)->pid,(*sptr)->pcmd);

  /* get UserModeTime, KernelModeTime and CreationDate */
  cmd = (char*)malloc( (strlen((*sptr)->pid)+12)*sizeof(char));
  strcpy( cmd, "/proc/");
  strcat( cmd, (*sptr)->pid);
  strcat( cmd, "/stat");

  if ( (fpstat=fopen(cmd,"r")) != NULL ) {
    fscanf(fpstat,"%*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s "
	   "%lld %lld %*s %*s %*s %*s %*s %ld", 
	   &umtime,&kmtime,&ctime );
    fclose(fpstat);
    (*sptr)->kmtime = kmtime*10;
    (*sptr)->umtime = umtime*10;

    /* char * createdate : yyyyMMddHHmmss.SSSSSSsutc , e.g. 20030112152345.000000+100 */
    uptime = _get_os_boottime();
    if( uptime == 0 ) { (*sptr)->createdate = NULL; }
    else {
      ctime = (ctime/100)+uptime;
      tmdate = gmtime( &ctime );
      (*sptr)->createdate = (char*)malloc(26+sizeof(char));
      rc = strftime((*sptr)->createdate,26,"%Y%m%d%H%M%S.000000",tmdate);
      _cat_timezone((*sptr)->createdate, get_os_timezone());
    }
    //    fprintf(stderr,"(*sptr)->createdate: %s\n",(*sptr)->createdate);
    /*    fprintf(stderr,"PID: %s ... um:%lli; km:%lli;\num:%lli; km:%lli; cd_sec:%li\n",
    	    (*sptr)->pid,umtime,kmtime,(*sptr)->umtime,(*sptr)->kmtime,(*sptr)->createtime);
    */
  }
  else { (*sptr)->createdate = NULL; }
  if(cmd) free(cmd);

  return 0;
}

static char * _get_process_execpath( char * id , char * cmd ) {
  char *  buf   = NULL;
  char *  path  = NULL;
  int     rc    = 0;  

  if( _debug ) { fprintf(stderr, "--- %s : _get_process_execpath()\n",_FILENAME); }  

  path = (char*)malloc((strlen(id)+11)*sizeof(char));
  strcpy(path, "/proc/");
  strcat(path, id);
  strcat(path, "/exe");

  buf = (char*)malloc(1024*sizeof(char));
  memset(buf, 0, 1024*sizeof(char));

  rc = readlink(path, buf, 1024*sizeof(char));
  if( rc == -1 ) {
    free(buf);
    buf = strdup(cmd);
  }
  return buf; 
}

void free_processlist( struct processlist * lptr ) {
  struct processlist * ls = NULL ;

  if( lptr == NULL ) return;
  for( ; lptr ; ) {
    if(lptr->p) { free_process(lptr->p); }
    ls = lptr;
    lptr = lptr->next;
    free(ls);
  }
}

void free_process( struct cim_process * sptr ) {
  if( sptr == NULL ) return;
  if(sptr->pid)    free(sptr->pid); 
  if(sptr->ppid)   free(sptr->ppid); 
  if(sptr->ptty)   free(sptr->ptty); 
  if(sptr->pcmd)   free(sptr->pcmd);
  if(sptr->path)   free(sptr->path);
  if(sptr->createdate) free(sptr->createdate);
  freeresultbuf(sptr->args);
  free(sptr);
}

/*
 * get system parameter from a file entry, such as /proc/sys/kernel/shmmax
 * and put it in the character array pointed by buffer
 * It returns the number of bytes read, or -1 in case of any error
 */
int get_system_parameter(char *path, char *entry, char *buffer, int size) {
  char * completePath = NULL;
  FILE * file         = NULL;
  int    res          = 0;

  if( _debug ) { fprintf(stderr, "--- %s : get_system_parameter()\n",_FILENAME); }  

  if (path == NULL || entry == NULL || buffer == NULL)
    return -1;
  completePath = (char *)malloc(strlen(path) + strlen(entry) + 1);
  if (completePath == NULL)
    return -1;
  sprintf(completePath, "%s/%s", path, entry);
  if ((file = fopen(completePath, "r")) == NULL)
    return -1;
  res = fread(buffer, 1, size - 1, file);
  fclose(file);
  if (res > 0)
    buffer[res] = '\0';
  free(completePath);
  return res;
}

/*
 * set system parameter to a file entry, such as /proc/sys/kernel/shmmax
 * using the value string 
 * It returns the number of bytes written, or -1 in case of any error
 */
int set_system_parameter(char *path, char *entry, char *value) {
  char * completePath = NULL;
  FILE * file         = NULL;
  int    res          = 0;

  if( _debug ) { fprintf(stderr, "--- %s : set_system_parameter()\n",_FILENAME); }  

  if (path == NULL || entry == NULL || value == NULL)
    return -1;
  completePath = (char *)malloc(strlen(path) + strlen(entry) + 1);
  if (completePath == NULL)
    return -1;
  sprintf(completePath, "%s/%s", path, entry);
  if ((file = fopen(completePath, "w")) == NULL)
    return -1;
  res = fwrite(value, 1, strlen(value), file);
  fclose(file);
  free(completePath);
  return res;
}

// -----------------------------------------------------------------------------
// end of class Linux_Process
// -----------------------------------------------------------------------------



// -----------------------------------------------------------------------------
// class Linux_Processor
// -----------------------------------------------------------------------------

static int _processor_data( int, struct cim_processor ** );
static unsigned short _processor_family( int );
static unsigned short _processor_load_perc( int );

int enum_all_processor( struct processorlist ** lptr ) {
  struct processorlist *  lptrhelp = NULL;
  char                 ** hdout    = NULL;
  char                 *  ptr      = NULL;
  char                 *  id       = NULL;
  int                     i        = 0;
  int                     rc       = 0;

  if( _debug ) { fprintf(stderr, "--- %s : enum_all_processor()\n",_FILENAME); }  

  lptrhelp = (struct processorlist *) malloc (sizeof(struct processorlist));
  memset(lptrhelp, 0, sizeof(struct processorlist));
  *lptr = lptrhelp;

  rc = runcommand( "cat /proc/cpuinfo | grep ^processor | sed -e s/processor//" , NULL , &hdout , NULL );
  if( rc == 0 ) {
    while( hdout[i] != NULL ) { 
      if ( lptrhelp->sptr != NULL) { 
	lptrhelp->next = (struct processorlist *) malloc (sizeof(struct processorlist));
	memset(lptrhelp->next, 0, sizeof(struct processorlist));
	lptrhelp = lptrhelp->next;
      }

      ptr = hdout[i];
      ptr = strchr( ptr , ':' );
#ifdef INTEL
      id = ptr++;
#elif S390
      id = (char*)malloc( (strlen(hdout[i])-strlen(ptr)+1)*sizeof(char) );
      id = strncpy(id, hdout[i], strlen(hdout[i])-strlen(ptr));
#endif
      rc = _processor_data( atoi(id), &(lptrhelp->sptr) );
      i++;
    }
    freeresultbuf(hdout);
  }

  return rc;
}

int get_processor_data( char * id, struct cim_processor ** sptr ) {
  char ** hdout = NULL;
  int     i     = 0;
  int     rc    = 0;

  if( _debug ) { fprintf(stderr, "--- %s : get_processor_data()\n",_FILENAME); }  

  rc = runcommand( "cat /proc/cpuinfo | grep ^processor" , NULL , &hdout , NULL );
  if( rc == 0 ) {
    while( hdout[i] != NULL ) {
      if( strstr(hdout[i],id) != NULL ) { 
	rc = _processor_data( atoi(id), sptr ); 
	break;
      }
      i++;
    }
    freeresultbuf(hdout);
  }
  return rc;
}

static int _processor_data( int id, struct cim_processor ** sptr ) {
  char ** hdout = NULL;
  char *  ptr   = NULL;
  int     rc    = 0;

  if( _debug ) { fprintf(stderr, "--- %s : _processor_data()\n",_FILENAME); }  

  *sptr = (struct cim_processor *) malloc (sizeof(struct cim_processor));
  memset(*sptr, 0, sizeof(struct cim_processor));

  (*sptr)->id = (char *) malloc (20*sizeof(char));
  sprintf((*sptr)->id, "%i", id);

  (*sptr)->stat = 1;

  /* Familiy */
  (*sptr)->family = _processor_family(id); 

  /* Stepping */
#ifdef INTEL 
  rc = runcommand( "cat /proc/cpuinfo | grep stepping" , NULL , &hdout , NULL );
  if( rc == 0 ) {
    ptr = strrchr( hdout[id], ' ');
    (*sptr)->step = (char*)malloc( (17+strlen((*sptr)->id)+strlen(ptr))*sizeof(char) );
    strcpy((*sptr)->step, "stepping level: ");
    strcat((*sptr)->step, ptr);
    ptr = strchr((*sptr)->step, '\n');
    *ptr = '\0';
  }
  freeresultbuf(hdout);
  rc = 0;
#elif S390
  (*sptr)->step = (char*)malloc( 12*sizeof(char) );
  strcpy((*sptr)->step,"no stepping");
#endif

  /* ElementName */
#ifdef INTEL 
  rc = runcommand( "cat /proc/cpuinfo | grep 'model name'" , NULL , &hdout , NULL );
#elif S390
  rc = runcommand( "cat /proc/cpuinfo | grep 'vendor_id'" , NULL , &hdout , NULL );
#endif
  if( rc == 0 ) {
    ptr = strchr( hdout[id], ':');
    ptr = ptr+2;
    (*sptr)->name = (char*)malloc( (1+strlen(ptr))*sizeof(char) );
    strcpy((*sptr)->name, ptr);
    ptr = strchr((*sptr)->name, '\n');
    *ptr = '\0';
  }
  freeresultbuf(hdout);
  rc = 0;

  /* LoadPercentage */
  (*sptr)->loadPct = _processor_load_perc(id); 

  /* MaxClockSpeed && CurrentClockSpeed */
#ifdef INTEL 
  rc = runcommand( "cat /proc/cpuinfo | grep 'cpu MHz'" , NULL , &hdout , NULL );
#elif S390
  rc = runcommand( "cat /proc/cpuinfo | grep 'bogomips per cpu'" , NULL , &hdout , NULL );
#endif
  if( rc == 0 ) {
    ptr = strchr( hdout[id], ':');
    ptr = ptr+1;
    (*sptr)->curClockSpeed = atol(ptr);
    (*sptr)->maxClockSpeed = atol(ptr);
  }
  freeresultbuf(hdout);

  return 0;
}

/* map model name of the processor 'id' to the respective representation in CIM */
static unsigned short _processor_family( int id ) {
  char **        hdout = NULL;
  unsigned short rv    = 0;
  int            rc    = 0;

  if( _debug ) { fprintf(stderr, "--- %s : _processor_family()\n",_FILENAME); }  

  rc = runcommand( "cat /proc/cpuinfo | grep vendor_id" , NULL , &hdout , NULL );
  if( rc == 0 ) {
    /* Intel Family */
    if( strstr( hdout[id], "Intel") != NULL ) {
      freeresultbuf(hdout);
      rc = runcommand( "cat /proc/cpuinfo | grep 'model name'" , NULL , &hdout , NULL );
      /* 486 */
      if(  strstr( hdout[id], "486") != NULL ) rv = 6; /* 80486 */
      /* Pentium */
      else if( strstr( hdout[id], "Pentium") != NULL ) {
	/* Pro */
	if( strstr( hdout[id], "Pro") != NULL ) rv = 12; /* Pentium(R) Pro */
	/* III */
	else if( strstr( hdout[id], "III") != NULL ) {
	  if( strstr( hdout[id], "Xeon") != NULL ) rv = 176; /* Pentium(R) III Xeon(TM) */
	  else if( strstr( hdout[id], "SpeedStep") != NULL ) rv = 177; /* Pentium(R) III Processor with Intel(R) SpeedStep(TM) Technology */
	  else rv = 17; /* Pentium(R) III */
	}
	/* II */
	else if( strstr( hdout[id], "II") != NULL ) {
	  if( strstr( hdout[id], "Xeon") != NULL ) rv = 16; /* Pentium(R) II Xeon(TM) */
	  else rv = 13; /* Pentium(R) II */
	}
	else if( strstr( hdout[id], "MMX") != NULL ) rv = 14; /* Pentium(R) Processor with MMX(TM) Technology */
	else if( strstr( hdout[id], "Celeron") != NULL ) rv = 15; /* Celeron(TM) */
	else if( strstr( hdout[id], "4") != NULL ) rv = 15; /* Celeron(TM) */
	else rv = 11; /* Pentium(R) Brand */
      }
      else rv = 1; /* Other */
    }

    /* AMD Family */
    else if( strstr( hdout[id], "AMD") != NULL ) {
      freeresultbuf(hdout);
      rc = runcommand( "cat /proc/cpuinfo | grep 'model name'" , NULL , &hdout , NULL );
      /* 486 */
      if(  strstr( hdout[id], "486") != NULL ) rv = 24; /* AMD Duron(TM) Processor Family */
      /*  */
      else if( strstr( hdout[id], "K5") != NULL ) rv = 25; /* K5 Family */
      else if( strstr( hdout[id], "K6-2") != NULL ) rv = 27; /* K6-2 */
      else if( strstr( hdout[id], "K6-3") != NULL ) rv = 28; /* K6-3 */
      else if( strstr( hdout[id], "K6") != NULL ) rv = 26; /* K6 Family */
      else if( strstr( hdout[id], "Athlon") != NULL ) rv = 26; /* AMD Athlon(TM) Processor Family */
    }

    /* S390 Family */
    else if( strstr( hdout[id], "S390") != NULL ) {
      rv = 200;
    }
  }
  else rv = 2; /* Unknown */
  freeresultbuf(hdout);
  return rv;
}

/* calculates the load of the processor 'id' in percent */
static unsigned short _processor_load_perc( int id ) {
  char ** hdout = NULL;
  char ** data  = NULL;
  char *  sid   = NULL;
  char *  cmd   = NULL;
  unsigned long  load      = 0;
  unsigned long  loadTotal = 0;
  unsigned short loadPct   = 0;
  int            rc        = 0;

  if( _debug ) { fprintf(stderr, "--- %s : _processor_load_perc()\n",_FILENAME); }  

  sid = (char *) malloc (5*sizeof(char));
  sprintf(sid, "%i", id);

  cmd = (char*)malloc( (26+strlen(sid))*sizeof(char) );
  strcpy(cmd, "cat /proc/stat | grep cpu");
  strcat(cmd, sid);
  rc = runcommand( cmd , NULL , &hdout , NULL );
  if(cmd) free(cmd);
  if( rc == 0 ) {
    data = line_to_array( hdout[0], ' ');
    loadTotal = atol(data[1])+atol(data[2])+atol(data[3])+atol(data[4]);
    load = loadTotal - atol(data[4]);
    loadPct = 100*load/loadTotal;
    freeresultbuf(data);
  }
  freeresultbuf(hdout);
  if(sid) free(sid);

  return loadPct;
}


void free_processorlist( struct processorlist * lptr ) {
  struct processorlist * ls = NULL ;

  if( lptr == NULL ) return;
  for( ; lptr ; ) {
    if(lptr->sptr) { free_processor(lptr->sptr); }
    ls = lptr;
    lptr = lptr->next;
    free(ls);
  }
}

void free_processor( struct cim_processor * sptr ) {
  if( sptr == NULL ) return;
  if(sptr->id) free(sptr->id); 
  if(sptr->step) free(sptr->step); 
  if(sptr->name) free(sptr->name); 
  free(sptr);
}


// -----------------------------------------------------------------------------
// end of class Linux_Processor
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// tool function : executes commandlines and returns their output
// ( stdout and stderr ) 
// -----------------------------------------------------------------------------


static void addstring(char ***buf, const char *str);

int runcommand(const char *cmd, char **in, char ***out, char ***err)
{
  int rc;
  char fcltinname[] = "/tmp/SBLIMXXXXXX";
  char fcltoutname[] = "/tmp/SBLIMXXXXXX";
  char fclterrname[] = "/tmp/SBLIMXXXXXX";
  int fdin, fdout, fderr;
  char * cmdstring;
  char * buffer;
  FILE * fcltin;
  FILE * fcltout;
  FILE * fclterr;

  /* No pipes -- real files */

  /* make the filenames for I/O redirection */
  if (in != NULL && ((fdin=mkstemp(fcltinname)) < 0 || close(fdin))) {
    perror("Couldn't create input file");
    return -1;
  }
  if (out != NULL && ((fdout=mkstemp(fcltoutname)) < 0 || close(fdout))) {
    perror("Couldn't create output file");
    return -1;
  }
  if (err != NULL && ((fderr=mkstemp(fclterrname)) < 0 || close(fderr))) {
    perror("Couldn't create error file");
    return -1;
  }

  /* if necessary feed in-buffer to child's stdin */
  if (in != NULL) {
    fcltin = fopen(fcltinname,"w");
    if (fcltin==NULL) {
      perror("Couldn't open client's input for writing");
      return -1;
    }
    while (*in != NULL)
      fprintf(fcltin,"%s\n",*in++);
    /* flush and close */
    fclose(fcltin);
  }

  /* prepare full command string */
  cmdstring = malloc(strlen(cmd) + 1 +
		     (in?strlen(fcltinname)+2:0) + 
		     (out?strlen(fcltoutname)+3:0) + 
		     (err?strlen(fclterrname)+3:0));
  strcpy(cmdstring,cmd);
  if (in) {
    strcat(cmdstring," <");
    strcat(cmdstring,fcltinname);
  }
  if (out) {
    strcat(cmdstring," >");
    strcat(cmdstring,fcltoutname);
  }
  if (err) {
    strcat(cmdstring," 2>");
    strcat(cmdstring,fclterrname);
  }
  /* perform the system call */
  if( _debug ) { fprintf(stderr,"runcommand: %s\n",cmdstring); }
  rc=system(cmdstring);
  free(cmdstring);
  
  buffer = NULL;
  /* cleanup in file. if necessary */
  if (in != NULL)
    unlink(fcltinname);
  /* retrieve data from stdout and stderr */
  if (out != NULL) {
    *out=calloc(1,sizeof(char*));
    buffer = malloc(4096);
    fcltout = fopen(fcltoutname,"r");
    if (fcltout!=NULL) {
      while(!feof(fcltout)) {
	if (fgets(buffer,4096,fcltout) != NULL)
	  addstring(out,buffer);
	else
	  break;
      }
      /* close out file */
      fclose(fcltout);
    } else
      perror("Couldn't open client's output for reading");    
    unlink(fcltoutname);
  }
  if (err != NULL) {
    *err=calloc(1,sizeof(char*));
    if (buffer == NULL)
      buffer = malloc(4096);
    fclterr = fopen(fclterrname,"r");
    if (fclterr!=NULL) {
	while(!feof(fclterr)) {
	  if (fgets(buffer,4096,fclterr) != NULL)
	    addstring(err,buffer);
	  else
	    break;
	}
	/* close err file */
	fclose(fclterr);
    } else
      perror("Couldn't open client's error output for reading");    
    unlink(fclterrname);
  }
  if (buffer != NULL)
    free(buffer);
  return rc;
}
  
void freeresultbuf(char ** buf)
{
  char ** bufstart = buf;
  if (buf == NULL)
    return;
  while(*buf != NULL) {
    free(*buf++);
  }
  free(bufstart);
}

static void addstring(char *** buf, const char * str)
{
  int i=0;
  while ((*buf)[i++] != NULL);
  *buf=realloc(*buf,(i+1)*sizeof(char*));
  (*buf)[i-1] = strdup(str);
  (*buf)[i] = NULL;
}

// -----------------------------------------------------------------------------
// end of tool function to executes commandlines
// -----------------------------------------------------------------------------



// -----------------------------------------------------------------------------
// tool function to convert ":"-seperated lines
// -----------------------------------------------------------------------------

/* converts a line into an array of char * , while
 * c ( e.g. ':' ) acts as seperator 
 *
 * example : 
 * line  -> value1:value2:value3'\n'
 *
 * array -> value1
 *          value2
 *          value3
 *          NULL
 */

char ** line_to_array( char * buf , int c ){
  char ** data = NULL ;
  char *  str  = NULL ;
  char *  ptr  = NULL ;
  char *  ent  = NULL ;
  
  str = strdup(buf);
  data = calloc(1,sizeof(char*));
  ent = str ;
  
  while( ( ptr = strchr( ent , c )) != NULL ) {
    *ptr='\0';
    addstring( &data , ent );
    ent = ptr+1;
    ptr = NULL ;
  }
  addstring( &data , ent );
  
  if(str) free(str);
  return data;
}

// -----------------------------------------------------------------------------
// end of tool function to convert ":"-seperated lines
// -----------------------------------------------------------------------------
