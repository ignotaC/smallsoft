// offered to God - Jezus, God Father, Saint Holy Spirit

#include "../ignotalib/src/ig_file/igf_opt.h"

#include <sys/socket.h>

#include <pthread.h>

#include <netdb.h>
#include <signal.h>
#include <unistd.h>

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>

ssize_t write_msg( const int fd, void *const passed_buff, size_t passed_buff_size )  {
  
  const uint8_t *buff_ptr = passed_buff;
  ssize_t  write_return = 0;
  
  for( int i = 0;;)  {
    
    if( ( write_return = ( write( fd, buff_ptr, passed_buff_size ) ) ) <= 0 )  {
      
      if( i > 100 )  {

	errno = 0;
	return -1;

      }

      if( write_return == 0 )  {
        
	i++;
        if( passed_buff_size == 0 )  return 0;
        continue;
        
      }

      if( errno == EINTR )  {
	
	i++;  
        continue;

      }

      return -1;
      
    }
    
    buff_ptr += write_return;
    passed_buff_size -= ( size_t )write_return;

    if( passed_buff_size == 0 )  return 0;
    
  }
    
}


void fail( const char *const fail_str )  {

  perror( fail_str );
  exit( EXIT_FAILURE );

}

struct network {

  char name[500];
  char host[500];
  char **msg;
  size_t msg_count;
  char **channels;
  size_t channels_count;
  char **ident;
  size_t ident_count;
  uint16_t port;

  int sockfd;

  FILE **logfiles;

};

struct network *pass_irc = NULL;
pthread_mutex_t irc_mutex;

void thread_fail( struct network *const ircnet_ptr,  const char *const str_fail )  {

  perror( str_fail );
  perror( strerror( errno ) );

  for( int a, e = 0;; sleep( 1 ), e++ )  {

    if( e > 100 )  exit( EXIT_FAILURE );
    if( ( a = pthread_mutex_lock( &irc_mutex ) ) == 0 )  break;
    if( a == EAGAIN )  continue;
    exit( EXIT_FAILURE );

  }

  pass_irc = ircnet_ptr;

  for( int a, e = 0;; sleep( 1 ), e++ )  {

    if( e > 100 )  exit( EXIT_FAILURE );
    if( ( a = pthread_mutex_unlock( &irc_mutex ) ) == 0 )  break;
    if( a == EAGAIN )  continue;
    exit( EXIT_FAILURE );

  }

  pthread_exit( NULL );

}

void closelogs( struct network *const ircnet, const size_t size_to_close )  {

  for( size_t i = 0; i < size_to_close; i++ )  {

    if( fclose( ircnet->logfiles[i] ) < 0 )
      perror( "Failed on closing file" );

  }

}


void synclogs( const struct network *const ircnet )  {

  for( size_t i = 0; i < ircnet->channels_count; i++ )  {

    if( fsync( fileno( ircnet->logfiles[i] ) ) < 0 )
      perror( "Fail on file sync" );

  }

}

int openlogs( struct network *const ircnet )  {
  
  const size_t tempbuff_size = 4096;
  char tempbuff[ tempbuff_size ];
  
  for( size_t i = 0; i < ircnet->channels_count; i++ )  {

    snprintf( tempbuff, tempbuff_size - 1,  "%s:%s_logs", ircnet->host, ircnet->channels[i] );
    puts( tempbuff );
    ircnet->logfiles[i] = fopen( tempbuff, "a" );
    if( ircnet->logfiles[i] == NULL )   {

      closelogs( ircnet, i );
      thread_fail( ircnet, "Fail on opening file" );

    }

    if( igf_cloexec( fileno( ircnet->logfiles[i] ) ) == -1 )  {

      closelogs( ircnet, i );
      thread_fail( ircnet, "Fail on setting fd to close on exec" );

    }

  }

  return 0;

}

