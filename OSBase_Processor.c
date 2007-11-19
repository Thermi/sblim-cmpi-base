/*
 * OSBase_Processor.c
 *
 * (C) Copyright IBM Corp. 2002
 *
 * THIS FILE IS PROVIDED UNDER THE TERMS OF THE COMMON PUBLIC LICENSE
 * ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE
 * CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
 *
 * You can obtain a current copy of the Common Public License from
 * http://www.opensource.org/licenses/cpl1.0.txt
 *
 * Author:       Heidi Neumann <heidineu@de.ibm.com>
 * Contributors: 
 *               
 * Description:
 * This shared library provides resource access functionality for the class
 * Linux_Processor. 
 * It is independent from any specific CIM technology.
*/

/* ---------------------------------------------------------------------------*/

#include "OSBase_Common.h"
#include "OSBase_Processor.h"
#include <unistd.h>

/* ---------------------------------------------------------------------------*/
// private declarations


/* ---------------------------------------------------------------------------*/

static int _processor_data( int, struct cim_processor ** );
static unsigned short _processor_family( int );
static unsigned short _processor_load_perc( int );

char * CPUINFO = "/proc/cpuinfo";

//char * CPUINFO = "/home/heidineu/local/sblim/cmpi-base-cpuinfo/x86_ibm_xSeries_2x";
//char * CPUINFO = "/home/heidineu/local/sblim/cmpi-base-cpuinfo/x86_ibm_xSeries_4x";
//char * CPUINFO = "/home/heidineu/local/sblim/cmpi-base-cpuinfo/x86_64_AMD";

//char * CPUINFO = "/home/heidineu/local/sblim/cmpi-base-cpuinfo/power_ibm_iSeries";
//char * CPUINFO = "/home/heidineu/local/sblim/cmpi-base-cpuinfo/power_ibm_iSeries_2x";
//char * CPUINFO = "/home/heidineu/local/sblim/cmpi-base-cpuinfo/power_ibm_pSeries";
//char * CPUINFO = "/home/heidineu/local/sblim/cmpi-base-cpuinfo/power_ibm_pSeries_2x";

//char * CPUINFO = "/home/heidineu/local/sblim/cmpi-base-cpuinfo/s390_ibm_s390";
//char * CPUINFO = "/home/heidineu/local/sblim/cmpi-base-cpuinfo/s390_ibm_s390_2x";
//char * CPUINFO = "/tmp/ia64_cpuinfo";

/* ---------------------------------------------------------------------------*/

int enum_all_processor( struct processorlist ** lptr ) {
  struct processorlist *  lptrhelp = NULL;
  char                 ** hdout    = NULL;
  char                 *  cmd      = NULL;
  char                 *  ptr      = NULL;
  char                 *  id       = NULL;
  int                     i        = 0;
  int                     rc       = 0;

  _OSBASE_TRACE(3,("--- enum_all_processor() called"));

  lptrhelp = (struct processorlist *) calloc (1,sizeof(struct processorlist));
  *lptr = lptrhelp;

  // check read access to CPUINFO
  if( access(CPUINFO,R_OK) != 0 ) {
    _OSBASE_TRACE(3,("--- enum_all_processor() failed : no read access to %s",CPUINFO));
    return -1;
  }

  cmd = (char *)malloc((strlen(CPUINFO)+46));
  strcpy(cmd, "cat ");
  strcat(cmd, CPUINFO);
  strcat(cmd, " | grep ^processor | sed -e s/processor//");

  rc = runcommand( cmd, NULL, &hdout, NULL );
  if( rc == 0 ) {
    while( hdout[i] != NULL ) { 
      if ( lptrhelp->sptr != NULL) { 
	lptrhelp->next = (struct processorlist *) calloc (1,sizeof(struct processorlist));
	lptrhelp = lptrhelp->next;
      }

      ptr = hdout[i];
      ptr = strchr( ptr , ':' );
//#if defined (INTEL) || defined (PPC) || defined (X86_64) || defined (GENERIC) || defined (IA64)
      id = ptr+1;
#if defined (S390)
      id = (char*)malloc( (strlen(hdout[i])-strlen(ptr)+1) );
      id = strncpy(id, hdout[i], strlen(hdout[i])-strlen(ptr));
#endif

      rc = _processor_data( atoi(id), &(lptrhelp->sptr) );
      i++;
#if defined (S390)
      if(id) free(id);
#endif
    }
  }
  freeresultbuf(hdout);

  if(cmd) free(cmd);
  _OSBASE_TRACE(3,("--- enum_all_processor() exited"));
  return rc;
}

