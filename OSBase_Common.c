/*
 * OSBase_Common.c
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
 * Contributors: Viktor Mihajlovski <mihajlov@de.ibm.com>
 *               C. Eric Wu <cwu@us.ibm.com>
 *
 * Description:
 * This shared library provides common functionality for OS Base Instrumentation,
 * and all other SBLIM packages. It is independent from any specific CIM class
 * or CIM technology at all.
*/

/* ---------------------------------------------------------------------------*/

#define _GNU_SOURCE

#ifdef DEBUG
    int _debug = 1;
#else
    int _debug = 0;
#endif

#include "OSBase_Common.h"

#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>  
#include <dirent.h>
#include <sys/stat.h>    
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>

/* ---------------------------------------------------------------------------*/

static char* _FILENAME = "OSBase_Common.c";

/* ---------------------------------------------------------------------------*/

char * CIM_HOST_NAME          = NULL;
char * CIM_OS_NAME            = NULL;
int    CIM_OS_TIMEZONE        = 999;
unsigned long CIM_OS_BOOTTIME = 0;

/* ---------------------------------------------------------------------------*/

/* initializes the variable CIM_HOST_NAME
 * contains the full qualified IP hostname of the system, e.g. host.domain
 */

char * get_system_name() {
  char *  host   = NULL;
  char *  domain = NULL;
  char *  ptr    = NULL;
  char ** hdout  = NULL;
  int     rc     = 0;

  if( !CIM_HOST_NAME ) {

    if( _debug ) { fprintf(stderr, "--- %s : get_system_name()\n",_FILENAME); }

    host = (char *)malloc(255*sizeof(char));
    if ( gethostname(host, 255 ) == -1 ) { return NULL ; }
    /* if host does not contain a '.' we can suppose, that the domain is not
     * available in the current value. but we try to get the full qualified
     * hostname.
     */
    if( strchr( host , '.') == NULL ) {
      /* get domain name */
      rc=runcommand("/bin/dnsdomainname",NULL,&hdout,NULL);
      if (rc == 0 && hdout != NULL) {
	if (hdout[0]) {
	  domain = strdup(hdout[0]);
	  ptr = strchr(domain, '\n');
	  *ptr = '\0';
	}
	freeresultbuf(hdout);
      }
    }

    /* initializes CIM_HOST_NAME */
    if( strlen(host) ) {
      if( !domain ) {
	CIM_HOST_NAME = (char *)malloc( (strlen(host)+1)*sizeof(char) );
	strcpy( CIM_HOST_NAME, host);
      }
      else {
	CIM_HOST_NAME = (char *)malloc( (strlen(host)+strlen(domain)+2)*sizeof(char) );
	strcpy( CIM_HOST_NAME, host);
	strcat( CIM_HOST_NAME, ".");
	strcat( CIM_HOST_NAME, domain );
      }
    }

    if(host) free(host);
    if(domain) free(domain);

    if( _debug ) { fprintf(stderr,"CIM_HOST_NAME initialized with %s\n",CIM_HOST_NAME); }
  }
  return CIM_HOST_NAME;
}


/* ---------------------------------------------------------------------------*/

/* ---------------------------------------------------------------------------*/

char * get_os_name(){

  if( _debug && !CIM_OS_NAME )
    { fprintf(stderr, "--- %s : get_os_name()\n",_FILENAME); }

  if( !CIM_OS_NAME ) {
    get_system_name();
    if( CIM_HOST_NAME ) {
      CIM_OS_NAME = (char*) malloc (strlen(CIM_HOST_NAME)+1);
      strcpy( CIM_OS_NAME, CIM_HOST_NAME );
    }

    if( _debug ) { fprintf(stderr,"CIM_OS_NAME initialized with %s\n",CIM_OS_NAME); }
  }
  return CIM_OS_NAME;
}

/* ---------------------------------------------------------------------------*/

/* ---------------------------------------------------------------------------*/

signed short get_os_timezone() {
  char ** hdout  = NULL;
  char *  ptr    = NULL;
  char    hour[] = "00";
  char    min[]  = "00"; 
  int     rc     = 0;

  if( _debug ) { fprintf(stderr, "--- %s : get_os_timezone()\n",_FILENAME); }

  if( CIM_OS_TIMEZONE == 999 ) {
    rc = runcommand( "date +%z" , NULL , &hdout , NULL );
    if( rc == 0 ) {
      ptr = hdout[0]+1;
      strncpy(hour,ptr,2);
      ptr = hdout[0]+3;
      strncpy(min,ptr,2);
      CIM_OS_TIMEZONE = (atoi(hour)*60)+atoi(min);
      //      freeresultbuf(hdout);
    }
    else {
      fprintf(stderr, "--- %s : command date +z not successful returned - Timezone set to 0\n",_FILENAME);
      CIM_OS_TIMEZONE = 0;
    }
  }
  return CIM_OS_TIMEZONE;
}

