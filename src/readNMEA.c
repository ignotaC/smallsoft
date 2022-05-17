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

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void fail( const char *const estr )  {

  perror( estr );
  exit( EXIT_FAILURE );

}


// global option we set in chkoption
unsigned char readgps_opt;

// Set the option and check for bugs
  #define OPTSTR_POS 1
int setoption( const char *const options )  {

  // '-' is a must
  if( options[0] != '-' )  return -1;
  // Size of option string is expected static
  if( strlen( options ) != 2 )  return -1;

  switch( options[ OPTSTR_POS ] )  {

     case 'i':
      // information
      readgps_opt = 'i';
      return 0;

     default :
      return -1;

  }

}

int chkarg( const int argc )  {

  switch( readgps_opt )  {

   case 'i':
     if( argc != 3 )  return -1
     return 0;

   default:
     return -1

  }

}

enum NMEAtalkerID {

  talkerINIT,
  talkerGA,
  talkerGB,
  talkerGI,
  talkerGL,
  talkerGP,
  talkerGN,
  talkerGQ

}



int print_talkerID( const int talkerID ) {

  switch( talkerID )  {

   case talkerGA:
    printf( "Galileo Positioning navigation systems (EU).\n" );
    return 0;
		  
   case talkerGB:
    printf( "BeiDou navigation systems (China).\n" );
    return 0;

   case talkerGI:
    printf( "NavIC(IRNSS) navigation systems (India).\n" );
    return 0;

   case talkerGL:
    printf( "GLONASS navigation systems (Russia).\n" );
    return 0;
	
   case talkerGP:
    printf( "GPS navigation systems (US).\n" );
    return 0;
	   
   case talkerGN:
    printf( "GNSS Global Navigation Satellite Systen ( any country ).\n" );
    return 0;

   case talkerGQ:
    printf( "QZSS navigation systems ( Japan ).\n" );
    return 0;

   default:
    return -1;
	
  }

}

enum NMEAmessageID {

  messageINIT,
  messageGGA

}

// NMEA core struct
struct NMEAent  {

  const char *line
  int talkerID,
  int messageID,
  void *data

};

void init_nmea( struct *const NMEAent nmea,
    const char *line )  {

  nmea->line = line;
  nmea->talkerID = talkerINIT;
  nmea->messagID = messageINIT;
  nmea->data = NULL;

}

// GGA = Global Positioning System Fix Data
struct GGAdata  {

  //  ss:mm:hh UTC  those are 13 bytes with nul
  char time[16]

  double latitude, // deg
  char latitude_hemisphere,
  double longtitude, // deg
  char longtitude_hemisphere,
  uint8_t typeoffix,  
  int satelite_inview_number,
  double horizontal_dilution,
  double altitude,

  // above WGS84 - elipsoid
  double geoid_height,
  int DGPS_lastupdate_time,
  int station_ID,
  int checksum

}

// extract data from nmea line
// this function sets null in buff
int readmem( char **const nmealinep,
    const char *const datastr, const size_t cplen )  {

  // Get it, then use it and set new location
  char *nmealine = *nmealinep;
  for( size_t i = 0; i < cplen; i++ )  {

    // nul should never appear in data this function uses
    if( nmealine[i] == '\0' )  {

      errno = 0;
      perror( "Reached nul too early, missign data" );
      return -1;

    }
    buff[i] = nmealine[i];

  }

  // set nul
  buff[i] = '\0';
  // move to place where we finished coping data.
  *nmealinep += cplen;
  return 0;

}

int readnmea( struct NMEAent *const nmea )  {

  char *nmealine = nmea->line;
  // $ should always start NMEA line
  if( nmeapos[0] != '$' )  {

    errno = 0;
    perror( "The nmea line has no $ at start" );
    return -1;

  }
  nmeapos++; // move forward, after $.

  const size_t nmeatalker_len = 2;  
  char nmeatalker[ nmeatalker_len + 1 ];
  // read data ( function will add nul 
  if( readmem( &nmealine, nmeatalker, nmeatalker_len ) == -1 )
    return -1;


  // TODO snow update nmea talker type
  const size_t nmeamessage_len = 3;
  char nmeamessage[ nmeamessage_len + 1 ];
  if( readmem( &nmealine, nmeamessage, nmeamessage_len ) == -1 )
    return -1;

  // TODO snow update nmea message type
  //
  // TODO
  // finished here last time


}

#define OPT_POS 1

int main( const int argc, const char *const argv[] )  {

  // basic argument check, there must be something.
  if( argc < 2 )  fail( "Not enought arguments" );
  // Check the option and set it to the global value
  if( setoption( argv[ OPT_POS ] ) == -1 )
    fail( "Broken options" );
  // See if the number of arguments is correct
  if( chkarg( const int argc ) == -1 )
    fail( "Broken number of arguments" );

  // dependign on option do...
  switch( readgps_opt )  {

  // output information of GPS data
case 'i':
  #define NMEALINE_ARGPOS 2

  // init nmea
  struct NMEAent nmea;
  init_nmea( &nmea, argv[ NMEALINE_ARGPOS ] );

  // read nmea data
  if( readNMEA( &nmea ) == -1 )
    fail( "Could not read NMEA entry" );
       	
  return 0;

default:
    return -1;

  }

}
