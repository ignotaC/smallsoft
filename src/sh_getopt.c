/*

Copyright (c) 2023 Piotr Trzpil  p.trzpil@protonmail.com

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

#include "../ignotalib/src/ig_miscellaneous/igmisc_opts.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void fail( const char *const estr )  {

  perror( estr );
  exit( EXIT_FAILURE );

}

// Very important:
// first we pass some arguments
// finally we have avilable options string
// if not existing options are met - program will exit with error
// example of passed data: args 2 

int main( const int argc, const char *const argv[] )  {

  if( argc < 2 )
    fail( "Need at least one argument with expected options" );

  // this is fine user never passed any arguments so we don't need to check them
  // the avilible options don't matter
  if( argc == 2 )  return 0;

  // don't count options on the last argument
  #define NOTOPTARG_COUNT 1

  // opt string is nothing more but avilable options
  // so our last argument
  const char *optstr = argv[ argc - 1 ]; // fine since we exit if argc < 2
  igmisc_short_opts sopts;
  igmisc_sopts_init( &sopts, ( unsigned char* )optstr );
  if( igmisc_sopts_load( &sopts, igmisc_sopts_readorder,
      argc - NOTOPTARG_COUNT, &( argv[0] ) ) == -1 )
    fail( "Passed option arguments are broken" );

  // now all you need to do is print options that appeared
  if( igmosic_sopts_print( &sopts, argv[ argc - 1 ] ) < 0 )
    fail( "Sopts print failed" );

  return 0;

}
