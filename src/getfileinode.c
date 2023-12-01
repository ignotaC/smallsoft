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

#include "../ignotalib/src/ig_file/igf_dir.h"
// #include "../ignotalib/src/ig_miscellaneous/igmisc_getans.h"

#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void fail( const char *const errstr )  {

  perror( errstr );
  exit( EXIT_FAILURE );

}

int printent( struct dirent *de )  {

  if( de == NULL )  return -1;

  if( printf( "Filename: %s\nInode: %" PRIuMAX "\n\n",
       de->d_name, de->d_ino ) < 0 )  return -1;

  return 0;

}

int main( int argc, char *argv[] )  {

  if( igf_usedirent( ".", printent ) == -1 )
    fail( "Crashed on igf_usedirent" );

  // TODO duno what but i smell it

  ( void )argc;
  ( void )argv;

  return 0;

}

/*
  size_t bf_size = 8192;  
  char *bf = malloc( bf_size );
  if( bf == NULL )  fail( "Could not allocate memory" );

  for( size_t i = 0; i < filenames.listlen; i++ )  {

    bool has_argstrs = true;
    for( size_t j = 1; j < ( size_t )argc; j++ )  {

      if( strstr( filenames.list[i], argv[j] ) == NULL )
        has_argstrs = false;

      if( ! has_argstrs )  break;

    }

    if( ! has_argstrs )  continue;

    if( printf( "\nDo you want to rename file: %s\n",
        filenames.list[i] ) < 0 )
      fail( "Printf failure" );

    if( printf( "Y/y is yes anything else is no\n" ) < 0 )
      fail( "Printf failure" );

    int ans = igmisc_getans_yn( ""  );
    if( ans == -1 )  fail( "get ans function fail" );
    if( ans == 0 )  continue;

    // pass new line
    for( int ch = 0;;)  {

      ch = getchar();
      if( ch == '\n' ) break;
      if( ch == EOF )
        fail( " Fail on getchar" );  // EOF should not happen

    }

    ///////////////////
    // at this point we want to change the file name
    if( printf( "Enetr new file name: " ) < 0 )
      fail( "Fail on printf" );

    if( getline( &bf, &bf_size, stdin ) == -1 )
      fail( "Error or EOF on getline" ); // EOF should not happen here

    char *find_LF = strchr( bf , '\n' );
    if( find_LF == NULL )
      fail( "New file name is too long" );
    *find_LF = '\0';
    // new line should appear else error ^
    
    FILE *oldfile = fopen( filenames.list[i], "r" );
    if( oldfile == NULL )  fail( "could not open oldfile" );
    FILE *newfile = fopen( bf, "w" );
    if( oldfile == NULL )  fail( "could not open newfile" );

    for( size_t ret = 0;;)  {

      ret = fread( bf, 1, bf_size, oldfile );
      if( ferror( oldfile ) )
        fail( "Error on reading old file" );
      if( ( ret == 0 ) && ( feof( oldfile ) ) )
        break;

      char *bf_pos = bf;
      for(;;)  {

         size_t wret = fwrite( bf_pos, 1, ret, newfile );
         if( ferror( newfile ) )
	   fail( "Failed on fwrite" );
	 if( wret == ret )  break;
	 ret -= wret;
	 bf_pos += wret;

      }

    }

    fclose( oldfile );
    fclose( newfile );

  }

  free( bf );

  return 0;

  */
