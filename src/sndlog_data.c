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

#include "../ignotalib/src/ig_net/ign_unixserv.h"
#include "../ignotalib/src/ig_net/ign_strtoport.h"
#include "../ignotalib/src/ig_net/ign_inetserv.h"
#include "../ignotalib/src/ig_event/igev_signals.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#include <limits.h>
#include <poll.h>
#include <signal.h>
#include <unistd.h>

#include <stdbool.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef DBG
 #define PUTSDBG(WRD) fprintf( stderr, "%s\n", WRD )
#else
 #define PUTSDBG(WRD)
#endif

#define LISTEN_QUEUE 1024

// TODO Handle EPIPE
// TODO this needs testing, too much new code
// TODO in man tell user reuseaddr is default socket option

// TODO MUST TEST THIS

void manual( void )  {

  fprintf( stderr, "You need to pass two arguments.\n" );	
  fprintf( stderr, "First - followed by options:\n" );	
  fprintf( stderr, "-s program will send one line from stdin per connection\n" );	
  fprintf( stderr, "-l program will log each message it recives to stdout \n" );	
  fprintf( stderr, "-u use unix socket\n" );	
  fprintf( stderr, "-i use inet socket\n" );
  fprintf( stderr, "-T use tcp\n" );
  fprintf( stderr, "-U use udp\n" );
  fprintf( stderr, "It is error to mix both options as those in pairs: sl ui TU\n" );	
  fprintf( stderr, "Second argument is unix socket name if -u option was chosen\n" );
  fprintf( stderr, "For -i socket in the second argument we must give port number \n" );
  fprintf( stderr, "Example: sndrc_data -suU '\\tmp\\xxxfile' \n" );
  fprintf( stderr, "Example: sndrc_data -liT '15015' \n" );
  exit( EXIT_FAILURE );

}

