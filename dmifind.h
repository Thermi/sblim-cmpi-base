/*
 * $Id: dmifind.h,v 1.1 2005/01/24 11:29:05 heidineu Exp $
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
 * Contributors: James Jeffers <jamesjef@us.ibm.com>
 *
 * Interface Type : Common Manageability Programming Interface ( CMPI )
 *
 * Description: Low-level DMI BIOS Data Structures for BIOS and
 *              System Informations.
 *
 */

#ifndef DMIFIND_H
#define DMIFIND_H

#ifdef __cplusplus
extern "C" {
#endif

struct DMI_FIND {
  char           dmi_sig[5]; /* 0 - 4 */
  char           dmi_res1;   /* 5 */
  unsigned short dmi_len;    /* 6 - 7 */
  unsigned       dmi_base;   /* 8 - 11 */
  unsigned short dmi_num;    /* 12 - 13 */
  unsigned char  dmi_ver;    /* 14 */
  char           dmi_res2;   /* 15 */
};


struct DMI_HDR {
  char           dmi_type;   /* 0 */
  unsigned char  dmi_hdrlen; /* 1 */
  unsigned short dmi_handle; /* 2-3 */
};

struct DMI_BIOS_INFO {
  unsigned char  bios_vendor_idx;  /* 0 */
  unsigned char  bios_version_idx; /* 1 */
  unsigned short bios_segment;     /* 2-3 */
  unsigned char  bios_date_idx;    /* 4 */
  unsigned char  bios_segment_num; /* 5 */
  unsigned long long bios_characteristics; /* 6-13 */
};

struct DMI_SYSTEM_INFO {
  unsigned char  system_manufacturer_idx; /* 0 */
  unsigned char  system_product_idx;      /* 1 */
  unsigned char  system_version_idx;      /* 2 */
  unsigned char  system_serial_idx;       /* 3 */
};

struct DMI_HWSECURITY_INFO {
  unsigned char  hwsecurity_setting; /* 0 */
};

struct DMI_STRING_MAP {
  char* dmi_string[255];
};

#ifdef __cplusplus
}
#endif

#endif