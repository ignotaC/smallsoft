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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// globs = 0
int OPTION_A;
int OPTION_B;
int OPTION_F;
char *EXPRESSION = NULL;

void fail( const char *const estr )  {

  perror( estr );
  exit( EXIT_FAILURE );

}

int check_args( const int argc, const char *const argv[] )  {

  int string_count = 0;
  int option_count = 0;
  for( int i = 1; i < argc; i++ )  {

    if( argv[i][0] != '-' )  {

      EXPRESSION = ( char *const )argv[i];
      string_count++;
      continue;
    
    }

    size_t arglen = strlen( argv[i] );
    for( size_t j = 1; j < arglen; j++ )  {

      switch( argv[i][j] )  {

        case 'w':
	 OPTION_F = 1;
	 option_count++;
	 break;
	case 'a':
	 OPTION_A = 1;
	 option_count++;
	 break;
	case 'b':
	 OPTION_B = 1;
	 option_count++;
	 break;
	default:
	 return -1;

      }

    }

  }
  
  if( string_count != 1 )  return -1;
  if( option_count == 0 )  return -1;

  // we can\t have both 0 or both 1
  // one must be true one must be false
  if( OPTION_A == OPTION_B )  return -1;

  return 0;

}

int main( const int argc, const char *const argv[] )  {

  if( argc < 3 )  {

    fprintf( stderr, "%s\n", "You must provide at least two arguments." );
    fprintf( stderr, "%s\n", "Option and string." );
    fprintf( stderr, "%s\n", "-a 'str' <- stdout file after string in line" );
    fprintf( stderr, "%s\n", "-b 'str' <- stdout file untill string in line" );
    fprintf( stderr, "%s\n", "-f <- print with found line containing string" );
    return -1;

  }

  if( check_args( argc, argv ) == -1 )  fail( "Broken arguments" );

  char *lineptr = NULL;
  size_t linesize = 0;
  for(;;)  {

    if( getline( &lineptr, &linesize, stdin ) == -1 )  {

      if( ferror( stdin ) )  fail( "Failed on stdin stream" );
      break;

    }

    if( strstr( lineptr, EXPRESSION ) == NULL )  {

      if( OPTION_B )  printf( "%s", lineptr );
      free( lineptr );
      lineptr = NULL;
      linesize = 0;
      continue;

    }

    if( OPTION_F ) printf( "%s", lineptr );
    free( lineptr );
    lineptr = NULL;
    linesize = 0;

    if( OPTION_B )  return 0;

    //Option A

    for(;;)  {

     if( getline( &lineptr, &linesize, stdin ) == -1 )  {

      if( ferror( stdin ) )  fail( "Failed on stdin stream" );
      break;

      }

      printf( "%s", lineptr );
      free( lineptr );
      lineptr = NULL;
      linesize = 0;

    }

  }

  return 0;

}
