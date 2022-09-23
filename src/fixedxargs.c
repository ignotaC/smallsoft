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

#include <sys/wait.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void leave( void )  {

  fprintf( stderr, "This xarg does need only one argument.\n" );
  fprintf( stderr, "Which is program name.\n" );
  fprintf( stderr, "We feed the program with options and arguments,\n" );
  fprintf( stderr, "using stdin\n" );
  exit( EXIT_FAILURE );

}

void fail( const char *const estr )  {

  perror( estr );
  exit( EXIT_FAILURE );

}

int main( const int argc, const char *const argv[] )  {

  if( argc != 2 )  leave();
  
  char *linestr = NULL;
  size_t linesize = 0;
  ssize_t bytes = 0;

  size_t argsize = strlen(  argv[1] );
  size_t keep_argsize = argsize + 1 ;
  char *arg = realloc( NULL, keep_argsize + 1 ); // +1 for nul
		// this protects us if none input in stdin
  if( arg == NULL )  fail( "Failed on first allocation." );
  strncpy( arg, argv[1], argsize );
  arg[ argsize ] = ' ';
  arg[ argsize + 1 ] = '\0'; // nul protector
  argsize = keep_argsize;


  for(;;)  {

    bytes = getline( &linestr, &linesize, stdin );
    if( bytes == -1 )  {
 
      if( ferror( stdin ) )  fail( "Failed on getline." );
      break;

    }

    linesize = ( ssize_t )bytes;  // protection against no LF
    for( ssize_t i = 0; i < bytes; i++ )  {

      if( linestr[i] == '\n' )  {

        linesize = ( size_t )i;
	break;

      }

    }

    //  bytes: "'linesize' "
    argsize += 1 + linesize + 1 + 1;
    arg = realloc( arg, argsize );
    if( arg == NULL )  fail( "Realloc fail." );
    arg[ keep_argsize ] = '\'';
    memcpy( arg + keep_argsize + 1, linestr, linesize );
    arg[ keep_argsize + linesize + 1 ] = '\'';
    arg[ keep_argsize + linesize + 2 ] = ' ';
    keep_argsize = argsize;

    free( linestr );
    linestr = NULL;
    linesize = 0;

  }

  arg = realloc( arg, keep_argsize + 1 );
  if( arg == NULL )  fail( "Failed on reallocing nul." );
  arg[ keep_argsize ] = 0;

  int retval = system( arg );
  free( arg );

  if( WIFEXITED( retval ) )  {

    retval = WEXITSTATUS( retval );

  }

  // TODO this need proper handling - errors signals returns

  return retval;

}
