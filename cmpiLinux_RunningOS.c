/*
 * Linux_RunningOS.c
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
 * 
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmpidt.h"
#include "cmpift.h"
#include "cmpimacs.h"

#include "cmpiprovsup.h"

#include "cimibase.h"

unsigned char CMPI_true=1;
unsigned char CMPI_false=0;

static CMPIBroker * _broker = NULL;

/* ---------------------------------------------------------------------------*/
/* private declarations                                                       */
#ifdef DEBUG
    int _debug = 1;
#else
    int _debug = 0;
#endif

static char * _ClassName     = "Linux_RunningOS";
static char * _RefLeft       = "Antecedent";
static char * _RefRight      = "Dependent";
static char * _RefLeftClass  = "Linux_OperatingSystem";
static char * _RefRightClass = "Linux_ComputerSystem";

/* ---------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------*/
/*                      Instance Provider Interface                           */
/* ---------------------------------------------------------------------------*/


CMPIStatus Linux_RunningOSCleanup( CMPIInstanceMI * mi, 
				   CMPIContext * ctx) {
  CMReturn(CMPI_RC_OK);
}

CMPIStatus Linux_RunningOSEnumInstanceNames( CMPIInstanceMI * mi, 
					     CMPIContext * ctx, 
					     CMPIResult * rslt, 
					     CMPIObjectPath * ref) {
  CMPIStatus rc    = {CMPI_RC_OK, NULL};
  int        refrc = 0;
  
  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI EnumInstanceNames()\n", _ClassName );
  
  refrc = _assoc_create_inst_1toN( _broker,ctx,rslt,ref,
				   _ClassName,_RefLeftClass,_RefRightClass,
				   _RefLeft,_RefRight,
				   1,0,&rc);

  if( refrc != 0 ) { 
    if( _debug ) {
      if( rc.msg != NULL ) 
	{ fprintf(stderr,"rc.msg: %s\n",CMGetCharPtr(rc.msg)); }
    }
    CMReturn(CMPI_RC_ERR_FAILED); 
  }

  CMReturnDone( rslt );
  CMReturn( CMPI_RC_OK );
}

CMPIStatus Linux_RunningOSEnumInstances( CMPIInstanceMI * mi, 
					 CMPIContext * ctx, 
					 CMPIResult * rslt, 
					 CMPIObjectPath * ref, 
					 char ** properties) {
  CMPIStatus rc    = {CMPI_RC_OK, NULL};
  int        refrc = 0;

  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI EnumInstances()\n", _ClassName ); 

  refrc = _assoc_create_inst_1toN( _broker,ctx,rslt,ref,
				   _ClassName,_RefLeftClass,_RefRightClass,
				   _RefLeft,_RefRight,
				   1,1,&rc);
  if( refrc != 0 ) { 
    if( _debug ) {
      if( rc.msg != NULL ) 
	{ fprintf(stderr,"rc.msg: %s\n",CMGetCharPtr(rc.msg)); }
    }
    CMReturn(CMPI_RC_ERR_FAILED); 
  }

  CMReturnDone( rslt );
  CMReturn( CMPI_RC_OK );
}


CMPIStatus Linux_RunningOSGetInstance( CMPIInstanceMI * mi, 
				       CMPIContext * ctx, 
				       CMPIResult * rslt, 
				       CMPIObjectPath * cop, 
				       char ** properties) {
  CMPIInstance * ci = NULL;
  CMPIStatus     rc = {CMPI_RC_OK, NULL};
  
  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI GetInstance()\n", _ClassName ); 
  
  ci = _assoc_get_inst( _broker,cop,_ClassName,_RefLeft,_RefRight,&rc);

  if( ci == NULL ) { 
    if( _debug ) {
      if( rc.msg != NULL ) 
	{ fprintf(stderr,"rc.msg: %s\n",CMGetCharPtr(rc.msg)); }
    }
    CMReturn(CMPI_RC_ERR_NOT_FOUND); 
  }

  CMReturnInstance( rslt, ci );
  CMReturnDone(rslt);
  CMReturn(CMPI_RC_OK);
}

