/*
 * OSBase_Processor.c
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


static char* _FILENAME = "OSBase_Processor.c";

/* ---------------------------------------------------------------------------*/

static int _processor_data( int, struct cim_processor ** );
static unsigned short _processor_family( int );
static unsigned short _processor_load_perc( int );

/* ---------------------------------------------------------------------------*/

int enum_all_processor( struct processorlist ** lptr ) {
  struct processorlist *  lptrhelp = NULL;
  char                 ** hdout    = NULL;
  char                 *  ptr      = NULL;
  char                 *  id       = NULL;
  int                     i        = 0;
  int                     rc       = 0;

  if( _debug ) { fprintf(stderr, "--- %s : enum_all_processor()\n",_FILENAME); }  

  lptrhelp = (struct processorlist *) malloc (sizeof(struct processorlist));
  memset(lptrhelp, 0, sizeof(struct processorlist));
  *lptr = lptrhelp;

  rc = runcommand( "cat /proc/cpuinfo | grep ^processor | sed -e s/processor//" , NULL , &hdout , NULL );
  if( rc == 0 ) {
    while( hdout[i] != NULL ) { 
      if ( lptrhelp->sptr != NULL) { 
	lptrhelp->next = (struct processorlist *) malloc (sizeof(struct processorlist));
	memset(lptrhelp->next, 0, sizeof(struct processorlist));
	lptrhelp = lptrhelp->next;
      }

      ptr = hdout[i];
      ptr = strchr( ptr , ':' );
#if defined (INTEL) || defined (PPC)
      id = ptr++;
#elif defined (S390)
      id = (char*)malloc( (strlen(hdout[i])-strlen(ptr)+1)*sizeof(char) );
      id = strncpy(id, hdout[i], strlen(hdout[i])-strlen(ptr));
#endif
      rc = _processor_data( atoi(id), &(lptrhelp->sptr) );
      i++;
    }
    freeresultbuf(hdout);
  }

  return rc;
}

int get_processor_data( char * id, struct cim_processor ** sptr ) {
  char ** hdout = NULL;
  int     i     = 0;
  int     rc    = 0;

  if( _debug ) { fprintf(stderr, "--- %s : get_processor_data()\n",_FILENAME); }  

  rc = runcommand( "cat /proc/cpuinfo | grep ^processor" , NULL , &hdout , NULL );
  if( rc == 0 ) {
    while( hdout[i] != NULL ) {
      if( strstr(hdout[i],id) != NULL ) { 
	rc = _processor_data( atoi(id), sptr ); 
	break;
      }
      i++;
    }
    freeresultbuf(hdout);
  }
  return rc;
}

static int _processor_data( int id, struct cim_processor ** sptr ) {
  char ** hdout = NULL;
  char *  ptr   = NULL;
  int     rc    = 0;

  if( _debug ) { fprintf(stderr, "--- %s : _processor_data()\n",_FILENAME); }  

  *sptr = (struct cim_processor *) malloc (sizeof(struct cim_processor));
  memset(*sptr, 0, sizeof(struct cim_processor));

  (*sptr)->id = (char *) malloc (20*sizeof(char));
  sprintf((*sptr)->id, "%i", id);

  (*sptr)->stat = 1;

  /* Familiy */
  (*sptr)->family = _processor_family(id); 

  /* Stepping */
#if defined (INTEL)
  rc = runcommand( "cat /proc/cpuinfo | grep stepping" , NULL , &hdout , NULL );
  if( rc == 0 ) {
    ptr = strrchr( hdout[id], ' ');
    (*sptr)->step = (char*)malloc( (17+strlen((*sptr)->id)+strlen(ptr))*sizeof(char) );
    strcpy((*sptr)->step, "stepping level: ");
    strcat((*sptr)->step, ptr);
    ptr = strchr((*sptr)->step, '\n');
    *ptr = '\0';
  }
  freeresultbuf(hdout);
  rc = 0;
#elif defined (S390) || defined (PPC)
  (*sptr)->step = (char*)malloc( 12*sizeof(char) );
  strcpy((*sptr)->step,"no stepping");
#endif

  /* ElementName */
#if defined (INTEL)
  rc = runcommand( "cat /proc/cpuinfo | grep '^model name'" , NULL , &hdout , NULL );
#elif defined (S390)
  rc = runcommand( "cat /proc/cpuinfo | grep '^vendor_id'" , NULL , &hdout , NULL );
#elif defined (PPC)
  rc = runcommand( "cat /proc/cpuinfo | grep '^cpu'" , NULL , &hdout , NULL );
#endif
  if( rc == 0 ) {
    ptr = strchr( hdout[id], ':');
    ptr = ptr+2;
    (*sptr)->name = (char*)malloc( (1+strlen(ptr))*sizeof(char) );
    strcpy((*sptr)->name, ptr);
    ptr = strchr((*sptr)->name, '\n');
    *ptr = '\0';
  }
  freeresultbuf(hdout);
  rc = 0;

  /* LoadPercentage */
  (*sptr)->loadPct = _processor_load_perc(id); 

  /* MaxClockSpeed && CurrentClockSpeed */
#if defined (INTEL)
  rc = runcommand( "cat /proc/cpuinfo | grep 'cpu MHz'" , NULL , &hdout , NULL );
#elif defined (S390)
  rc = runcommand( "cat /proc/cpuinfo | grep 'bogomips per cpu'" , NULL , &hdout , NULL );
#elif defined (PPC)
  rc = runcommand( "cat /proc/cpuinfo | grep '^clock' | sed -e s/mhz//i" , NULL , &hdout , NULL );
#endif
  if( rc == 0 ) {
    ptr = strchr( hdout[id], ':');
    ptr = ptr+1;
    (*sptr)->curClockSpeed = atol(ptr);
    (*sptr)->maxClockSpeed = atol(ptr);
  }
  freeresultbuf(hdout);

  return 0;
}

