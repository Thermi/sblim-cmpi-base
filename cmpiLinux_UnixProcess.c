/*
 * Linux_UnixProcess.c
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

#include "mlogsup.h"
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

static char * _ClassName = "Linux_UnixProcess";

/* private methods                                                            */
/*                                                                            */

/* - method to create a CMPIObjectPath of this class                          */
static CMPIObjectPath * _makePath( CMPIObjectPath * cop,
				   struct cim_process * sptr,
				   CMPIStatus * rc);

/* - method to create a CMPIInstance of this class                            */
static CMPIInstance * _makeInst( CMPIObjectPath * cop,
				 struct cim_process * sptr,
				 CMPIStatus * rc);
/* ---------------------------------------------------------------------------*/

#ifdef MODULE_SYSMAN

typedef struct _SysMan {
  unsigned long maxChildProc;
  unsigned long maxOpenFiles;
  unsigned long maxRealStack;
} SysMan;

static char   pathBuffer[PATH_MAX + 1];
static char   tempBuffer[PATH_MAX + 1];
static char   makeBuffer[PATH_MAX + 1];  
static char * dirPath  = "/proc/sysman";
static char * fileName = "pid_rlimit";
static char * fileEntry = "/proc/sysman/pid_rlimit";
static int    moduleInstalled = 0;

static void _sysman_init();
static int _sysman_data( char * pid , struct _SysMan * sm );
static int _sysman_CI( CMPIInstance * ci , char * pid );
static char *searchPath(char * paths, char * entry);
static int installModule(char *namep);


#endif


/* ---------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------*/
/*                      Instance Provider Interface                           */
/* ---------------------------------------------------------------------------*/


CMPIStatus Linux_UnixProcessCleanup( CMPIInstanceMI * mi, 
				     CMPIContext * ctx) {
  CMReturn(CMPI_RC_OK);
}

CMPIStatus Linux_UnixProcessEnumInstanceNames( CMPIInstanceMI * mi, 
					       CMPIContext * ctx, 
					       CMPIResult * rslt, 
					       CMPIObjectPath * ref) {
  CMPIObjectPath     * op    = NULL;  
  CMPIStatus           rc    = {CMPI_RC_OK, NULL};
  struct processlist * lptr  = NULL;
  struct processlist * rm    = NULL;
  int                  cmdrc = 0;
  
  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI EnumInstanceNames()\n", _ClassName );

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
      op = _makePath( ref,lptr->p, &rc );
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
  CMReturn( CMPI_RC_OK );
}

CMPIStatus Linux_UnixProcessEnumInstances( CMPIInstanceMI * mi, 
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
    fprintf( stderr, "--- %s.c : CMPI EnumInstances()\n", _ClassName ); 
  
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
      ci = _makeInst( ref, lptr->p, &rc );
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
  CMReturn( CMPI_RC_OK );
}


CMPIStatus Linux_UnixProcessGetInstance( CMPIInstanceMI * mi, 
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
    fprintf( stderr, "--- %s.c : CMPI GetInstance()\n", _ClassName );

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

  ci = _makeInst( cop, sptr, &rc );
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
  CMReturn(CMPI_RC_OK);
}

CMPIStatus Linux_UnixProcessCreateInstance( CMPIInstanceMI * mi, 
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

CMPIStatus Linux_UnixProcessSetInstance( CMPIInstanceMI * mi, 
					 CMPIContext * ctx, 
					 CMPIResult * rslt, 
					 CMPIObjectPath * cop,
					 CMPIInstance * ci, 
					 char **properties) {
#ifdef MODULE_SYSMAN
  struct cim_process * sptr  = NULL;
  CMPIString         * pid   = NULL;
  CMPIData             data;
  CMPIStatus           rc    = {CMPI_RC_OK, NULL};
  struct _SysMan       sm;
  int                  cmdrc = 0;
#endif

  if(_debug)
    fprintf(stderr,"--- %s.c : CMPI SetInstance()\n", _ClassName);

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

  ci = _makeInst( cop, sptr, &rc );
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
#endif

  CMReturn(CMPI_RC_OK);
}

CMPIStatus Linux_UnixProcessDeleteInstance( CMPIInstanceMI * mi,
					    CMPIContext * ctx, 
					    CMPIResult * rslt, 
					    CMPIObjectPath * cop) {  
  CMPIStatus rc = {CMPI_RC_OK, NULL};

  if(_debug)
    fprintf(stderr,"--- %s.c : CMPI DeleteInstance()\n", _ClassName);

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "NOT_SUPPORTED" ); 
  return rc;
}