unsigned long _get_os_boottime() {
  char ** hdout = NULL;
  int     rc    = 0;

  if( _debug ) { fprintf(stderr, "--- %s : get_os_boottime()\n",_FILENAME); }

  if( CIM_OS_BOOTTIME == 0 ) {
    rc = runcommand( "cat /proc/stat | grep btime" , NULL , &hdout , NULL );
    if( rc == 0 ) {
      CIM_OS_BOOTTIME = atol(hdout[0]+6);
      freeresultbuf(hdout);
    }
    else {
      fprintf(stderr, "--- %s : was not able to get boottime - set to 0\n",_FILENAME);
    }
  }
  return CIM_OS_BOOTTIME;
}

/* add timezone to datetime 'str'of format %Y%m%d%H%M%S.000000
 * out : yyyyMMddHHmmss.SSSSSSsutc, e.g. 20030112152345.000000+100
 */
void _cat_timezone( char * str , signed short zone ) {
  char * tz = NULL;

  tz = (char *)malloc(5*sizeof(char));
  sprintf(tz, "%i", zone);
  if( str != NULL ) {
    if(zone > 0) {
      strcat(str,"+");
    }
    strcat(str,tz);
  }
  if(tz) free(tz);
}


/* ---------------------------------------------------------------------------*/
/* tool function : executes commandlines and returns their output             */
/* ( stdout and stderr )                                                      */
/* ---------------------------------------------------------------------------*/


static void addstring(char ***buf, const char *str);

int runcommand(const char *cmd, char **in, char ***out, char ***err)
{
  int rc;
  char fcltinname[] = "/tmp/SBLIMXXXXXX";
  char fcltoutname[] = "/tmp/SBLIMXXXXXX";
  char fclterrname[] = "/tmp/SBLIMXXXXXX";
  int fdin, fdout, fderr;
  char * cmdstring;
  char * buffer;
  FILE * fcltin;
  FILE * fcltout;
  FILE * fclterr;

  /* No pipes -- real files */

  /* make the filenames for I/O redirection */
  if (in != NULL && ((fdin=mkstemp(fcltinname)) < 0 || close(fdin))) {
    perror("Couldn't create input file");
    return -1;
  }
  if (out != NULL && ((fdout=mkstemp(fcltoutname)) < 0 || close(fdout))) {
    perror("Couldn't create output file");
    return -1;
  }
  if (err != NULL && ((fderr=mkstemp(fclterrname)) < 0 || close(fderr))) {
    perror("Couldn't create error file");
    return -1;
  }

  /* if necessary feed in-buffer to child's stdin */
  if (in != NULL) {
    fcltin = fopen(fcltinname,"w");
    if (fcltin==NULL) {
      perror("Couldn't open client's input for writing");
      return -1;
    }
    while (*in != NULL)
      fprintf(fcltin,"%s\n",*in++);
    /* flush and close */
    fclose(fcltin);
  }

  /* prepare full command string */
  cmdstring = malloc(strlen(cmd) + 1 +
		     (in?strlen(fcltinname)+2:0) +
		     (out?strlen(fcltoutname)+3:0) +
		     (err?strlen(fclterrname)+3:0));
  strcpy(cmdstring,cmd);
  if (in) {
    strcat(cmdstring," <");
    strcat(cmdstring,fcltinname);
  }
  if (out) {
    strcat(cmdstring," >");
    strcat(cmdstring,fcltoutname);
  }
  if (err) {
    strcat(cmdstring," 2>");
    strcat(cmdstring,fclterrname);
  }
  /* perform the system call */
  if( _debug ) { fprintf(stderr,"runcommand: %s\n",cmdstring); }
  rc=system(cmdstring);
  free(cmdstring);

  buffer = NULL;
  /* cleanup in file. if necessary */
  if (in != NULL)
    unlink(fcltinname);
  /* retrieve data from stdout and stderr */
  if (out != NULL) {
    *out=calloc(1,sizeof(char*));
    buffer = malloc(4096);
    fcltout = fopen(fcltoutname,"r");
    if (fcltout!=NULL) {
      while(!feof(fcltout)) {
	if (fgets(buffer,4096,fcltout) != NULL)
	  addstring(out,buffer);
	else
	  break;
      }
      /* close out file */
      fclose(fcltout);
    } else
      perror("Couldn't open client's output for reading");
    unlink(fcltoutname);
  }
  if (err != NULL) {
    *err=calloc(1,sizeof(char*));
    if (buffer == NULL)
      buffer = malloc(4096);
    fclterr = fopen(fclterrname,"r");
    if (fclterr!=NULL) {
	while(!feof(fclterr)) {
	  if (fgets(buffer,4096,fclterr) != NULL)
	    addstring(err,buffer);
	  else
	    break;
	}
	/* close err file */
	fclose(fclterr);
    } else
      perror("Couldn't open client's error output for reading");
    unlink(fclterrname);
  }
  if (buffer != NULL)
    free(buffer);
  return rc;
}

