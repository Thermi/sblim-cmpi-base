#ifndef _OSBASE_BASEBOARD_H_
#define _OSBASE_BASEBOARD_H_

/*
 * OSBase_BaseBoard.h
 *
 * (C) Copyright IBM Corp. 2005
 *
 * THIS FILE IS PROVIDED UNDER THE TERMS OF THE COMMON PUBLIC LICENSE
 * ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE
 * CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
 *
 * You can obtain a current copy of the Common Public License from
 * http://oss.software.ibm.com/developerworks/opensource/license-cpl.html
 *
 * Author:       Heidi Neumann <heidineu@de.ibm.com>
 * Contributors: 
 *
 * Description: 
 * This file defines the interfaces for the resource access implementation 
 * of the CIM class Linux_BaseBoard.
 * 
*/

/* ---------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------------------*/

struct cim_baseboard {
  char * tag;
  char * vendor;
  char * type;
  char * model;
  char * partNumber;
  char * serialNumber;
  char * UUID;
};

int get_baseboard_data(struct cim_baseboard *data);
int get_baseboard_tag(char *tag, int size);

void free_baseboard_data(struct cim_baseboard *data);

/* ---------------------------------------------------------------------------*/

#ifdef __cplusplus
   }
#endif

/* ---------------------------------------------------------------------------*/

#endif