CMPIStatus Linux_UnixProcessExecQuery( CMPIInstanceMI * mi, 
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


CMPIStatus Linux_UnixProcessMethodCleanup( CMPIMethodMI * mi, 
					   CMPIContext * ctx) {
  CMReturn(CMPI_RC_OK);
}

CMPIStatus Linux_UnixProcessInvokeMethod( CMPIMethodMI * mi,
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
    fprintf( stderr, "--- %s.c : CMPI InvokeMethod()\n", _ClassName );
  
  valrc.uint8 = 0;

  class = CMGetClassName(ref, &rc);

  /* "terminate" */
  if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 && 
      strcasecmp(methodName, "terminate") == 0 ) {

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
  }

  CMReturnData( rslt, &valrc, CMPI_uint8);
  CMReturnDone( rslt );
  CMReturn(CMPI_RC_OK);
}



/* ---------------------------------------------------------------------------*/
/*                              private methods                               */
/* ---------------------------------------------------------------------------*/


/* ---------- method to create a CMPIObjectPath of this class ----------------*/

static CMPIObjectPath * _makePath( CMPIObjectPath * ref,
				   struct cim_process * sptr,
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
  CMAddKey(op, "OSCreationClassName", OSCreationClassName, CMPI_chars);
  CMAddKey(op, "OSName", CIM_OS_NAME, CMPI_chars);
  CMAddKey(op, "CreationClassName", _ClassName, CMPI_chars);
  CMAddKey(op, "Handle", sptr->pid, CMPI_chars);

 exit:
  return op;
}


/* ----------- method to create a CMPIInstance of this class ----------------*/

