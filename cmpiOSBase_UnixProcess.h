#ifndef _CMPIOSBASE_UNIXPROCESS_H_
#define _CMPIOSBASE_UNIXPROCESS_H_

/*
 * cmpiOSBase_UnixProcess.h
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
 * Author:       
 * Contributors:
 *
 * Interface Type : Common Magabeablity Programming Interface ( CMPI )
 *
 * Description:
 * This file defines the interfaces for the factory implementation of the
 * CIM class Linux_UnixProcess.
 *
*/


/* ---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "cmpidt.h"
#include "OSBase_UnixProcess.h"

/* ---------------------------------------------------------------------------*/

static char * _ClassName = "Linux_UnixProcess";

/* ---------------------------------------------------------------------------*/

#ifdef MODULE_SYSMAN

typedef struct _SysMan {
  unsigned long maxChildProc;
  unsigned long maxOpenFiles;
  unsigned long maxRealStack;
} SysMan;

int           moduleInstalled = 0;
static char   pathBuffer[PATH_MAX + 1]; 
static char * dirPath  = "/proc/sysman";
static char * fileName = "pid_rlimit";

int _sysman_data( char * pid , struct _SysMan * sm );

#endif

/* ---------------------------------------------------------------------------*/


/* method to create a CMPIObjectPath of this class                            */

CMPIObjectPath * _makePath( CMPIBroker * _broker,
                            CMPIContext * ctx, 
                            CMPIObjectPath * cop,
			    struct cim_process * sptr,
                            CMPIStatus * rc);


/* method to create a CMPIInstance of this class                              */

CMPIInstance * _makeInst( CMPIBroker * _broker,
                          CMPIContext * ctx, 
                          CMPIObjectPath * cop,
			  struct cim_process * sptr,
                          CMPIStatus * rc);


/* ---------------------------------------------------------------------------*/

#endif

