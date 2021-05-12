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


#ifndef SUN_LEN
  #define SUN_LEN(sun) ( sizeof( *( sun ) ) \
    - sizeof( ( sun )->sun_path ) \
    + strlen( ( sun )->sun_path  ) )
#endif



int getsock( char *name )  {

  struct sockaddr_un sun = { 0 };
  sun.sun_family = AF_UNIX;
  strcpy( sun.sun_path, name );

  int sockfd = socket( AF_UNIX, SOCK_STREAM, 0 );
  if( sockfd == -1 )  return -1;

  if( bind( sockfd, ( struct sockaddr* ) &sun, SUN_LEN( &sun ) )
      == -1 )  goto sockfail;

  // bigger since if no place instant drop
  if( listen( sockfd, 8192 ) == -1 )
    goto sockfail;

  return sockfd;

  sockfail:
  close( sockfd );
  return -1;

}

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

  if( argc != 2 )  {

    fail( "You need to pass socket name, \n"
      "Each line on stdin will be passed per connection" ); 
 
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

  struct sigaction sa;
  sa.sa_mask = 0;
  sa.sa_flags = 0;
  sa.sa_handler = sigint_handler;

  if( sigaction( SIGINT, &sa, NULL ) == -1 )
    fail( "Could not set signal handler" );

  int sockfd = getsock( sockname );
  if( sockfd == -1 ) fail( "Could not create socket." );

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
      sockfd = getsock( sockname );
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


      char *linestr = NULL;
      size_t linesize = 0;
      for( int leaveloop = 0; ! leaveloop;)  {

 	PUTSDBG( "NOW WAIT WE PASS LINE" );
	
	errno = 0;
	if( getline( &linestr, &linesize, stdin ) ==
          -1 )  {

	  if( errno )
	    fail( "Error on getting line in stdin" );

	  shutdown( newsock, SHUT_RDWR );
	  close( newsock );
	  break;

	}

	char *linestr_pos = linestr;
	size_t bytes = strlen( linestr );
	for( ssize_t writeret = 0; bytes;)  {

	  writeret = write( newsock, linestr_pos, bytes );
	  if( writeret < 0 )  {

	    switch( errno )  {

	      case EINTR:
	       continue;
	      
	      case EPIPE:
	      case ENETDOWN:
	       goto finish;
              
	      default:	  
	       fail( "Write error" );  

	    }

	  }

	  bytes -= ( size_t ) writeret;
	  linestr_pos += ( size_t ) writeret;

	}

	shutdown( newsock, SHUT_RDWR );
       finish:
	free( linestr );
	linestr = NULL;
	close( newsock );
	break;

      }

    }

  }

}
