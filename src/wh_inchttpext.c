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


#include <unistd.h>

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFF_SIZE 8192
char buff[ BUFF_SIZE ];

void manual( void )  {

  puts( "First argument is command it's crucial to make it correct" );
  puts( "Always do it using variable like in example" );
  puts( "Example: 'wget -O '\\''%s'\\'' '\\''http://some.site/x/y/%lld/data'\\'''" );
  puts( "In this case first argument will be file name" );
  puts( "and second incremented number" );
  puts( "Qoutes are important do it just as in example" );
  puts( "Second argument is starting number for incrementation" );
  puts( "Thrid argument is last incrementation" );
  puts( "Forth one are strings, file is cut to pass.");
  puts( "It can be like \"form img\"  so first we will pass" );
  puts( "untill \"form\" is found and  than same goes for \"img\"" );
  puts( "Fifth we have start string after which we save the data" );
  puts( "Sixth is end string and it will pass" );
  puts( "Output goes in stdout, save it with > for example" );

  exit( EXIT_SUCCESS );

}

void fail( char *str_fail )  {

  perror( str_fail );
  exit( EXIT_FAILURE );

}


int pass_strings( FILE *tmpfile, char *strtopass,
		  char *buff, size_t buff_size, char **ansptr )  {

  *ansptr = NULL;
  char *strpart = strtok( strtopass, " " );
  for(;;)  {

    if( strpart == NULL )  {

      *ansptr = buff;
      return 0;

    }

    while( fgets( buff, buff_size, tmpfile ) != NULL )  {
 
      if( strstr( buff, strpart ) != NULL )  break;

    }
    
    if( ferror( tmpfile ) )  return -1;
    if( feof( tmpfile ) )  return 0;
    strpart = strtok( NULL, " " );

  }

}



int findline( FILE *tmpfile, char *str_start, char *str_end,
              char *buff, size_t buff_size, char **ansptr )  {

  char *start_point = NULL;
  char *end_point = NULL;
  ptrdiff_t start_len = strlen( str_start );
  *ansptr = NULL;
  while( fgets( buff, buff_size, tmpfile ) != NULL )  {

    start_point = strstr( buff, str_start );
    if( start_point == NULL )  continue;
    start_point += start_len; // pass it
    end_point = strstr( start_point, str_end );
    if( end_point == NULL )  continue;
    *end_point = 0;
    *ansptr = start_point;
    return 0;

  }

  if( ferror( tmpfile ) )  return -1;
  return 0;

}

int main( int argc, char *argv[]  )  {

  if( argc != 7 )  manual();
  
  char *httplink = argv[1];
  long long int start_num = 0;
  if( sscanf( argv[2], "%lld", &start_num ) != 1 )
    fail( "Failed on getting start_num" );
  long long int end_num = 0;
  if( sscanf( argv[3], "%lld", &end_num ) != 1 )
    fail( "Failed on getting end_num" );
  char *strtopass = argv[4];
  char *str_start = argv[5];
  char *str_end = argv[6];

  char *ansptr = NULL;

  int sysret = 0;
  while( start_num <= end_num )  {

    char tmpfile_name[] = "/tmp/tmpincXXXXXXXX";
    int tmpfd = mkstemp( tmpfile_name );
    if( tmpfd == -1 )  fail( "Failed on mkstemp" );
    close( tmpfd );

    if( sprintf( buff, httplink, tmpfile_name, start_num ) < 0 )
      fail( "Failed on sprintf command" );

    for(;;)  {

      if( ( sysret = system( buff ) ) == -1 )
        fail( "Failed on system function" );

      // CHECK SYSTEM RETURN and proceed.
      //
      break;

    }
     
    FILE *tmpfile = fopen( tmpfile_name, "r" );
    if( tmpfile == NULL )  fail( "could not open tmpfilename" );
 
    if( pass_strings( tmpfile, strtopass, buff, BUFF_SIZE, &ansptr ) == -1 )
      fail( "Failed on passing by strings" );

    if( ansptr == NULL )  goto finish;

    if( findline( tmpfile, str_start, str_end, buff, BUFF_SIZE, &ansptr ) == -1 )
      fail( "Failed on finding value we look for" );
	
    if( ansptr == NULL )  goto finish;
    
    fprintf( stderr, "%lld ", start_num );
    errno = 0;
    if( fprintf( stdout, "%s\n", ansptr ) < 0 )
      if( errno )  fail( "Failed on fputs" );
    fflush( stdout );

   finish:
    start_num++;
    fclose( tmpfile );
    remove( tmpfile_name );

  }

  return 0;

}
