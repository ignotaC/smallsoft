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

#include <sys/time.h>

#include <fcntl.h>
#include <unistd.h>

#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BUFF_SIZE 8192
#define MIN_MOUSEMOVE_TIMEDIFF 10
#define NUM_TO_STR(x) #x

#define SMALL_IN_LEN 100
#define SMALL_IN_SIZ 150 

#define KEY_PRESS 2
#define KEY_RELEASE 3
#define BUTTON_PRESS 15
#define BUTTON_RELEASE 16
#define MOTION 6

#ifdef _POSIX_C_SOURCE
  #define IMPLEMENT_PURGE
#endif

#ifndef __unix__
  #define IMPLEMENT_PURGE
#endif

#ifdef IMPLEMENT_PURGE

  int fpurge( FILE *stream )  {

    char *pbuff[ BUFF_SIZE ];

    int streamfd = fileno( stream );
    int keepflag = fcntl( streamfd, F_GETFL );
    if( keepflag == -1 )  return EOF;

    if( fcntl( streamfd, F_SETFL, keepflag | O_NONBLOCK ) == -1 )
      return EOF;

    ssize_t readret = 0;
    for(;;)  {

      readret = read( streamfd, pbuff, BUFF_SIZE );
      if( readret == -1 )  {

	switch( errno )  {

          case EINTR:
	    continue;
	  
         #ifdef EAGAIN
	  case EAGAIN:
	    break;
	 #else
          case EWOULDBLOCK:
            break;
	 #endif

	  default:
	    return EOF;

	}
        if( errno != EINTR )  return EOF;
	continue;

      }
      else if( readret == 0 )  break;

    }

    if( fcntl( streamfd, F_SETFL, keepflag ) == -1 )
      return EOF;

    return 0;

  }

#endif

// macro to remove compiler  *dupicate case* rambling
#if( EWOULDBLOCK != EAGAIN )

  #define CASE_EBLOCK \
    case EAGAIN: \
    case EWOULDBLOCK:

#else

  #define CASE_EBLOCK \
    case EAGAIN:

#endif



char buff[ BUFF_SIZE ];
char sec_buff[ BUFF_SIZE ];

void fail( char *str_error )  {

  perror( str_error );
  exit( EXIT_FAILURE );

}

int clearfile( FILE *file_to_clear )  {
 
  int std_fd = fileno( file_to_clear );

  // set stdin as nonblocking.
  int flag = fcntl( std_fd, F_GETFL );
  if( flag == -1 )  fail( "Fail on fcntl get flag" );
  if( fcntl( std_fd, F_SETFL, flag | O_NONBLOCK ) == -1 )
    fail( "Fail on fcntl set flag" );

  for(;;)  {

    ssize_t ret = read( std_fd, buff, 1 );
    if( ret == -1 )  {

      switch( errno )  {

        case EINTR:  continue;
	CASE_EBLOCK
	errno = 0;
        ret = 0;  // we will spoof it's eof so it leaves loop 
	break;
	default:  return -1;

      }

    }
    if( ret == 0 )  break;

  }

  getchar(), getchar(), getchar(); // sometimes one lone char will prevail somewhere.
  if( fcntl( std_fd, F_SETFL, flag ) == -1 )  return -1;
  if( fpurge( file_to_clear ) != 0 )  return -1; // additional atempt to clear stuff
  return 0;

}


int clearstdin( void )  {

  return clearfile( stdin );

}
  

int findkey( FILE *shfile )  {

  char *line = NULL;
  size_t linelen = 0;
  while( getline( &line, &linelen, shfile ) != -1 )  {

    if( strstr( line, buff ) != NULL )  {

      puts( "Entry from xmodmap:" );
      printf( "%s", line );
      int key;
      if( sscanf( line, "%d", &key ) != 1 )  {

        free( line );
	errno = EINVAL;
	return -1;

      } 

      free( line );
      return key;

    }

    free( line );
    line = NULL;
    linelen = 0;

  }

  if( ferror( shfile ) )  clearerr( shfile );
  return -1;

}

int sleep_msec( long long int msec )  {

  struct timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = msec * 1000000;
  return nanosleep( &ts, NULL );

}

