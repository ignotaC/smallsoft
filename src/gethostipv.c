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

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

#include <sys/socket.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int inet_hint;
int check_count; // zero if nothing prefered
char *domain_name;

void fail( const char *const estr )  {

  perror( estr );
  exit( EXIT_FAILURE );

}

int chkarg( const int argc, const char *const argv[] )  {

  if( argc > 3 )  return -1;
  if( argc < 2 )  return -1;
  if( argc == 2 )  {

    // we are passing simply domain,
    // and will accept what ever answer is returned
    // so the first one to be specific.
    domain_name = ( char* )argv[1];
    inet_hint = AF_UNSPEC;
    return 0; 

  } 

  int optpos = 1;
  if( argv[1][0] != '-' )  {

    optpos = 2;
    domain_name = ( char* )argv[1];

  }  else  {

    domain_name = ( char* )argv[2];

  }
  if( argv[ optpos ][0] != '-' )  return -1;

  size_t argsize = strlen( argv[ optpos ] );
  if( argsize == 1 )  return -1;
  for( size_t i = 1; i < argsize; i++ )  {

    switch( argv[ optpos ][i] )  {

      case '4':
	if( check_count )   {
         
	  if( inet_hint == AF_INET )  return -1;
	  check_count++;
	  continue;

	}
	check_count++;
        inet_hint = AF_INET;
	continue;
      case '6':
	if( check_count )   {

	  if( inet_hint == AF_INET6 )  return -1;
	  check_count++;
	  continue;

	}
	check_count++;
        inet_hint = AF_INET6;
        continue;
      case '\0':
	break;
      default:
	return -1;

    }

  }

  // option like -466 is a bug the only possibilities are
  // -4 -6 or -46 -64
  if( check_count > 2 ) return -1;

  return 0;

}

int main( const int argc, const char *const argv[] )  {

  if( chkarg( argc, argv ) == -1 )
    fail( "Broken arguments" );

  // now set rest of 

  struct addrinfo hint = {0};
  hint.ai_flags = AI_ADDRCONFIG | AI_CANONNAME;
  hint.ai_family = inet_hint;
  hint.ai_socktype = SOCK_STREAM; 
  hint.ai_protocol = IPPROTO_TCP;

  struct addrinfo  *aidata = NULL, *aipos = NULL;
  int error = 0;
new_hint_restart:

  error = getaddrinfo( domain_name, NULL, &hint, &aidata );
  if( error != 0 )  {

    if( error == EAI_NODATA )  {

      if( hint.ai_family == AF_UNSPEC )  return 0;
      if( check_count == 1 )  return 0;

      check_count = 0;
      if( hint.ai_family == AF_INET )
        hint.ai_family = AF_INET6;
      else hint.ai_family = AF_INET;
      goto new_hint_restart;

    }
    fail( gai_strerror( error ) );
    return -1;

  }

  const socklen_t ipname_size = 128;
  char ipname[ ipname_size ];
  aipos = aidata;
  for(;;)  {
    
    void *inet_struct = NULL;
    if( aipos->ai_family == AF_INET )  {

      inet_struct = &( ( ( struct sockaddr_in* )aipos->ai_addr )->sin_addr );

    } else {

      inet_struct = &( ( ( struct sockaddr_in6* )aipos->ai_addr )->sin6_addr );

    }
    
    const char *ans = inet_ntop( aipos->ai_family, inet_struct,
      ipname, ipname_size );
    if( ans == NULL ) fail( "failed on inet_ntop" );

    printf( "%s\n", ans );
    if( aipos->ai_next == NULL )  break;
    aipos = aipos->ai_next;

  }

  freeaddrinfo( aidata );

  return 0;

}
