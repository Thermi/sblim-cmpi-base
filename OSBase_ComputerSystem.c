/*
 * OSBase_ComputerSystem.c
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
 * Linux_ComputerSystem. 
 * It is independent from any specific CIM technology.
*/

/* ---------------------------------------------------------------------------*/

#include "OSBase_Common.h"
#include "OSBase_ComputerSystem.h"
#include <unistd.h>

/* ---------------------------------------------------------------------------*/
// private declarations


static char* _FILENAME = "OSBase_ComputerSystem.c";

/* ---------------------------------------------------------------------------*/

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
  char * ptr  = NULL;
  char * own  = NULL;
  char * host = NULL;
  if( _debug ) { fprintf(stderr, "--- %s : get_cs_primownercontact()\n",_FILENAME); }
  if( (own = get_cs_primownername()) != NULL ) {
    host = get_system_name();
    ptr = (char*)malloc( (strlen(own)+strlen(host)+2)*sizeof(char));
    strcpy( ptr, own);
    strcat( ptr,"@");
    strcat( ptr, host);
    if(own) free(own);
    return ptr;
  }
  return NULL;
}

/* ---------------------------------------------------------------------------*/
/*                       end of OSBase_ComputerSystem.c                       */
/* ---------------------------------------------------------------------------*/