int choose_key( void )  {
  
  errno = 0; // if errno 0 and -1 we can relook for key
  system ( "clear" );
  puts( "Type in key name that will be used to stop recording events" );
  puts( "Example key names: Escape, h, O, 0, plus, F8, etc." );
  puts( "You can find them with xev program" );
  puts( "Note you can't use big letters, shift + something, vide any key combinations." );
  if( clearstdin() == -1 )  return -1;
  if( snprintf( sec_buff, SMALL_IN_SIZ, "%%%ds", SMALL_IN_SIZ ) < 0 )
    return -1;
  if( scanf( sec_buff, buff ) != 1 )  return -1;
  if( sprintf( sec_buff, "(%.*s)", ( int ) SMALL_IN_SIZ, buff ) < 0 ) // tu jest błąd
    return -1;
  strncpy( buff, sec_buff, SMALL_IN_SIZ );
  
  puts( "\nLooking up your key" );
  FILE* shfile = popen( "xmodmap -pk", "r" );
  if( shfile == NULL )  return -1;

  int key = findkey( shfile );
  pclose( shfile );
  return key;

}

int ask_isok( void )  {

  puts( "Are you fine to continue or want to restart step?" );
  puts( "y/Y for YES anything other is treated as NO!" );
  if( clearstdin() == -1 )  return -1;
  int ans = getchar();
  if( tolower( ans ) == 'y' )  return 1;
  return 0;

}

int lf_to_nul( char *str )  {

  while( *str != '\0' )  {

    if( *str == '\n' )  {

      *str = '\0';
      return 0;

    }

    str++;

  }

  return -1;

}

char name_buff[ SMALL_IN_SIZ ];
char *get_name( void )  {

  printf( "Enter name not longer than %d:\n", SMALL_IN_LEN - 1 );
  if( clearstdin() == -1 )  return NULL;
  if( fgets( name_buff, SMALL_IN_LEN, stdin ) == NULL )  {

    if( ferror( stdin ) )  {

      clearerr( stdin );
      return NULL;

    }

    errno = 0;  // this should not happen
    return NULL;

  }

  if( lf_to_nul( name_buff ) == -1 )  puts( "Your long name has been cuta to fit size" );
  return name_buff;

}


void fgets_rec( char *buff, size_t buff_size, FILE *file_rec ) {

  if( fgets( buff, buff_size, file_rec ) == NULL )  {

    if( feof( file_rec ) )  errno = 0;
    fail( "There is fail on fgets_rec" );

  }

}


char* fgets_rec2( char *buff, size_t buff_size, FILE *file_rec ) {

  if( fgets( buff, buff_size, file_rec ) == NULL )  {

    if( feof( file_rec ) )  {

      clearerr( stdin );
      return NULL;

    }
    fail( "There is fail on fgets_rec2" );

  }

  return buff;

}



void mv_toalp( char **strpos )  {

  while( ! isspace( **strpos ) )  {

    if( **strpos == '\0' )  return;	  
    ( *strpos )++;

  }
  while( isspace( **strpos ) )  {

    if( **strpos == '\0' )  return;
    ( *strpos )++;

  }

}

int check_ev_num( int ev_num )  {

  switch( ev_num )  {

    case KEY_PRESS:
    case KEY_RELEASE:
    case BUTTON_PRESS:
    case BUTTON_RELEASE:
    case MOTION:
      return 1;
    default:
      return 0;

  }

}

// get us ptr to searched value
char *readf_tillstr( char *buff, size_t buff_size, FILE *ftoread, char *str_stop )  {

  for(;;)  {
 
    if( fgets_rec2( buff, buff_size, ftoread ) == NULL )
      return NULL;
    char *retptr = strstr( buff, str_stop );
    if( retptr != NULL )  return retptr;

  }

}


