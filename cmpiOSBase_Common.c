/*
 * cmpiOSBase_Common.c
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
 * Contributors: Adrian Schuur <schuur@de.ibm.com>
 *
 * Interface Type : Common Manageability Programming Interface ( CMPI )
 *
 * Description:
 * This library contains common methods to write CMPI providers.
 *
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmpidt.h"
#include "cmpift.h"
#include "cmpimacs.h"

#include "cmpiOSBase_Common.h"
#include "OSBase_Common.h"

unsigned char CMPI_true=1;
unsigned char CMPI_false=0;


/* ---------------------------------------------------------------------------*/

char * CSCreationClassName = "Linux_ComputerSystem";
char * OSCreationClassName = "Linux_OperatingSystem";

static char * _FILENAME    = "cmpiOSBase_Common.c";

/* ---------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------*/
/*                    _check_system_key_value_pairs()                         */
/* ---------------------------------------------------------------------------*/
/*     method to check the keys of ComputerSystem and OperatingSystem for     */
/*     their correctness                                                      */
/*                                                                            */
/* ---------------------------------------------------------------------------*/

void _check_system_key_value_pairs( CMPIBroker * _broker,
				    CMPIObjectPath * cop,
				    char * creationClassName,
				    char * className,
				    CMPIStatus * rc ) {
  CMPIString * name = NULL;

   name = CMGetKey( cop, className, rc).value.string;
   if ( rc->rc != CMPI_RC_OK || name == NULL ) {
      CMSetStatusWithChars( _broker, rc,
                            CMPI_RC_ERR_FAILED, "Could not get CS/OS Name of instance." );
      return;
   }
   get_system_name();
   if (( strcasecmp(CMGetCharPtr(name),CIM_HOST_NAME) != 0 ) &&
       ( strcasecmp(CMGetCharPtr(name),CIM_OS_NAME) != 0 )) {
      CMSetStatusWithChars( _broker, rc,
                            CMPI_RC_ERR_NOT_FOUND, "This instance does not exist (wrong CS/OS Name)." );
      return;
   }

   name = CMGetKey( cop, creationClassName, rc).value.string;
   if ( rc->rc != CMPI_RC_OK || name == NULL ) {
      CMSetStatusWithChars( _broker, rc,
                            CMPI_RC_ERR_FAILED, "Could not get CS/OS CreationClassName of instance." );
      return;
   }
   if ( (strcasecmp(CMGetCharPtr(name),CSCreationClassName) != 0) &&
        (strcasecmp(CMGetCharPtr(name),OSCreationClassName) != 0)   ) {
      CMSetStatusWithChars( _broker, rc,
                            CMPI_RC_ERR_NOT_FOUND, "This class name does not exist (wrong CS/OS CreationClassName)." );
      return;
   }

}


/* ---------------------------------------------------------------------------*/



/* ---------------------------------------------------------------------------*/
/*                      _assoc_create_inst_1toN()                             */
/* ---------------------------------------------------------------------------*/
/*     method to create CMPIInstance(s) / CMPIObjectPath(s) of association    */
/*                                                                            */
/* combination of int <left> and int <inst> :
 * 0 0 -> enum CMPIObjectPath(s) of right end;
 *        return CMPIObjectPath(s) of association
 * 1 0 -> enum CMPIObjectPath(s) of left end;
 *        return CMPIObjectPath(s) of association
 * 0 1 -> enum CMPIObjectPath(s) of right end;
 *        return CMPIInstance(s) of association
 * 1 1 -> enum CMPIObjectPath(s) of left end;
 *        return CMPIInstance(s) of association
 */