/* map model name of the processor 'id' to the respective representation in CIM */
static unsigned short _processor_family( int id ) {
  char **        hdout = NULL;
  unsigned short rv    = 0;
  int            rc    = 0;

  if( _debug ) { fprintf(stderr, "--- %s : _processor_family()\n",_FILENAME); }  

#if defined (INTEL) || defined (S390)
  rc = runcommand( "cat /proc/cpuinfo | grep vendor_id" , NULL , &hdout , NULL );
#elif defined (PPC)
  rc = runcommand( "cat /proc/cpuinfo | grep cpu" , NULL , &hdout , NULL );
#endif
  if( rc == 0 ) {
    /* Intel Family */
    if( strstr( hdout[id], "Intel") != NULL ) {
      freeresultbuf(hdout);
      rc = runcommand( "cat /proc/cpuinfo | grep 'model name'" , NULL , &hdout , NULL );
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
      rc = runcommand( "cat /proc/cpuinfo | grep 'model name'" , NULL , &hdout , NULL );
      /* 486 */
      if(  strstr( hdout[id], "486") != NULL ) rv = 24; /* AMD Duron(TM) Processor Family */
      /*  */
      else if( strstr( hdout[id], "K5") != NULL ) rv = 25; /* K5 Family */
      else if( strstr( hdout[id], "K6-2") != NULL ) rv = 27; /* K6-2 */
      else if( strstr( hdout[id], "K6-3") != NULL ) rv = 28; /* K6-3 */
      else if( strstr( hdout[id], "K6") != NULL ) rv = 26; /* K6 Family */
      else if( strstr( hdout[id], "Athlon") != NULL ) rv = 26; /* AMD Athlon(TM) Processor Family */
    }

    /* S390 Family */
    else if( strstr( hdout[id], "S390") != NULL ) {
      rv = 200;
    }

    /* Power PC Family */
    else if( strstr( hdout[id], "POWER") != NULL ) {
      rv = 32;
    }
  }
  else rv = 2; /* Unknown */
  freeresultbuf(hdout);
  return rv;
}

/* calculates the load of the processor 'id' in percent */
static unsigned short _processor_load_perc( int id ) {
  char ** hdout = NULL;
  char ** data  = NULL;
  char *  sid   = NULL;
  char *  cmd   = NULL;
  unsigned long  load      = 0;
  unsigned long  loadTotal = 0;
  unsigned short loadPct   = 0;
  int            rc        = 0;

  if( _debug ) { fprintf(stderr, "--- %s : _processor_load_perc()\n",_FILENAME); }  

  sid = (char *) malloc (5*sizeof(char));
  sprintf(sid, "%i", id);

  cmd = (char*)malloc( (26+strlen(sid))*sizeof(char) );
  strcpy(cmd, "cat /proc/stat | grep cpu");
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

