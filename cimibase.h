
#ifndef _CIMIBASE_H_
#define _CIMIBASE_H_

/*
 * cimibase.h
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
 * Description: CIM Base Instrumentation for Linux
*/

// ------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

extern char * CIM_HOST_NAME;
extern char * CIM_OS_NAME;
extern char * CIM_OS_DISTRO;

extern char * CSCreationClassName;
extern char * OSCreationClassName;

// ------------------------------------------------------------------

// ------------------------------------------------------------------
// class Linux_ComputerSystem
// ------------------------------------------------------------------

char * get_system_name();
char * get_cs_primownername();
char * get_cs_primownercontact();

// ------------------------------------------------------------------
// end of class Linux_ComputerSystem
// ------------------------------------------------------------------

// ------------------------------------------------------------------
// class Linux_OperatingSystem
// ------------------------------------------------------------------

struct cim_operatingsystem {
  char * version ;                    // major.minor
  int    osType ;                     // 36
  unsigned long numOfProcesses ;      // ps
  unsigned long numOfUsers ;          // who -u

  char * installDate;
  char * lastBootUp;
  char * localDate;
  int    curTimeZone;                // CurrentTimeZone

  unsigned long long maxNumOfProc;   // MaxNumberOfProcesses
  unsigned long long maxProcMemSize; // MaxProcessMemorySize

  unsigned long long totalVirtMem;   // TotalVirtualMemorySize
  unsigned long long freeVirtMem;    // FreeVirtualMemory
  unsigned long long totalPhysMem;   // TotalVisibleMemorySize
  unsigned long long freePhysMem;    // FreePhysicalMemory
  unsigned long long totalSwapMem;   // SizeStoredInPagingFiles
  unsigned long long freeSwapMem;    // FreeSpaceInPagingFiles

};

int get_operatingsystem_data( struct cim_operatingsystem ** );
void free_os_data( struct cim_operatingsystem * );

void   _cat_timezone( char * str, int zone );
char * get_os_distro();
char * get_os_name();
char * get_os_installdate();
char * get_os_lastbootup();
int    get_os_timezone();
unsigned long get_os_numOfProcesses();
unsigned long get_os_numOfUsers();
unsigned long get_os_maxNumOfProc();
unsigned long get_os_maxProcMemSize();

char * get_kernel_version();

// ------------------------------------------------------------------
// end of class Linux_OperatingSystem
// ------------------------------------------------------------------


// ------------------------------------------------------------------
// class Linux_Process
// ------------------------------------------------------------------

struct cim_process{
  char * pid ;                   // Process ID
  char * ppid;                   // Parent Process ID
  char * ptty;                   // TTY
  char * pcmd;                   // command
  char * path;                   // ModulePath
  char ** args;                  // args[]
  char * createdate;             // CreationDate
  unsigned long prio ;           // Priority
  long      nice ;               // Nice value
  long long uid ;                // User ID
  long long gid ;                // Group ID
  long long pmem ;               // Real Data
  long      pcpu ;               // CPU Time
  long long sid ;                // Session ID
  int       state ;              // currenty execution state
  unsigned long long kmtime ;    // Kernel Mode Time
  unsigned long long umtime ;    // User Mode Time
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

// ------------------------------------------------------------------
// end of class Linux_Process
// ------------------------------------------------------------------


// ------------------------------------------------------------------
// class Linux_Processor
// ------------------------------------------------------------------

struct cim_processor {
  char * id;
  char * step;
  char * name;
  int    family;
  int    loadPct;
  int    stat;
  unsigned long maxClockSpeed;
  unsigned long curClockSpeed;
};

struct processorlist {
  struct cim_processor * sptr ;
  struct processorlist * next ;
};

int enum_all_processor( struct processorlist ** );
int get_processor_data( char * , struct cim_processor ** );

void free_processorlist( struct processorlist * );
void free_processor( struct cim_processor * );

// ------------------------------------------------------------------
// end of class Linux_Processor
// ------------------------------------------------------------------

// ------------------------------------------------------------------

int runcommand(const char *, char **, char ***, char ***);
void freeresultbuf(char **);

char ** line_to_array( char * , int );

// ------------------------------------------------------------------

int get_system_parameter(char *, char *, char *, int);
int set_system_parameter(char *, char *, char *);

#ifdef __cplusplus
   }
#endif

#endif