/* return value : SUCCESS = 0 ; FAILED = -1                                   */
/* !!! while calling _assoc_create_refs_1toN(), this method returns each      */
/* found CMPIInstance / CMPIObjectPath object to the Object Manager (OM)                                                    */
/* ---------------------------------------------------------------------------*/

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
			     CMPIStatus * rc) {
  CMPIObjectPath  * op  = NULL;
  CMPIEnumeration * en  = NULL;
  CMPIData          data ;
  int               arc = 0;

  if( _debug )
    fprintf( stderr, "--- %s : _assoc_create_inst_1toN()\n",_FILENAME);

  if( left == 0 ) {
    op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(cop,rc)),
			  _RefRightClass, rc );
  }
  else { /* left == 1 */
    op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(cop,rc)),
			  _RefLeftClass, rc );
  }
  if( CMIsNullObject(op) ) {
    CMSetStatusWithChars( _broker, rc,
			  CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." );
    return -1;
  }

  en = CBEnumInstanceNames( _broker, ctx, op, rc);
  if( en == NULL ) {
    CMSetStatusWithChars( _broker, rc,
			  CMPI_RC_ERR_FAILED, "CBEnumInstanceNames( _broker, ctx, op, rc)" );
    return -1;
  }

  /* this approach works only for 1 to N relations
   * int <left> contains the information, which side of the association has
   * only one instance
   */

  while( CMHasNext( en, rc) ) {

    data = CMGetNext( en, rc);
    if( data.value.ref == NULL ) {
      CMSetStatusWithChars( _broker, rc,
			    CMPI_RC_ERR_FAILED, "CMGetNext( en, rc)" );
      return -1;
    }

    //fprintf(stderr,"_assoc_create_inst_1toN(): %s\n",
    //	    CMGetCharPtr(CDToString(_broker, data.value.ref, rc)));

    arc = _assoc_create_refs_1toN( _broker,ctx,rslt,data.value.ref,
				   _ClassName,_RefLeftClass,_RefRightClass,
				   _RefLeft,_RefRight,inst,0,rc);
  }

  return arc;
}



/* ---------------------------------------------------------------------------*/
/*                           _assoc_get_inst()                                */
/* ---------------------------------------------------------------------------*/
/*     method to get CMPIInstance object of an association CMPIObjectPath     */
/*                                                                            */
/* ---------------------------------------------------------------------------*/

CMPIInstance * _assoc_get_inst( CMPIBroker * _broker,
				CMPIContext * ctx,
				CMPIObjectPath * cop,
				char * _ClassName,
				char * _RefLeft,
				char * _RefRight,
				CMPIStatus * rc ) {
  CMPIInstance   * ci = NULL;
  CMPIObjectPath * op = NULL;
  CMPIData         dtl;
  CMPIData         dtr;

  if( _debug ) { fprintf( stderr, "--- %s : _assoc_get_inst()\n",_FILENAME); }

  dtl = CMGetKey( cop, _RefLeft, rc);
  if( dtl.value.ref == NULL ) {
    CMSetStatusWithChars( _broker, rc,
			  CMPI_RC_ERR_FAILED, "CMGetKey( cop, _RefLeft, rc)" );
    goto exit;
  }

  CMSetNameSpace(dtl.value.ref,CMGetCharPtr(CMGetNameSpace(cop,rc)));
  ci = CBGetInstance(_broker, ctx, dtl.value.ref, NULL, rc);
  if( ci == NULL ) {
    CMSetStatusWithChars( _broker, rc,
			  CMPI_RC_ERR_FAILED, "GetInstance of left reference failed.");
    goto exit;
  }

  dtr = CMGetKey( cop, _RefRight, rc);
  if( dtr.value.ref == NULL ) {
    CMSetStatusWithChars( _broker, rc,
			  CMPI_RC_ERR_FAILED, "CMGetKey( cop, _RefRight, rc)" );
    goto exit;
  }

  CMSetNameSpace(dtr.value.ref,CMGetCharPtr(CMGetNameSpace(cop,rc)));
  ci = CBGetInstance(_broker, ctx, dtr.value.ref, NULL, rc);
  if( ci == NULL ) {
    CMSetStatusWithChars( _broker, rc,
			  CMPI_RC_ERR_FAILED, "GetInstance of right reference failed.");
    goto exit;
  }

  op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(cop,rc)),
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

  CMSetProperty( ci, _RefLeft, (CMPIValue*)&(dtl.value.ref), CMPI_ref );
  CMSetProperty( ci, _RefRight, (CMPIValue*)&(dtr.value.ref), CMPI_ref );

 exit:
  return ci;
}



