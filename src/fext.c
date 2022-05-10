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

#define BUFF_SIZE 8192
#define BUFF_SIZE_STR( BUFF_SIZE ) #BUFF_SIZE 

#include "../ignotalib/src/ig_file/igf_offset.h"
#include "../ignotalib/src/ig_file/igf_open.h"
#include "../ignotalib/src/ig_file/igf_read.h"
#include "../ignotalib/src/ig_file/igf_search.h"
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

void wrong( char *const wstr )  {

  errno = 0;
  fprintf( stderr, "%s", wstr );
  exit( EXIT_FAILURE );

}

void manual( void )  {

  puts( "How to use this program:" );
  puts( "fext phrase1 phrase2 filename" );
  puts( "Program will extract data blocks between strings" );
  printf( "%s", "Program uses buffor of "BUFF_SIZE_STR( BUFF SIZE )" size, so strings passed,\n" );
  puts( "should have lower size / length" );
  exit( EXIT_SUCCESS );

}

int main( int ac, char *av[] )  {

  // Program expects three arguments, everything other is wrong
  // phrases we will look for and output data found between
  // last is file we will search.
  if( ac != 4 )  manual();

  // set the phrases length
  char *phrase1 = av[1];
  size_t phrase1len = strlen( phrase1 );
  char *phrase2 = av[2];
  size_t phrase2len = strlen( phrase2 );

  // Check if length is not too big so it does not
  // outrun the buff size 
  if( phrase1len >= BUFF_SIZE )
    wrong( "phrase1 length is too big\n"
           "it must be smaller than program "
	   "buffor size"BUFF_SIZE_STR( BUFF_SIZE )"\n" );

  if( phrase2len >= BUFF_SIZE )
    wrong( "phrase2 length is too big\n"
           "it must be smaller than program"
	   " buffor size"BUFF_SIZE_STR( BUFF_SIZE )"\n" );

  // Now try opening the file we search
  int fd = igf_openrd( av[3] );
  if( fd == -1 )
    fail( "Could not open provided file for extracting" );

  // Set program buff
  char buff[ BUFF_SIZE ];
  const size_t buffsize = BUFF_SIZE;

  // get the sdout fd for writing out answer.
  int stdout_fd = fileno( stdout );

  // size igf_amidmem will tell us is between phrase1
  // and phrease2, sceond buff is for a read loop
  size_t sizetoread = 0, buffread = 0;
  for( int ans = 0;;)  {

    // look for phrases in our fd
    ans = igf_amidmem( fd, phrase1, phrase1len,
      phrase2, phrase2len, buff, buffsize, &sizetoread );

    if( ans == 0 )  break; // nothing found, leave
    if( ans == -1 )  fail( "Failure on igf_amidmem" );

    // output on stdout extracted data
    while( sizetoread != 0 )  {

      // set the buffread size depending on how much we must read
      if( sizetoread >= buffsize )  buffread = buffsize;
      else buffread = sizetoread;
      
      // read the fd, the starting offset is correct due to amidmem function
      // we set errno to zero so the error message suggest that 
      // altho all was ok - for example we reached eof too early which is 
      // a bug
      errno = 0;
      if( igf_read( fd, buff, buffread ) != ( ssize_t )buffread )
        fail( "Failed on igf_read" );
      // Write can only fail here or succeed.
      if( igf_write( stdout_fd, buff, buffread ) == -1 )
        fail( "Failed on igf_write" );

      // subtrack sizetoread 
      sizetoread -= buffread;

      // now move after phrase2 in the file
      if( igf_offset_mv( fd, ( off_t ) phrase2len ) == -1 )
        return -1;


    }

  }

  // close the file fd.
  close( fd );

  return 0;

}
