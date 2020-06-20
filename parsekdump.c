
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CMD_SIZE ( 10 * PATH_MAX )
#define SYSCALL_NAME_SIZE 128

struct timepoint  {

uint64_t sec;
uint64_t usec; // microseconds

};

int add_time( struct timepoint *tp,
      struct timepoint *tp_add )  {

uint64_t maxint = ( uint64_t ) -1;

if( maxint - tp->sec < tp_add->sec )
return -1;
tp->sec += tp_add->sec;

if( maxint - tp->usec < tp_add->usec )
return -1;
tp->usec += tp_add->usec;

// and now add seconds that we got from usec.
uint64_t sec = tp->usec % 1000000;

if( maxint - tp->sec < sec )  return -1;
tp->sec += sec;
return 0;

}

void  init_time( struct timepoint *tp )  {

tp->sec = 0;
tp->usec = 0;

}

int time_diff( struct timepoint *tp_old,
       struct timepoint *tp_new )  {

uint64_t sec = 0;
if( tp_new->usec < tp_old->usec )  {

sec = 1;
tp_new->usec += 1000000; // add one second
if( tp_new->usec < tp_old->usec )  return -1;
tp_new->usec -= tp_old->usec;

}

if( tp_new->sec < tp_old->sec )  return -1;
tp_new->sec -= tp_old->sec;
if( sec && ( tp_new->sec == 0 ) )  return -1;
tp_new->sec -= sec;

return 0;

}

int gettime( char *time,
struct timepoint *tp )  {

if( sscanf( time, "%" SCNu64 "llu.%" SCNu64 "llu",
&( tp->sec ), &( tp->usec ) ) != 2 )  return -1;
return 0;

}

struct syscall_data  {

  char *name;
  uint64_t count;
  struct timepoint tp_sum;

};

struct pending_syscall  {

  char *syscall;
  struct timepoint tp;

};

struct loaded_syscall  {

  char *pid;
  char *tid;
  char *time;
  char *syscall_type;
  char *syscall;

};

void free_ls( struct loaded_syscall *ls )  {

  free( ls->pid );
  free( ls->tid );
  free( ls->time );
  free( ls->syscall_type );
  free( ls->syscall );

}

struct proc_data  {

  char *pid;
  char *tid;
  struct syscall_data *sd;
  struct pending_syscall *ps;
  size_t sd_len;
  size_t ps_len;

};

void init_pd( struct proc_data *pd )  {

  pd->pid = NULL;
  pd->tid = NULL;
  pd->sd = NULL;
  pd->sd_len = 0;
  pd->ps = NULL;
  pd->ps_len = 0;

}

int grow_pd( struct proc_data **pd, size_t *pd_len )  {

  ( *pd_len )++;
  *pd = realloc( *pd, *pd_len * sizeof **pd );
  if( *pd == NULL )  return -1;
  return 0;

}

// moves to char or nul
void movetochr( char **linepos, char chr )  {

  for(;;)  {

    if( **linepos == '\0' )  return;
    if( **linepos == chr )  return;
    ( *linepos )++;

  }

}

void movebackword( char **linepos,
		   char *minpos )  {

  for(;;)  {

    if( *linepos == minpos ) return;
    if( isspace( **linepos ) ) return;
    ( *linepos )--;

  }

}

void movebackspace( char **linepos,
		   char *minpos )  {

  for(;;)  {

    if( *linepos == minpos ) return;
    if( ! isspace( **linepos ) ) return;
    ( *linepos )--;

  }

}

// moves to nonspace or nul
void passspace( char **linepos )  {

  for(;;)  {

    if( **linepos == '\0' )  return;
    if( ! isspace( **linepos ) )  return;
    ( *linepos )++;

  }

}

// moves to space or nul
void password( char **linepos )  {

  for(;;)  {

    if( **linepos == '\0' )  return;
    if( isspace( **linepos ) )  return;
    ( *linepos )++;

  }

}

//gets word AND moves linepos 
char *getword( char **linepos )  {

  passspace( linepos );
  char *wordpos = *linepos;
  password( linepos );
  
  // count word and nul;
  size_t word_size = *linepos - wordpos + 1;
  char *word = malloc( word_size );
  if( word == NULL )  return NULL;
  strncpy( word, wordpos, word_size - 1 );
  word[ word_size - 1 ] = '\0';
  return word;

}