/* ---------------------------------------------------------------------------*/
/*                      _assoc_create_refs_1toN()                             */
/* ---------------------------------------------------------------------------*/
/* method to return CMPIInstance(s) / CMPIObjectPath(s) of related objects    */
/* and the association itself                                                 */
/*                                                                            */
/* combination of int <inst> and int <associators> :
 * 0 0 -> referenceNames()
 * 1 0 -> references()
 * 0 1 -> associatorNames()
 * 1 1 -> associators()
 */
/* return value : SUCCESS = 0 ; FAILED = -1                                   */
/* !!! this method returns each found CMPIInstance / CMPIObjectPath object to */
/* the Object Manager (OM)                                                    */
/* ---------------------------------------------------------------------------*/

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
			     CMPIStatus * rc) {

  CMPIInstance    * ci  = NULL;
  CMPIObjectPath  * op  = NULL;
  CMPIObjectPath  * rop = NULL;
  CMPIObjectPath  * cop = NULL;
  CMPIEnumeration * en  = NULL;
  CMPIData          data ;
  char            * targetName = NULL;

  if( _debug )
    { fprintf( stderr, "--- %s : _assoc_create_refs_1toN()\n",_FILENAME); }

  op = _assoc_targetClass_OP(_broker,ref,_RefLeftClass,_RefRightClass,rc);
  if( op == NULL ) { goto exit; }

  rop = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(ref,rc)),
			 _ClassName, rc );
  if( CMIsNullObject(rop) ) {
    CMSetStatusWithChars( _broker, rc,
			  CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." );
    return -1;
  }

  if( (associators == 1) && (inst == 1) ) {
    /* associators() */
    en = CBEnumInstances( _broker, ctx, op, NULL, rc);
    if( en == NULL ) {
      CMSetStatusWithChars( _broker, rc,
			    CMPI_RC_ERR_FAILED, "CBEnumInstances( _broker, ctx, op, rc)" );
      return -1;
    }

    while( CMHasNext( en, rc) ) {

      data = CMGetNext( en, rc);
      if( data.value.inst == NULL ) {
        CMSetStatusWithChars( _broker, rc,
			      CMPI_RC_ERR_FAILED, "CMGetNext( en, rc)" );
        return -1;
      }
      
      CMReturnInstance( rslt, data.value.inst );
    }
  }

  else {
    en = CBEnumInstanceNames( _broker, ctx, op, rc);
    if( en == NULL ) {
      CMSetStatusWithChars( _broker, rc,
			    CMPI_RC_ERR_FAILED, "CBEnumInstanceNames( _broker, ctx, op, rc)" );
      return -1;
    }

    while( CMHasNext( en, rc) ) {

      data = CMGetNext( en, rc);
      if( data.value.ref == NULL ) {
	CMSetStatusWithChars( _broker, rc,
			      CMPI_RC_ERR_FAILED, "CMGetNext( en, rc)" );
	return -1;
      }
      //    fprintf(stderr,"_assoc_create_refs_1toN(): %s\n",
      //    	    CMGetCharPtr(CDToString(_broker, data.value.ref, rc)));
      //    if( _debug) { fprintf(stderr,"data.value.ref - namespace : %s\n",
      //    			  CMGetCharPtr(CMGetNameSpace(data.value.ref,rc))); }
      
      if( associators == 0 ) {

	/* references() || referenceNames() */
	ci = CMNewInstance( _broker, rop, rc);
	if( CMIsNullObject(ci) ) {
	  CMSetStatusWithChars( _broker, rc,
				CMPI_RC_ERR_FAILED, "Create CMPIInstance failed." );
	  return -1;
	}
	
	targetName = _assoc_targetClass_Name(_broker,ref,_RefLeftClass,_RefRightClass,rc);
	
	if( strcmp( targetName,_RefRightClass) == 0 ) {
	  CMSetProperty( ci, _RefLeft, (CMPIValue*)&(ref), CMPI_ref );
	  CMSetProperty( ci, _RefRight, (CMPIValue*)&(data.value.ref), CMPI_ref );
	}
	else if( strcmp( targetName,_RefLeftClass) == 0 ) {
	  CMSetProperty( ci, _RefLeft, (CMPIValue*)&(data.value.ref), CMPI_ref );
	  CMSetProperty( ci, _RefRight, (CMPIValue*)&(ref), CMPI_ref );
	}
	
	//      fprintf(stderr,"_assoc_create_refs_1toN() inst: %s\n",
	//      	      CMGetCharPtr(CDToString(_broker, ci, rc)));
	
	if( inst == 0 ) {
	  cop = CMGetObjectPath(ci,rc);
	  if( cop == NULL ) {
	    CMSetStatusWithChars( _broker, rc,
				  CMPI_RC_ERR_FAILED, "CMGetObjectPath(ci,rc)" );
	  return -1;
	  }
	  CMSetNameSpace(cop,CMGetCharPtr(CMGetNameSpace(ref,rc)));
	  //	fprintf(stderr,"_assoc_create_refs_1toN() cop: %s\n",
	  //		CMGetCharPtr(CDToString(_broker, cop, rc)));
	  CMReturnObjectPath( rslt, cop );
	}
	else {
	  //	fprintf(stderr,"_assoc_create_refs_1toN() returns instance\n");
	  CMReturnInstance( rslt, ci );
	}
      }
      else {
	/* associatorNames() */
	if( inst == 0 ) { CMReturnObjectPath( rslt, data.value.ref ); }
      }
      
    }
  }

 exit:
  return 0;
}


