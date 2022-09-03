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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

void fail()  {

  perror( "Httpreq_addcrlf Failure: " );
  exit( EXIT_FAILURE );

}

// TODO this should READ instead of silly putchc.
// Since it's very slow because of it.

int main( void )  {

  int letter;
  bool newline = 0;
  for(;;)  {
 
    letter = getc( stdin );
    if( letter == '\n' )  {

      newline = 1;
      continue;

    }
    if( letter == EOF )  {

      if( ferror( stdin ) )  fail();
      if( printf( "\r\n\r\n" ) < 0 )  fail();
      return 0;

    }

    if( newline )  {

      newline = 0;
      if( printf( "\r\n" ) < 0 )  fail();

    }

    if( putc( letter, stdout ) == EOF )  fail();

  }

}