int irc_connect( struct network *ircnet )  {  

  struct addrinfo hint = {0};

  hint.ai_flags = AI_CANONNAME;
  hint.ai_socktype = SOCK_STREAM;

  struct addrinfo *ai_ret;
  
  int gai_ret;
  if( ( gai_ret = getaddrinfo( ircnet->host, NULL, &hint, &ai_ret ) ) != 0 )  {

    perror(  gai_strerror( gai_ret ) );
    return -1;

  }

  int family = 0;
  for( struct addrinfo *ai_ptr = ai_ret; ! family; ai_ptr = ai_ptr->ai_next )  {

    if( ai_ptr == NULL )  {

      freeaddrinfo( ai_ret );
      perror( "Could not get any address." );
      return -1;
    
    }

    switch( ai_ret->ai_family )  {

    case AF_INET:
      ( ( struct sockaddr_in* )ai_ret->ai_addr )->sin_port =
         htons( ircnet->port );
      break;

    case AF_INET6:
      ( ( struct sockaddr_in6* )ai_ret->ai_addr )->sin6_port =
        htons( ircnet->port );
      break;

    default:
      ircnet->sockfd = -1;
      continue;

    }
    
    if( ( ircnet->sockfd = socket( ai_ret->ai_family,
        SOCK_STREAM, 0 ) ) == -1 )  continue; 
    if( connect( ircnet->sockfd, ( struct sockaddr* ) ai_ret->ai_addr,
        ai_ret->ai_addrlen ) == -1 )  {
      
      close( ircnet->sockfd );
      continue;
      
    }   

    family = ai_ret->ai_family;     
    
  }

  freeaddrinfo( ai_ret );
  if( ircnet->sockfd == -1 )  return -1;

  if( igf_nonblock( ircnet->sockfd ) )  {

    close( ircnet->sockfd );
    return -1;

  }

  if( igf_cloexec( ircnet->sockfd ) )  {

    close( ircnet->sockfd );
    return -1;

  }

  printf( "Connected to: %s\n", ircnet->host );

  return 0;

}


int read_err_check( struct network *ircnet )  {

  switch( errno )  {

  case ENOBUFS:
    sleep( 5 );
  case EAGAIN:
#if ( EAGAIN != EWOULDBLOCK )
  case EWOULDBLOCK:
#endif
  case EINTR:
    return 1;
    
  case ECONNRESET:
  case ETIMEDOUT:
  case ENOTCONN:
  case EBADF:
    close( ircnet->sockfd ); // maybe it should not be after badfd but fuck this
                             // eventually somethign will get closed 
    if( irc_connect( ircnet ) < 0 )  return -1;
    sleep( 1 );
    return 0;
  
  default:
    close( ircnet->sockfd );
    return -1;

  }

}

int write_err_check( struct network *ircnet )  {

  switch( errno )  {

  case ENOBUFS:
  case ENETDOWN:
  case ENETUNREACH:
    sleep( 5 );
  case EAGAIN:
#if ( EAGAIN != EWOULDBLOCK )
  case EWOULDBLOCK:
#endif
  case EINTR:
  case 0: // just in case
    return 1;
  
  case EPIPE:
  case ECONNRESET:
  case EBADF:
    close( ircnet->sockfd );
    if( irc_connect( ircnet ) < 0 )  return -1;
    sleep( 1 );
    return 0;
  
  default:
    close( ircnet->sockfd );
    return -1;

  }

}

////   <------------ stop
// get msg
ssize_t get_msg_line( char **const buff_pp, size_t *const buff_left_size, ssize_t *const read_size,
		      char *line_buff, size_t line_buff_size )  {

  ssize_t ans_len = 0;
  char byte;
  int cr = 0;

  for(; *read_size > 0; ( *read_size )-- )  {

    if( *buff_left_size == 0 )  return 0;

    byte = **buff_pp;
    ++( *buff_pp );
    ( *buff_left_size )--;

    // this should never happen
    if( line_buff_size == 0 )  return 0;
    
    *line_buff = byte;
    line_buff_size--;
    line_buff++;
    ans_len++;

    if( byte == '\r' )  {
  
      cr = 1;
      continue;

    }

    if( byte == '\n' )  {

      if( cr )  {

	( *read_size )--;
	*( line_buff - 1 ) = 0;
	return ans_len;

      }

    }
    
    cr = 0;

  }

  return -ans_len;

}