/* ---------------------------------------------------------------------------*/
/*                      _assoc_targetClass_Name()                             */
/* ---------------------------------------------------------------------------*/
/*                  method to get the name of the target class                */
/*                                                                            */
/* return value NULL : association is not responsible for this request        */
/* ---------------------------------------------------------------------------*/

char * _assoc_targetClass_Name( CMPIBroker * _broker,
				CMPIObjectPath * ref,
				char * _RefLeftClass,
				char * _RefRightClass,
				CMPIStatus * rc) {

  CMPIString * sourceClass = NULL;

  if( _debug )
    { fprintf( stderr, "--- %s : _assoc_targetClass_Name()\n",_FILENAME); }

  sourceClass = CMGetClassName(ref, rc);
  if( sourceClass == NULL ) { return NULL; }

  if( strcmp(CMGetCharPtr(sourceClass), _RefLeftClass ) == 0 ) {
    /* pathName = left end -> get right end */
    return _RefRightClass;
  }
  else if( strcmp(CMGetCharPtr(sourceClass), _RefRightClass ) == 0 ) {
    /* pathName = right end -> get left end */
    return _RefLeftClass;
  }
  else if( CMClassPathIsA(_broker,ref,_RefLeftClass,rc) == 1 ) {
    /* pathName = left end -> get right end */
    return _RefRightClass;
  }
  else if( CMClassPathIsA(_broker,ref,_RefRightClass,rc) == 1 ) {
    /* pathName = right end -> get left end */
    return _RefLeftClass;
  }

  return NULL;
}


/* ---------------------------------------------------------------------------*/
/*                       _assoc_targetClass_OP()                              */
/* ---------------------------------------------------------------------------*/
/*        method to create an empty CMPIObjectPath of the target class        */
/*                                                                            */
/* return value NULL : association is not responsible for this request        */
/* ---------------------------------------------------------------------------*/

CMPIObjectPath * _assoc_targetClass_OP( CMPIBroker * _broker,
					CMPIObjectPath * ref,
					char * _RefLeftClass,
					char * _RefRightClass,
					CMPIStatus * rc ) {
  CMPIObjectPath  * op         = NULL;
  char            * targetName = NULL;

  if( _debug )
    { fprintf( stderr, "--- %s : _assoc_targetClass_OP()\n",_FILENAME); }

  targetName = _assoc_targetClass_Name(_broker,ref,_RefLeftClass,_RefRightClass,rc);

  if( targetName != NULL ) {
    op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(ref,rc)),
			  targetName, rc );
  }
  return op;
}




/* ---------------------------------------------------------------------------*/
/*                       _assoc_targetClass_OP()                              */
/* ---------------------------------------------------------------------------*/
/*    method to check the input parameter resultClass, role and resultRole    */
/*           submitted to the methods of the association interface            */
/*                                                                            */
/* return value NULL : association is not responsible for this request        */
/* ---------------------------------------------------------------------------*/