static CMPIInstance * _makeInst( CMPIObjectPath * ref,
				 struct cim_process * sptr,
				 CMPIStatus * rc) {
  CMPIObjectPath *  op     = NULL;
  CMPIInstance   *  ci     = NULL;
  CMPIDateTime   *  cdt    = NULL;
  CMPIArray      *  args   = NULL;
  CMPIString     *  val    = NULL;
  int               i      = 0;
  int               max    = 0;
#ifndef CIM26COMPAT
  unsigned short    status  = 2; /* Enabled */
#endif
#ifdef MODULE_SYSMAN
  int               sysrc  = 0;

  if (!moduleInstalled) { _sysman_init(); }
#endif

  DBGM0("_makeInst()");

  /* tool method call to get the unique name of the system */
  get_system_name();
  if( !CIM_HOST_NAME ) {
    DBGM0("_makeInst() no hostname found ");
    CMSetStatusWithChars( _broker, rc,
			    CMPI_RC_ERR_FAILED, "no host name found" );
    goto exit;
  }
  DBGM1("_makeInst() host: %s",CIM_HOST_NAME);

  get_os_name();
  if( !CIM_OS_NAME ) {
    CMSetStatusWithChars( _broker, rc,
			  CMPI_RC_ERR_FAILED, "no OS name found" );
    goto exit;
  }
  DBGM1("_makeInst() os: %s",CIM_OS_NAME);

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

  CMSetProperty( ci, "CSCreationClassName",CSCreationClassName , CMPI_chars );
  CMSetProperty( ci, "CSName", CIM_HOST_NAME, CMPI_chars );
  CMSetProperty( ci, "OSCreationClassName", OSCreationClassName, CMPI_chars );
  CMSetProperty( ci, "OSName", CIM_OS_NAME, CMPI_chars );
  CMSetProperty( ci, "CreationClassName", _ClassName, CMPI_chars );
  CMSetProperty( ci, "Handle", sptr->pid, CMPI_chars );

  CMSetProperty( ci, "Caption", "Linux (Unix) Process", CMPI_chars);
  CMSetProperty( ci, "Description", "This class represents instances of currently running programms.", CMPI_chars);

  CMSetProperty( ci, "Status", "NULL", CMPI_chars);

  CMSetProperty( ci, "ParentProcessID", sptr->ppid, CMPI_chars);
  CMSetProperty( ci, "ProcessTTY", sptr->ptty, CMPI_chars);
  CMSetProperty( ci, "Name", sptr->pcmd, CMPI_chars);
  CMSetProperty( ci, "ModulePath", sptr->path, CMPI_chars);

  CMSetProperty( ci, "Priority", (CMPIValue*)&(sptr->prio), CMPI_uint32);

  CMSetProperty( ci, "RealUserID", (CMPIValue*)&(sptr->uid), CMPI_uint64);
  CMSetProperty( ci, "ProcessGroupID", (CMPIValue*)&(sptr->gid), CMPI_uint64);
  CMSetProperty( ci, "ProcessSessionID", (CMPIValue*)&(sptr->sid), CMPI_uint64);
  CMSetProperty( ci, "ProcessNiceValue", (CMPIValue*)&(sptr->nice), CMPI_uint32);

  /* CIM Mapping between ExecutionState and int value */
  /* 0 ... Unknown
   * 1 ... Other
   * 2 ... Ready
   * 3 ... Running (supported)
   * 4 ... Blocked (supported)
   * 5 ... Suspended Blocked
   * 6 ... Suspended Ready (supported)
   * 7 ... Terminated (supported)
   * 8 ... Stopped (supported)
   * 9 ... Growing
   */
  CMSetProperty( ci, "ExecutionState", (CMPIValue*)&(sptr->state), CMPI_uint16);

  CMSetProperty( ci, "KernelModeTime", (CMPIValue*)&(sptr->kmtime), CMPI_uint64);
  CMSetProperty( ci, "UserModeTime", (CMPIValue*)&(sptr->umtime), CMPI_uint64);

  if( sptr->createdate != NULL ) {
    cdt = CMNewDateTimeFromChars(_broker,sptr->createdate,rc);
    CMSetProperty( ci, "CreationDate", (CMPIValue*)&(cdt), CMPI_dateTime);
  }

  /* Parameters */
  while( sptr->args[max] != NULL ) { max++; }
  args = CMNewArray(_broker,max,CMPI_stringA,rc);
  for( ; i< max; i++ ) {
    val = CMNewString(_broker,sptr->args[i],rc);
    CMSetArrayElementAt(args,i,(CMPIValue*)&(val),CMPI_string);
  }
  //  fprintf(stderr,"args %s\n",CMGetCharPtr(CDToString(_broker,args,rc)));
  CMSetProperty( ci, "Parameters", (CMPIValue*)&(args), CMPI_stringA);


  /* 2.7 */
#ifndef CIM26COMPAT
  CMSetProperty( ci, "ElementName", sptr->pcmd, CMPI_chars);
  CMSetProperty( ci, "EnabledStatus", (CMPIValue*)&(status), CMPI_uint16);
  CMSetProperty( ci, "OtherEnabledStatus", "NULL", CMPI_chars);
  CMSetProperty( ci, "RequestedStatus", (CMPIValue*)&(status), CMPI_uint16);
  CMSetProperty( ci, "EnabledDefault", (CMPIValue*)&(status), CMPI_uint16);
#endif

#ifdef MODULE_SYSMAN
  sysrc = _sysman_CI( ci, sptr->pid );
#endif	
  
 exit:
  return ci;
}


/* ---------------------- methods of module SYSMAN --------------------------*/

#ifdef MODULE_SYSMAN

