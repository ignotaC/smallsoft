/*

Copyright (c) 2020 Piotr Trzpil  p.trzpil@protonmail.com

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

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

void pfail( const char *const pstr )  {

  fprintf( stderr, "%s", pstr );
  exit( EXIT_FAILURE );

}

void fail( const char *const estr )  {

  perror( estr );
  exit( EXIT_FAILURE );

}

int main( 
    const int argc,
    const char *const argv[] 
)  {

  if( argc != 3 )  pfail( "Wrong number of arguments" );

  const char *const searchstr = argv[1];
  char replacestr[ 1 + strlen( argv[2] ) ];
  strcpy( replacestr, argv[2] );
  
  char *replacestr_pos = replacestr;
  char *replacestr_newpos = replacestr;
  for(;;)  {

    if( *replacestr_pos == '\\' )  {

      replacestr_pos++;
      switch( *replacestr_pos )  {

       case 'r':
         *replacestr_newpos = '\r';
	 replacestr_newpos++;
	 replacestr_pos++;
	 continue;
       case 'n':
         *replacestr_newpos = '\n';
	 replacestr_newpos++;
	 replacestr_pos++;
	 continue;
       case 't':
         *replacestr_newpos = '\t';
	 replacestr_newpos++;
	 replacestr_pos++;
	 continue;
       case 'v':
         *replacestr_newpos = '\v';
	 replacestr_newpos++;
	 replacestr_pos++;
	 continue;
       case '\\':
         *replacestr_newpos = '\\';
	 replacestr_newpos++;
	 replacestr_pos++;
	 continue;
       default:
	 fail( "Broken character sequance" );
	 break; // will never reach this

      }

    }

    *replacestr_newpos = *replacestr_pos;
    if( *replacestr_newpos == '\0' )  break;

    replacestr_pos++;
    replacestr_newpos++;

  }

  char *lineptr = NULL;
  char *found_phrease = NULL;

  const size_t searchstr_len = strlen( searchstr );

  char *line = NULL;
  size_t linesize = 0;

  for( ssize_t getlineret = 0;;) {

    getlineret = getline( &line, &linesize, stdin );
    if( getlineret == -1 )  {

      if( ferror( stdin ) )
        fail( "Failed on getline" );
      return 0;

    }

    lineptr = line;
    for(;;)  {

      found_phrease = strstr( lineptr, searchstr );
      if( found_phrease == NULL )  {

        printf( "%s", lineptr );
	break;

      }

     *found_phrease = '\0';
     printf( "%s%s", lineptr, replacestr );
     lineptr = found_phrease + searchstr_len;

    }
    
    free( line );
    line = NULL;
    linesize = 0;

  }

}