CMPIStatus Linux_RunningOSCreateInstance( CMPIInstanceMI * mi, 
					  CMPIContext * ctx, 
					  CMPIResult * rslt, 
					  CMPIObjectPath * cop, 
					  CMPIInstance * ci) {
  CMPIStatus rc = {CMPI_RC_OK, NULL};

  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI CreateInstance()\n", _ClassName ); 

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "NOT_SUPPORTED" ); 
  return rc;
}

CMPIStatus Linux_RunningOSSetInstance( CMPIInstanceMI * mi, 
				       CMPIContext * ctx, 
				       CMPIResult * rslt, 
				       CMPIObjectPath * cop,
				       CMPIInstance * ci, 
				       char **properties) {
  CMPIStatus rc = {CMPI_RC_OK, NULL};

  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI SetInstance()\n", _ClassName ); 

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "NOT_SUPPORTED" ); 
  return rc;
}

CMPIStatus Linux_RunningOSDeleteInstance( CMPIInstanceMI * mi, 
					  CMPIContext * ctx, 
					  CMPIResult * rslt, 
					  CMPIObjectPath * cop) { 
  CMPIStatus rc = {CMPI_RC_OK, NULL};
  
  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI DeleteInstance()\n", _ClassName ); 

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "NOT_SUPPORTED" ); 
  return rc;
}

CMPIStatus Linux_RunningOSExecQuery( CMPIInstanceMI * mi, 
				     CMPIContext * ctx, 
				     CMPIResult * rslt, 
				     CMPIObjectPath * cop, 
				     char * lang, 
				     char * query) {
  CMPIStatus rc = {CMPI_RC_OK, NULL};

  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI ExecQuery()\n", _ClassName ); 

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "NOT_SUPPORTED" ); 
  return rc;
}



/* ---------------------------------------------------------------------------*/
/*                    Associator Provider Interface                           */
/* ---------------------------------------------------------------------------*/


CMPIStatus Linux_RunningOSAssociationCleanup( CMPIAssociationMI * mi,
					      CMPIContext * ctx) {
  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI AssociationCleanup()\n", _ClassName ); 
  CMReturn(CMPI_RC_OK);
}


/* 
 * The intention of associations is to show the relations between different
 * classes and their instances. Therefore an association has two properties. 
 * Each one representing a reference to a certain instance of the specified
 * class. We can say, that an association has a left and the right "end". 
 * 
 * Linux_RunningOS : 
 *    < role >   -> < class >
 *    Antecedent -> Linux_OperatingSystem
 *    Dependent  -> Linux_ComputerSystem
 *
 */

/*
 * general API information
 *
 * cop :
 *    Specifies the CMPIObjectPath to the CMPIInstance of the known end of 
 *    the association. Its absolutelly necessary to define this anchor point,
 *    from where the evaluation of the association starts.
 * assocPath : 
 *    If not NULL, it contains the name of the association the caller is 
 *    looking for. The provider should only return values, when the assocPath 
 *    contains the name(s) of the association(s) he is responsible for !
 * role :
 *    The caller can specify that the source instance ( cop ) has to play 
 *    a certain <role> in the association. The <role> is specified by the
 *    association definition (see above). That means, role has to contain
 *    the same value as the <role> the source instance plays in this assoc.
 *    If this requirement is not true, the provider returns nothing.
 */

/*
 * specification of associators() and associatorNames()
 *
 * These methods return CMPIInstance ( in the case of associators() ) or 
 * CMPIObjectPath ( in the case of associatorNames() ) object(s) of the 
 * opposite end of the association.
 *
 * resultRole :
 *    The caller can specify that the target instance(s) has/have to play 
 *    a certain <role> in the association. The <role> is specified by the
 *    association definition (see above). That means, resultRole has to 
 *    contain the same value as the <role> the target instance(s) plays
 *    in this assoc. If this requirement is not true, the provider returns 
 *    nothing.
 * resultClass :
 *    The caller can specify that the target instance(s) has/have to be 
 *    instances of a certain <class>. The <class> is specified by the
 *    association definition (see above). That means, resultClass has to 
 *    contain the same value as the <class> of the target instance(s).
 *    If this requirement is not true, the provider returns nothing.
 */