static void _sysman_init() {
  FILE *file;
  if ((file = fopen(fileEntry, "r")) != NULL) {
    moduleInstalled = 1;
    fclose(file);
  } else
    moduleInstalled = installModule("sysman.o");
  if (moduleInstalled < 0) {
    fprintf(stderr, "Warning: kernel module sysman.o not found\n");
    moduleInstalled = 0;
  }
}

static int _sysman_data( char * pid , struct _SysMan * sm ) {
  char * tokp = NULL;

  sprintf(makeBuffer, "%s", pid);
  set_system_parameter(dirPath, fileName, makeBuffer);
  
  get_system_parameter(dirPath, fileName, makeBuffer, PATH_MAX + 1);
  tokp = strtok(makeBuffer, " \t");

  /* return if the pid is an invalid process id */
  if (strcmp(tokp, pid))
    return -1;

  tokp = strtok(0, " \t");
  sm->maxChildProc = atol(tokp);
  tokp = strtok(0, " \t");
  sm->maxOpenFiles = atol(tokp);
  tokp = strtok(0, " \t");
  sm->maxRealStack = atol(tokp);

  return 0;
}

static int _sysman_CI( CMPIInstance * ci , char * pid ) {
  struct _SysMan  sm;
  int             rc   = 0;

  rc = _sysman_data( pid , &sm );
  if( rc != 0 ) return -1;

  CMSetProperty( ci, "MaxNumberOfChildProcesses", (CMPIValue*)&(sm.maxChildProc), CMPI_uint32);
  CMSetProperty( ci, "MaxNumberOfOpenFiles", (CMPIValue*)&(sm.maxOpenFiles), CMPI_uint32);
  CMSetProperty( ci, "MaxRealStack", (CMPIValue*)&(sm.maxRealStack), CMPI_uint32);

  return 0;
}


/*
 * searchPath() searches paths defined in paths for the entry 
 */

static char *searchPath(char *paths, char *entry) {
  char *tokp;
  FILE *filp;
  
  tokp = (char *) strtok(paths, ", :");
  do {
    sprintf(pathBuffer, "%s/%s", tokp, entry);
    if ((filp = fopen(pathBuffer, "r")) != NULL)
      break;
  } while ((tokp = (char *) strtok(0, ", :")) != NULL);
  if (filp != NULL) {
    fclose(filp);
    return tokp;
  }
  return NULL;
}


/*
 * installModule() searches paths defined in MODPATH and LD_LIBRARY_PATH
 * for the module sysman.o
 */

static int installModule(char *namep) {
  int pid, status;
  char *cp, *sysdirp;
  
  sysdirp = getenv("MODPATH");
  cp = searchPath(sysdirp, namep);
  if (cp == NULL) {
    sysdirp = getenv("LD_LIBRARY_PATH");
    cp = searchPath(sysdirp, namep);
  }
  if (cp == NULL) return -1;
  pid = fork();
  if (pid == -1) return -1;
  if (pid == 0) { /* child */
    char *argv[4];

    argv[0] = "sh";
    argv[1] = "-c";
    sprintf(tempBuffer, "/sbin/insmod %s/%s", cp, namep);
    fprintf(stderr,"%s\n", tempBuffer);
    argv[2] = tempBuffer;
    argv[3] = 0;
    execve("/bin/sh", argv, NULL);
    exit(127);
  }
  if (waitpid(pid, &status, 0) == -1) return -1;
  return 1;
}


#endif


/* ---------------------------------------------------------------------------*/
/*                              Provider Factory                              */
/* ---------------------------------------------------------------------------*/

CMInstanceMIStub( Linux_UnixProcess,
		  Linux_UnixProcess,
		  _broker,
		  CMNoHook);

CMMethodMIStub( Linux_UnixProcess,
		Linux_UnixProcess,
		_broker,
		CMNoHook);

/* ---------------------------------------------------------------------------*/
/*                         end of Linux_UnixProcess                           */
/* ---------------------------------------------------------------------------*/

