#ifndef _CMPIOSBASE_PROCESSOR_H_
#define _CMPIOSBASE_PROCESSOR_H_

/*
 * cmpiOSBase_Processor.h
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
 * Contributors:
 *
 * Interface Type : Common Magabeablity Programming Interface ( CMPI )
 *
 * Description:
 * This file defines the interfaces for the factory implementation of the
 * CIM class Linux_Processor.
 *
*/


/* ---------------------------------------------------------------------------*/

#include "cmpidt.h"
#include "OSBase_Processor.h"

/* ---------------------------------------------------------------------------*/

#ifndef _CMPIOSBASE_UTIL_H_
static char * _ClassName = "Linux_Processor";
#endif

/* ---------------------------------------------------------------------------*/


/* method to create a CMPIObjectPath of this class                            */

CMPIObjectPath * _makePath_Processor( CMPIBroker * _broker,
                 CMPIContext * ctx, 
                 CMPIObjectPath * cop,
		 struct cim_processor * sptr,
                 CMPIStatus * rc);


/* method to create a CMPIInstance of this class                              */

CMPIInstance * _makeInst_Processor( CMPIBroker * _broker,
               CMPIContext * ctx, 
               CMPIObjectPath * cop,
	       struct cim_processor * sptr,
               CMPIStatus * rc);


/* ---------------------------------------------------------------------------*/

#endif

