/*
 * cmpiOSBase_ComputerSystem.c
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
 * This is the factory implementation for creating instances of CIM
 * class Linux_ComputerSystem.
 *
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmpidt.h"
#include "cmpimacs.h"

#include "cmpiOSBase_Common.h"
#include "OSBase_Common.h"

#include "cmpiOSBase_ComputerSystem.h"
#include "OSBase_ComputerSystem.h"


/* ---------------------------------------------------------------------------*/
/* private declarations                                                       */

/* ---------------------------------------------------------------------------*/



/* ---------------------------------------------------------------------------*/
/*                            Factory functions                               */
/* ---------------------------------------------------------------------------*/

/* ---------- method to create a CMPIObjectPath of this class ----------------*/

CMPIObjectPath * _makePath_ComputerSystem( CMPIBroker * _broker,
		 CMPIContext * ctx, 
	         CMPIObjectPath * ref,
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

  CMAddKey(op, "CreationClassName", _ClassName, CMPI_chars);
  CMAddKey(op, "Name", CIM_HOST_NAME, CMPI_chars);
    
 exit:
  return op;                
}

/* ----------- method to create a CMPIInstance of this class ----------------*/

CMPIInstance * _makeInst_ComputerSystem( CMPIBroker * _broker,
	       CMPIContext * ctx, 
	       CMPIObjectPath * ref,
	       CMPIStatus * rc) {
  CMPIObjectPath * op        = NULL;
  CMPIInstance   * ci        = NULL;
  char * owner               = NULL;
  char * contact             = NULL;
#ifndef CIM26COMPAT
  CMPIArray      * dedic     = NULL;
  unsigned short   status    = 2; /* Enabled */
  unsigned short   dedicated = 0; /* Not Dedicated */
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
  
  CMSetProperty( ci, "CreationClassName", _ClassName, CMPI_chars );
  CMSetProperty( ci, "Name", CIM_HOST_NAME, CMPI_chars );
  CMSetProperty( ci, "Status", "NULL", CMPI_chars);
  CMSetProperty( ci, "NameFormat", "IP", CMPI_chars);
  CMSetProperty( ci, "Caption", "Computer System", CMPI_chars);
  CMSetProperty( ci, "Description", "A class derived from ComputerSystem that represents the single node container of the Linux OS.", CMPI_chars);

  if( (owner=get_cs_primownername()) != NULL ) {
      CMSetProperty( ci, "PrimaryOwnerName", owner, CMPI_chars);
      free(owner);
  }
  if( (contact=get_cs_primownercontact()) != NULL) {
    CMSetProperty( ci, "PrimaryOwnerContact", contact, CMPI_chars);
    free(contact);
  }

  /* 2.7 */
#ifndef CIM26COMPAT
  CMSetProperty( ci, "ElementName", CIM_HOST_NAME, CMPI_chars);
  CMSetProperty( ci, "EnabledState", (CMPIValue*)&(status), CMPI_uint16);
  CMSetProperty( ci, "OtherEnabledState", "NULL", CMPI_chars);
  CMSetProperty( ci, "RequestedState", (CMPIValue*)&(status), CMPI_uint16);
  CMSetProperty( ci, "EnabledDefault", (CMPIValue*)&(status), CMPI_uint16);

  dedic = CMNewArray(_broker,1,CMPI_uint16A,rc);
  CMSetArrayElementAt(dedic,0,(CMPIValue*)&(dedicated),CMPI_uint16);
  CMSetProperty( ci, "Dedicated", (CMPIValue*)&(dedic), CMPI_uint16A);
#endif

 exit:
  return ci;
}



/* ---------------------------------------------------------------------------*/
/*          end of cmpiOSBase_ComputerSystem.c                      */
/* ---------------------------------------------------------------------------*/

