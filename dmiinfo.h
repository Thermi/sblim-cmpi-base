/*
 * $Id: dmiinfo.h,v 1.1 2005/01/24 11:29:05 heidineu Exp $
 *
 * (C) Copyright IBM Corp. 2003
 *
 * THIS FILE IS PROVIDED UNDER THE TERMS OF THE COMMON PUBLIC LICENSE 
 * ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE 
 * CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
 *
 * You can obtain a current copy of the Common Public License from
 * http://oss.software.ibm.com/developerworks/opensource/license-cpl.html
 *
 * Author:       Viktor Mihajlovski <mihajlov@de.ibm.com>
 * Contributors:
 *
 * Interface Type : Common Manageability Programming Interface ( CMPI )
 *
 * Description: Data Structures and API for SMBIOS/DMI Resource Access.
 *
 */

#ifndef DMIINFO_H
#define DMIINFO_H

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _DMI_BIOSPRODUCT {
  char * dmi_Name;
  char * dmi_IdentifyingNumber;
  char * dmi_Vendor;
  char * dmi_Version;
} DMI_BIOSPRODUCT;

typedef struct _DMI_BIOSFEATURE {
  char * dmi_Name;
  char * dmi_ProductName;
  char * dmi_IdentifyingNumber;
  char * dmi_Vendor;
  char * dmi_Version;
} DMI_BIOSFEATURE;

typedef struct _DMI_BIOSELEMENT {
  char   * dmi_Name;
  char   * dmi_Version;
  short    dmi_SWEState;
  char   * dmi_SWEID;
  short    dmi_TargetOS;
  char   * dmi_Manufacturer;
  char   * dmi_Serial;
  unsigned dmi_StartingAddress;
  unsigned dmi_EndingAddress;
  time_t   dmi_ReleaseDate;
} DMI_BIOSELEMENT;

typedef struct _DMI_HWSECURITY {
  char *        dmi_Name;
  unsigned char dmi_PowerOnPasswordState;
  unsigned char dmi_AdminPasswordState;
} DMI_HWSECURITY;

void cimdmi_init();
void cimdmi_term();
DMI_BIOSPRODUCT * cimdmi_getBiosProduct();
DMI_BIOSFEATURE * cimdmi_getBiosFeature();
DMI_BIOSELEMENT * cimdmi_getBiosElement();
DMI_HWSECURITY  * cimdmi_getHwSecurity();

#ifdef __cplusplus
}
#endif

#endif
