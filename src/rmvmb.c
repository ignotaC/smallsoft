/*

Copyright (c) 2022 Piotr Trzpil  p.trzpil@protonmail.com

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
#include <locale.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define TURNSTR( x ) #x

void fail( const char *const estr )  {

  perror( estr );
  exit( EXIT_FAILURE );

}

//TODO it could use buff instead to not allocate all shit

int main( int argc, char *argv[] )  {

  char *locale_type = getenv( TURNSTR( LC_CTYPE ) );
  if( locale_type == NULL )  fail( "Could not get LC_TYPE locale" );
  if( setlocale( LC_CTYPE, locale_type ) == NULL )
    fail( "Setlocale failed" );

  int fopt = 0;

  if( argc > 2 )  fail( "Wrong number of arguments" );
  
  if( argc == 2 )  {

    if( argv[1][0] != '-' )  fail( "Broken option" );
    if( argv[1][1] != 'f' )  fail( "Broken option" );
    fopt = 1; // we have only one option

  }

  char *line = NULL;
  size_t linesize = 0;

  for(;;)  {

    if( getline( &line, &linesize, stdin ) == -1 )  {

      if( ferror( stdin ) ) fail( "Failed on getline" );
      break;

    }

    char *linepos = line;
    for( size_t i = linesize; i > 0; )  {

      int mblenret = mblen( linepos, i );
      if( mblenret == -1 )  {
	      
        if( ! fopt )  fail( "Failed on mblen" );
	mblenret = 1; // this way we ignore the error
		      // when fopt is set

      }
      if( mblenret == 0 )  break;

      if( mblenret == 1 )  putc( ( int ) *linepos, stdout );
      i -= ( size_t )mblenret;
      linepos += ( ptrdiff_t )mblenret;

    }

    free( line );
    line = NULL;
    linesize = 0;

  }

  return 0;

}
