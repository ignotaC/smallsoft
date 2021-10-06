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
int OPTION_D;
int OPTION_E;

void fail( const char *const estr )  {

  perror( estr );
  exit( EXIT_FAILURE );

}

char *decode_entity_tab[][2] = {

  { "&amp;", "&" },
  { "&nbsp;", " " },
  { "&lt;", "<" },
  { "&gt;", ">" },
  { "&apos;", "'" },
  { "&qout;", "\"" }

};

int decode_entity( char *entity )  {

  size_t ent_len = sizeof decode_entity_tab / sizeof *decode_entity_tab;
  for( size_t i = 0; i < ent_len; i++ )  {

    if( strcmp( entity, decode_entity_tab[i][0] ) == 0 )  {

      printf( "%s", decode_entity_tab[i][1] );
      free( entity );
      return 0;

    }

  }

  free( entity );
  return -1;

}

int check_args( const int argc, const char *const argv[] )  {

  for( int i = 1; i < argc; i++ )  {

    if( argv[i][0] != '-' )
      return -1;

    size_t arglen = strlen( argv[i] );
    for( size_t j = 1; j < arglen; j++ )  {

      switch( argv[i][j] )  {

        case 'e':
	 OPTION_E = 1;
	 break;
	case 'd':
	 OPTION_D = 1;
	 break;
	default:
	 return -1;

      }

    }

  }
  
  // we can\t have both 0 or both 1
  // one must be true one must be false
  if( OPTION_E == OPTION_D )  return -1;

  return 0;

}

int main( const int argc, const char *const argv[] )  {

  if( argc != 2 )  {

    fprintf( stderr, "%s\n", "You must provide one option. Example:" );
    fprintf( stderr, "%s\n", "-d  <- encode url entities" );
    fprintf( stderr, "%s\n", "-e  <- decode url entities" );
    fprintf( stderr, "%s\n", "data | urlentities -d <- this writes to stdout" );

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

    if( OPTION_E )  {

      for( size_t i = 0; i < linesize; i++ )  {

        switch( lineptr[i] )  {

          case ' ':
            printf( "&nbsp;" );
	    break;

          case '\'':
            printf( "&apos;" );
	    break;

          case '"':
            printf( "&qout;" );
	    break;

          case '<':
            printf( "&lt;" );
	    break;

          case '>':
            printf( "&gt;" );
	    break;

          case '&':
            printf( "&amp;" );
	    break;

	  default:
	   putchar( lineptr[i] );
	   break;

	}

      }

    }  else  {

      char *keeppos = NULL;
      for( size_t i = 0; i < linesize; i++ )  {

        switch( lineptr[i] ) {

	  case '&':
	    keeppos = &lineptr[i];
	    for( size_t j = i + 1; j < linesize; j++ )  {  

	       // kind of broken but lets say we meet & again.
	       if( lineptr[j] == '&' )  {

	         for(; i != j; i++ )  putchar( lineptr[i] );
		 continue; 

	       }

	      if( lineptr[j] == '\0' )  {

	        printf( "%s", &lineptr[i] );
		i=j;
		continue;

	      }

	      if( lineptr[j] == ';' )  {

		// +2 because nul and we take j also
		// aligment will take much more anyway.
		size_t entity_size = j - i + 2;
		char *entity = malloc( entity_size );
		memset( entity, 0, entity_size ); // this sets nul
		strncpy( entity, &lineptr[i], entity_size - 1 );
		if( entity == NULL )  fail( "Could not allocate entity" );
		// function down here will free entity
		if( decode_entity( entity ) == -1 ) 
                  fail( "broken entity value" );
	        i=j;
	        break;

	      }

	    }
	    continue;
	    
	  default:
	    putchar( lineptr[i] );
	    break;


	}

      }

    }

    free( lineptr );
    lineptr = NULL;
    linesize = 0;

  }

  return 0;

}
