/*
 * cmpiOSBase_Processor.c
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
 * Author:       
 * Contributors:
 *
 * Interface Type : Common Magabeablity Programming Interface ( CMPI )
 *
 * Description:
 * This is the factory implementation for creating instances of CIM
 * class Linux_Processor.
 *
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmpidt.h"
#include "cmpimacs.h"

#include "cmpiOSBase_Common.h"
#include "OSBase_Common.h"

#include "cmpiOSBase_Processor.h"
#include "OSBase_Processor.h"

unsigned char CMPI_true  = 1;
unsigned char CMPI_false = 0;

/* ---------------------------------------------------------------------------*/
/* private declarations                                                       */

#ifdef DEBUG
    int _debug = 1;
#else
    int _debug = 0;
#endif

/* ---------------------------------------------------------------------------*/



/* ---------------------------------------------------------------------------*/
/*                            Factory functions                               */
/* ---------------------------------------------------------------------------*/

/* ---------- method to create a CMPIObjectPath of this class ----------------*/

CMPIObjectPath * _makePath( CMPIBroker * _broker,
                            CMPIContext * ctx, 
                            CMPIObjectPath * ref,
			    struct cim_processor * sptr,
			    CMPIStatus * rc) {
  CMPIObjectPath * op = NULL;
 
  /* the sblim-cmpi-base package offers some tool methods to get common
   * system datas 
   * CIM_HOST_NAME contains the unique hostname of the local system 
  */ 
  if( !CIM_HOST_NAME ) {
    if( !get_system_name() ) {   
      CMSetStatusWithChars( _broker, rc, 
			    CMPI_RC_ERR_FAILED, "no host name found" );
      goto exit;
    }
  }

  op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(ref,rc)), 
			_ClassName, rc );
  if( CMIsNullObject(op) ) { 
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." ); 
    goto exit; 
  }

  CMAddKey(op, "SystemCreationClassName", CSCreationClassName, CMPI_chars);  
  CMAddKey(op, "SystemName", CIM_HOST_NAME, CMPI_chars);
  CMAddKey(op, "CreationClassName", _ClassName, CMPI_chars);  
  CMAddKey(op, "DeviceID", sptr->id, CMPI_chars);
    
 exit:
  return op;                
}

/* ----------- method to create a CMPIInstance of this class ----------------*/

CMPIInstance * _makeInst( CMPIBroker * _broker,
                          CMPIContext * ctx, 
                          CMPIObjectPath * ref,
			  struct cim_processor * sptr,
			  CMPIStatus * rc) {
  CMPIObjectPath * op     = NULL;
  CMPIInstance   * ci     = NULL;
#ifndef CIM26COMPAT
  unsigned short   status = 2; /* Enabled */
#endif

  /* the sblim-cmpi-base package offers some tool methods to get common
   * system datas 
   * CIM_HOST_NAME contains the unique hostname of the local system 
  */
  if( !CIM_HOST_NAME ) { 
    if( !get_system_name() ) {   
      CMSetStatusWithChars( _broker, rc, 
			    CMPI_RC_ERR_FAILED, "no host name found" );
      goto exit;
    }
  }

  op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(ref,rc)), 
			_ClassName, rc );
  if( CMIsNullObject(op) ) { 
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." ); 
    goto exit; 
  }

  ci = CMNewInstance( _broker, op, rc);
  if( CMIsNullObject(ci) ) { 
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "Create CMPIInstance failed." ); 
    goto exit; 
  }
      
  CMSetProperty( ci, "SystemCreationClassName",CSCreationClassName , CMPI_chars ); 
  CMSetProperty( ci, "SystemName", CIM_HOST_NAME, CMPI_chars );
  CMSetProperty( ci, "CreationClassName", _ClassName, CMPI_chars ); 
  CMSetProperty( ci, "DeviceID", sptr->id, CMPI_chars );

  CMSetProperty( ci, "Caption", "Linux Processor", CMPI_chars);
  CMSetProperty( ci, "Description", "This class represents instances of available processors.", CMPI_chars);
  CMSetProperty( ci, "Status", "NULL", CMPI_chars);

  CMSetProperty( ci, "Role", "Central Processor", CMPI_chars);  
  CMSetProperty( ci, "CPUStatus", (CMPIValue*)&(sptr->stat), CMPI_uint16);
  CMSetProperty( ci, "LoadPercentage", (CMPIValue*)&(sptr->loadPct), CMPI_uint16);
  CMSetProperty( ci, "Stepping", sptr->step, CMPI_chars);
  CMSetProperty( ci, "Family", (CMPIValue*)&(sptr->family), CMPI_uint16);
  CMSetProperty( ci, "OtherFamilyDescription", "NULL", CMPI_chars);

  CMSetProperty( ci, "MaxClockSpeed", (CMPIValue*)&(sptr->maxClockSpeed), CMPI_uint32);
  CMSetProperty( ci, "CurrentClockSpeed", (CMPIValue*)&(sptr->curClockSpeed), CMPI_uint32);

  CMSetProperty( ci, "Name", sptr->id, CMPI_chars );

  /* 2.7 */
#ifndef CIM26COMPAT
  CMSetProperty( ci, "ElementName", sptr->name, CMPI_chars);  
  CMSetProperty( ci, "EnabledStatus", (CMPIValue*)&(status), CMPI_uint16);
  CMSetProperty( ci, "OtherEnabledStatus", "NULL", CMPI_chars);
  CMSetProperty( ci, "RequestedStatus", (CMPIValue*)&(status), CMPI_uint16);
  CMSetProperty( ci, "EnabledDefault", (CMPIValue*)&(status), CMPI_uint16);
#endif

 exit:
  return ci;
}



/* ---------------------------------------------------------------------------*/
/*                      end of cmpiOSBase_Processor.c                         */
/* ---------------------------------------------------------------------------*/
