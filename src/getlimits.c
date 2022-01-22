/*

Copyright (c) 2021 Piotr Trzpil  p.trzpil@protonmail.com

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

#include "../ignotalib/src/ig_print/igp_double.h"

#include <sys/resource.h>

#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void fail( const char *const estr )  {

  perror( estr );
  exit( EXIT_FAILURE );

}

void printstr( const char *const datastr ) {

  errno = 0;
  printf( "%s", datastr );
  if( errno )  fail( "Data string printing error" );

}


enum {

  NONE,
  BYTES,
  SECONDS

};


int type = 0;
void printmax( uintmax_t number )  {

  double answer = number;

  char *typename = "";
  if( type == BYTES )  {

    typename = "B";
    
    if( number / 1024 != 0 )  {

      typename = "kB";
      number /= 1024;
      answer /= 1024;

    }    
     
    if( number / 1024 != 0 )  {

      typename = "MB";
      number /= 1024;
      answer /= 1024;

    }    

   
    if( number / 1024 != 0 )  {

      typename = "GB";
      number /= 1024;
      answer /= 1024;

    }    


  }  else if ( type == SECONDS )  {

    typename = "seconds";
      
    if( number / 60 != 0 )  {

      typename = "minutes";
      number /= 60;
      answer = number;

    }    

    if( number / 60 != 0 )  {

      typename = "hours";
      number /= 60;
      answer = number;

    }    
   
    if( number / 24 != 0 )  {

      typename = "days";
      number /= 24;
      answer = number;

    }    


  }

  if( igp_double_rmvtrailzeros( answer, 4 ) ==
      -1 )
    fail( "double without trailing zeros print fail" );

  printstr( " " );
  printstr( typename );
  printstr( "\n" );

}

void print_rls( const struct rlimit *const rls )  {

  printstr( "Soft limit: " );
  printmax( ( uintmax_t )( rls->rlim_cur  ) );
  printstr( "Hard limit: " );
  printmax( ( uintmax_t )( rls->rlim_max  ) );
  printstr( "\n" );

}

void getlimit_data( const int resource,
    struct rlimit *const rls )  {

  if( getrlimit( resource, rls ) == -1 )
    fail( "getrlimit error" );	  

}

int main( void )  {

  struct rlimit rls = {0};

  // POSIX

  type = BYTES;
  getlimit_data( RLIMIT_CORE, &rls );
  printstr( "Max size of core file\n" );
  print_rls( &rls );

  type = SECONDS;
  getlimit_data( RLIMIT_CPU, &rls );
  printstr( "Max ammount of CPU time used"
    " by each process\n" );
  print_rls( &rls );

  type = BYTES;
  getlimit_data( RLIMIT_DATA, &rls );
  printstr( "Max size of data segment for a process,"
    "including memory allocated data via malloc, mmap\n" );
  print_rls( &rls );

  type = BYTES;
  getlimit_data( RLIMIT_FSIZE, &rls );
  printstr( "Max size of a file, that can be created\n" );
  print_rls( &rls );

  type = NONE;
  getlimit_data( RLIMIT_NOFILE, &rls );
  printstr( "Max number of file descriptos"
    " process can open\n" );
  print_rls( &rls );

  type = BYTES;
  getlimit_data( RLIMIT_STACK, &rls );
  printstr( "Max size process stack can take,"
    " defines how far the stack can be extended\n" );
  print_rls( &rls );

  // openbsd bug
  #ifndef __OpenBSD__

  type = BYTES;
  getlimit_data( RLIMIT_AS, &rls );
  printstr( "Mximum size in bytes of totall avilable"
    " memory for a process, if this limit is passed,"
    " malloc, mmap shall fail. Also stack can't grow\n" );
  print_rls( &rls );

  #endif

  // END OF POSIX

  // NON PORTABLE

  #ifdef __OpenBSD__

  type = BYTES;
  getlimit_data( RLIMIT_MEMLOCK, &rls );
  printstr( "Max size process can lock in memory"
    " using mlock\n" );
  print_rls( &rls );

  type = NONE;
  getlimit_data( RLIMIT_NPROC, &rls );
  printstr( "Max number of processes for this"
  " user id that could run simultaneously\n" );
  print_rls( &rls );

  #endif

  #ifdef __linux__

  type = BYTES;
  getlimit_data( RLIMIT_MEMLOCK, &rls );
  printstr( "Max size in bytesprocess can lock in memory"
    " using mlock\n" );
  print_rls( &rls );

  type = BYTES;
  getlimit_data( RLIMIT_MSGQUEUE, &rls );
  printstr( "Max size in bytes that can be allocated"
    " by POSIX message queue\n" );
  print_rls( &rls );

  type = NONE;
  getlimit_data( RLIMIT_NICE, &rls );
  printstr( "Max value of process priority"
    "we can set it for example with nice\n" );
  print_rls( &rls );

  type = NONE;
  getlimit_data( RLIMIT_NPROC, &rls );
  printstr( "Max number of processes for this"
    " user id that could run simultaneously\n" );
  print_rls( &rls );

  type = NONE;
  getlimit_data( RLIMIT_RTPRIO, &rls );
  printstr( "Maximum value for real time priority" );
    " it can be set with for example shed_setparam\n" );
  print_rls( &rls );

  type = NONE;
  getlimit_data( RLIMIT_RTTIME, &rls );
  printstr( "Time limit in microseconds on the"
    " amount on CPU a process scheduled"
    "under real-time sheduling policy\n" );
  print_rls( &rls );

  type = NONE;
  getlimit_data( RLIMIT_SIGPENDING, &rls );
  printstr( "Limit for signals queued for process"
    "this is for both, standard and real-time\n" );
  print_rls( &rls );

  #endif

  return 0;

}



