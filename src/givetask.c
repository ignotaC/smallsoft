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

#include <arpa/inet.h>
#include <netinet/in.h>

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
      == -1 )  goto usockfail;

  // bigger since if no place instant drop
  if( listen( sockfd, 8192 ) == -1 )
    goto usockfail;

  return sockfd;

  usockfail:
  close( sockfd );
  return -1;

}


// TODO set reuseaddr sock option
int getnetsock( unsigned short port )  {

  struct sockaddr_in sin = {0};
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl( INADDR_ANY );
  sin.sin_port = htons( port );

  int sockfd = socket( AF_INET, SOCK_STREAM, 0 );
  if( sockfd == -1 )  return -1;

  if( bind( sockfd, ( struct sockaddr* ) &sin, sizeof( sin ) ) == -1 )
    goto netsockfail;

  if( listen( sockfd, 8192 ) == -1 )
    goto netsockfail;

  return sockfd;

  netsockfail:
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

// Syntax possibilities
// -u 'sockname'
// -n 'ip range accepted'  'port'

enum {

  UNIXSOCK,
  INETSOCK

};


int main( int argc, char *argv[] )  {

  PUTSDBG( "Program start" );

  if( argc < 2 )  {

    fail( "How to use:\n"
      "Each line on stdin will be passed per connection\n"
      "Either you start a unix socket or netsocket\n"
      "-u unixsocketname\n"
      "-n IPrangetoaccept port\n" );

  }


  if( argv[1][0] != '-' )
    fail( "Option was not provided" );

  int socktype = 0;
  switch( argv[1][1] ) {

    case 'u':
      socktype = UNIXSOCK;
      if( argc != 3 ) fail( "Brokne arguments" );
      break;
    case 'n':
      socktype = INETSOCK;
      if( argc != 4 ) fail( "Broken arguments" );
      break;
    default:
      fail( "There is no such option" );

  }
  
  
  if( socktype == UNIXSOCK )  {
	  
    char *sockname = argv[2];

      PUTSDBG( "Passed path:" );
      PUTSDBG( argv[2] );
  
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

    int sockfd = getsock( sockname );
    if( sockfd == -1 ) fail( "Could not create socket." );

    int poll_ms_timeout = 50;
    const size_t pfd_len = 1;
    struct pollfd pfd[ pfd_len ];
    pfd[0].fd = sockfd;
    pfd[0].events = POLLIN;

    for(;;)  {

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

        
        char *linestr;
        size_t linesize;
	ssize_t bytes;

	for(;;)  {

 	  PUTSDBG( "NOW WAIT WE PASS LINE" );
	  
	  linestr = NULL;
	  linesize = 0;

	  bytes = getline( &linestr, &linesize, stdin );
	  if( bytes == -1 ) {

            PUTSDBG( "ERROR" );
	    if( errno )
	      fail( "Error on getting line in stdin" );

	    shutdown( newsock, SHUT_RDWR );
	    close( newsock );
	    return 0;

  	  }

	  PUTSDBG( linestr );

	  char *linestr_pos = linestr;
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

	    bytes -= writeret;
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

  PUTSDBG( "PICKED UNIX NET SOCKET" );

  char *iprange = argv[2];

  PUTSDBG( "Passed IPRANGE:" );
  PUTSDBG( argv[2] );
  
  struct sigaction sa = { 0 };
  sa.sa_handler = sigint_handler;
  if( sigaction( SIGINT, &sa, NULL ) == -1 )
    fail( "Could not set signal handler" );

  PUTSDBG( "Port:" );
  PUTSDBG( argv[3] );
  unsigned short port = 0;
  if( sscanf( argv[3], "%hu", &port ) != 1 )
    fail( "Could not load port from arguments with scanf" );
  
  int sockfd = getnetsock( port );
  if( sockfd == -1 ) fail( "Could not create socket." );

  int poll_ms_timeout = 50;
  const size_t pfd_len = 1;
  struct pollfd pfd[ pfd_len ];
  pfd[0].fd = sockfd;
  pfd[0].events = POLLIN;
    
  for(;;)  {

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
      sockfd = getnetsock( port );
      if( sockfd == -1 )
        fail( "Could not create socket" );
      pfd[0].fd = sockfd;
      continue;

    }

    // Error on device - read to see what happened.
    if( pfd[0].revents & ( POLLERR | POLLIN ) )  {

      PUTSDBG( "We have a connection" );

      // TODO -> check the ip range if not in - drop connection
      int newsock = accept( sockfd, NULL, NULL  );
      if( newsock == -1 )  {

        perror( "Error on new sock creation" );
  	continue;

      }

      char *linestr = NULL;
      size_t linesize = 0;
      ssize_t bytes = 0;
      for( int leaveloop = 0; ! leaveloop;)  {

 	PUTSDBG( "NOW WAIT WE PASS LINE" );
	
	errno = 0;
	bytes = getline( &linestr, &linesize, stdin);
	if( bytes == -1 )  {

	  PUTSDBG( "ERROR ON LINE" );
	  if( errno )
	    fail( "Error on getting line in stdin" );

	  shutdown( newsock, SHUT_RDWR );
	  close( newsock );
	  return 0;

  	}
        
        PUTSDBG( linestr );

	char *linestr_pos = linestr;
	for( ssize_t writeret = 0; bytes > 0;)  {

	  writeret = write( newsock, linestr_pos, bytes );
	  if( writeret < 0 )  {

	    switch( errno )  {

	      case EINTR:
	       continue;
	      
	      case EPIPE:
              case ENETDOWN:
	       goto finishnet;
              
	      default:	  
	       fail( "Write error" );  

	    }
      	
	  }

	  bytes -= writeret;
	  linestr_pos += ( size_t ) writeret;

  	}

        shutdown( newsock, SHUT_RDWR );
       finishnet:
	free( linestr );
	linestr = NULL;
	close( newsock );
	break;

      }

    }

  }

}
