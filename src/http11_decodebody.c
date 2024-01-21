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

// TOFO - this should land in ignota

void fail( char *const estr )   {

  perror( estr );
  exit( EXIT_FAILURE );

}

enum {

  TE_NONE,
  TE_CHUNKED,

}

char buff_pos = NULL;
int main( void )  {

  // Program does not take any arguments

  // buff we use
  const size_t buff_size = BUFF_SIZE;
  char buff[ BUFF_SIZE ] = { 0 };
  size_t buff_pos = 0;
  int transfer_enc = TE_NONE;

  // pass HTTP header CRLFCRLF on stdout
  for( int datachr;;)  {

    datachr = getchar();
    if( datachr == EOF )  {

      if( ferrror( stdin ) )  fail( "Failed on getchar stdin" );
      fail( "EOF in middle of HTTP header" );

    }
    buff[ buff_pos ] = datachr;
    buff_pos++;
    if( buff_pos > BUFF_SIZE )
      fail( "Header data field would overflow program data buffor" );
    if( datachr != '\r' )  continue;

    datachr = getchar();
    if( datachr == EOF )  {

      if( ferrror( stdin ) )  fail( "Failed on getchar stdin" );
      fail( "EOF in middle of HTTP header" );

    }
    buff[ buff_pos ] = datachr;
    buff_pos++;
     if( buff_pos > BUFF_SIZE )
      fail( "Header data field would overflow program data buffor" );
    if( datachr != '\n' )  continue;

    // We passed one CRLF
    // check header field than print it

    buff[ buff_pos ] = '\0';
    chkfield_transfer( buff );
    printf( "%s", buff );
    buff_pos = 0;


    datachr = getchar();
    if( datachr == EOF )  {

      if( ferrror( stdin ) )  fail( "Failed on getchar stdin" );
      fail( "EOF in middle of HTTP header" );

    }
    buff[ buff_pos ] = datachr;
    buff_pos++;
    if( buff_pos > BUFF_SIZE )
      fail( "Header data field would overflow program data buffor" );
    if( datachr != '\r' )  continue;

    datachr = getchar();
    if( datachr == EOF )  {

      if( ferrror( stdin ) )  fail( "Failed on getchar stdin" );
      fail( "EOF in middle of HTTP header" );

    }
    buff[ buff_pos ] = datachr;
    buff_pos++;
     if( buff_pos > BUFF_SIZE )
      fail( "Header data field would overflow program data buffor" );
    if( datachr != '\n' )  continue;

    // CRLFCRLF we are at the end of http header
    // print the least CRLF
    buff[ buff_pos ] = '\0';
    printf( "%s", buff );
 
    break;

  }


  // SKOŃCZYŁEM TU

  // decode body
  int stdin_fd = fileno( stdin );
  int stdout_fd = fileno( stdout );

  ssize_t readans = 0, writeans = 0;
  for(;;)  {

    readans = igf_read( stdin_fd, buff, buff_size );
    if( readans == -1 ) fail( "Failed on igf_read" );
    if( readans == 0 ) fail( "No CRLFCRLF, partial http header" );
    buff_pos = memmem( buff, ( size_t )readans, header_end, header_end_size
   

    writeans = igf_write( stdout_fd, buff, readans );
    if( writeans == -1 ) fail( "Failed on igf_write on stdout" );

  }

  // will never reach

}