FILE *rmvlines_str( FILE* oldfile, char *str_todel )  {

  FILE *newfile = tmpfile();
  if( newfile == NULL )  goto failure;
 
  errno = 0;
  rewind( oldfile );
  if( errno )  goto failure;
 
  while( fgets( buff, BUFF_SIZE, oldfile ) != NULL )  {

    if( strstr( buff, str_todel ) != NULL )  continue;
    if( fprintf( newfile, "%s", buff ) < 0 )  goto failure;

  }

  if( ferror( oldfile ) )  goto failure;
  fclose( oldfile );
  return newfile;

 failure:
  fclose( oldfile );
  fclose( newfile );
  return NULL;

}

FILE *rmvmouse_sametime( FILE *oldfile )  {

  FILE *newfile = tmpfile();
  if( newfile == NULL )  goto failure;
 
  errno = 0;
  rewind( oldfile );
  if( errno )  goto failure;
  
  char mv_buff[ BUFF_SIZE ];
  long long int time_new = 0;
  long long int time_old = 0;
  long long int time_dif = 0;

  if( fgets( buff, BUFF_SIZE, oldfile ) == NULL )
    if( ferror( oldfile ) )  goto failure;

  if( sscanf( buff, "%lld", &time_new ) != 1 )  goto failure;
  strncpy( sec_buff, buff, BUFF_SIZE - 1 );
  time_old = time_new;
  
  int start = 1;
  while( fgets( buff, BUFF_SIZE, oldfile ) != NULL )  {

    time_dif = time_new - time_old;
    time_old = time_new;
    if( sscanf( buff, "%lld", &time_new ) != 1 )  goto failure;

    if( start )  {

      start = 0;
      goto finish;

    }

    if( strstr( sec_buff, "mousemove" ) == NULL )
      goto finish;

    if( strstr( buff, "mousemove" ) == NULL )
      goto finish;

    if( time_dif < MIN_MOUSEMOVE_TIMEDIFF )
      goto noprint;

   finish:  ;// goto calm down
    char *bptr = sec_buff;
    while( ! isspace( *bptr ) )  {

      if( *bptr == '\0' )  goto failure;
      bptr++;

    }
    
    if( snprintf( mv_buff, BUFF_SIZE, "%lld%s", time_dif, bptr ) < 0 )
      goto failure;
    if( fprintf( newfile, "%s", mv_buff ) < 0 )  goto failure;
   noprint:
    strncpy( sec_buff, buff, BUFF_SIZE - 1 );

  }
 
  time_dif = time_new - time_old;
  char *bptr = sec_buff; 
  while( ! isspace( *bptr ) )  {

    if( *bptr == '\0' )  goto failure;
    bptr++;

  }
    
  if( snprintf( mv_buff, BUFF_SIZE, "%lld%s", time_dif, bptr ) < 0 )
    goto failure;
  if( fprintf( newfile, "%s", mv_buff ) < 0 )  goto failure;
 

  if( ferror( oldfile ) )  goto failure;
  fclose( oldfile );
  return newfile;

 failure:
  fclose( oldfile );
  fclose( newfile );
  return NULL;

}

int save_recev( FILE *oldfile, char *filename )  {
 
  FILE *proper_file = fopen( filename, "w" );
  if( proper_file == NULL )  goto failure;

  errno = 0;
  rewind( oldfile );
  if( errno )  goto failure;

  while( fgets( buff, BUFF_SIZE, oldfile ) )  {
  
    if( fprintf( proper_file, "%s", buff ) < 0 )
      goto failure;

  }  

  if( ferror( oldfile ) )  goto failure;
  fclose( oldfile );
  fclose( proper_file );
  return 0;

 failure:
  fclose( oldfile );
  fclose( proper_file );
  return -1;

}

