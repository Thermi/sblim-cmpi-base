#ifndef _OSBASE_COMMOM_H_
#define _OSBASE_COMMOM_H_

/*
 * OSBase_Common.h
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
 * Description: CIM Base Instrumentation for Linux
*/

/* ---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------------------*/

extern int _debug;

extern char * CIM_HOST_NAME;
extern char * CIM_OS_NAME;

/* ---------------------------------------------------------------------------*/

#define _OSBASE_TRACE(LEVEL,STR) \
  if ( (LEVEL<=_debug) && (LEVEL>0) ) \
  _osbase_trace(LEVEL,__FILE__,__LINE__,_format_trace STR)

/* ---------------------------------------------------------------------------*/

char * get_system_name();
char * get_os_name();

signed short  get_os_timezone();
unsigned long _get_os_boottime();
void          _cat_timezone( char * str, signed short zone );

/* ---------------------------------------------------------------------------*/

int runcommand(const char *, char **, char ***, char ***);
void freeresultbuf(char **);

char ** line_to_array( char * , int );

/* ---------------------------------------------------------------------------*/

int get_system_parameter(char *, char *, char *, int);
int set_system_parameter(char *, char *, char *);

/* ---------------------------------------------------------------------------*/

/* ---------------------------------------------------------------------------*/
// TRACE FACILITY
/* ---------------------------------------------------------------------------*/

char * _format_trace(char *fmt,...);
void _osbase_trace(  int, char *, int, char *);

/* ---------------------------------------------------------------------------*/

#ifdef __cplusplus
   }
#endif

/* ---------------------------------------------------------------------------*/

#endif








