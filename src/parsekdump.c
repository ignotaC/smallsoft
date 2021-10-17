/*

Copyright (c) 2020 Piotr Trzpil  p.trzpil@protonmail.com

Permission to use, copy, modify, and distribute 
this software for any purpose with or without fee
is hereby granted, provided that the above copyright
notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR
DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE
FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
OF THIS SOFTWARE.

*/

// TODO add a SUMMARY fo all the processes

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
#define KDUMP_FNAME "ktrace.out"

// debug function
void stop( char * problem  )  {

  puts( problem );
  exit( EXIT_SUCCESS );

}

struct timepoint  {

  uint64_t sec;
  uint64_t usec; // microseconds

};

double timeperc(
    struct timepoint *part,
    struct timepoint *sum )  {


  double anser = part->sec + part->usec / 1000000.0;
  anser /= sum->sec + sum->usec / 1000000.0;
  return anser * 100.0;

}

void printtime( struct timepoint *tp )  {

  printf( "%" PRIu64 ".%06" PRIu64, tp->sec, tp->usec );

}

int time_add( struct timepoint *tp,
      struct timepoint *tp_add )  {

  uint64_t maxint = ( uint64_t ) -1;

  if( maxint - tp->sec < tp_add->sec )
    return -1;
  tp->sec += tp_add->sec;

  if( maxint - tp->usec < tp_add->usec )
    return -1;
  tp->usec += tp_add->usec;

  // and now add seconds that we got from usec.
  uint64_t sec = tp->usec / 1000000;
  tp->usec %= 1000000;

  if( maxint - tp->sec < sec )  return -1;
    tp->sec += sec;
  return 0;

}


