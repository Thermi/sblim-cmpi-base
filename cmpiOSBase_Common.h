
#ifndef _CMPIOSBASE_COMMON_H_
#define _CMPIOSBASE_COMMON_H_

/*
 * cmpiOSBase_Common.h
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
 * Interface Type : Common Manageability Programming Interface ( CMPI )
 *
 * Description:
 * This library contains common /  generic methods to write CMPI Provider.
 *
*/
/* ---------------------------------------------------------------------------*/

#include "cmpidt.h"

/* ---------------------------------------------------------------------------*/

extern char * CSCreationClassName;
extern char * OSCreationClassName;

extern unsigned char CMPI_true;
extern unsigned char CMPI_false;

/* ---------------------------------------------------------------------------*/

void _check_system_key_value_pairs( CMPIBroker * _broker,
				    CMPIObjectPath * cop,
				    char * creationClassName,
				    char * className,
				    CMPIStatus * rc );


int _assoc_create_inst_1toN( CMPIBroker * _broker,
			     CMPIContext * ctx,
			     CMPIResult * rslt,
			     CMPIObjectPath * cop,
			     char * _ClassName,
			     char * _RefLeftClass,
			     char * _RefRightClass,
			     char * _RefLeft,
			     char * _RefRight,
			     int left,
			     int inst,
			     CMPIStatus * rc);


/* - method to create CMPIInstance out of the CMPIObjectPath of an            */
/*   association instance                                                     */
CMPIInstance * _assoc_get_inst( CMPIBroker * _broker,
				CMPIContext * ctx,
				CMPIObjectPath * cop,
				char * _ClassName,
				char * _RefLeft,
				char * _RefRight,
				CMPIStatus * rc );


/* - method to create CMPIInstance(s) / CMPIObjectPath(s) of association      */
int _assoc_create_refs_1toN( CMPIBroker * _broker,
			     CMPIContext * ctx,
			     CMPIResult * rslt,
			     CMPIObjectPath * ref,
			     char * _ClassName,
			     char * _RefLeftClass,
			     char * _RefRightClass,
			     char * _RefLeft,
			     char * _RefRight,
			     int inst,
			     int associators,
			     CMPIStatus * rc);


/* - method to get the name of the target class                               */
char * _assoc_targetClass_Name( CMPIBroker * _broker,
				CMPIObjectPath * ref,
				char * _RefLeftClass,
				char * _RefRightClass,
				CMPIStatus * rc);


/* - method to get an empty CMPIObjectPath of the target class                */
CMPIObjectPath * _assoc_targetClass_OP( CMPIBroker * _broker,
					CMPIObjectPath * ref,
					char * _RefLeftClass,
					char * _RefRightClass,
					CMPIStatus * rc );


/* - method to check the input parameter resultClass, role and resultRole     */
/*   submitted to the methods of the association interface                    */
int _assoc_check_parameter_const( CMPIBroker * _broker,
				  CMPIObjectPath * cop,
				  char * _RefLeft,
				  char * _RefRight,
				  char * _RefLeftClass,
				  char * _RefRightClass,
				  char * resultClass,
				  char * role,
				  char * resultRole,
				  CMPIStatus * rc );


/* ---------------------------------------------------------------------------*/
/*                       end of cmpiOSBase_Common.h                           */
/* ---------------------------------------------------------------------------*/


#endif