CMPIStatus Linux_RunningOSAssociators( CMPIAssociationMI * mi,
				       CMPIContext * ctx,
				       CMPIResult * rslt,
				       CMPIObjectPath * cop,
				       char * assocClass,
				       char * resultClass,
				       char * role,
				       char * resultRole,
				       char ** propertyList ) {
  CMPIStatus       rc    = {CMPI_RC_OK, NULL};
  CMPIObjectPath * op    = NULL;
  int              refrc = 0;

  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI Associators()\n", _ClassName ); 

  if( assocClass ) {
    op = CMNewObjectPath( _broker, _ClassName,
			    CMGetCharPtr(CMGetNameSpace(cop,&rc)), &rc );
  }

  if( ( assocClass==NULL ) || ( CMClassPathIsA(_broker,op,assocClass,&rc) == 1 ) ) {
  
    if( _assoc_check_parameter_const( _broker,cop,_RefLeft,_RefRight,
				      _RefLeftClass,_RefRightClass,
				      resultClass,role,resultRole, 
				      &rc ) == 0 ) { goto exit; }

    /* TODO */
    /* currently not discovered if resultClass, role or resultRole are set */

    refrc = _assoc_create_refs_1toN(_broker, ctx, rslt, cop, 
				    _ClassName,_RefLeftClass,_RefRightClass,
				    _RefLeft,_RefRight,
				    1, 1, &rc);

    if( refrc != 0 ) { 
      if( _debug ) {
	if( rc.msg != NULL ) 
	  { fprintf(stderr,"rc.msg: %s\n",CMGetCharPtr(rc.msg)); }
      }
      CMReturn(CMPI_RC_ERR_FAILED); 
    }
  }
  
 exit:
  CMReturnDone( rslt );
  CMReturn(CMPI_RC_OK);
}

CMPIStatus Linux_RunningOSAssociatorNames( CMPIAssociationMI * mi,
					   CMPIContext * ctx,
					   CMPIResult * rslt,
					   CMPIObjectPath * cop,
					   char * assocClass,
					   char * resultClass,
					   char * role,
					   char * resultRole ) {
  CMPIStatus       rc    = {CMPI_RC_OK, NULL};
  CMPIObjectPath * op    = NULL;
  int              refrc = 0;

  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI AssociatorNames()\n", _ClassName ); 

  if( assocClass ) {
    op = CMNewObjectPath( _broker, _ClassName,
			  CMGetCharPtr(CMGetNameSpace(cop,&rc)), &rc );
  }

  if( ( assocClass==NULL ) || ( CMClassPathIsA(_broker,op,assocClass,&rc) == 1 ) ) {

    if( _assoc_check_parameter_const( _broker,cop,_RefLeft,_RefRight,
				      _RefLeftClass,_RefRightClass,
				      resultClass,role,resultRole, 
				      &rc ) == 0 ) { goto exit; }
    
    /* TODO */
    /* currently not discovered if resultClass, role or resultRole are set */

    refrc = _assoc_create_refs_1toN(_broker, ctx, rslt, cop, 
				    _ClassName,_RefLeftClass,_RefRightClass,
				    _RefLeft,_RefRight,
				    0, 1, &rc);
    if( refrc != 0 ) { 
      if( _debug ) {
	if( rc.msg != NULL ) 
	  { fprintf(stderr,"rc.msg: %s\n",CMGetCharPtr(rc.msg)); }
      }
      CMReturn(CMPI_RC_ERR_FAILED); 
    }
  }

 exit:
  CMReturnDone( rslt );
  CMReturn(CMPI_RC_OK);
}


/*
 * specification of references() and referenceNames()
 *
 * These methods return CMPIInstance ( in the case of references() ) or 
 * CMPIObjectPath ( in the case of referenceNames() ) object(s) of th 
 * association itself.
 */