int irc_startup_msg( struct network *ircnet, char *const buff, const size_t buff_size )  {

  for( size_t i = 0; i < ircnet->msg_count; i++ )  {

    if( snprintf( buff, buff_size, "%s\r\n", ircnet->msg[i] ) < 0 )  return -1;

    /*
    if( buff[0] == '/' )  {

      if( buff[1] == '/' )
	memmove( &buff[0], &buff[1], strlen( &buff[1] ) + 1 );
      else  {
	
	puts( "waiting now" );
	int wait = 0;
	printf( "%s\n", buff );
	sscanf( &buff[1], "%d", &wait );
	sleep( wait );
	puts( "done" );
	return 0;
	
      }
      
    }

    */
    
    if( write_msg( ircnet->sockfd, buff, strnlen( buff, buff_size ) ) < 0 )  return -1;

  }

  return 0;

}


int irc_join_channels( struct network *ircnet, void *const buff, const size_t buff_size )  {

  for( size_t i = 0; i < ircnet->channels_count; i++ )  {
    
    sleep( 3 );

    if( snprintf( buff, buff_size, "JOIN %s\r\n", ircnet->channels[i] ) < 0 )
      return -1;

    if( write_msg( ircnet->sockfd, buff, strlen( buff ) ) < 0 )  return -1;

  }

  return 0;

}

void start_write_chk( struct network *const ircnet )  {

  if( write_err_check( ircnet ) < 0 )  thread_fail( ircnet, "Fail inloop on write" );

}



ssize_t inloop_write_chk( struct network * const ircnet )  {

  ssize_t ret = 0;
  if( ( ret = write_err_check( ircnet ) ) < 0 )  thread_fail( ircnet, "Fail inloop on write" );
  return ret;

}


ssize_t inloop_read_chk( struct network *const ircnet )  {

  ssize_t ret = 0;
  if( ( ret = read_err_check( ircnet ) ) < 0 )  thread_fail( ircnet, "Fail inloop on write" );
  return ret;

}

int send_irc_msg( struct network *ircnet, void *const passed_buff,
		  const size_t passed_buff_size )  {

  errno = 0;
  size_t msg_len = strnlen( passed_buff, passed_buff_size );
  if( passed_buff_size <= msg_len + 3 )  return -1;
  ( ( char* )passed_buff )[ msg_len++ ] = '\r';
  ( ( char* )passed_buff )[ msg_len++ ] = '\n';
  ( ( char* )passed_buff )[ msg_len ] = '\0';
  
  for( ssize_t write_ret = 0;; )  {

    write_ret = write_msg( ircnet->sockfd, passed_buff, msg_len );
    if( write_ret >= 0 )  break;

    if( inloop_write_chk( ircnet ) < 1 ) return -1;

  }

  return 0;

}

int use_ircmsg( char *msg, char *buff,
		size_t buff_size )  {

  char *msg_pos = msg;
  while( !isspace( *msg_pos ) && buff_size )  {

    // garbage at this point, those can't show up, same later
    if( *msg_pos == '\0' )  return -1;
    msg_pos++;
    buff_size--;

  }
  while( isspace( *msg_pos ) && buff_size )  {

    if( *msg_pos == '\0' )  return -1;
    msg_pos++;
    buff_size--;

  }
  
  int command = 0;
  for(; !isspace( *msg_pos ) && buff_size; )  {
    
    if( isdigit( *msg_pos ) )
      command = 10 * command + ( *msg_pos - 48 );
    else return -1; // if text from ping for example than we ignore
    // since we just check it responds
    msg_pos++;
    buff_size--;

  }

  while( isspace( *msg_pos ) && buff_size )  {

    if( *msg_pos == '\0' )  return -1;
    msg_pos++;
    buff_size--;

  }

  size_t i = 0;
  for(; buff_size--; i++ )  {

    if( *msg_pos == '\0' )  break;
    buff[i] = *msg_pos;
    msg_pos++;

  }
  buff[i] = 0;

  return command;

}


/* TODO
int use_msg( const int command, char *const buff, const size_t buff_size )  {

  switch( command )  {

  case 302:
    get_nick();
    whois_yourself();
    break;

  }

} 
*/
void child_restart( const char *const fail_str, char *argv[] )  {

  perror( fail_str );
  while( kill( getppid(), SIGKILL ) == -1 )
    fail( "Could not sigkill parent" );

  execv( argv[0], argv ); 
  fail( "Could not execve - ending" );
  
}