int time_diff( struct timepoint *tp_old,
       struct timepoint *tp_new )  {

  uint64_t sec = 0;
  if( tp_new->usec < tp_old->usec )  {

    sec = 1;
    tp_new->usec += 1000000; // add one second
    if( tp_new->usec < tp_old->usec )  return -1;
    tp_new->usec -= tp_old->usec;

  }  else  {

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

  char *split_time = strchr( time, '.' );
  if( split_time == NULL ) return -1;
  if( sscanf( time, "%" SCNu64 ".%" SCNu64, 
    &( tp->sec ), &( tp->usec ) ) != 2 )  return -1;
  return 0;

}

struct syscall_data  {

  char *syscall;
  uint64_t count;
  struct timepoint tp_sum;

};

void sd_init( struct syscall_data *sd, char *syscall ) {

  sd->syscall = syscall;
  sd->count = 0;
  sd->tp_sum.sec = 0;
  sd->tp_sum.usec = 0;

}

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

void ls_init( struct loaded_syscall *ls )  {

  ls->pid = NULL;
  ls->tid = NULL;
  ls->time = NULL;
  ls->syscall_type = NULL;
  ls->syscall = NULL;

}

struct proc_data  {

  char *pid;
  char *tid;
  struct syscall_data **sd;
  struct pending_syscall **ps;
  size_t sd_len;
  size_t ps_len;
  struct timepoint tp_appear;
  struct timepoint tp_last;
  struct timepoint tp_callsum;

};

void pd_init( struct proc_data *pd )  {

  pd->pid = NULL;
  pd->tid = NULL;
  pd->sd = NULL;
  pd->sd_len = 0;
  pd->ps = NULL;
  pd->ps_len = 0;
  memset( &( pd->tp_appear ), 0, sizeof pd->tp_appear );
  memset( &( pd->tp_last ), 0, sizeof pd->tp_last );
  memset( &( pd->tp_callsum ), 0, sizeof pd->tp_callsum );

}

struct proc_data *get_pd( struct proc_data **pd,
    size_t *pd_len,
    struct loaded_syscall *ls )  {

  struct timepoint tp_action = { 0 };
  if( gettime( ls->time, &tp_action ) == -1 )
    return NULL;

  for( size_t i = 0; i < *pd_len; i++ )  {

    if( ! strcmp( ( *pd )[i].pid, ls->pid ) )  {

      if( ! strcmp( ( *pd )[i].tid, ls->tid ) )  {

	( *pd )[i].tp_last = tp_action;
        return &( ( *pd )[i] );

      }

    }

  }

  ( *pd_len )++;
  *pd = realloc( *pd, *pd_len * sizeof **pd );
  if( *pd == NULL )  return NULL;
  struct proc_data *new_pd =
    &( ( *pd )[ *pd_len -1 ] );
  pd_init( new_pd );
  // we set to null because we free it later
  new_pd->pid = ls->pid;
  ls->pid = NULL;
  new_pd->tid = ls->tid;
  ls->tid = NULL;
  new_pd->tp_appear = tp_action;
  new_pd->tp_last = tp_action;
  return new_pd;

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
  char *keepwordret_ptr = wordret;
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
  // we don't need the word anymorel
  free( keepwordret_ptr );

  // Pass program name, we expect there is RET or CALL
  // and we pray it's not program name ;-)

  char *jumppos = strstr( linepos, "RET" );
  if( jumppos == NULL )  {

    jumppos = strstr( linepos, "CALL" );
    if( jumppos == NULL )  return -1;

  }
  linepos = jumppos;
  // Now that we found command all we need to do is.
  // Go a little bit back so we can obtain timestamp
  // position.

  movebackspace( &linepos, lineptr );
  movebackword( &linepos, lineptr );
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

int get_pendingsys( struct pending_syscall *ps,
		    struct loaded_syscall *ls )  {

  ps->syscall = ls->syscall;
  ls->syscall = NULL;
  return gettime( ls->time, &( ps->tp ) );

}

int add_call( struct proc_data *pd,
    struct loaded_syscall *ls )  {

  for( size_t i = 0; i < pd->ps_len; i++ )  {

    if( ( pd->ps )[i] == NULL )  {

      pd->ps[i] = malloc( sizeof **( pd->ps ) );
      if( pd->ps[i] == NULL )  return -1;
      
      return get_pendingsys( pd->ps[i], ls );

    }

  }

  pd->ps_len++;
  pd->ps = realloc( pd->ps, pd->ps_len *
    sizeof *( pd->ps ) );
  if( pd->ps == NULL )  return -1;
  struct pending_syscall *new_ps =
    pd->ps[ pd->ps_len - 1 ];
  new_ps = malloc( sizeof **( pd->ps ) );
  if( new_ps == NULL )  return -1;
  pd->ps[ pd->ps_len - 1 ] = new_ps;
  return get_pendingsys( new_ps, ls );

}

int ret_call( struct proc_data *pd,
    struct loaded_syscall *ls )  {

  // find pending syscall, if no ignore this return
  size_t ps_pos = ( size_t ) -1;
  for( size_t i = 0; i < pd->ps_len; i++ )  {
   
    if( ! strcmp( pd->ps[i]->syscall, ls->syscall ) ) {

       ps_pos = i;
       break;

    }

  }

  if( ps_pos == ( size_t ) -1 )  return 0;

  size_t sd_pos = ( size_t ) -1;
  for( size_t i = 0; i < pd->sd_len; i++ )  {
 
    if( ! strcmp( pd->sd[i]->syscall, ls->syscall ) ) {

      sd_pos = i;
      break;

    }

  }

  if( sd_pos == ( size_t ) -1 )  {


    ( pd->sd_len )++;
    sd_pos = pd->sd_len - 1; 
    pd->sd = realloc( pd->sd,
      pd->sd_len * sizeof *( pd->sd ) );
    if( pd->sd == NULL )  return -1;
    pd->sd[ sd_pos ] = malloc( sizeof **( pd->sd ) );
    if( pd->sd[ sd_pos ] == NULL )  return -1;
    sd_init( pd->sd[ sd_pos ], ls->syscall );
    ls->syscall = NULL; // sd will now keep pointer

  }

  ( pd->sd[ sd_pos ]->count )++;
  struct timepoint ret_tp;
  if( gettime( ls->time, &ret_tp ) == -1 )
    return -1;
  if( time_diff( &( pd->ps[ ps_pos ]->tp ),
   &ret_tp ) == -1 ) return -1;
  if( time_add( &( pd->sd[ sd_pos ]->tp_sum ),
    &ret_tp ) == -1 )  return -1;

  // now since all data has been moved we will free
  // pending syscall.
  free( pd->ps[ ps_pos ]->syscall );
  free( pd->ps[ ps_pos ] );
  // now set it to null so we can reuse the memory.
  // Check call function for more data
  pd->ps[ ps_pos ] = NULL;
  return 0;

}

int main( void )  {

  char cmd_buff[ CMD_SIZE ];
  char tmpname_1[] = "/tmp/tmpkdump1XXXXXXXX";
  int tmpfd = mkstemp( tmpname_1 );
  if( tmpfd == -1 )
    fail( "Could not get temporary file name 1" );
  close( tmpfd );

  char tmpname_2[] = "/tmp/tmpkdump2XXXXXXXX";
  tmpfd = mkstemp( tmpname_2 );
  if( tmpfd == -1 )
    fail( "Could not get temporary file name 2" ); 
  close( tmpfd );

  //  Get kdump readable output
  if( sprintf( cmd_buff, "kdump -TH > %s", tmpname_1 )
    == -1 )  fail( "Fail on sprintf cmd get kdump" );
  if( system( cmd_buff ) == -1 )
    fail( "Fail on system, cmd kdump" );
  
  // Remove everything that is not a return or call
  if( sprintf( cmd_buff, "grep -E 'RET|CALL' %s > %s",
    tmpname_1, tmpname_2 ) == -1 )
      fail( "Fail on sprintf cmd grep RET CALL" );
  if( system( cmd_buff ) == -1 )
    fail( "Failed on system grep RET CALL" );
  remove( tmpname_1 );

  // Remove some of CALL data from line end
  if( sprintf( cmd_buff, "cut -d '(' -f 1  %s > %s",
    tmpname_2, tmpname_1 ) == -1 )
      fail( "Fail on sprintf cmd cut sys calls" );
  if( system( cmd_buff ) == -1 )
    fail( "Failed on system cut sys calls" );
  remove( tmpname_2 );

  FILE *data_file = fopen( tmpname_1, "r" );
  if( data_file == NULL )
    fail( "Could not open our prepared kernel trace" );

  //create basic struct tree
  struct proc_data *pd = NULL;
  size_t pd_len = 0;

  char *start_time = NULL;
  char *end_time = NULL;

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
    ls_init( &ls );
    errno = 0;
    if( loadline( &ls, lineptr ) == -1 )  {

      if( errno == 0 )  {

        free( lineptr );
	free_ls( &ls );
	continue;

      }
      fail( "Could not load syscall line" );

    }

    free( lineptr );

    // update time to know how long we were capturing syscalls
    if( start_time == NULL )  {

      start_time = strdup( ls.time );
      if( start_time == NULL )
        fail( "Out of memory for start_time" );

    }

    free( end_time );
    if( ( end_time = strdup( ls.time ) ) == NULL )
      fail( "Out of memory for end_time" );

    struct proc_data *pd_point = get_pd( &pd,
      &pd_len, &ls );
    if( pd_point == NULL )
      fail( "Failed on finding pd pointer" );
    // First thing, is it call or ret ?
    if( ! strcmp( ls.syscall_type, "CALL" ) )  {
    
      if( add_call( pd_point, &ls ) == -1 )
        fail( "fail on add_call" );
     
    }  else  {

      if( ret_call( pd_point, &ls ) == -1 )
        fail( "Fail on ret_call" );

    }

    free_ls( &ls );

  }

  remove( tmpname_1 );

  
  struct timepoint  start_tp, end_tp, sum_tp;
  if( gettime( start_time, &start_tp ) == -1 )
    fail( "Failed on converting start time to tp" );
  if( gettime( end_time, &end_tp ) == -1 )
    fail( "Failed on converting end time to tp" );
  sum_tp = end_tp;

  if( time_diff( &start_tp, &sum_tp ) == -1 )
    fail( "Fail on time start end diff" );
  
  printf( "There were %zu threads and forks\n", pd_len );
  printf( "Recording time: " );
  printtime( &sum_tp );
  puts( " sec" );

  struct timepoint calls_time = { 0 };

  //Fill time_sum struct and compute calls time
  for( size_t i = 0; i < pd_len; i++ )  {

    for( size_t sc_pos = 0;
        sc_pos < pd[i].sd_len; sc_pos++ )  {

      if( time_add( &( pd[i].tp_callsum ),
          &( pd[i].sd[sc_pos]->tp_sum ) ) == -1 )
        fail( "Failed adding sum time" );

    } 

    if( time_add( &calls_time,
        &( pd[i].tp_callsum ) ) == -1 )
      fail( "Failed on adding stuff to callsum" ); 

  }
   
  double calls_sum = 
    timeperc( &calls_time, &sum_tp );
  printf( "System calls took  %.3f%% of "
          "recording time\n\n", calls_sum );
 
  // Sort syscall data, max count on top.
  for( size_t i = 0; i < pd_len; i++ )  {

    for( size_t sc_pos = 0;
        sc_pos < pd[i].sd_len; sc_pos++ )  {

      struct syscall_data **pos_1, **pos_2, *swap;
      pos_1 = &( pd[i].sd[ sc_pos ] );

      for( size_t j = sc_pos + 1; j < pd[i].sd_len; j++ )  {

        pos_2 = &( pd[i].sd[j] );

	if( ( *pos_1 )->count < ( *pos_2 )->count )  {

	  swap = *pos_1;
	  *pos_1 = *pos_2;
	  *pos_2 = swap;

	}

      }


    }

  }



  for( size_t i = 0; i < pd_len; i++ )  {

    printf( "Program PID/TID %s/%s\n",
      pd[i].pid, pd[i].tid );

    for( size_t sc_pos = 0;
        sc_pos < pd[i].sd_len; sc_pos++ )  {

      double proc_time = timeperc( 
        &( pd[i].sd[ sc_pos ]->tp_sum ),
        &calls_time );

      double only_proc_time = timeperc(
        &( pd[i].sd[ sc_pos ]->tp_sum ),
	&( pd[i].tp_callsum ) );

      printf( "%15s %-8" PRIu64 " %8.3f%8.3f%%\n",
        pd[i].sd[ sc_pos ]->syscall,
        pd[i].sd[ sc_pos ]->count,
        proc_time, only_proc_time );

    }

    // at this point last value will be erased
    // but we won't use it anymore
    // I hope so.
    for( size_t j = 0; j < pd[i].ps_len; j++ )  {

      if( pd[i].ps[j] != NULL )  {

	// Never returned
        pd[i].tp_last = end_tp;

      }

    }

    if( time_diff( &( pd[i].tp_appear ),
         &( pd[i].tp_last ) ) == -1 )
      return -1;

    double proc_call_perc = 
      timeperc( &( pd[i].tp_callsum ), &calls_time );
    printf( "Process took %.3f%% of sumary call time\n",
      proc_call_perc );

    double proc_call_to_run = 
      timeperc( &( pd[i].tp_callsum ), &sum_tp );
    printf( "Process calls took %.3f%% of record time\n",
      proc_call_to_run );

    double proc_runtime_perc = 
      timeperc( &( pd[i].tp_last ), &sum_tp );
    printf( "Process was running %.3f%% of"
      " record time\n", proc_runtime_perc );

    puts( "" );

  }

  // then free

}  
