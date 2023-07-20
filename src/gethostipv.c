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

#include "../ignotalib/src/ig_miscellaneous/igmisc_opts.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

#include <sys/socket.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int inet_hint;

void fail( const char *const estr )  {

  perror( estr );
  exit( EXIT_FAILURE );

}

int main( const int argc, const char *const argv[] )  {

  if( argc < 2 )
    fail( "Need at least one argument with domain" );

  // one is argv[0] other argv[last]
  #define NOTOPTARG_COUNT 2

  // 4 -> ipv4
  // 6 -> ipv6
  char *optstr = "46";
  igmisc_short_opts sopts;
  igmisc_sopts_init( &sopts, ( unsigned char* )optstr );
  if( igmisc_sopts_load( &sopts, igmisc_sopts_readorder,
      argc - NOTOPTARG_COUNT, &( argv[1] ) ) == -1 )
    fail( "Passed option arguments are broken" );
  // Note argv[1] since we avoid argv[0]
  // it would alert error 

  const char *const domain_name = argv[ argc - 1 ];

  int count_ipv = 1;

  int inet_hint = AF_INET;
  if( sopts[ '6' ] != 0 )  inet_hint = AF_INET6;
  if( sopts[ '6' ] && sopts[ '4' ] )  {

    count_ipv = 2;
    if( sopts[ '6' ] > sopts[ '4' ] ) {

      inet_hint = AF_INET;

    } 
 
  }

  if( ( ! sopts[ '6' ] ) && ( ! sopts[ '4' ] ) )
    inet_hint = AF_UNSPEC;

  // TODO this needs some more look into 
  // especialy AI_ADDRCONFIG
  // also -46 does not work properly
  // wrap in for and break when empty

  struct addrinfo hint = {0};
//hint.ai_flags = AI_ADDRCONFIG | AI_CANONNAME;
  hint.ai_family = inet_hint;
 //hint.ai_socktype = SOCK_DGRAM; 
  hint.ai_protocol = IPPROTO_TCP;  // else double resoults for udp

  for(;;)  {

    struct addrinfo  *aidata = NULL, *aipos = NULL;
    int error = 0;

    error = getaddrinfo( domain_name, NULL, &hint, &aidata );
    if( error != 0 )  { // e

      if( error == EAI_AGAIN )  continue;

 // fix to bsd chaos.
#ifdef EAI_NODATA
      if( ( error == EAI_NODATA ) 
        || ( error == EAI_NONAME ) ) { // e1
#else   
      if( error == EAI_NONAME )  {  // e1
#endif
        if( hint.ai_family == AF_UNSPEC )  return 0;
        if( count_ipv == 1 )  return 0;

        // 1 or 2 are possibilities so if not 1 
        // than we had 2 and we decrement to 1
        count_ipv = 1; 
        if( hint.ai_family == AF_INET )
          hint.ai_family = AF_INET6;
        else hint.ai_family = AF_INET;
        continue;

      } // e1
      fail( gai_strerror( error ) );
      return -1;

    } //e

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

      if( ans[0] != '\0' ) printf( "%s\n", ans );
      if( aipos->ai_next == NULL )  break;
      aipos = aipos->ai_next;

    }

    freeaddrinfo( aidata );
  
    if( count_ipv == 1 )  return 0;

    // 1 or 2 are possibilities so if not 1 
    // than we had 2 and we decrement to 1

    count_ipv = 1; 
    if( hint.ai_family == AF_INET )
      hint.ai_family = AF_INET6;
    else hint.ai_family = AF_INET;

  }

  return 0;

}