void freeresultbuf(char ** buf)
{
  char ** bufstart = buf;
  if (buf == NULL)
    return;
  while(*buf != NULL) {
    free(*buf++);
  }
  free(bufstart);
}

static void addstring(char *** buf, const char * str)
{
  int i=0;
  while ((*buf)[i++] != NULL);
  *buf=realloc(*buf,(i+1)*sizeof(char*));
  (*buf)[i-1] = strdup(str);
  (*buf)[i] = NULL;
}

/* ---------------------------------------------------------------------------*/
// end of tool function to executes commandlines
/* ---------------------------------------------------------------------------*/



/* ---------------------------------------------------------------------------*/
// tool function to convert ":"-seperated lines
/* ---------------------------------------------------------------------------*/

/* converts a line into an array of char * , while
 * c ( e.g. ':' ) acts as seperator
 *
 * example :
 * line  -> value1:value2:value3'\n'
 *
 * array -> value1
 *          value2
 *          value3
 *          NULL
 */

char ** line_to_array( char * buf , int c ){
  char ** data = NULL ;
  char *  str  = NULL ;
  char *  ptr  = NULL ;
  char *  ent  = NULL ;

  str = strdup(buf);
  data = calloc(1,sizeof(char*));
  ent = str ;

  while( ( ptr = strchr( ent , c )) != NULL ) {
    *ptr='\0';
    addstring( &data , ent );
    ent = ptr+1;
    ptr = NULL ;
  }
  addstring( &data , ent );

  if(str) free(str);
  return data;
}

/* ---------------------------------------------------------------------------*/
// end of tool function to convert ":"-seperated lines
/* ---------------------------------------------------------------------------*/



/* ---------------------------------------------------------------------------*/
/*
 * get system parameter from a file entry, such as /proc/sys/kernel/shmmax
 * and put it in the character array pointed by buffer
 * It returns the number of bytes read, or -1 in case of any error
 */

int get_system_parameter(char *path, char *entry, char *buffer, int size) {
  char * completePath = NULL;
  FILE * file         = NULL;
  int    res          = 0;

  if( _debug ) { fprintf(stderr, "--- %s : get_system_parameter()\n",_FILENAME); }  

  if (path == NULL || entry == NULL || buffer == NULL)
    return -1;
  completePath = (char *)malloc(strlen(path) + strlen(entry) + 1);
  if (completePath == NULL)
    return -1;
  sprintf(completePath, "%s/%s", path, entry);
  if ((file = fopen(completePath, "r")) == NULL)
    return -1;
  res = fread(buffer, 1, size - 1, file);
  fclose(file);
  if (res > 0)
    buffer[res] = '\0';
  free(completePath);
  return res;
}


/* ---------------------------------------------------------------------------*/
/*
 * set system parameter to a file entry, such as /proc/sys/kernel/shmmax
 * using the value string 
 * It returns the number of bytes written, or -1 in case of any error
 */

int set_system_parameter(char *path, char *entry, char *value) {
  char * completePath = NULL;
  FILE * file         = NULL;
  int    res          = 0;

  if( _debug ) { fprintf(stderr, "--- %s : set_system_parameter()\n",_FILENAME); }  

  if (path == NULL || entry == NULL || value == NULL)
    return -1;
  completePath = (char *)malloc(strlen(path) + strlen(entry) + 1);
  if (completePath == NULL)
    return -1;
  sprintf(completePath, "%s/%s", path, entry);
  if ((file = fopen(completePath, "w")) == NULL)
    return -1;
  res = fwrite(value, 1, strlen(value), file);
  fclose(file);
  free(completePath);
  return res;
}


/* ---------------------------------------------------------------------------*/
