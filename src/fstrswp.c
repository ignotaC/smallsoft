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

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#define BUFF_SIZE 8192

void pfail( const char *const pstr )  {

  fprintf( stderr, "%s", pstr );
  exit( EXIT_FAILURE );

}

void fail( const char *const estr )  {

  perror( estr );
  exit( EXIT_FAILURE );

}

int main( const int argc, const char *const argv[] )  {

  if( argc != 3 )  pfail( "Wrong number of arguments" );

  char buff[ BUFF_SIZE ];
  memset( buff, '\0', BUFF_SIZE );

  const char *const search_str = argv[1];
  const char *const replace_str = argv[2];
  char *buff_ptr = NULL;
  char *found_phrease = NULL;

  const size_t search_str_len = strlen( search_str );

  while( fgets( buff, BUFF_SIZE, stdin ) != NULL )  {

    if( buff[ BUFF_SIZE - 1 ] != '\0' )  pfail( "Too long line" );
    buff_ptr = buff;
    while( 1 )  {

      if( ( found_phrease = strstr( buff_ptr, search_str ) ) == NULL )  {

        printf( "%s", buff_ptr );
	break;

      }

     *found_phrease = 0;
     printf( "%s%s", buff_ptr, replace_str );
     buff_ptr = found_phrease + search_str_len;

    }

  }

  if( ferror( stdin ) )  fail( "Fail on read" );
  return 0;

}
