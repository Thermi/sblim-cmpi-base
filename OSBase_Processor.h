#ifndef _OSBASE_PROCESSOR_H_
#define _OSBASE_PROCESSOR_H_

/*
 * OSBase_Processor.h
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
 * Contributors: 
 *
 * Description: 
 * This file defines the interfaces for the resource access implementation 
 * of the CIM class Linux_Processor.
 * 
*/

/* ---------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------------------*/

struct cim_processor {
  char * id;
  char * step;
  char * name;
  unsigned short family;
  unsigned short loadPct;
  unsigned short stat;
  unsigned long  maxClockSpeed;
  unsigned long  curClockSpeed;
};

struct processorlist {
  struct cim_processor * sptr ;
  struct processorlist * next ;
};

int enum_all_processor( struct processorlist ** );
int get_processor_data( char * , struct cim_processor ** );

void free_processorlist( struct processorlist * );
void free_processor( struct cim_processor * );

/* ---------------------------------------------------------------------------*/

#ifdef __cplusplus
   }
#endif

/* ---------------------------------------------------------------------------*/

#endif