int get_processor_data( char * id, struct cim_processor ** sptr ) {
  char ** hdout = NULL;
  char *  cmd   = NULL;
  int     i     = 0;
  int     rc    = 0;

  _OSBASE_TRACE(3,("--- _get_processor_data() called"));

  // check read access to CPUINFO
  if( access(CPUINFO,R_OK) != 0 ) {
    _OSBASE_TRACE(3,("--- enum_all_processor() failed : no read access to %s",CPUINFO));
    return -1;
  }

  cmd = (char *)malloc((strlen(CPUINFO)+23));
  strcpy(cmd, "cat ");
  strcat(cmd, CPUINFO);  
  strcat(cmd, " | grep ^processor");

  rc = runcommand( cmd, NULL, &hdout, NULL );
  if( rc == 0 ) {
    while( hdout[i] != NULL ) {
      if( strstr(hdout[i],id) != NULL ) { 
	rc = _processor_data( atoi(id), sptr ); 
	break;
      }
      i++;
    }
  }
  freeresultbuf(hdout);
  if( sptr == NULL ) {
    _OSBASE_TRACE(3,("--- get_processor_data() failed : ID %s not valid",id));
  }

  if(cmd) free(cmd);
  _OSBASE_TRACE(3,("--- _get_processor_data() exited"));
  return rc;
}

static int _processor_data( int id, struct cim_processor ** sptr ) {
  char ** hdout = NULL;
  char *  cmd   = NULL;
  char *  ptr   = NULL;
  int     count = 0;
  int     lines = 0;
  int     rc    = 0;

  _OSBASE_TRACE(4,("--- _processor_data() called"));

  count = id;
  lines = id;
  *sptr = (struct cim_processor *) calloc (1,sizeof(struct cim_processor));

  (*sptr)->id = (char*)malloc(20);
  sprintf((*sptr)->id, "%i", id);

  (*sptr)->stat = 1;

  /* Familiy */
  (*sptr)->family = _processor_family(id); 

  /* Stepping */
#if defined (INTEL) || defined (X86_64)
  cmd = (char *)malloc((strlen(CPUINFO)+64));
  strcpy(cmd, "cat ");
  strcat(cmd, CPUINFO);
  strcat(cmd, " | grep stepping");
  rc = runcommand( cmd , NULL , &hdout , NULL );
  if( rc == 0 ) {
    ptr = strrchr( hdout[id], ' ');
    (*sptr)->step = (char*)malloc( (17+strlen((*sptr)->id)+strlen(ptr)) );
    strcpy((*sptr)->step, "stepping level: ");
    strcat((*sptr)->step, ptr);
    ptr = strchr((*sptr)->step, '\n');
    *ptr = '\0';
  }
  freeresultbuf(hdout);
  hdout=NULL;
  if(cmd) free(cmd);
  rc = 0;
#elif defined (S390) || defined (PPC) || defined (IA64)
  (*sptr)->step = (char*)malloc(12);
  strcpy((*sptr)->step,"no stepping");
#elif defined (GENERIC)
  (*sptr)->step = (char*)malloc(10);
  strcpy((*sptr)->step,"not known");
#endif

  /* ElementName */  
  cmd = (char *)malloc((strlen(CPUINFO)+64));
  strcpy(cmd, "cat ");
  strcat(cmd, CPUINFO);
#if defined (INTEL) || defined (X86_64)
  strcat(cmd, " | grep '^model name'");
  rc = runcommand( cmd , NULL , &hdout , NULL );
#elif defined (IA64)
  strcat(cmd, " | grep '^family'");
  rc = runcommand( cmd, NULL , &hdout , NULL );
#elif defined (S390)
  strcat(cmd, " | grep '^vendor'");
  rc = runcommand( cmd, NULL , &hdout , NULL );
#elif defined (PPC)
  strcat(cmd, " | grep '^cpu'");
  rc = runcommand( cmd, NULL , &hdout , NULL );
#elif defined (GENERIC)
  strcat(cmd, " | grep '^processor'");
  rc = runcommand( cmd , NULL , &hdout , NULL );
  if(rc==0) {
    count = 0;
    while(hdout[count]!=NULL) {
      count++;
    }
  }
#endif

  if( rc == 0 ) {
#if defined (S390)
    ptr = strchr( hdout[0], ':');
#elif defined (INTEL) || defined (X86_64) || defined (PPC) || defined (IA64) || defined (GENERIC) 
    ptr = strchr( hdout[id], ':');
#endif
    ptr = ptr+2;
    (*sptr)->name = (char*)malloc((1+strlen(ptr)));
    strcpy((*sptr)->name, ptr);
    ptr = strchr((*sptr)->name, '\n');
    *ptr = '\0';
  }
  freeresultbuf(hdout);
  hdout=NULL;
  if(cmd) free(cmd);
  rc = 0;

  /* LoadPercentage */
  (*sptr)->loadPct = _processor_load_perc(id); 

  /* MaxClockSpeed && CurrentClockSpeed */
  cmd = (char *)malloc((strlen(CPUINFO)+64));
  strcpy(cmd, "cat ");
  strcat(cmd, CPUINFO);
#if defined (INTEL) || defined (X86_64) || defined (IA64)
  strcat(cmd, " | grep 'cpu MHz'");
  rc = runcommand( cmd, NULL, &hdout, NULL );
#elif defined (S390)
  rc = 0; /* clock speed cannot be determined on zSeries */
#elif defined (PPC)
  strcat(cmd, " | grep '^clock' | sed -e s/mhz//i");
  rc = runcommand( cmd, NULL, &hdout, NULL );
#elif defined (GENERIC)
  // find all lines with mhz and take care if mhz occurs
  // more than one times for each processor
  strcat(cmd, " | grep -i 'mhz' | sed -e s/mhz//i");
  rc = runcommand( cmd, NULL, &hdout, NULL );
  if( rc == 0 ) {
    lines = 0;
    while(hdout[lines]!=NULL) {
      lines++;
    }
    id = (lines/count)*id;
    if( lines != count ) {
      if( strstr(hdout[id],"model") != NULL ) {
	id++;
      }
    }
  }
#endif

  if( rc == 0 ) {
#if defined (S390)
    ptr = ":0"; /* don't support clock speed on zSeries */
#elif defined (INTEL) || defined (X86_64) || defined (PPC) || defined (IA64) || defined (GENERIC) 
    ptr = strchr( hdout[id], ':');
#endif
    ptr = ptr+1;
    (*sptr)->curClockSpeed = atol(ptr);
    (*sptr)->maxClockSpeed = atol(ptr);
  }
  freeresultbuf(hdout);
  if(cmd) free(cmd);

  _OSBASE_TRACE(4,("--- _processor_data() exited"));
  return 0;
}

