#ifndef _OSBASE_UNIXPROCESS_H_
#define _OSBASE_UNIXPROCESS_H_

/*
 * OSBase_UnixProcess.h
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
 * Description: 
 * This file defines the interfaces for the resource access implementation 
 * of the CIM class Linux_UnixProcess.
 * 
*/

/* ---------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------------------*/

struct cim_process{
  char * pid ;                   // Handle (Process ID)
  char * ppid;                   // ParentProcessID
  char * ptty;                   // ProcessTTY
  char * pcmd;                   // Name
  char * path;                   // ModulePath
  char ** args;                  // Parameters
  char * createdate;             // CreationDate
  unsigned long prio ;           // Priority
  unsigned long nice ;           // ProcessNiceValue
  unsigned long long uid ;       // RealUserID
  unsigned long long gid ;       // ProcessGroupID
  unsigned long long sid ;       // ProcessSessionID
  unsigned short     state ;     // ExecutionState
  unsigned long long kmtime ;    // KernelModeTime
  unsigned long long umtime ;    // UserModeTime

  unsigned long long pmem ;      // Real Data
  unsigned long      pcpu ;      // CPU Time
};

struct processlist{
    struct cim_process * p ;
    struct processlist * next ;
};

/* Linux_Process calls */
int enum_all_process( struct processlist ** lptr);
int get_process_data( char * pid, struct cim_process ** sptr);

void free_processlist( struct processlist * lptr);
void free_process( struct cim_process * sptr);

/* ---------------------------------------------------------------------------*/

#ifdef __cplusplus
   }
#endif

/* ---------------------------------------------------------------------------*/

#endif


