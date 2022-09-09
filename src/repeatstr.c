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

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//TODO \i no manual - sequance !!!

void fail( const char *const estr )  {

  perror( estr );
  exit( EXIT_FAILURE );

}

#define DIGIT_POS 1
#define STR_POS 2


int main( const int argc, const char *const argv[]  )  {

  if( argc != 3 )  // TODO  if no number passed - repeate string 1 time
    fail( "You need to pass number of times string\n"
      "will be repeated and the string it self.\n"
      "Example: repeatestr 100 'some text'\n" );

  char *endptr = NULL;
  char *continue_str = NULL;

  errno = 0;
  if( argv[ DIGIT_POS ][0] == '\0' )  fail( "Broken number" );
  if( ! isdigit( argv[ DIGIT_POS ][0] ) )  fail( "Broken number" );
  uintmax_t repeate_count = 
    strtoumax( argv[ DIGIT_POS ], &endptr, 10 );
  if( *endptr != '\0' )  fail( "Broken number string" );
  if( errno ) perror( "Conversion fail" );

  char *resolvestr = malloc( strlen( argv[ STR_POS ] ) + 1 );
  if( resolvestr == NULL )  
    fail( "Failed to malloc resolved sequances string." );
  // resolve char sequances
   
  // TODO  this can be moved to ignota + add \000 support backspace and rest ;-)
  for( size_t i = 0, j = 0;; i++, j++)   {

    if( argv[ STR_POS ][i] == '\\' )  {

      i++;
      switch( argv[ STR_POS ][i] )  {

        case 'r':
	  resolvestr[j] = '\r';
	  continue;
	case 'n':
	  resolvestr[j] = '\n';
	  continue;
	case '\\':
	  resolvestr[j] = '\\';
	  continue;
	case 'i':;
	  continue_str = &resolvestr[ j + 1 ];
	  resolvestr[j] = '\0'; // wee will print incrementor after this and than print rest
	  continue;
	case 't':
	  resolvestr[j] = '\t';
	  continue;
	case 'v':
	  resolvestr[j] = '\v';
	  continue;
	case 'f':
	  resolvestr[j] = '\f';
	  continue;
	case 'b':
	  resolvestr[j] = '\b';
	  continue;
	case 'a':
	  resolvestr[j] = '\a';
	  continue;
/*        case '0':
	  resolvestr[j] = '\0';
	  continue;    TODO*/  
	default:
	  resolvestr[j] = '\\';
	  j++;
	  break; // now go down and take the last character

      }

    }

    resolvestr[j] = argv[ STR_POS ][i];
    if( resolvestr[j] == '\0' )  break;

  }

  if( continue_str == NULL )
    while( repeate_count-- )  printf( "%s", resolvestr );
  else  {

    uint64_t increment = 0; 
    while( repeate_count-- )  {

      printf( "%s%" PRIu64 "%s", resolvestr, increment, continue_str );
      increment++;

    }

  }

  free( resolvestr );

  return 0;

}