void* irclog_thread( void *const passed )  {
  
  struct network *const irc_network = ( struct network* ) passed;
  ( void ) passed;  

  if( ( errno = pthread_detach( pthread_self() ) ) != 0 )
    thread_fail( irc_network, "Could not detach" );

  const size_t thread_buff_size = 8192;
  char thread_buff[ thread_buff_size ];
  char thread_buff2[ thread_buff_size ];
  char line_buff[ thread_buff_size ];

  memset( thread_buff, 1, thread_buff_size );
  memset( line_buff, 1, thread_buff_size );

  if( openlogs( irc_network ) < 0 )  {
    
    thread_fail( irc_network, "Could not open all irclog files" );
    
  }

  while ( irc_connect( irc_network ) < 0 )  {

    perror( "Could not connect" );
    sleep( 30 );
    
  }

  while( 1 )  {

    if( irc_startup_msg( irc_network, thread_buff, thread_buff_size ) < 0 )  {

      start_write_chk( irc_network );
      perror( "Write fail" );
      continue;

    }

    if( irc_join_channels( irc_network, thread_buff, thread_buff_size ) < 0 )  {

      start_write_chk( irc_network );
      perror( "Write fail" );
      continue;

    }

    char *tb_start_ptr = thread_buff;
    size_t buff_left_size = thread_buff_size;
    
    time_t current_time = time( NULL );
    time_t lastping_time = time( NULL );
    time_t lastpingans_time = time( NULL );
    time_t sync_time = time( NULL );
    time_t check_time = 0;
    const double send_ping_after = 20.0;
    const double sync_after_time = 1800.0;
    const double check_after_time = 180.0;
    const double ping_waitmax_time = 500.0;

    int command;
    
    // main logger loop
    for( ssize_t read_size = 0, get_ret = 0;;)  {

      current_time = time( NULL );

      if( sync_after_time < difftime( current_time, sync_time ) )  {
	
	synclogs( irc_network );
	sync_time = time( NULL );

      }

      if( check_after_time < difftime( current_time, check_time ) )  {

	sprintf( thread_buff, "USERHOST *" ); // get our name.
	if( send_irc_msg( irc_network, thread_buff, thread_buff_size ) < 0 )  {
	  
	  perror( "Write error" );
	  break;
	  
	}
	
        check_time = time( NULL );
	
      }
      
      if( send_ping_after < difftime( current_time, lastping_time ) ) {
	
	sprintf( line_buff, "ping hue" );
	if( send_irc_msg( irc_network, line_buff, thread_buff_size ) < 0 )  {
	
	  perror( "Error on write" );
	  break;
	
	}      

	lastping_time = time( NULL );

      }
      if( ping_waitmax_time <  difftime( current_time,  lastpingans_time ) )
	thread_fail( irc_network, "Server went completly silent" );

      if( ( read_size = read( irc_network->sockfd, tb_start_ptr, buff_left_size ) ) < 0 )  {

      	if( inloop_read_chk( irc_network ) < 1 )  {

	  perror( "Error on read" );
	  break;
	
	}

	sleep( 1 );
	continue;
	
      }  else  {

	// if it talks we assume all is ok.
	// even if ping/pong never finished
	lastpingans_time = time( NULL );
	lastping_time = time( NULL );

      }

      tb_start_ptr = thread_buff;
      buff_left_size = thread_buff_size;
      read_size -= get_ret;

      // FIN came, wtf?
      if( read_size == 0 )  {
        
	irc_network->sockfd = 0;
	sleep( 60 );
	if( irc_connect( irc_network ) < 0 )  thread_fail( irc_network, "Server send FIN" );
	break;

      }
     
      for(;;)  {
	
	get_ret = get_msg_line( &tb_start_ptr, &buff_left_size, &read_size,
				line_buff, thread_buff_size );

	if( get_ret == 0 )  {

	  tb_start_ptr = thread_buff;
	  buff_left_size = thread_buff_size;
	  break;

	}  else  if( get_ret < 0 )  {
	  
	  if( thread_buff_size < ( size_t  ) -get_ret )  get_ret = 0;  // should never happen
	  tb_start_ptr = thread_buff - get_ret;
	  buff_left_size = thread_buff_size + get_ret;
	  break;
	  
	}  else  puts( line_buff );	
	
	switch( line_buff[0] )  {

	case ':':  //fix
	  if( ( command = use_ircmsg( line_buff, thread_buff2,
				      thread_buff_size ) ) < 0 )
		  continue;
	    // use_msg( command, thread_buff2, thread_buff_size );
	    //use_msg(); FOR NOW UNSUED

	case 'P':
	  line_buff[1] = 'O';
	  if( send_irc_msg( irc_network, line_buff, thread_buff_size ) < 0 )  {

	    perror( "Write error" );
	    break;

	  }
	  continue;

	default:
	  continue;

	}
	
	// we cought some not critical error but we had to reconnect  so we restart 
	break;

      }			

    }

  }
  
  return NULL;

}