int loadline( struct loaded_syscall *ls,
    char *lineptr )  {

  char *linepos = lineptr;
  
  // This gets Pid and Tid.
  char *wordret = getword( &linepos );
  if( wordret == NULL )  return -1;
  // Now we need to split those two.
  
  char *pid_pos = wordret;
  movetochr( &wordret, '/' );
  //should not happen but better to detect
  if( *wordret == '\0' )  return -1;
  *wordret = '\0';
  char *tid_pos = wordret + 1;

  ls->pid = strdup( pid_pos );
  if( ls->pid == NULL )  return -1;
  ls->tid = strdup( tid_pos );
  if( ls->tid == NULL )  return -1;

  // We free since we already duplicated
  free( wordret ); // we don't need the word anymorel

  // Pass program name, we expect there is RET or CALL
  // and we pray it's not program name ;-)

  char *jumppos = strstr( linepos, "RET" );
  if( jumppos == NULL )  {

    jumppos = strstr( linepos, "CALL" );
    if( jumppos == NULL )  return -1;

  }

  // Now that we found command all we need to do is.
  // Go a little bit back so we can obtain timestamp
  // position.

  movebackspace( &linepos, lineptr );
  movebackword( &linepos, lineptr );

  wordret = getword( &linepos );
  if( wordret == NULL )  return -1;
  ls->time = wordret;

  wordret = getword( &linepos );
  if( wordret == NULL )  return -1;
  ls->syscall_type = wordret;

  wordret = getword( &linepos );
  if( wordret == NULL )  return -1;
  ls->syscall = wordret;

  return 0;

}

void fail( char *estr )  {

  perror( estr );
  exit( EXIT_FAILURE );

}

int main( void )  {

  char cmd_buff[ CMD_SIZE ];
  char tmpname_1[ PATH_MAX + 10 ];
  if( tmpnam( tmpname_1 ) )
    fail( "Could not get temporary file name 1" );
  char tmpname_2[ PATH_MAX + 10 ];
  if( tmpnam( tmpname_2 ) )
    fail( "Could not get temporary file name 2" ); 

  //  Get kdump readable output
  if( sprintf( cmd_buff, "kdump -TH > %s", tmpname_1 )
    == -1 )  fail( "Fail on sprintf cmd get kdump" );
  if( system( cmd_buff ) == -1 )
    fail( "Fail on system, cmd kdump" );
  puts( "Kdump saved as readable content" );
  
  // Remove everything that is not a return or call
  if( sprintf( cmd_buff, "grep -E 'RET|CALL' %s > %s",
    tmpname_1, tmpname_2 ) == -1 )
      fail( "Fail on sprintf cmd grep RET CALL" );
  if( system( cmd_buff ) == -1 )
    fail( "Failed on system grep RET CALL" );
  puts( "Leaving only CALL and RET lines" );

  // Remove some of CALL data from line end
  if( sprintf( cmd_buff, "cut -d '(' -f 1  %s > %s",
    tmpname_2, tmpname_1 ) == -1 )
      fail( "Fail on sprintf cmd cut sys calls" );
  if( system( cmd_buff ) == -1 )
    fail( "Failed on system cut sys calls" );
  puts( "Cutting data after syscall" );

  FILE *data_file = fopen( tmpname_1, "r" );
  if( data_file == NULL )
    fail( "Could not open our prepared kernel trace" );

  //create basic struct tree
  struct proc_data *pd = NULL;
  size_t pd_len = 0;

  for(;;)  {

    char *lineptr = NULL;
    size_t linesize = 0;

    errno = 0;
    if( getline( &lineptr, &linesize, data_file ) ==
      -1 )  {

      if( errno == 0 )  break;
      fail( "Fail on getting line from data file" );

    }

    struct loaded_syscall ls;
    if( loadline( &ls, lineptr ) == -1 )
      fail( "Could not load syscall line" );

    free( lineptr );

    // First thing, is it call or ret ?
    if( ! strcmp( ls.syscall_type, "CALL" ) )  {
    
     // add_call( &ls, &pd, &pd_size );

    }  else  {

      //finish_call( &ls, &pd, &pd_size );

    }

    printf( "%s\n%s\n%s\n%s\n%s",
            ls.pid, ls.tid, ls.time, ls.syscall_type,
	    ls.syscall );

    // after we are done - free the loaded syscall  
    free_ls( &ls );

  }

  // sum up stuff 
  // then free

}
