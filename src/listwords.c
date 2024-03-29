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

#include "../ignotalib/src/ig_file/igf_open.h"
#include "../ignotalib/src/ig_file/igf_read.h"
#include "../ignotalib/src/ig_file/igf_readword.h"

#include <stdio.h>
#include <stdlib.h>

#define FILENAME_ARG 1

void fail( const char *const estr )  {

  perror( estr );
  exit( EXIT_FAILURE );

}

int main( int argc, char *argv[]  )  {

  if( argc > 2 )  fail( "Wrong number of arguments\n"
    "use like this:  listwords filename, or nothing and input goes to stdin\n" );

  const size_t buffsize = 8192;
  char buff[ buffsize ];

  // set fd  stdin or file
  int fd = 0;
  if( argc == 2 )  fd = igf_openrd( argv[ FILENAME_ARG ] );
  else fd = fileno( stdin );  // TODO - this doe snot work  segfaults etc

  if( fd == -1 )  fail( "Could not open file" );

  for( char *word = NULL;;)  {

    word = igf_readword( fd, buff, buffsize );
    if( word[0] == '\0' )  {

      free( word );
      return 0;

    }

    puts( word );
    free( word );

  }

}
