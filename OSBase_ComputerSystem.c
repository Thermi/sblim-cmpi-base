/*
 * OSBase_ComputerSystem.c
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
 * Linux_ComputerSystem. 
 * It is independent from any specific CIM technology.
*/

/* ---------------------------------------------------------------------------*/

#include "OSBase_Common.h"
#include "OSBase_ComputerSystem.h"
#include <unistd.h>

/* ---------------------------------------------------------------------------*/
// private declarations


/* ---------------------------------------------------------------------------*/

/* returns the primary owner of the system ... always root ;-) */
char * get_cs_primownername() {
  char * ptr = NULL;

  _OSBASE_TRACE(4,("--- get_cs_primownername() called"));

  ptr = (char*)malloc(5);
  strcpy(ptr,"root");

  _OSBASE_TRACE(4,("--- get_cs_primownername() exited"));
  return ptr;
}

/* returns the contact address of the primary owner of the system ...
 * always root@host.domain
 */
char * get_cs_primownercontact() {
  char * ptr  = NULL;
  char * own  = NULL;
  char * host = NULL;

  _OSBASE_TRACE(4,("--- get_cs_primownercontact() called"));

  if( (own = get_cs_primownername()) != NULL ) {
    host = get_system_name();
    ptr = (char*)malloc( (strlen(own)+strlen(host)+2));
    strcpy( ptr, own);
    strcat( ptr,"@");
    strcat( ptr, host);
    if(own) free(own);
    _OSBASE_TRACE(4,("--- get_cs_primownercontact() exited"));
    return ptr;
  }

  _OSBASE_TRACE(4,("--- get_cs_primownercontact() failed"));
  return NULL;
}

/* ---------------------------------------------------------------------------*/
/*                       end of OSBase_ComputerSystem.c                       */
/* ---------------------------------------------------------------------------*/

