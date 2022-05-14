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

#ifdef DPROG
  #define dstr(x) fprintf( stderr, "%s\n", x)
#else
  #define dstr(x)
#endif

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void fail( char *str_error )   {

  perror( str_error );
  exit( EXIT_FAILURE );

}

void manual( void )  {

  puts( "This program needs commands to work" );
  puts( "-p 'x x x' pass lines until line like this" );
  puts( "-c 'x x x' 'y y y' cut string between strings and print" );
  puts( "-l loop last command" );
  exit( EXIT_SUCCESS );

}

int main( int ac, char *av[] )  {

  if( ac < 2 )  manual();

  char *line = NULL;
  size_t line_size = 0;
  int av_pos = 1; // we start from this.
  int av_earlier_pos = 0;
  char *bptr = NULL;
  char *helppos = NULL;

  dstr( "Init complete move to main loop" );

  while( getline( &line, &line_size, stdin ) != -1 )  {

    bptr = line;

    for(;;)  {
      
      dstr( "Inside command loop" );

      if( av_pos >= ac )  break;
 
      if( av[ av_pos ][0] != '-' )  {

        fprintf( stderr, "%s: ", av[ av_pos ] );
        fail( "This is not a command" );  

      }

      switch( av[ av_pos ][1] )  {

        case 'p':
         dstr( "Pass command" );
	 av_earlier_pos = av_pos;
	 if( av_pos + 1 >= ac )  {

	   fprintf( stderr, "%s: ", av[ av_pos ] );
	   fail( "Not enought arguments for this command" );

	 }
	 bptr = strstr( bptr, av[ av_pos + 1 ] );
	 if( bptr == NULL )  break;
	 bptr += strlen( av[ av_pos + 1 ] );
	 av_pos += 2; // move to next command
	 break;
        case 'c':
	 dstr( "Cut command" );
	 av_earlier_pos = av_pos;
	 if( av_pos + 2 >= ac )  {

           fprintf( stderr, "%s: ", av[ av_pos ] );
           fail( "Not enought arguments for this command" );

	 }
	 if( av[ av_pos + 1 ][0] != '\0' )  {
		 
	   bptr = strstr( bptr, av[ av_pos + 1 ] );
	   if( bptr == NULL )  break;
	   bptr += strlen( av[ av_pos + 1 ] );

	 }
	 
	 if( av[ av_pos + 2 ][0] == '\0' )  {

	   printf( "%s", bptr );
	   bptr = NULL;
	   break;

	 }
         helppos = strstr( bptr, av[ av_pos + 2 ] );
	 if( helppos == NULL )  {

           bptr = NULL;
	   break;

	 }
	 char keep = *helppos;
	 *helppos = '\0';
	 puts( bptr );
	 *helppos = keep;
	 bptr = helppos;
	 bptr += strlen( av[ av_pos + 2 ] );
	 av_pos += 3; // move to a new command.
	 break;

        case 'l':
	 dstr( "Loop command" );
         if( av_earlier_pos == 0 )  fail( "Can't loop on first argument" );
         av_pos = av_earlier_pos;
         break;  
        default:
	  fprintf( stderr, "%s: ", av[ av_pos ] );
          fail( "Wrong command argument" );

      }

      if( bptr == NULL )  break;
 
    }

    free( line );
    line = NULL;
    line_size = 0;
    if( av_pos >= ac )  break;

  }

  dstr( "Left main loop" );
  if( errno )  fail( "Failed on getline reading from stdin" );
  return 0;

}