int main( int argc, char *argv[] )  {
  
  ( void ) argc;
  
  struct sigaction sa;
  memset( &sa, 0, sizeof( sa ) );
  sa.sa_handler = SIG_IGN;
  if( sigaction( SIGPIPE, &sa, NULL ) < 0 )  fail( "Could not ignore SIGPIPE" );

  if( ( errno = pthread_mutex_init( &irc_mutex, NULL ) ) != 0 )  fail( "could not init mutex" );
  puts( "Main mutex init." );

  int un_sock[2];  
  socketpair( AF_LOCAL, SOCK_STREAM, 0, un_sock );

  if( igf_nonblock( un_sock[0] ) < 0 )
    fail( "Could not set unsock to nonblocking" );

  if( igf_nonblock( un_sock[1] ) < 0 )
    fail( "Could not set unsock2 to nonblocking" );

  pid_t ch_pid = fork();
  if( ch_pid < 0 )  fail( "Failed on fork" );
  if( ch_pid )  { 
    // parent

    const size_t parent_buff_size = 1024;
    char parent_buff[ parent_buff_size ];
    memset( parent_buff, 0, parent_buff_size );
    for( int count = 0, max_count = 240;; count++, sleep( 1 ) )  {

      if( read( un_sock[0], parent_buff, parent_buff_size ) < 0 )  {

	if( ( errno != EAGAIN ) && ( errno != EINTR ) )  count = max_count;

      }  else  count = 0;

      if( write( un_sock[0], parent_buff, 1 ) < 0 )  {

	if( ( errno != EAGAIN ) && ( errno != EINTR ) )  count = max_count;

      }

      if( count >= max_count )  {

	sleep( 5 );
	perror( "Had to restart everything" );

	while( kill( ch_pid, SIGKILL ) == -1 ) 
	  fail( "Could not send kill signal to child" );

	ch_pid = fork();
	if( ch_pid < 0 )  fail( "Failed on fork" );
	if( ! ch_pid )  break;
	count = 0;

      }

    }

  }

  if( igf_cloexec( un_sock[0] ) < 0 )
    fail( "Could not set unsock to close on exec" );

  if( igf_cloexec( un_sock[1] ) < 0 )
    fail( "Could not set unsock2 to close on exec" );

  const  size_t buff_size = 8192;
  char buff[ buff_size ];
  memset( buff, 0 , buff_size );

  FILE *serv_data_file = fopen( "log_serv_data", "r" );
  if( serv_data_file == NULL )  fail( "No log_serv_data file" );

  struct network *irc = NULL;
  
  int command = 0;
  puts( "Starting log servers..." );
  pthread_t pt_id;
  for(;;)  {

    if( irc != NULL )  {

      irc->logfiles = malloc( sizeof( FILE* ) * irc->channels_count );
      if( irc->logfiles == NULL )  fail( "Failed on allocating on logfiles" );

      if( ( errno = pthread_create( &pt_id, NULL, irclog_thread, irc ) ) != 0 )
	fail( "Fail on thread creation" );
      irc = NULL;

    }
    
    if( command != 'S' ) {
    
      errno = 0;
      while( ( command = fgetc( serv_data_file ) ) != 'S' )  {
	
	if( command == EOF )  {

	  if( errno != 0 )  fail( "Fail while reading server config file" );

	  const size_t child_buff_size = 1024;
	  char child_buff[ child_buff_size ];
	  for(int count = 0, count_max = 100, wait_read = 0, wait_read_max = 240;;
	      wait_read++, sleep(1) )  {
	    
	    if( read( un_sock[1], child_buff, child_buff_size ) < 0 )  {

	      if( ( errno != EAGAIN ) && ( errno != EINTR ) )
		fail( "Failed to read un sock" );

	    }  else wait_read = 0;

	    if( wait_read > wait_read_max )
	      child_restart( "There is no life signs from parent", argv );

	    if( write( un_sock[1], child_buff, 1 ) < 0 )  {

	      if( errno == EPIPE )  
		child_restart( "Seems the other side unix sock is broken", argv );
	      if( ( errno != EAGAIN ) && ( errno != EINTR ) )
		fail( "Failed on socket read" );

	    }


	    while( ( errno = pthread_mutex_lock( &irc_mutex ) ) )  {
		
	      if( errno == EAGAIN )  {
	
		sleep(1);
		count++;
	
		if( count > count_max )
		  fail( "Could not execv also fail on locking thread" );
	    
		continue;

	      }
	      fail( "Failed on mutex lock" );
	      
	    }
	    
	    if( pass_irc != NULL )  {
	      
	      synclogs( pass_irc );
	      closelogs( pass_irc, pass_irc->channels_count );

	      if( ( errno = pthread_create( &pt_id, NULL, irclog_thread, pass_irc ) ) != 0 )
		fail( "Fail on resurecting dead thread" );

	      pass_irc = NULL;
	      puts( "Thread restarted" );
 
	    }
	    
	    while( ( errno = pthread_mutex_unlock( &irc_mutex ) ) )  {

	      if( errno == EAGAIN )  {

		sleep( 1 );
		count++;
		if( count > count_max )
		  fail( "Failed multiply times on lock/unlock thread" );

		continue;

	      }
	      fail( "Could not lock thread" );

	    }

	    count = 0;
	      
	  }  

	}
      
      }

      command = 0; // just in case
      errno = 0;
    
    }

    irc = malloc( sizeof( struct network ) );   
    memset( irc, 0, sizeof( *irc ) );
    irc->channels = NULL;
    irc->msg = NULL;
    irc->ident = NULL;

    if( fscanf( serv_data_file, "%400s %hu", irc->host, &( irc->port ) ) != 2 )  {
      
      perror( "Somethign wrong with host:port data" );
      free( irc );
      irc = NULL;
      continue;
      
    }
    
    printf( "Added: %s:%hu\n", irc->host, irc->port );
    
    while( ( command = fgetc( serv_data_file ) ) != EOF )  {

      fflush( stdout );

      if( command == 'S' )  break;
      if( ( command == 'M' ) || ( command == 'C' ) || ( command == 'I' ) )  {

	char ***string_ptr;
	size_t *count_ptr;

	if( command == 'M' )  {

	  string_ptr = &( irc->msg );
	  count_ptr = &( irc->msg_count );

	}  else if( command == 'C' )  {

	  string_ptr = &( irc->channels );
	  count_ptr = &( irc->channels_count );

	}  else  {

	  string_ptr = &( irc->ident );
	  count_ptr = &( irc->ident_count );

	}

	*string_ptr = realloc( *string_ptr, sizeof( **string_ptr ) * ( 1 + *count_ptr ) );
	if( *string_ptr == NULL )  fail( "Exploded on realloc irc msg" );
	( *string_ptr )[ *count_ptr ] = NULL;

	size_t garbage = 0;	
	errno = 0;
// TODO Check if this does not leak
	if( getline( &( ( *string_ptr )[ *count_ptr ] ), &garbage, serv_data_file ) < 0 )  {
	    
	  if( errno )  fail( "Fail on getline" );
	    
	}

	( *count_ptr )++;

	for( size_t i = strlen( ( *string_ptr )[ *count_ptr - 1 ] ); i > 0; i-- ) {

	  if( ( *string_ptr )[ *count_ptr - 1 ][i] == '\n' )  {

	    ( *string_ptr )[ *count_ptr - 1 ][i] = 0;
	    break;

	  }

	} 

	if( command == 'M' )  printf( "%s starting msg", irc->host );
	else  printf( "%s channel", irc->host );
	printf( " %zu: %s\n", *count_ptr, ( *string_ptr )[ *count_ptr - 1 ] );
       	continue;

      }

      // just  read whole line and ignore it.
      errno = 0;
      while( ( command != '\n' ) && ( command != EOF ) )  {
	
	command = fgetc( serv_data_file );

      }
      
      if( errno != 0 )  fail( "Could not read file while removing garbage line" );

    }

  }

}
