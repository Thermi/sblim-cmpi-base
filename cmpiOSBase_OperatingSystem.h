#ifndef _CMPIOSBASE_OPERATINGSYSTEM_H_
#define _CMPIOSBASE_OPERATINGSYSTEM_H_

/*
 * cmpiOSBase_OperatingSystem.h
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
 *
 * Interface Type : Common Magabeablity Programming Interface ( CMPI )
 *
 * Description:
 * This file defines the interfaces for the factory implementation of the
 * CIM class Linux_OperatingSystem.
 *
*/


/* ---------------------------------------------------------------------------*/

#include "cmpidt.h"

/* ---------------------------------------------------------------------------*/

#ifndef _CMPIOSBASE_UTIL_H_
static char * _ClassName = "Linux_OperatingSystem";
#endif

/* ---------------------------------------------------------------------------*/

/* method to create a CMPIObjectPath of this class                            */

CMPIObjectPath * _makePath_OperatingSystem( CMPIBroker * _broker,
                 CMPIContext * ctx, 
                 CMPIObjectPath * cop,
                 CMPIStatus * rc);


/* method to create a CMPIInstance of this class                              */

CMPIInstance * _makeInst_OperatingSystem( CMPIBroker * _broker,
               CMPIContext * ctx, 
               CMPIObjectPath * cop,
               CMPIStatus * rc);


/* ---------------------------------------------------------------------------*/

#endif

