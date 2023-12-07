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

#define BUFF_SIZE 8192

#include "../ignotalib/src/ig_file/igf_read.h"
#include "../ignotalib/src/ig_file/igf_write.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void fail( char *const estr )   {

  perror( estr );
  exit( EXIT_FAILURE );

}


int main( void )  {

  // Program does not take any arguments

  // buff we use
  const size_t buff_size = BUFF_SIZE;
  char buff[ BUFF_SIZE ];

  // get the descriptors
  int stdin_fd = fileno( stdin );
  int stdout_fd = fileno( stdout );
  int stderr_fd = fileno( stderr );

  for( ssize_t readans = 0, writeans = 0;;)  {

    readans = igf_read( stdin_fd, buff, buff_size );
    if( readans == -1 ) fail( "Failed on igf_read" );
    if( readans == 0 )  return 0;

    writeans = igf_write( stdout_fd, buff, readans );
    if( writeans == -1 ) fail( "Failed on igf_write on stdout" );
    writeans = igf_write( stderr_fd, buff, readans );
    if( writeans == -1 ) fail( "Failed on igf_write on stderr" );

  }

  // will never reach

}