/* map model name of the processor 'id' to the respective representation in CIM */
static unsigned short _processor_family( int id ) {
  char **        hdout = NULL;
  char *         cmd   = NULL;
  unsigned short rv    = 2;       /* Unknown */
  int            rc    = 0;

  _OSBASE_TRACE(4,("--- _processor_family() called"));

  cmd = (char *)malloc((strlen(CPUINFO)+64));
  strcpy(cmd, "cat ");
  strcat(cmd, CPUINFO);

#if defined (INTEL) || defined (S390) || defined (X86_64) || defined (IA64)
  strcat(cmd, " | grep vendor");
  rc = runcommand( cmd, NULL , &hdout , NULL );
#elif defined (PPC)
  strcat(cmd, " | grep cpu");
  rc = runcommand( cmd, NULL , &hdout , NULL );
#endif
  if(cmd) free(cmd);

  if( rc == 0 ) {

    cmd = (char *)malloc((strlen(CPUINFO)+64));
    strcpy(cmd, "cat ");
    strcat(cmd, CPUINFO);

#if defined (S390)
    /* S390 Family */
    if( strstr( hdout[0], "S390") != NULL ) {
      rv = 200;
    }
#elif defined (IA64)
    /* Itanium Family */
    if (strstr (hdout[id], "Intel") ) {
      freeresultbuf(hdout);
      strcat(cmd, " | grep 'family'");
      rc = runcommand( cmd, NULL , &hdout , NULL );
      if ( strstr(hdout[id], "Itanium 2") != NULL ) {
	rv = 180; 
      } else {
	rv = 144;
      }
    }
#elif defined (INTEL) || defined (PPC) || defined (X86_64) 

    /* Intel Family */
    if( strstr( hdout[id], "Intel") != NULL ) {
      freeresultbuf(hdout);
      strcat(cmd, " | grep 'model name'");
      rc = runcommand( cmd, NULL , &hdout , NULL );
      /* 486 */
      if(  strstr( hdout[id], "486") != NULL ) rv = 6; /* 80486 */
      /* Pentium */
      else if( strstr( hdout[id], "Pentium") != NULL ) {
	/* Pro */
	if( strstr( hdout[id], "Pro") != NULL ) rv = 12; /* Pentium(R) Pro */
	/* III */
	else if( strstr( hdout[id], "III") != NULL ) {
	  if( strstr( hdout[id], "Xeon") != NULL ) rv = 176; /* Pentium(R) III Xeon(TM) */
	  else if( strstr( hdout[id], "SpeedStep") != NULL ) rv = 177; /* Pentium(R) III Processor with Intel(R) SpeedStep(TM) Technology */
	  else rv = 17; /* Pentium(R) III */
	}
	/* II */
	else if( strstr( hdout[id], "II") != NULL ) {
	  if( strstr( hdout[id], "Xeon") != NULL ) rv = 16; /* Pentium(R) II Xeon(TM) */
	  else rv = 13; /* Pentium(R) II */
	}
	else if( strstr( hdout[id], "MMX") != NULL ) rv = 14; /* Pentium(R) Processor with MMX(TM) Technology */
	else if( strstr( hdout[id], "Celeron") != NULL ) rv = 15; /* Celeron(TM) */
	else if( strstr( hdout[id], "4") != NULL ) rv = 15; /* Celeron(TM) */
	else rv = 11; /* Pentium(R) Brand */
      }
      else rv = 1; /* Other */
    }

    /* AMD Family */
    else if( strstr( hdout[id], "AMD") != NULL ) {
      freeresultbuf(hdout);
      strcat(cmd, " | grep 'model name'");
      rc = runcommand( cmd, NULL , &hdout , NULL );
      /* 486 */
      if(  strstr( hdout[id], "486") != NULL ) rv = 24; /* AMD Duron(TM) Processor Family */
      /*  */
      else if( strstr( hdout[id], "K5") != NULL ) rv = 25; /* K5 Family */
      else if( strstr( hdout[id], "K6-2") != NULL ) rv = 27; /* K6-2 */
      else if( strstr( hdout[id], "K6-3") != NULL ) rv = 28; /* K6-3 */
      else if( strstr( hdout[id], "K6") != NULL ) rv = 26; /* K6 Family */
      else if( strstr( hdout[id], "Athlon") != NULL ) rv = 26; /* AMD Athlon(TM) Processor Family */
    }

    /* Power PC Family */
    else if( strstr( hdout[id], "POWER") != NULL ) {
      rv = 32;
    }

#endif

    if(cmd) free(cmd);

  }
  freeresultbuf(hdout);

  _OSBASE_TRACE(4,("--- _processor_family() exited : %i",rv));
  return rv;
}