int _assoc_check_parameter_const( CMPIBroker * _broker,
				  CMPIObjectPath * cop,
				  char * _RefLeft,
				  char * _RefRight,
				  char * _RefLeftClass,
				  char * _RefRightClass,
				  char * resultClass,
				  char * role,
				  char * resultRole,
				  CMPIStatus * rc ) {
  CMPIObjectPath * op          = NULL;
  CMPIObjectPath * scop        = NULL;
  CMPIObjectPath * rcop        = NULL;
  CMPIString     * sourceClass = NULL;
  int              intrc       = 0;

  if( resultClass || role || resultRole) {

    sourceClass = CMGetClassName(cop, rc);
    scop = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(cop,rc)),
			    CMGetCharPtr(sourceClass), rc );

    /* check if resultClass is parent or the class itslef of the target class */
    if( resultClass ) {

      if( strcasecmp(CMGetCharPtr(sourceClass),_RefLeftClass) == 0 ||
	  CMClassPathIsA(_broker,scop,_RefLeftClass,rc) == 1 ) {
	op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(cop,rc)),
			      _RefRightClass, rc );
      }
      else if( strcasecmp(CMGetCharPtr(sourceClass),_RefRightClass) == 0 ||
	       CMClassPathIsA(_broker,scop,_RefRightClass,rc) == 1 ) {
	op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(cop,rc)),
			      _RefLeftClass, rc );
      }
      else { goto exit; }

      rcop = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(cop,rc)),
			      resultClass, rc );

      if( CMClassPathIsA(_broker,op,resultClass,rc) == 1 ) { intrc = 1; }
      else if( ( CMClassPathIsA(_broker,rcop,_RefRightClass,rc) == 1 &&
		 strcasecmp(CMGetCharPtr(sourceClass),_RefLeftClass) == 0 ) ||
	       ( CMClassPathIsA(_broker,rcop,_RefRightClass,rc) == 1 &&
		 CMClassPathIsA(_broker,scop,_RefLeftClass,rc) == 1 ) ) {
	intrc = 1;
      }
      else if( ( CMClassPathIsA(_broker,rcop,_RefLeftClass,rc) == 1 &&
		 strcasecmp(CMGetCharPtr(sourceClass),_RefRightClass) == 0 ) ||
	       ( CMClassPathIsA(_broker,rcop,_RefLeftClass,rc) == 1 &&
		 CMClassPathIsA(_broker,scop,_RefRightClass,rc) == 1 ) ) {
	intrc = 1;
      }
      else { intrc = 0;  goto exit; }

    }

    /* check if the source object (cop) plays the Role ( specified in input
     * parameter 'role' within this association */
    if( role ) {
      if( strcasecmp(CMGetCharPtr(sourceClass),_RefLeftClass) == 0 ||
	  CMClassPathIsA(_broker,scop,_RefLeftClass,rc) == 1 ) {
	if( strcasecmp(role,_RefLeft) != 0 ) { intrc = 0; goto exit; }
	else { intrc = 1; }
      }
      else if( strcasecmp(CMGetCharPtr(sourceClass),_RefRightClass) == 0 ||
	       CMClassPathIsA(_broker,scop,_RefRightClass,rc) == 1 ) {
	if( strcasecmp(role,_RefRight) != 0 ) { intrc = 0; goto exit; }
	else { intrc = 1; }
      }
      else { goto exit; }
    }

    /* check if the target object plays the Role ( specified in input
     * parameter 'resultRole' within this association */
    if( resultRole ) {
      if( strcasecmp(CMGetCharPtr(sourceClass),_RefLeftClass) == 0 ||
	  CMClassPathIsA(_broker,scop,_RefLeftClass,rc) == 1 ) {
	if( strcasecmp(resultRole,_RefRight) != 0 ) { intrc = 0; goto exit; }
	else { intrc = 1; }
      }
      else if( strcasecmp(CMGetCharPtr(sourceClass),_RefRightClass) == 0 ||
	       CMClassPathIsA(_broker,scop,_RefRightClass,rc) == 1 ) {
	if( strcasecmp(resultRole,_RefLeft) != 0 ) { intrc = 0; goto exit; }
	else { intrc = 1; }
      }
      else { goto exit; }
    }
  }
  else { intrc = 1; }

 exit:
  return intrc;
}

/* ---------------------------------------------------------------------------*/
/*                    end of cmpiOSBase_Common.c                              */
/* ---------------------------------------------------------------------------*/

