/*
 * mlogsup.h
 *
 * THIS FILE IS PROVIDED UNDER THE TERMS OF THE COMMON PUBLIC LICENSE
 * ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE
 * CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
 *
 * You can obtain a current copy of the Common Public License from
 * http://www.opensource.org/licenses/cpl1.0.txt
 *
 * Author: Adrian Schuur <schuur@de.ibm.com>
 * Contributors:
 *
 * Description: Functions to log and free malloc'ed areas.
 */


#ifndef _MLOGSUP_H_
#define _MLOGSUP_H_

#ifdef DEBUGM
   #define DBGM0(m) \
      fprintf(stderr,"- DBGM0 - %s(%d): " m "\n",__FILE__,__LINE__)
   #define DBGM1(m,p1) \
      fprintf(stderr,"- DBGM1 - %s(%d): " m "\n",__FILE__,__LINE__,p1)
   #define DBGM2(m,p1,p2) \
      fprintf(stderr,"- DBGM2 - %s(%d): " m "\n",__FILE__,__LINE__,p1,p2)
   #define DBGM3(m,p1,p2,p3) \
      fprintf(stderr,"- DBGM3 - %s(%d): " m "\n",__FILE__,__LINE__,p1,p2,p3)
   #define DBGM4(m,p1,p2,p3,p4) \
      fprintf(stderr,"- DBGM4 - %s(%d): " m "\n",__FILE__,__LINE__,p1,p2,p3,p4)
#else
   #define DBGM0(m) ;
   #define DBGM1(m,p1) ;
   #define DBGM2(m,p1,p2) ;
   #define DBGM3(m,p1,p2,p3) ;
   #define DBGM4(m,p1,p2,p3,p4) ;
#endif


#ifdef __cplusplus
extern "C" {
#endif

typedef struct mLogHeader {
   int max,cur;
   void* area[1];
} MLogHeader;


typedef struct mLogIndex {
   int max,free;
   union {
      MLogHeader* hdr[1];
      unsigned int nextFree[1];
   } entry;
} MLogIndex;


extern int makeMLog();
extern void* addToMLog(int idx,void* area);
extern void freeMLog(int idx);
extern void freeMLogAll();

#ifdef __cplusplus
}
#endif


#endif