void usr_fail( const char *const error_str )  {

  fprintf( stderr, "%s\n", error_str );
  exit( EXIT_FAILURE );

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

int main( int argc, char *argv[] )  {

  if( argc != 3 )  manual();

  PUTSDBG( "Program start, passed arguments:" );
  PUTSDBG( argv[1] );
  PUTSDBG( argv[2] );


  // OPTIONS
  char *opts = argv[1];

  if( opts[0] != '-' )
    usr_fail( "Option string is broken" );


  bool setopt[255] = {0};
  for( size_t i = 1; opts[i] != '\0'; i++ )  {

    switch( opts[i] )  {

      case 'i':
      case 'l':
      case 's':
      case 'u':
      case 'T':
      case 'U':
        setopt[ ( size_t )( ( unsigned char )opts[i] ) ] = 1;
	break;
      default:
	usr_fail( "User has chosen a not existing option" );

    }

  }

  if( setopt[ 'i' ] && setopt[ 'u' ] )
    usr_fail( "Option i and u can't be picked both" );

  if( setopt[ 'l' ] && setopt[ 's' ] )
    usr_fail( "Option l and s can't be picked both" );

  if( setopt[ 'T' ] && setopt[ 'U' ] )
    usr_fail( "Option T and U can't be picked both" );

   if( !( setopt[ 'l' ] || setopt[ 's' ] ) )
    usr_fail( "s or l option must appear" );

  if( !( setopt[ 'i' ] || setopt[ 'u' ] ) )
    usr_fail( "i or u option must appear" );


  // SET SIGNAL HANDLER - SIGINT causes
  // server to get closed
  struct sigaction sa = { 0 };
  sa.sa_handler = sigint_handler;
  sigfillset( &( sa.sa_mask ) );

  if( sigaction( SIGINT, &sa, NULL ) == -1 )
    fail( "Could not set signal handler" );

  // also get rid of SIGPIPE

  if( igev_sigign( SIGPIPE ) == -1 )
    fail( "Could not ignore signal SIGPIPE" );
 
  // SOCKET ARGUMENT depends on server type
  const char *const sockname = argv[2];
  const char *const sockport = argv[2];

  // unix sock
  const size_t sun_name_size = setopt[ 'u' ] ?
    strlen( sockname ) + 1 : 0;  // 1 is for nul

  // inet sock
  int portchk = -1;
  if( setopt[ 'i' ] )  {

    portchk = ign_strtoport( sockport );
    if( portchk == -1 )
      usr_fail( "Broken port number" );

  }
  const unsigned short port = portchk;
  
  // SET SOCKET
  int sockfd = -1;

  // unix sock
  if( setopt[ 'u' ] )  {

    if( sun_name_size > PATH_MAX )
      usr_fail( "Unix socket path string too long" );
    struct sockaddr_un sun;
    if( sun_name_size > sizeof sun.sun_path )  {

      fprintf( stderr,
        "Your max unix socket name path is %zu\n",
        sizeof sun.sun_path );
      usr_fail( "Unix socket path is "
	    "too small for your socket name path" );

    }

    FILE *chk_sockpath = fopen( sockname, "r" );
    if( chk_sockpath != NULL )  {

      fclose( chk_sockpath );
      fail( "File under socket name already exists." );
    
    }

    // This will still get NULL on existing unix socket 
    // but further the error gets cought on socket creation.
    // since it is NULL we don't need to close it
    
    sockfd = ign_unixserv( sockname, LISTEN_QUEUE );

  }  else  { // inet sock
 
    sockfd = ign_inettcpserv( port, LISTEN_QUEUE );

  }

  // now check what ever we tried to create
  if( sockfd == -1 )  fail( "Socket creation failure" );

  const size_t buff_size = 8192;
  char buff[ buff_size ];

  int poll_ms_timeout = 50;
  const size_t pfd_len = 1;
  struct pollfd pfd[ pfd_len ];
  pfd[0].fd = sockfd;
  pfd[0].events = POLLIN;
        
  char *linestr;
  size_t linesize;
  ssize_t bytes;

  for(;;)  {

    PUTSDBG( "Poll loop start" );
    if( exit_interrupt )  { // SIGINT came

      close( sockfd );
      if( setopt[ 'u' ] )  {

        if( remove( sockname ) == -1 )
          fprintf( stderr, "fail on unixsock removal" );

      }
      fprintf( stderr, "Exiting on sigint - gentle fin." );
      return 0;

    }

    // error on poll
    int ret = poll( pfd, pfd_len, poll_ms_timeout );
    if( ret == -1 )  {

      switch( errno )  {

        case EINTR:
        case EAGAIN:
	  continue;
	default:
	  fail( "Fail on poll call" );

      }

    } else if( ret == 0 )  continue;

    // there is connection.- probably.
    
    // Bad device - should not happen.
    // Blowe up so everyone sees
    if( pfd[0].revents == POLLNVAL )
      fail( "Poll alerts fd is invalid" );

    // Error on device - read to see what happened.
    // Pollerr might mean we have simply RST
    if( pfd[0].revents & ( POLLERR | POLLIN ) )  {

      int newsock = accept( sockfd, NULL, NULL  );
      if( newsock == -1 )  {

        switch( errno )  {

          case EINTR:
            continue;
	  default:
	    fail( "Fail on socket accepting" );

	}

      }

      for( int leaveloop = 0; ! leaveloop;)  {

	PUTSDBG( "Client connected loop start" );

	if( setopt[ 'l' ] )  {  // LOG
		
	  // -1 because i might need last one for nul
	  ssize_t read_size =
	    read( newsock, buff, buff_size - 1 );      
          if( read_size == -1 )  {

	    switch( errno )  {

	      case EINTR:
	        errno = 0; // clear errno
	       continue;
	      default:  
	        perror( "Unusuall error on read" );
	      case ECONNRESET:
	      case ENOTCONN:
	      case ETIMEDOUT:
	        leaveloop = 1;
	        errno = 0;  //clear errno
	        break;

	    }

   	  } else if( read_size == 0 )  {

	    PUTSDBG( "EOF/FIN" );
	  
	    close( newsock );
	    leaveloop = 1;

	  } else {

	    PUTSDBG( "MESSAGE" );

	    // set nul;
	    buff[ read_size ] = '\0';
	    if( printf( "%s", buff ) < 0 ) {
// TODO just write it instea dof printfgame
	      perror( "Printf error" );
	      errno = 0;  //clear errno

	    }

	  }
  
	}  else  {  // SEND

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
// TODO use write ignota functions
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

}