/* calculates the load of the processor 'id' in percent */
static unsigned short _processor_load_perc( int id ) {
  char ** hdout = NULL;
  char ** hderr = NULL;
  char ** data  = NULL;
  char *  sid   = NULL;
  char *  cmd   = NULL;
  unsigned long  load      = 0;
  unsigned long  loadTotal = 0;
  unsigned short loadPct   = 0;
  int            rc        = 0;

  _OSBASE_TRACE(4,("--- _processor_load_perc() called"));

  sid = (char*)malloc(5);
  sprintf(sid, "%i", id);

  cmd = (char*)malloc((26+strlen(sid)));
  strcpy(cmd, "cat /proc/stat");
  rc = runcommand( cmd , NULL , &hdout , &hderr );
  if( rc != 0 ) {
    if(hdout != NULL) {
      if( hdout[0] != NULL )
	{ _OSBASE_TRACE(3,("--- _processor_load_perc() failed : %s",hdout[0])); }
      freeresultbuf(hdout);
      freeresultbuf(hderr);
      free ( cmd );
      free ( sid );
      return rc;
    }
  }
  rc = 0;
  freeresultbuf(hdout);
  freeresultbuf(hderr);

  strcat(cmd, " | grep cpu");
  strcat(cmd, sid);
  rc = runcommand( cmd , NULL , &hdout , NULL );
  if(cmd) free(cmd);
  if( rc == 0 ) {
    data = line_to_array( hdout[0], ' ');
    loadTotal = atol(data[1])+atol(data[2])+atol(data[3])+atol(data[4]);
    load = loadTotal - atol(data[4]);
    loadPct = 100*load/loadTotal;
    freeresultbuf(data);
  }
  freeresultbuf(hdout);
  if(sid) free(sid);

  _OSBASE_TRACE(4,("--- _processor_load_perc() exited : %i",loadPct));
  return loadPct;
}


/* ---------------------------------------------------------------------------*/

void free_processorlist( struct processorlist * lptr ) {
  struct processorlist * ls = NULL ;

  if( lptr == NULL ) return;
  for( ; lptr ; ) {
    if(lptr->sptr) { free_processor(lptr->sptr); }
    ls = lptr;
    lptr = lptr->next;
    free(ls);
  }
}

void free_processor( struct cim_processor * sptr ) {
  if( sptr == NULL ) return;
  if(sptr->id) free(sptr->id); 
  if(sptr->step) free(sptr->step); 
  if(sptr->name) free(sptr->name); 
  free(sptr);
}

/* ---------------------------------------------------------------------------*/
/*                          end of OSBase_Processor.c                         */
/* ---------------------------------------------------------------------------*/

