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

#include "../ignota/src/ig_net/ign_unixsock.h"

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>

#include <limits.h>
#include <poll.h>
#include <signal.h>
#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef DBG
 #define PUTSDBG(WRD) fprintf( stderr, "%s\n", WRD )
#else
 #define PUTSDBG(WRD)
#endif

#define QUEUE_LIMIT 8192

void fail( const char *const error_str )  {

  perror( error_str );
  exit( EXIT_FAILURE );

}


volatile sig_atomic_t exit_interrupt = 0;
void sigint_handler( int sig )  {

  ( void ) sig;
  exit_interrupt = 1;

}

mode_t glob_sunperm = S_IRWXU;

int setperm( const char *const permstr )  {

  const char *spos = ( const char*) permstr;
  for(; *spos != '\0'; spos++ )  {

    switch( *spos )  {

     case 'g':
      glob_sunperm |= S_IWGRP;
      continue;

     case 'o':
      glob_sunperm |= S_IWOTH;
      continue;

     default:
      return -1;

    }

  }

  return 0;

}

int main( int argc, char *argv[] )  {

  PUTSDBG( "Program start" );

  if( argc != 3 )  {

    if( argc == 4 )  {

      if( setperm( argv[3] ) == -1 )
        fail( "Wrong file permissions\n"
	      "Example: 'go' which is group and other\n"
	      "User can by default write to unix socket\n" );

    }
    else fail( "You need to pass socket name and script you want to run.\n"
               "Also you can pass unix socket file permissions,\n"
	       "else only default permissions, also you can ask server,\n"
	       "to wait before you finish.\n"
	       "Example ./prog sockname command go\n" ); 
  }

  char *sockname = argv[1];

  PUTSDBG( "Passed path:" );
  PUTSDBG( argv[1] );
  
  const size_t sockname_size = 
    strlen( sockname ) + sizeof( *sockname );
  if( sockname_size > PATH_MAX )
     fail( "Socket name too long." );

  {

    struct sockaddr_un sun;
    size_t sunpath_size = sizeof( sun.sun_path );
    if( sockname_size > sunpath_size )  {

      fprintf( stderr,
        "Your max unix socket name path is %zu\n",
        sunpath_size );
      fail( "Unix socket path is "
	    "too small for your socket name path" );

    }

    PUTSDBG( "Provided socket path is fine." );

  }

  PUTSDBG( "Checking socket path" );
  FILE *check_socket_path = fopen( sockname, "r" );
  if( check_socket_path != NULL )
    fail( "File under socket name already exists." );

  // since it is NULL we don't need to close it

  PUTSDBG( "File with provided socket name"
	   " does not exist" );
  errno = 0; // clear errno

  struct sigaction sa = { 0 };
  sa.sa_handler = sigint_handler;

  if( sigaction( SIGINT, &sa, NULL ) == -1 )
    fail( "Could not set signal handler" );

  int sockfd = ign_getsun( sockname, QUEUE_LIMIT );
  if( sockfd == -1 ) fail( "Could not create socket." );

  if( argc == 4 )
    if( chmod( sockname, glob_sunperm ) == -1 )
      fail( "Failed on socket permissions settings" );

  int poll_ms_timeout = 50;
  const size_t pfd_len = 1;
  struct pollfd pfd[ pfd_len ];
  pfd[0].fd = sockfd;
  pfd[0].events = POLLIN;

  for(;;)  {

    PUTSDBG( "Poll loop start" );
    if( exit_interrupt )  {

      close( sockfd );
      fprintf( stderr, "Exiting on sigint - gentle fin." );
      return 0;

    }

    int ret = poll( pfd, pfd_len, poll_ms_timeout );
    if( ret == -1 )  {

      switch( errno )  {

        case EINTR:
        case EAGAIN:
	  errno = 0;  // clear errno
	  continue;
	default:
	  fail( "Fail on poll call" );

      }

    } else if( ret == 0 )  continue;

    // there is connection.- probably.
    
    // Bad device - should not happen.
    // As for error on device let's just reset it
    if( pfd[0].revents == POLLNVAL )  {

      close( sockfd );
      sockfd = ign_getsun( sockname, QUEUE_LIMIT );
      if( sockfd == -1 )
        fail( "Could not create socket" );
      pfd[0].fd = sockfd;
      continue;

    }

    // Error on device - read to see what happened.
    if( pfd[0].revents & ( POLLERR | POLLIN ) )  {

      int newsock = accept( sockfd, NULL, NULL  );
      if( newsock == -1 )  {

        perror( "Error on new sock creation" );
	continue;

      }

      for( int leaveloop = 0; ! leaveloop;)  {

 	PUTSDBG( "NOW WAIT WE FINISH COMMAND" );
	system( argv[2] );
	shutdown( newsock, SHUT_RDWR );
	close( newsock );
	break;

      }

    }

  }

}
