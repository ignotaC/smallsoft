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


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void fail( const char *const str_err )  {

  perror( str_err );
  exit( EXIT_FAILURE );

}

/*
 *  Write down all lines of first passed file after differance in lines.
 */

int main( const int argc, char *const argv[] )  {

  if( argc != 3 )  fail( "Fail - must be two file names" );

  FILE *f_one = fopen( argv[1], "r" );
  if( f_one == NULL )  fail( "Failed on opening f_one" );
  FILE *f_two = fopen( argv[2], "r" );
  if( f_two == NULL )  fail( "Failed on opening f_two" );

  size_t linesize = 0;
  char *line1 = NULL;
  char *line2 = NULL;
  for(;;)  {

     if( getline( &line1, &linesize, f_one ) == -1 )  {

       if( ferror( f_one ) )
         fail( "Failed on getline from f_one" );
       break;
     
     }

     if( getline( &line2, &linesize, f_two ) == -1 )  {

       if( ferror( f_two ) )
         fail( "Failed on getline from f_two" );
       printf( "%s", line1 );
       free( line1 );
       break;

     }

     if( strcmp( line1, line2 ) )  {

       printf( "%s", line1 );
       free( line1 );
       free( line2 );
       break;

     }

     free( line1 );
     free( line2 );

     line1 = NULL;
     line2 = NULL;

  }

  line1 = NULL;
  line2 = NULL;

  while( getline( &line1, &linesize, f_one ) != -1 )  {

    printf( "%s", line1 );
    free( line1 );
    line1 = NULL;

  }

  if( ferror( f_one ) )  fail( "Failed on f_one getline" );

  return 0;

}
