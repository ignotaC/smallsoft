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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void fail( const char *const errstr )  {

  perror( errstr );
  exit( EXIT_FAILURE );

}

int noturlcode( char chrin )  {

  if( isalnum( chrin ) )
    return 0;
  if( ( chrin == '.' ) || ( chrin == ',' ) || 
      ( chrin == '-' ) || ( chrin == '_' ) )
    return 0;
  return -1;

}

int main( int argc, char *argv[] )  {

  if( argc < 2 )  fail( "No options specified" );

  // read the options
  if( argv[1][0] != '-' )
    fail( "No options indicator -> '-'" );

  // for now we can have only one option chosen
  if( strlen( argv[1] ) != 2 )
    fail( "Too many options" );

  switch( argv[1][1] )  {

    case 'c': // code url
    
    {

     ;
     char chrin;
     for(;;)  {

        chrin = getc( stdin );
	if( chrin == EOF )  {

	  if( ferror( stdin ) )
	    fail( "Error while reading stdin" );
	  return 0;

	}

        if( ! noturlcode( chrin ) )  {

	  if( putc( chrin, stdout ) == EOF )
	    fail( "Failed on putc" );

	}  else  {

	  if( printf( "%%%02x",
	    ( unsigned char )chrin ) < 0 )
	      fail( "failed on printing encoded char" );

	}

     }

    }
    case 'd': // decode url
    
    {
     
     ;
     char chrin;
     char chrin_sec;
     char hexans[3] = {0};
     unsigned int chr_code;
     for(;;)  {

       chrin = getc( stdin );
       if( chrin == EOF )  {

	 if( ferror( stdin ) )
	   fail( "Error while reading stdin" );
	 return 0;

       }

       if( chrin == '%' )  {
  
         chrin = getc( stdin );
         if( chrin == EOF )
           fail( "broken URL encoding" );
	 if( ! isxdigit( chrin ) )
	   fail( "broken URL encoding" );
         
	 chrin_sec = getc( stdin );
         if( chrin_sec == EOF )
           fail( "broken URL encoding" );
	 if( ! isxdigit( chrin_sec ) )
	   fail( "broken URL encoding" );

	 hexans[0] = chrin;
	 hexans[1] = chrin_sec;

	 if( sscanf( hexans, "%x", &chr_code ) != 1 )
           fail( "Could not scan hex code" ); 
 	 if( putc( ( int ) chr_code , stdout ) == EOF )
	   fail( "Failed on putc encoded chr" );

       }  else  {
	
         if( putc( chrin, stdout ) == EOF )
	   fail( "Failed on putc" );

       }

     }

    }

    case 'p':  // code post arguments
 
    {

      char chrin;
      for( int arg_pos = 2; arg_pos < argc;
          arg_pos++ )  {

	char *argstr = argv[ arg_pos ];
	size_t arglen = strlen( argstr );

        for( size_t i = 0; i < arglen; i+=1 )  {

	  chrin = argstr[i];
	  if( ! noturlcode( chrin ) )  {

	    if( putc( chrin, stdout ) == EOF )
	      fail( "Failed on putc" );

	  }  else  {

	   
	    if( printf( "%%%02x",
	        ( ( unsigned char ) chrin ) ) < 0 )
	      fail( "failed on printing encoded char" );

	  }

	}

	if( arg_pos + 1 == argc )  return 0;
	arg_pos++;
	printf( "%s", argv[ arg_pos ] );

      }

      return 0;

    }

    default :
     fail( "Wrong option parameter" );

  }

}


