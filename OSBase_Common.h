#ifndef _OSBASE_COMMOM_H_
#define _OSBASE_COMMOM_H_

/*
 * OSBase_Common.h
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

/* ---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------------------*/

extern char * CIM_HOST_NAME;
extern char * CIM_OS_NAME;

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

#ifdef __cplusplus
   }
#endif

/* ---------------------------------------------------------------------------*/

#endif








