#ifndef _CMPIOSBASE_OPERATINGSYSTEMSTATISTICALDATA_H_
#define _CMPIOSBASE_OPERATINGSYSTEMSTATISTICALDATA_H_

/*
 * cmpiOSBase_OperatingSystemStatisticalData.h
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
 * Author:       Michael Schuele <schuelem@de.ibm.com>
 * Contributors:
 *
 * Interface Type : Common Magabeablity Programming Interface ( CMPI )
 *
 * Description:
 * This file defines the interfaces for the factory implementation of the
 * CIM class Linux_OperatingSystemStatisticalData.
 *
*/


/* ---------------------------------------------------------------------------*/

#include "cmpidt.h"
#include "OSBase_OperatingSystemStatisticalData.h"

/* ---------------------------------------------------------------------------*/

static char * _ClassName = "Linux_OperatingSystemStatisticalData";
static const char * _INSTANCE_PREFIX = "Linux:";
static int _INSTANCE_PREFIX_LENGTH = 6;

/* ---------------------------------------------------------------------------*/


/* method to create a CMPIObjectPath of this class                            */

CMPIObjectPath * _makePath_OperatingSystemStatisticalData(const CMPIBroker * _broker,
                                                          const CMPIContext * ctx, 
                                                          const CMPIObjectPath * cop,
                                                          CMPIStatus * rc);


/* method to create a CMPIInstance of this class                              */

CMPIInstance * _makeInst_OperatingSystemStatisticalData(const CMPIBroker * _broker,
                                                        const CMPIContext * ctx, 
                                                        const CMPIObjectPath * cop,
                                                        const char ** properties,
                                                        CMPIStatus * rc);


/* ---------------------------------------------------------------------------*/

#endif

