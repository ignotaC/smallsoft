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

#define LITTLEBUFF_ELEMENT_COUNT 4

// globs = 0
int OPTION_H;
int OPTION_B;

void fail( const char *const estr )  {

  perror( estr );
  exit( EXIT_FAILURE );

}

int printlittlebuff( int *littlebuff )  {

  for( int i = 0; i < LITTLEBUFF_ELEMENT_COUNT; i++ )  {

    if( littlebuff[i] == '\0' )  return 0;
    if( putchar( littlebuff[i] ) == EOF ) return -1;

  }

  return 0;

}

int check_args( const int argc, const char *const argv[] )  {

  for( int i = 1; i < argc; i++ )  {

    if( argv[i][0] != '-' )
      return -1;

    size_t arglen = strlen( argv[i] );
    for( size_t j = 1; j < arglen; j++ )  {

      switch( argv[i][j] )  {

        case 'h':
	 OPTION_H = 1;
	 break;
	case 'b':
	 OPTION_B = 1;
	 break;
	default:
	 return -1;

      }

    }

  }
  
  // we can\t have both 0 or both 1
  // one must be true one must be false
  if( OPTION_H == OPTION_B )  return -1;

  return 0;

}

int main( const int argc, const char *const argv[] )  {

  if( argc != 2 )  {

    fprintf( stderr, "%s\n", "You must provide one option. Example:" );
    fprintf( stderr, "%s\n", "-h  <- leave http header" );
    fprintf( stderr, "%s\n", "-e  <- leave http body" );
    fprintf( stderr, "%s\n", "httpans | httphb_split -h <- output http header" );

    return -1;

  }

  if( check_args( argc, argv ) == -1 )  fail( "Broken arguments" );

  if( OPTION_H )  {

    int charmem = 0;
    int littlebuff[ LITTLEBUFF_ELEMENT_COUNT ];

    for(;;)  {

      charmem = getchar();
      if( charmem == EOF )  {

        if( ferror( stdin ) )
          fail( "Failed reading file with getchar" );
	return 0;

      }

restart_h:
      if( charmem == '\r' ) {

	// this will set all on nul
	memset( littlebuff, 0, sizeof littlebuff );

        littlebuff[0] = charmem;
	charmem = getchar();
	if( charmem == EOF )  {

          if( ferror( stdin ) )
            fail( "Failed reading file with getchar" );
	  if( printlittlebuff( littlebuff ) == -1 )
	    fail( "Failed on writing littlebuff" );
	  return 0;

	}
	if( charmem != '\n' )  {

          if( printlittlebuff( littlebuff ) == -1 )
	    fail( "Failed on writing littlebuff" );
	  goto restart_h;

	}
	littlebuff[1] = charmem;

        charmem = getchar();
	if( charmem == EOF )  {

          if( ferror( stdin ) )
            fail( "Failed reading file with getchar" );
          if( printlittlebuff( littlebuff ) == -1 )
	    fail( "Failed on writing littlebuff" );
	  return 0;

	}
	if( charmem != '\r' )  {
	
          if( printlittlebuff( littlebuff ) == -1 )
	    fail( "Failed on writing littlebuff" );
	  goto restart_h;

	}
	littlebuff[2] = charmem;

	charmem = getchar();
	if( charmem == EOF )  {

          if( ferror( stdin ) )
            fail( "Failed reading file with getchar" );
	  if( printlittlebuff( littlebuff ) == -1 )
	    fail( "Failed on writing littlebuff" );
	  return 0;

	}
	if( charmem != '\n' )  {

          if( printlittlebuff( littlebuff ) == -1 )
	    fail( "Failed on writing littlebuff" );
	  goto restart_h;

	}

	// CRLFCRLF found we finish, head is our.
	return 0;

      }

      if( putchar( charmem ) == EOF )
        fail( "Failed on writing a char" );

    }

  }  else  {

    int charmem = 0;

    for(;;)  {

      charmem = getchar();
      if( charmem == EOF )  {

        if( ferror( stdin ) )
          fail( "Failed reading file with getchar" );
	return 0;

      }

restart_b:
      if( charmem == '\r' ) {

	charmem = getchar();
	if( charmem == EOF )  {

          if( ferror( stdin ) )
            fail( "Failed reading file with getchar" );
	  return 0;

	}
	if( charmem != '\n' )  goto restart_b;
	
        charmem = getchar();
	if( charmem == EOF )  {

          if( ferror( stdin ) )
            fail( "Failed reading file with getchar" );
	  return 0;

	}
	if( charmem != '\r' )  goto restart_b;

	charmem = getchar();
	if( charmem == EOF )  {

          if( ferror( stdin ) )
            fail( "Failed reading file with getchar" );
	  return 0;

	}
	if( charmem != '\n' )  goto restart_b;

	// CRLFCRLF found, we passed header, output everything what is left
	for(;;)  {

          charmem = getchar();
	  if( charmem == EOF )  {

            if( ferror( stdin ) )  fail( "Faled on getting char from stdin" );
	    return 0;

	  }
	  if( putchar( charmem ) == EOF )
	    fail( "Failed to put a character" );

	}

      }

    }


  }

  // because it should never be here
  return -1;

}
