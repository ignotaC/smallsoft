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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>

#include <unistd.h>

#define BUFF_SIZE 1024
#define NOWENNA_PATH ".novena"
char novena_name[ BUFF_SIZE ];
time_t novena_start_time;
long long int novena_start_time_int;


void end_program( void )  {

  exit( EXIT_SUCCESS );

}

void fail( const char *const estr )  {

  perror( estr );
  exit( EXIT_FAILURE );

}

int open_file_novena( void )  {

  FILE *f_now = fopen( NOWENNA_PATH, "r" );
  if( f_now == NULL )  {

    if( errno != ENOENT )  fail( "file open fail" );


    puts( "\nNo novena file found!" );
    puts( "Do you want to create new novena?" );
    puts( "Y for yes other respond will mean no" );
    int ans;
    ans = getchar();
    if( ans != 'Y' )  end_program();
    puts( "Choose, novena name" );
    int its_ok = 0;
    while(  fgets( novena_name, BUFF_SIZE, stdin ) != NULL )  {
      
      for( size_t i = 0; i < BUFF_SIZE; i++ )  {

        if( isalnum( novena_name[i] ) )  {

	  its_ok = 1;
	  break;

	}
	if( isalnum( novena_name[i] == '\0' ) )  break;

      }

      if( its_ok )  break;

    }

    puts( "Maybe novena started some time ago already?" );
    puts( "Insert how many days ago if today just put in zero" );

    int day_lag = 0;
    scanf( "%d", &day_lag );
    
    novena_start_time = time( NULL ) - day_lag * 60 * 60 * 24;
    novena_start_time_int = ( int long long ) novena_start_time;
    f_now = fopen( NOWENNA_PATH, "w" );
    if( f_now == NULL )  fail( "fail on write new novena" );
    if( fprintf( f_now, "%lld\n%s",
		 novena_start_time_int, novena_name ) < 0 )
      fail( "fprintf fail - writing to new novena file" );

    puts( "New novena was saved" );
    end_program();

  }

  if( fscanf( f_now, "%lld", &novena_start_time_int ) != 1 )  {

    fclose( f_now );
    remove( NOWENNA_PATH );
    return -1;

  }

  novena_start_time = ( time_t ) novena_start_time_int;

  int data_char;  
  memset( novena_name, 0, BUFF_SIZE );
  for( size_t i = 0;; i++ )  {

    data_char = fgetc( f_now );	
    if( data_char == EOF )  break;
    novena_name[i] = data_char;

  }


  fclose( f_now );
  
  if( novena_name[0] == '\0' )  {

    remove( NOWENNA_PATH );
    return -1;

  }
  
  return 0;

}



int main( void )  {
// use glob

  puts( "Nowenna for today, to restart novena remove .novena file" );
 
  char *ptr = getenv( "HOME" );
  if( ptr == NULL )  fail( "getenv fail" );
  if( chdir( ptr ) )  fail( "chdir fail" ); 

  if( open_file_novena() != 0 )  fail( "broken file novena" );

  puts( "\nCurrent novena:" );
  puts( novena_name );
  puts( "" );


  time_t current_time = time( NULL );
  printf( "Nowenna was set on :%s\n", ctime( &novena_start_time ) );
 
  struct tm *novena_tm = localtime( &novena_start_time );
  struct tm buff_tm = *novena_tm;
  novena_tm = &buff_tm;
  struct tm *current_tm = localtime( &current_time );
  
  // novena takes 9 days.. So it can't take more than few years.
  int day_diff = 0;
  
  int leap_year = 0;
  int novena_year = novena_tm->tm_year;
  if( ! ( novena_year % 400 ) )  leap_year = 1;
  else  {

    if( ( ! ( novena_year % 4 ) ) && ( novena_year % 100 ) )  leap_year = 1;

  }

  int year_diff = current_tm->tm_year - novena_tm->tm_year;
  if( year_diff )   {

    if( year_diff != 1 )  {

      puts( "Nowenna has ended long time ago" );
      remove( NOWENNA_PATH );
      return 0;

    }

    // 364 for normal year and than we add onem ore day if leap year.
    day_diff = current_tm->tm_yday + 364 - novena_tm->tm_yday;
    if( leap_year )  day_diff++;

  }  else  {

    day_diff = current_tm->tm_yday - novena_tm->tm_yday;

  }

  day_diff++;

  printf( "Current novena day: %d\n", day_diff );

  if( day_diff > 9 )  {

    puts( "Nowenna, has ended. removing old novena" );
    remove( NOWENNA_PATH );

  }

  return 0;

}
