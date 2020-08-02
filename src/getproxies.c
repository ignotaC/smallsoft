/*

Copyright (c) 2019 Piotr Trzpil  p.trzpil@protonmail.com

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

#define _POSIX_C_SOURCE 200809L

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#include <fcntl.h>
#define BUFFSIZE 20000000


int cpyfile( const char *const old_name,
             const char *const new_name )  {
  FILE *const old_file = fopen( old_name, "r" );
  if( old_file == NULL )  return -1;
  FILE *const new_file = fopen( new_name, "w" );
  if( new_file == NULL )  return -1;
  const size_t membuff_size = 8192;
  char *const membuff[ membuff_size ];
  memset( ( char* )membuff, 0, membuff_size );

  while( fgets( ( char* )membuff, membuff_size, old_file ) != NULL )  {

    if( ( int )strlen( ( char * )membuff ) !=
         fprintf( new_file, "%s", ( char * )membuff ) )  return -1;

  }

  if( ferror( old_file ) )  return -1;
  if( ferror( new_file ) )  return -1;

  fclose( new_file );
  fclose( old_file );

  return 0;

}

int fill_addr4( struct sockaddr_in *const sin,
		const char *const ip, const uint16_t port )  {

  int inet_return;
  if( ( inet_return = inet_pton( AF_INET, ip, &sin->sin_addr ) ) == -1 )  return -1;
  if( inet_return == 0 )  return -2;
  
  sin->sin_port = htons( port );
  sin->sin_family = AF_INET;
  
  return 0;
  
}


// this needs rework.
int connect_timeout( const int sockfd, const uint32_t time_out_us,
		     struct sockaddr_in *const sin )  {
  int flags;
  if( ( flags = fcntl( sockfd, F_GETFL ) ) < 0 )
    return -1;

  if( fcntl( sockfd, F_SETFL, O_NONBLOCK | flags  ) < 0 )
    return -1;
  
  // it should pass EIPROGRESS
  if( connect( sockfd, ( struct sockaddr* ) sin, sizeof( *sin ) ) < 0 )  {

    if( errno == EISCONN )  {

      return 0;

    }
      
    if( errno != EINPROGRESS )  return -1;
    
  }

  struct timeval tv;
  fd_set fs;
  FD_ZERO( &fs );
  FD_SET( sockfd, &fs );

  for(;;)  {

    tv.tv_sec = 0;
    tv.tv_usec = time_out_us;
    if( select( sockfd + 1, NULL, &fs, NULL, &tv ) < 0 )  {

      if( errno != EINTR )
	return -1;
      
      continue;
      
    }

    break;
    
  }

  if( FD_ISSET( sockfd, &fs ) )  {
    
    if( connect( sockfd, ( struct sockaddr* ) &sin, sizeof( sin ) ) < 0 )  {
      
      if( errno != EISCONN )  return -1;

    }
  
    int sock_err = 0;
    socklen_t sock_err_len = sizeof( sock_err );
    
    if( getsockopt( sockfd, SOL_SOCKET, SO_ERROR,
		    &sock_err, &sock_err_len ) < 0 )  return -1;
    
    if( sock_err != 0 )  {

      errno = sock_err;
      return -1;
      
    }
    
    return 0;
    
  }

  return -1;
  
}



void err( void )  {
  
  printf( "\n%s\n", strerror( errno ) );
  exit( EXIT_FAILURE );
  
}

void php_rot13( char* word )  {
  
  size_t wordSize = strlen( word );
  for( size_t i = 0 ; i < wordSize; i++ )  {
    
    if( ! isalpha(  word[i] ) ) continue;
    
    if( tolower( word[i] ) < 'n' )   word[i] += 13;
    else			     word[i] -= 13;
    
  }
  
}

void decode_url( char *str )  {  //not secure, can be overflowed
  
  char *pos1 = &str[0];
  char *pos2 = &str[0];
  char temp2[3];
  temp2[2] = 0;
  
  long int temp = 0;
  
  size_t strSiz = strlen( str );
  
  size_t j = 0;
  for( size_t i = 0; i < strSiz; j++ )  {
    
    if( pos1[i] != '%' )  {
      
      pos2[j] = pos1[i++];
      
    }  else  {  // here not secure
      
      i++;
      temp2[0] = pos1[i++];
      temp2[1] = pos1[i++];
            
      temp = strtol( temp2, NULL, 16 );
      
      pos2[j] = ( char ) *( unsigned char* ) &temp;
      
    }
    
  }
  
  pos2[j] = 0;

}


void addspace( char *ptr )  {

  size_t nulpos = strlen( ptr );
  ptr[ nulpos ] = ' ';
  ptr[ nulpos + 1 ] = 0;

}

void decode_base64( char* word )  {
      
  char keyStr[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  
  size_t keySize = strlen( keyStr );
  size_t wordSize = strlen( word );

  if( ( wordSize % 4 ) || ( wordSize == 0 ) )  {
    
    return;
    
  }
  
  uint8_t* wordCode = malloc( wordSize );
  wordCode[ wordSize - 1 ] = 0; // if '=' than it will not find it thus we have zero
  wordCode[ wordSize - 2 ] = 0;
  
  for( size_t i = 0, j = 0; i < wordSize; i++ )  {
        
    for( j = 0; j < keySize; j++  )  {
      
      if( word[i] == keyStr[j] )  {
	
	wordCode[i] = j;
	break;
	
      }
      
    }
        
  }
  
  
  size_t tempPos = 0, moved = 0;
  uint32_t tempCode = 0, p_contain = 0;
  uint8_t p1 = 0, p2 = 0, p3 = 0, p4 = 0;
  for( tempPos = 0; tempPos < wordSize; )  {
        
    tempCode = 0;
    
    p1 = wordCode[ tempPos++ ];
    p2 = wordCode[ tempPos++ ];
    p3 = wordCode[ tempPos++ ];
    p4 = wordCode[ tempPos++ ];
    
    memcpy( &tempCode, &p1, 1 );    
    tempCode <<= 6;
    memcpy( &p_contain, &tempCode, 1 );
    
    memcpy( &tempCode, &p2, 1 );
    tempCode |= p_contain;
    tempCode <<= 6;
    memcpy( &p_contain, &tempCode, 1 );

    memcpy( &tempCode, &p3, 1 );
    tempCode |= p_contain;
    tempCode <<= 6;
    memcpy( &p_contain, &tempCode, 1 );
  
    memcpy( &tempCode, &p4, 1 );
    tempCode |= p_contain;
    
    memcpy( &word[ moved++ ], ( (char*) &tempCode ) + 2 , 1 );
    memcpy( &word[ moved++ ], ( (char*) &tempCode ) + 1 , 1 );
    memcpy( &word[ moved++ ], ( (char*) &tempCode ) + 0 , 1 );
    
  }
  
  word[ moved ] = 0;
  
  free( wordCode );
  
}

int remove_garbage( char *filename )  {

  int count_garbage = 0;
  char *temp_name = tmpnam( NULL );

  FILE *old = fopen( filename, "r" );
  FILE *new = fopen( temp_name, "w" ); 

  const size_t buff_size = 1024;
  char buff[ buff_size ];

  char arg1[ buff_size ];
  char arg2[ buff_size ];
  char arg3[ buff_size ];
  char arg4[ buff_size ];
  char arg5[ buff_size ];
  char arg6[ buff_size ]; // this should never get filled.


  while( fgets( buff, buff_size, old ) != NULL )  {

    if( sscanf( buff, "%s %s %s %s %s %s", arg1, arg2, arg3, arg4, arg5, arg6 ) != 5 )  {

      count_garbage++;
      continue;

    }

    fprintf( new, "%s", buff );

  } 

  if( ferror( old ) )  err();
  if( ferror( new ) )  err();

  fclose( old );
  fclose( new );

  remove( filename );
  if( cpyfile( temp_name, filename ) != 0 )  err();
  remove( temp_name );

  return count_garbage;

}


int remove_phrase( char *filename, char *phrase )  {

  int count_removed = 0;
  char *temp_name = tmpnam( NULL );

  FILE *old = fopen( filename, "r" );
  FILE *new = fopen( temp_name, "w" ); 

  size_t strlen_val = strlen( phrase );
  for( size_t i = 0; i < strlen_val; i++ )
    phrase[i] = tolower( phrase[i] );
  
  const size_t buff_size = 1024;
  char buff[ buff_size ];


  while( fgets( buff, buff_size, old ) != NULL )  {

    strlen_val = strlen( buff );
    for( size_t i = 0; i < strlen_val; i++ )
      buff[i] = tolower( buff[i] );
    
    if( strstr( buff, phrase ) != NULL )  {

      count_removed++;
      continue;

    }

    fprintf( new, "%s", buff );

  } 

  if( ferror( old ) )  err();
  if( ferror( new ) )  err();

  fclose( old );
  fclose( new );

  remove( filename );
  if( cpyfile( temp_name, filename ) != 0 )  err();
  remove( temp_name );

  return count_removed;

}




int remove_slowconnect( char *filename, uint32_t timeout )  {
  
  int count_removed = 0;
  char *temp_name = tmpnam( NULL ); 

  FILE *old = fopen( filename, "r" );
  FILE *new = fopen( temp_name, "w" ); 

  const size_t buff_size = 1024;
  char buff[ buff_size ];
  char ip[ buff_size ];
  uint16_t port;

  struct sockaddr_in sin;
  int checked_count = 0;

  while( fgets( buff, buff_size, old ) != NULL )  {
    
    checked_count++;
    sscanf( buff, "%s %hu", ip, &port );
    memset( &sin, 0, sizeof( sin ) );
    int sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    if( sockfd == -1 )  err();

    if( fill_addr4( &sin, ip, port ) )  { 

      count_removed++;
      close( sockfd );
      continue;

    }

    if( connect_timeout( sockfd, timeout, &sin ) )  {

      count_removed++;
      close( sockfd );
      continue;      

    }

    fprintf( new, "%s", buff );
    close( sockfd );

    printf( "Checked: %d  disposed: %d          \r", checked_count, count_removed );
    fflush( stdout );

  }

  printf( "                                                      " );

  if( ferror( old ) )  err();
  if( ferror( new ) )  err();

  fclose( old );
  fclose( new );

  remove( filename );
  cpyfile( temp_name, filename );
  remove( temp_name );
  return count_removed;

}

int real_count( char* filename )  {

  int count = 0;
  FILE *cf = fopen( filename, "r" );
  if( cf == NULL )  return -1;

  const size_t bsize = 50000;
  char buff[ bsize ];

  while( fgets( buff, bsize, cf ) != NULL )  {

    count++;

  } 

  if( ferror( cf ) )  return -1;
  fclose( cf );
  return count;

}

int removeSame( char* fileName )  {
  
  int removed = 0;
  char *newFileName = tmpnam( NULL );
  
  FILE *file = NULL, *fileNew = NULL;
  if( ( file = fopen( fileName, "r" ) ) == NULL )   err();
    
  char **charTab = NULL;
  size_t charTabElem = 0;
  
  char temp[10000];
  
  for(; fgets( temp, 10000, file) != NULL ;)  {
    
    if( ( charTab = realloc( charTab, sizeof( char* ) * ( 1 + charTabElem ) ) ) == NULL )   err();
    if( ( charTab[charTabElem] = malloc( strlen( temp ) + 1 ) ) == NULL )   err();
    
    strcpy( charTab[ charTabElem ], temp );
    
    charTabElem++;
    
  }
  
  if( fclose( file ) < 0 )  err();
  
  char addrs1[200], addrs2[200];
  char port1[200], port2[200];
  
  int f_repeated = 0;
  if( ( fileNew = fopen( newFileName, "w+" ) ) == NULL )   err();
  for( size_t i = 0; i < charTabElem; i++ )  {
    
    f_repeated = 0;
    
    sscanf( charTab[i], "%s%s", addrs1, port1 );
    
    for( size_t j = i + 1; j < charTabElem; j++ )  {
      
      sscanf( charTab[j], "%s%s", addrs2, port2 );
      
      if( ! strcmp( addrs1, addrs2 ) )  {
	
	if( ! strcmp( port1, port2 ) )  {
	  
	  removed++;
	  f_repeated = 1;
	  break; 
	  
	}
	
      }
      
    }
    
    
    if( f_repeated )  {
    
      free( charTab[i] );
      continue;
    
    }
     
    fprintf( fileNew, "%s", charTab[i] );
    free( charTab[i] );
    
  }
  
  free( charTab );
  
  if( fclose( fileNew ) < 0 )  err();
  remove( fileName );
  if( cpyfile( newFileName, fileName ) < 0 )   err();
  remove( newFileName );
  return removed;
  
}


int checkInternet( const char *website  )  {
  
  char fullAddr[5000] = {0};
  if( strlen( website ) <= strlen("http://") )  {
    
    printf( "too short addrs: %s", website );
    err();
    
  }
  
  strcpy( fullAddr, website + strlen("http://") );
  
  
  char domainName[5000] = {0};
  strcpy( domainName, fullAddr );
    
  for( unsigned int i = 0; i < sizeof( domainName ); i++ )  {
    
    if( domainName[i] == '/' )  {
      
      domainName[i] = 0; // end string
      break;
      
    }
   
  }

  struct addrinfo ai_hint, *ai = NULL;
  memset( &ai_hint, 0, sizeof( ai_hint ) );
  ai_hint.ai_flags |= AI_CANONNAME;
  ai_hint.ai_socktype |= SOCK_STREAM;
  
  int ai_err = 0;
  if( ( ai_err = getaddrinfo( domainName, "http", &ai_hint, &ai ) ) < 0 )   return -1;

  
  struct sockaddr_storage sa_stor;
  memset( &sa_stor, 0, sizeof( sa_stor ) );
  socklen_t sa_len = ai->ai_addrlen;
  memcpy( &sa_stor, ai->ai_addr, sa_len );
  int s_family = ai->ai_family;
  
  freeaddrinfo( ai );
  
  int sockfd = 0;
  if( ( sockfd = socket( s_family, SOCK_STREAM, 0 ) ) < 0 )   err();
  
  if( connect( sockfd, ( struct sockaddr* ) ( &sa_stor ), sa_len ) < 0 )   return -1;
  
  shutdown( sockfd, SHUT_RDWR );
  return 0;
        
}


char find_buff[ BUFFSIZE ];
char temp_buff[ BUFFSIZE ];
char* getline_rmv( const char* phrase, const char* FileName )  {
 
  FILE* searched = NULL;
  FILE* new = NULL;
  if( ( searched = fopen( FileName, "r" ) ) == NULL )  err();
  const char *const tempname = ( const char *const )tmpnam( NULL );
  if( ( new = fopen( tempname, "w" ) ) == NULL )  err();
  while( fgets( find_buff, BUFFSIZE, searched ) != NULL )  {
    
    if( strstr( find_buff, phrase ) != NULL )  {
         
      while( fgets( temp_buff, BUFFSIZE, searched ) != NULL )
	fprintf( new, "%s", temp_buff );
      
      if( ferror( searched ) )  err();

      fclose( searched );
      fclose( new );      
      
      remove( FileName );
      if( cpyfile( tempname, FileName ) != 0 )  err();
      if( remove( tempname ) < 0 ) err();

      return find_buff;

    }
    
  }
  
  if( ferror( searched ) )  err();
  
  fclose( searched );
  fclose( new );
  remove( tempname );
  return( NULL );
  
}


// line is lost.
char* find_between( char *line, char *phrase, char *end )  {

  char *pos1 = strstr( line, phrase );
  if( pos1 == NULL )  return NULL;

  pos1 += strlen( phrase );  
  char *pos2 = strstr( pos1, end );
  if( pos2 == NULL )  return NULL;

  *pos2 = 0;
  return pos1;
  
}

char *movafter( char **line, char *phrase )  {

  char *newpos = strstr( *line, phrase );
  if( newpos == NULL )  return *line = NULL;
  
  *line = newpos + strlen( phrase );
  return *line;

}

char* find_between_mov( char **line, char *phrase, char *end,
			char *const buff, const size_t buff_size )  {

  if( movafter( line, phrase ) == NULL )  return NULL;
  char *pos1 = *line;
  
  if( movafter( line, end ) == NULL )  return NULL;
  char *pos2 = strstr( pos1, end );
  
  char keep_char = *pos2;
  *pos2 = 0;
  strncpy( buff, pos1, buff_size );
  *pos2 = keep_char;
  return buff;
  
}

void spacetounderscore( char *words )  {

  size_t len = strlen( words );
  for( size_t i = 0; i < len; i++ )  {

    if( isspace( words[i] ) )  words[i] = '_';

  }

}

// buff we will use
char bigline[BUFFSIZE];
char ans[BUFFSIZE];
int main( int argc, char *argv[] )  {
  
  puts("Start...");
  
  for(;;)  {
    
    if( ! checkInternet( "http://duckduckgo.com/" ) ) break;
    sleep( 60 );
    
  }

  puts("Seems there is connection...");
  
  FILE *proxylist = NULL;
  
  remove( "proxylist" );
  if( ( proxylist = fopen( "proxylist", "a" ) ) == NULL )   err(); 

  char *ptr;
  int count, count_all = 0;
  const size_t tempbuff_size = 1024;
  char tempbuff[ tempbuff_size ];
  char port[ tempbuff_size ];
  char ip[ tempbuff_size ];
    
  //////////////////////////////////////////////////////////////////////

  char tempfile_1[8192];
  char tempfile_2[8192];
  if( tmpnam( tempfile_1 ) == NULL )  err(); 
  if( tmpnam( tempfile_2 ) == NULL )  err(); 

  count = 0;
  
  sprintf( bigline, "wget -q -O %s 'https://api.proxyscrape.com/?request=getproxies&"
		    "proxytype=http&timeout=40000&country=all&"
		    "ssl=all&anonymity=all'" , tempfile_1 );

  system( bigline );
  
  sprintf( bigline, "cat '%s' | tr -d '\\r' > '%s'", tempfile_1, tempfile_2 );
  system( bigline );

  FILE *biglist = fopen( tempfile_2, "r" );
  if( biglist == NULL )  err();
  #define LINESIZ 8192
  #define LINESIZ_SCAN "8191"
  for( char ip_scan[ LINESIZ ] = {0} , port_scan[ LINESIZ ] = {0};; )  {

    int scan_ret = fscanf( biglist, "%" LINESIZ_SCAN "[^:]%" LINESIZ_SCAN "[^\n]",
      ip_scan, port_scan );
    getc( biglist );

    if( scan_ret == EOF )  break;
    if( scan_ret != 2 )  continue;

    count++;
    fprintf( proxylist, "%s %s unknown unknown unknown\n" , ip_scan, &port_scan[1] );
  }
  
  fclose( biglist );
  remove( tempfile_2 );
  printf( "Taken %d proxies from proxy scrape site.\n", count );

  ////////////////////////////////////////////////////////////////////////// 
  
  count_all += count;
  count = 0;
  sprintf( bigline, "wget http://proxysearcher.sourceforge.net/Proxy%%20List.php?type=http\\&filtered=true -q -O %s", tempfile_1 );
  system( bigline );

  ptr = getline_rmv( "<tr><td>1</td><td>", tempfile_1 );

  for(; ptr;)  {

    ans[0] = 0;

    // pass ip count
    find_between_mov( &ptr, "<tr><td", ">", tempbuff, tempbuff_size );
    if( ptr == NULL )  break;

    // ip & port
    find_between_mov( &ptr, "/td><td>", "<", tempbuff, tempbuff_size );
    if( ptr == NULL )  break;
    for( size_t i = 0; i < strlen( tempbuff ); i++ )  {
      
      if( tempbuff[i] == ':' )  {

	tempbuff[i] = ' ';
	break;

      }
      
    }
    
    sscanf( tempbuff, "%s %s", ip, port );
    sprintf( ans, "%s %s Unknown ", ip, port );

    // anon type
    find_between_mov( &ptr, "/td><td>", "<", tempbuff, tempbuff_size );
    if( ptr == NULL )  break;
    if( tolower( tempbuff[0] ) == 't' )  strcat( ans, "Open");
    else  strcat( ans, tempbuff );

    fprintf( proxylist, "%s Http\n", ans );
    count++;

  }

  printf( "Taken %d proxies from proxy search site.\n", count );

  ///////////////////////////////////////////////////////////////////

  count_all += count;
  count = 0;
  sprintf( bigline, "wget https://free-proxy-list.net/ -q -O %s", tempfile_1 );
  system( bigline );

  ptr = getline_rmv( "</thead><tbody", tempfile_1 );
  find_between_mov( &ptr, "</thead><tbody", ">", tempbuff, tempbuff_size );

  for( char *tempptr; ptr;)  {

    ans[0] = 0;

    // ip
    tempptr = find_between_mov( &ptr, "<tr><td>", "<", tempbuff, tempbuff_size );
    if( ptr == NULL )  break;
    addspace( tempptr );
    strcat( ans, tempptr );

    // port
    tempptr = find_between_mov( &ptr, "/td><td>", "<", tempbuff, tempbuff_size );
    if( ptr == NULL )  break;
    addspace( tempptr );
    strcat( ans, tempptr );

    // pass stuff
    tempptr = find_between_mov( &ptr, "/td><td class", "=", tempbuff, tempbuff_size );
    if( ptr == NULL )  break;
    // country
    tempptr = find_between_mov( &ptr, ">", "<", tempbuff, tempbuff_size );
    if( ptr == NULL )  break;
    spacetounderscore( tempptr );
    addspace( tempptr );
    strcat( ans, tempptr );

    // anon type
    tempptr = find_between_mov( &ptr, "/td><td>", "<", tempbuff, tempbuff_size );
    if( ptr == NULL )  break;
    addspace( tempptr );
    if( tolower( tempbuff[0] ) == 'e' )  strcat( ans, "HighAnonymus ");
    else strcat( ans, tempptr );

    // pass stuff
    tempptr = find_between_mov( &ptr, "/td><td class", "=", tempbuff, tempbuff_size );
    if( ptr == NULL )  break;
    // pass stuff
    tempptr = find_between_mov( &ptr, "/td><td class", "=", tempbuff, tempbuff_size );
    if( ptr == NULL )  break;
    // http type
    tempptr = find_between_mov( &ptr, ">", "<", tempbuff, tempbuff_size );
    if( ptr == NULL )  break;
    if( tolower( tempbuff[0] ) == 'y' )  strcat( ans, "Https" );
    else  strcat( ans, "Http" );

    fprintf( proxylist, "%s\n", ans );
    count++;

  }

  printf( "Taken %d proxies from free proxy list site.\n", count );
 
  count_all += count;
  count = 0;
  sprintf( bigline, "wget https://sslproxies.org/ -q -O %s", tempfile_1 );
  system( bigline );

  ptr = getline_rmv( "</thead><tbody", tempfile_1 );
  find_between_mov( &ptr, "</thead><tbody", ">", tempbuff, tempbuff_size );

  for( char *tempptr; ptr;)  {

    ans[0] = 0;

    // ip
    tempptr = find_between_mov( &ptr, "<tr><td>", "<", tempbuff, tempbuff_size );
    if( ptr == NULL )  break;
    addspace( tempptr );
    strcat( ans, tempptr );

    // port
    tempptr = find_between_mov( &ptr, "/td><td>", "<", tempbuff, tempbuff_size );
    if( ptr == NULL )  break;
    addspace( tempptr );
    strcat( ans, tempptr );

    // pass stuff
    tempptr = find_between_mov( &ptr, "/td><td class", "=", tempbuff, tempbuff_size );
    if( ptr == NULL )  break;
    // country
    tempptr = find_between_mov( &ptr, ">", "<", tempbuff, tempbuff_size );
    if( ptr == NULL )  break;
    spacetounderscore( tempptr );
    addspace( tempptr );
    strcat( ans, tempptr );

    // anon type
    tempptr = find_between_mov( &ptr, "/td><td>", "<", tempbuff, tempbuff_size );
    if( ptr == NULL )  break;
    addspace( tempptr );
    if( tolower( tempbuff[0] ) == 'e' )  strcat( ans, "HighAnonymus ");
    else strcat( ans, tempptr );

    // pass stuff
    tempptr = find_between_mov( &ptr, "/td><td class", "=", tempbuff, tempbuff_size );
    if( ptr == NULL )  break;
    // pass stuff
    tempptr = find_between_mov( &ptr, "/td><td class", "=", tempbuff, tempbuff_size );
    if( ptr == NULL )  break;
    // http type
    tempptr = find_between_mov( &ptr, ">", "<", tempbuff, tempbuff_size );
    if( ptr == NULL )  break;
    if( tolower( tempbuff[0] ) == 'y' )  strcat( ans, "Https" );
    else  strcat( ans, "Http" );

    fprintf( proxylist, "%s\n", ans );
    count++;

  }

  printf( "Taken %d proxies from ssl proxies site.\n", count );
 
  count_all += count;
  count = 0;
  sprintf( bigline, "wget https://us-proxy.org/ -q -O %s", tempfile_1 );
  system( bigline );

  ptr = getline_rmv( "</thead>", tempfile_1 );
  find_between_mov( &ptr, "</thead><tbody", ">", tempbuff, tempbuff_size );

  for( char *tempptr; ptr;)  {

    ans[0] = 0;

    // ip
    tempptr = find_between_mov( &ptr, "<tr><td>", "<", tempbuff, tempbuff_size );
    if( ptr == NULL )  break;
    addspace( tempptr );
    strcat( ans, tempptr );


    // port
    tempptr = find_between_mov( &ptr, "/td><td>", "<", tempbuff, tempbuff_size );
    if( ptr == NULL )  break;
    addspace( tempptr );
    strcat( ans, tempptr );

    // pass stuff
    tempptr = find_between_mov( &ptr, "/td><td class", "=", tempbuff, tempbuff_size );
    if( ptr == NULL )  break;
    // country
    tempptr = find_between_mov( &ptr, ">", "<", tempbuff, tempbuff_size );
    if( ptr == NULL )  break;
    spacetounderscore( tempptr );
    addspace( tempptr );
    strcat( ans, tempptr );

    // anon type
    tempptr = find_between_mov( &ptr, "/td><td>", "<", tempbuff, tempbuff_size );
    if( ptr == NULL )  break;
    addspace( tempptr );
    if( tolower( tempbuff[0] ) == 'e' )  strcat( ans, "HighAnonymus ");
    else strcat( ans, tempptr );

    // pass stuff
    tempptr = find_between_mov( &ptr, "/td><td class", "=", tempbuff, tempbuff_size );
    if( ptr == NULL )  break;
    // pass stuff
    tempptr = find_between_mov( &ptr, "/td><td class", "=", tempbuff, tempbuff_size );
    if( ptr == NULL )  break;
    // http type
    tempptr = find_between_mov( &ptr, ">", "<", tempbuff, tempbuff_size );
    if( ptr == NULL )  break;
    if( tolower( tempbuff[0] ) == 'y' )  strcat( ans, "Https" );
    else  strcat( ans, "Http" );


    fprintf( proxylist, "%s\n", ans );
    count++;

  }

  printf( "Taken %d proxies from us proxies site.\n", count );

  fclose( proxylist );
  count_all += count;
  printf( "\n\t***\tSuming up, loaded %d proxies\t***\n\n", count_all );

  count_all = real_count( "proxylist" );
  if( count_all < 0 )  err();

  printf( "\nThe proxy file contains now %d proxies\n\n", count_all );

  int removed = 0;
  for( int i = 1; i < argc; i++ )  {

    printf( "Removing lines with: \"%s\" phrase\n", argv[i] );
    removed += remove_phrase( "proxylist", argv[i] );

  }

  if( removed )  {
   
    count_all -= removed;
    printf( "\n\t***\tAfter removing lines there were left %d proxies\t***\n\n", count_all );

  }

  removed = removeSame( "proxylist" );
  count_all -= removed;
  printf( "\n\t***\tAfter removing repeated there were left %d proxies\t***\n\n", count_all );

  removed = remove_garbage( "proxylist" );
  count_all -= removed;
  printf( "\n\t***\tAfter removing garbage there were left %d proxies\t***\n\n", count_all );

  puts( "Removing dead / too slow proxies" );
  removed = remove_slowconnect( "proxylist", 333333 );
  count_all -= removed;
  printf( "\n\t***\tAfter removing slow ones there were left %d proxies\t***\n\n", count_all );

  remove( tempfile_1 );
  
  return 0;

}