// It uses logtime program from small soft, xinput and xmodmap
// Out put is in format timestamp + xdotool 
int main( void )  {

  // We must do it so the cleanstdin function work properly.
  setvbuf( stdin, NULL, _IONBF, 0 );

  puts( "Hello, to record events" \
        " you must choose key for start/stop recording" );
  puts( "For now press any key when you are ready to continue" );
  getchar();

  int key = -1;
  for(;;)  {

    key = choose_key();

    if( key == -1 )  {

      if( errno != 0 )  fail( "Failed on choose_key function" );
      key = choose_key();
      continue;

    }

    int ans = ask_isok();
    if( ans < 0 )  fail( "Failed on ask_isok - key part" );
    if( ans > 0 )  break;

  }

  char *rec_fname = NULL;
  for(;;)  {

    system( "clear" );
    puts( "Enter File name for keeping recorded events data" );
    rec_fname = get_name();
    if( rec_fname == NULL )  fail( "Failed on getting rec_fname" );
    int ans = ask_isok();
    if( ans < 0 )  fail( "Failed on ask_isok - file name part" );
    if( ans > 0 )  break;

  }

  FILE *fevent = tmpfile();
  if( fevent == NULL )  fail( "Failed on fevent - temp file open" );

  system( "clear" );
  puts( "Press any key when ready to run event scaner" );
  if( clearstdin() == -1 )  fail( "Failed to clear stdin" );
  getchar();
  puts( "Started scanning events" );
  puts( "Use your key of choice to start and stop recording" );

  char *helppos = NULL;
  int detail_val = 0;
  int record = 0;
  FILE *frec = popen( "xinput --test-xi2 --root", "r" );
  if( frec == NULL )  fail( "Failed on frec popen" );

  char detail_str[] = "detail: ";
  size_t detail_len = strlen( detail_str );
  while( ! record )  {

    fgets_rec( buff, BUFF_SIZE, frec );
    if( ( helppos = strstr( buff, "EVENT" ) )  == NULL )  continue;
    if( strstr( helppos, "KeyPress" ) == NULL )  continue;
    fgets_rec( buff, BUFF_SIZE, frec );  // skip device line info
    fgets_rec( buff, BUFF_SIZE, frec );
    helppos = strstr( buff, detail_str );
    if( helppos == NULL )  fail( "Can't find detail string" );
    helppos += detail_len;
    if( sscanf( helppos, "%d", &detail_val ) != 1 )
      fail( "Can't obtain detail value" );
    if( key == detail_val )  record = 1;

  } // don't do anything untill we start recording
 
  pclose( frec );
  frec = popen( "xinput --test-xi2 --root", "r" );
  if( frec == NULL )  fail( "Failed on frec popen" );

  puts( "Started recording events" );

  // there is realy lots of data floating around so we will simply
  // write it and parse after recording is finished.
  struct timeval tcont;
  struct timeval *tcont_p = &tcont;
  long long int ts_val;

  while( record )  {
    
    fgets_rec( buff, BUFF_SIZE, frec );
    if( ( helppos = strstr( buff, "EVENT" ) )  == NULL )  goto save;
    if( strstr( helppos, "KeyPress" ) == NULL )  goto save;

    //we still must save it.
    gettimeofday( tcont_p, NULL );
    ts_val = ( long long int ) tcont.tv_sec * 1000 + tcont.tv_usec / 1000;
    if( fprintf( fevent, "%lld %s", ts_val, buff ) < 0 )
      fail( "Failed on file write" );

    // passing line and saving it.
    fgets_rec( buff, BUFF_SIZE, frec );  // skip device line info
    gettimeofday( tcont_p, NULL );
    ts_val = ( long long int ) tcont.tv_sec * 1000 + tcont.tv_usec / 1000;
    if( fprintf( fevent, "%lld %s", ts_val, buff ) < 0 )
      fail( "Failed on file write" );

    // again pass line but this will get save down below.
    fgets_rec( buff, BUFF_SIZE, frec ); 
    helppos = strstr( buff, detail_str );
    if( helppos == NULL )  fail( "Can't find detail string" );
    helppos += detail_len;
    if( sscanf( helppos, "%d", &detail_val ) != 1 )
      fail( "Can't obtain detail value" );
    if( key == detail_val )  record = 0;

   save:
    gettimeofday( tcont_p, NULL );
    ts_val = ( long long int ) tcont.tv_sec * 1000 + tcont.tv_usec / 1000;
    if( fprintf( fevent, "%lld %s", ts_val, buff ) < 0 )
      fail( "Failed on file write" );

  }

  puts( "End of event recording" );
  puts( "Processing scanned events, this might take few sec" );
  pclose( frec );
  fflush( fevent );
  
  //we need to process our file.
  errno = 0;
  clearerr( stdin );
  rewind( fevent );
  if( errno )  fail( "failed on rewin" );
  FILE *fevent_proc = tmpfile();
  if( fevent_proc == NULL )  fail( "Failed to open new tempfile for processed fevent" );

  for(;;)  {
 
    if( fgets_rec2( buff, BUFF_SIZE, fevent ) == NULL )  break;
    if( ( helppos = strstr( buff, "EVENT" ) )  == NULL )  continue;
    
    // this stays same since we are looking for specifyic
    mv_toalp( &helppos );  // pass EVENT
    mv_toalp( &helppos );  // pass type and here we have number
    
    // get number defining event type
    int ev_num = -1;
    if( sscanf( helppos, "%d", &ev_num ) != 1 )  fail( "scanf failure" );
    if( check_ev_num( ev_num ) == 0 )  continue;
    // If we are interested in this event we continue

    // get time stamp
    long long int timestamp;
    if( sscanf( buff, "%lld", &timestamp ) != 1 )  fail( "scanf failure" );

    char *chosenstr = NULL;
    switch( ev_num )  {

      case KEY_PRESS:
      case KEY_RELEASE:
      case BUTTON_PRESS:
      case BUTTON_RELEASE:
        chosenstr = "detail:";
	break;
      case MOTION:
        chosenstr = "event:";
	break;
      default:
	fail( "Case failure, should not happen" );
 
    }
    
    // move to line with chosenstr and point it
    helppos = readf_tillstr( buff, BUFF_SIZE, fevent, chosenstr );
    if( helppos == NULL )  break;
    mv_toalp( &helppos ); // pass chosenstr
    if( sscanf( helppos, "%s", sec_buff ) != 1 )  return 0;
    
    // at this point we have all data.we need for xdotools
    switch( ev_num )  {
      
      case KEY_PRESS:
        if( fprintf( fevent_proc, "%lld xdotool keydown %s\n", timestamp, sec_buff ) < 0 )
          fail( "Failed on fevent_proc fprintf" );
        break;
      case KEY_RELEASE:
        if( fprintf( fevent_proc, "%lld xdotool keyup %s\n", timestamp, sec_buff ) < 0 )
          fail( "Failed on fevent_proc fprintf" );
        break;
      case BUTTON_PRESS:
        if( fprintf( fevent_proc, "%lld xdotool mousedown %s\n", timestamp, sec_buff ) < 0 )
          fail( "Failed on fevent_proc fprintf" );
        break;
      case BUTTON_RELEASE:
        if( fprintf( fevent_proc, "%lld xdotool mouseup %s\n", timestamp, sec_buff ) < 0 )
          fail( "Failed on fevent_proc fprintf" );
        break;
      case MOTION:
	helppos = strchr( sec_buff, '/' );
	if( helppos == NULL )  fail( "Fail on split mousemove cords" );
	*helppos = ' '; // split the cords
        if( fprintf( fevent_proc, "%lld xdotool mousemove %s\n", timestamp, sec_buff ) < 0 )
          fail( "Failed on fevent_proc fprintf" );
        break;
      default:
	fail( "Case failure, should not happen" );

    }
    
  }

  // we don't need this anymore
  fclose( fevent );

  if( sprintf( sec_buff, "keydown %d", key ) < 0 )  fail( "Failed on sprintf" );
  fevent_proc = rmvlines_str( fevent_proc, sec_buff );
  if( fevent_proc == NULL )  fail( "Failed at removing special key being down" );

  if( sprintf( sec_buff, "keyup %d", key ) < 0 )  fail( "Failed on sprintf" );
  fevent_proc = rmvlines_str( fevent_proc, sec_buff );
  if( fevent_proc == NULL )  fail( "Failed at removing special key being up" );
 
  fevent_proc = rmvmouse_sametime( fevent_proc );
  if( fevent_proc == NULL )  fail( "Failed at removing mouse same time lines" );

  // this will close both files after simply coping stuff to file with name user pointed
  if( save_recev( fevent_proc, name_buff ) == -1 )
    fail( "Failed at saving our record" );

  puts( "Finished" );
  // last thing to do is set sane stime for doing stuff 

  return 0;

}
