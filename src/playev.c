/*

Copyright (c) 2019 Piotr Trzpil  p.trzpil@protonmail.com

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

#include <sys/time.h>

#include <unistd.h>

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BUFF_SIZE 8192
char buff[ BUFF_SIZE ];
char sec_buff[ BUFF_SIZE ];

void fail( char *str_error )  {

  perror( str_error );
  exit( EXIT_FAILURE );

}

void wrong_arg( void )  {

  puts( "Wrong number of arguments" );

  puts( "You must provide four arguments for this program to run" );
  puts( "Like this:  playev file 10 15 3" );
  puts( "First argument is the file name of file generated by recev" );
  puts( "Second argument, in example -> 10 is number of times to play recorded events." );
  puts( "0 means repeat forever" );
  puts( "Thrid argument, in example -> 15 is number of seconds to wait," );
  puts( "before starting to run events" );
  puts( "Last argument, in example -> 3 is number of seconds to wait between replaying event" );

  exit( EXIT_SUCCESS );

}

void isplus( int num )  {

  if( num < 0 )  fail( "Can't have negative numbers" );

}

void mv_toalp( char **strpos )  {

  while( ! isspace( **strpos ) )  {

    if( *strpos == '\0' )  fail( "mv_toalp should not meet nul" );
    ( *strpos )++;

  }
  while( isspace( **strpos ) )  {

    if( *strpos == '\0' )  fail( "mv_toalp should not meet nul" );
    ( *strpos )++;

  }

}

int main( int argc, char *argv[]  )  {

  if( argc != 5 )  wrong_arg();

  FILE *recev_file = fopen( argv[1], "r" );
  if( recev_file == NULL )  fail( "Failed on opening recorded events file" );
  
  int replay_times = 0;
  int wait_before_start = 0;
  int wait_between = 0;

  errno = 0;
  if( sscanf( argv[2], "%d", &replay_times ) != 1 )
    fail( "Failed to get number of times to replay" );
  errno = 0;
  if( sscanf( argv[3], "%d", &wait_before_start ) != 1 )
    fail( "Failed to get time to wait before start" );
  errno = 0;
  if( sscanf( argv[4], "%d", &wait_between ) != 1 )
    fail( "Failed to get time to wait between replaying events" );

  isplus( replay_times );
  isplus( wait_before_start );
  isplus( wait_between );

  struct timespec ts;
  long long int time_wait;
  char *buff_ptr;

  sleep( wait_before_start );
  for(;;)  {

    if( fgets( buff, BUFF_SIZE, recev_file ) == NULL )  {

      if( ferror( recev_file ) )  fail( "Failed on reading recev_file" );
      if( replay_times != -1 )  {

	replay_times--;
        if( replay_times == 0 )  break;

      }

      errno = 0;
      rewind( recev_file );
      if( errno )  fail( "Failed on rewinding recev_file" );
      sleep( wait_between );
      continue;

    }

    if( sscanf( buff, "%lld", &time_wait ) != 1 )
      fail( "Failed on sscanf getting time_wait" );
    
    buff_ptr = buff;
    mv_toalp( &buff_ptr );

    ts.tv_sec = time_wait / 1000;
    ts.tv_nsec = ( time_wait % 1000 ) * 1000000;

    if( nanosleep( &ts, NULL ) == -1 )
      fail( "Failed on waittime function" );

    if( system( buff_ptr ) == -1 )
      fail( "Failed on system function" );

  }

  fclose( recev_file );

}




