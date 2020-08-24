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


#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 *
 * We can have three commands that decide what to do.
 * No commands means we just output anything
 * uppercase -u A-Z
 * lowercase -l a-z
 * digits -d 0-9
 *
 */

int uppercase;
int lowercase;
int digits;

void fail( char *failstr )  {

  perror( failstr );
  exit( EXIT_FAILURE );

}


char *check_args( int argc, char *argv[] )  {

  int number_count = 0;
  char *numpos = NULL;

  size_t pos;
  while( argc-- )  {
    if( argc <= 0 )  break;

    pos = 0;
    char start = argv[ argc ][0];
    if( start == '-' )  {

      pos++;
      while( argv[ argc ][ pos ] != '\0' )  {

        switch( argv[ argc ][ pos ] )  {

	  case 'u':
	    uppercase = 1;
	    break;
	  case 'l':
	    lowercase = 1;
	    break;
	  case 'd':
	    digits = 1;
	    break;
	  default:
	    return NULL;

	}

	pos++;

      }

    }  else if( isdigit( start ) )  {

      numpos = argv[ argc ];
      number_count++;
      if( number_count != 1 )  return NULL;

      pos++;
      while( argv[ argc ][ pos ] != '\0' )  {

        if( ! isdigit( argv[ argc ][ pos ] ) )
	  return NULL;
	pos++;

      }

    }  else  return NULL;

  }

  return numpos;

}

int main( int argc, char *argv[] )  {

  if( argc < 1 ) 
    fail( "No arguments" );

  char *numberstr = check_args( argc, argv );
  if( numberstr == NULL )
    fail( "Passed arguments are broken" ); 

  errno = 0;
  long long int randcount = strtoll( numberstr,
    NULL, 10 );
  if( errno )  fail( "Number is not valid" );
  srand( time( NULL ) );

  signed char ans;
  if( uppercase | lowercase | digits )  {

    while( randcount-- )  {
	    
      ans = ( signed char )rand();
      if( digits && isdigit( ans ) )  {
 
       putchar( ans );
       continue;

      }  else if( lowercase && islower( ans ) )  {

        putchar( ans );
	continue;

      }  else if( uppercase && isupper( ans ) )  {

        putchar( ans );
	continue;

      }


      randcount++;

    }

  }  else  {
	  
    while( randcount-- )  {

      ans = ( signed char )rand();
      putchar( ans );

    }

  }

  return 0;
  
}