CMPIStatus Linux_RunningOSReferences( CMPIAssociationMI * mi,
				      CMPIContext * ctx,
				      CMPIResult * rslt,
				      CMPIObjectPath * cop,
				      char * assocClass,
				      char * role,
				      char ** propertyList ) {
  CMPIStatus       rc    = {CMPI_RC_OK, NULL};
  CMPIObjectPath * op    = NULL;
  int              refrc = 0;

  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI References()\n", _ClassName ); 

  if( assocClass ) {
    op = CMNewObjectPath( _broker, _ClassName,
			    CMGetCharPtr(CMGetNameSpace(cop,&rc)), &rc );
  }

  if( ( assocClass==NULL ) || ( CMClassPathIsA(_broker,op,assocClass,&rc) == 1 ) ) {
  
    if( _assoc_check_parameter_const( _broker,cop,_RefLeft,_RefRight,
				      _RefLeftClass,_RefRightClass,
				      NULL,role,NULL, 
				      &rc ) == 0 ) { goto exit; }

    /* TODO */
    /* currently not discovered if role is set */
    
    refrc = _assoc_create_refs_1toN(_broker, ctx, rslt, cop, 
				    _ClassName,_RefLeftClass,_RefRightClass,
				    _RefLeft,_RefRight,
				    1, 0, &rc);
    
    if( refrc != 0 ) { 
      if( _debug ) {
	if( rc.msg != NULL ) 
	  { fprintf(stderr,"rc.msg: %s\n",CMGetCharPtr(rc.msg)); }
      }
      CMReturn(CMPI_RC_ERR_FAILED); 
    }
  }

 exit:
  CMReturnDone( rslt );
  CMReturn(CMPI_RC_OK);
}


CMPIStatus Linux_RunningOSReferenceNames( CMPIAssociationMI * mi,
					  CMPIContext * ctx,
					  CMPIResult * rslt,
					  CMPIObjectPath * cop,
					  char * assocClass,
					  char * role) {
  CMPIStatus       rc    = {CMPI_RC_OK, NULL};
  CMPIObjectPath * op    = NULL;
  int              refrc = 0;

  if( _debug )
    fprintf( stderr, "--- %s.c : CMPI ReferenceNames()\n", _ClassName ); 
  
  if( assocClass ) {
    op = CMNewObjectPath( _broker, _ClassName,
			    CMGetCharPtr(CMGetNameSpace(cop,&rc)), &rc );
  }

  if( ( assocClass==NULL ) || ( CMClassPathIsA(_broker,op,assocClass,&rc) == 1 ) ) {
  
    if( _assoc_check_parameter_const( _broker,cop,_RefLeft,_RefRight,
				      _RefLeftClass,_RefRightClass,
				      NULL,role,NULL, 
				      &rc ) == 0 ) { goto exit; }

    /* TODO */
    /* currently not discovered if role is set */
    
    refrc = _assoc_create_refs_1toN(_broker, ctx, rslt, cop, 
				    _ClassName,_RefLeftClass,_RefRightClass,
				    _RefLeft,_RefRight,
				    0, 0, &rc);

    if( refrc != 0 ) { 
      if( _debug ) {
	if( rc.msg != NULL ) 
	  { fprintf(stderr,"rc.msg: %s\n",CMGetCharPtr(rc.msg)); }
      }
      CMReturn(CMPI_RC_ERR_FAILED); 
    }
  }

 exit:
  CMReturnDone( rslt );
  CMReturn(CMPI_RC_OK);
}


/* ---------------------------------------------------------------------------*/
/*                              Provider Factory                              */
/* ---------------------------------------------------------------------------*/

CMInstanceMIStub( Linux_RunningOS, 
		  Linux_RunningOS, 
		  _broker, 
		  CMNoHook);

CMAssociationMIStub( Linux_RunningOS, 
		     Linux_RunningOS, 
		     _broker, 
		     CMNoHook);

/* ---------------------------------------------------------------------------*/
/*                        end of Linux_RunningOS                              */
/* ---------------------------------------------------------------------------*/

