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
#include <stdlib.h>

void fail( const char *const estr )  {

  perror( estr );
  exit( EXIT_FAILURE );

}

#define DIGIT_POS 1
#define STR_POS 2

int main( const int argc, const char *const argv[]  )  {

  if( argc != 3 )
    fail( "You need to pass number of times string"
      "will be repeated and the string it self."
      "Example: repeatestr 100 'some text'" );

  char *endptr = NULL;

  errno = 0;
  if( argv[ DIGIT_POS ][0] == '\0' )  fail( "Broken number" );
  if( ! isdigit( argv[ DIGIT_POS ][0] ) )  fail( "Broken number" );
  uintmax_t repeate_count = 
    strtoumax( argv[ DIGIT_POS ], &endptr, 10 );
  if( *endptr != '\0' )  fail( "Broken number string" );
  if( errno ) perror( "Conversion fail" );

  while( repeate_count-- )  printf( "%s", argv[ STR_POS ] );

  return 0;

}
