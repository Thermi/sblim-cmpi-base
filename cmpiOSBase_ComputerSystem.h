#ifndef _CMPIOSBASE_COMPUTERSYSTEM_H_
#define _CMPIOSBASE_COMPUTERSYSTEM_H_

/*
 * cmpiOSBase_ComputerSystem.h
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
 * Interface Type : Common Magabeablity Programming Interface ( CMPI )
 *
 * Description:
 * This file defines the interfaces for the factory implementation of the
 * CIM class Linux_ComputerSystem.
 *
*/


/* ---------------------------------------------------------------------------*/

#include "cmpidt.h"

/* ---------------------------------------------------------------------------*/

static char * _ClassName = "Linux_ComputerSystem";

/* ---------------------------------------------------------------------------*/


/* method to create a CMPIObjectPath of this class                            */

CMPIObjectPath * _makePath( CMPIBroker * _broker,
			    CMPIContext * ctx, 
			    CMPIObjectPath * cop,
			    CMPIStatus * rc);


/* method to create a CMPIInstance of this class                              */

CMPIInstance * _makeInst( CMPIBroker * _broker,
			  CMPIContext * ctx, 
			  CMPIObjectPath * cop,
			  CMPIStatus * rc);


/* ---------------------------------------------------------------------------*/

#endif
