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


// basic error funtion - finish program
void fail( const char *const estr )  {

  perror( estr );
  exit( EXIT_FAILURE );

}

//////////////////////////////////////
//  PROGRAM OPTIONS / ARGUMENTS FUNCTIONS

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


// END OF PROGRAM OPTIONS / ARGUMENTS FUNCTIONS
////////////////////////////////////////////////
//  CORE DEFINITIONS

// it is used for keeping message ID or talker ID
#define TALKMSG_SIZE
struct idname {

  int id,
  char name[ TALKMSG_SIZE ] 

};

#define TALKER_NAME_LEN 2
#define MESSAGE_NAME_LEN 3

// Talker list
#define GENERATE_TALKER \
  X( GA ), \
  X( GB ), \
  X( GI ), \
  X( GL ), \
  X( GP ), \
  X( GN ), \
  X( GQ )

// generatea talker integer ID
enum NMEAtalkerID {

  talkerINIT,
#define X( VARNAME ) talker##VARNAME
  GENERATE_TALKER
#undef X

};

struct idname talker_idname[] {

#define X( VARNAME ) { talker##VARNAME, #VARNAME },
  GENERATE_TALKER
#undef X  

};
const size_t talker_idname_len = sizeof talker_idname / sizeof talker_idname[0];

// message list
#define GENERATE_MESSAGE \
  X( GGA ), \
  X( RMC )

// generate message integer ID
enum NMEAmessageID {

  messageINIT,
#define X( VARNAME ) message##VARNAME
  GENERATE_MESSAGE
#undef X

};


struct idname message_idname[] {

#define X( VARNAME ) { talker##VARNAME, #VARNAME },
  GENERATE_MESSAGE
#undef X  

};
const size_t message_idname_len = sizeof message_idname / sizeof message_idname[0];

// END OF CORE DEFINITIONS
//////////////////////////////////////////////////
// NMEA STRUCTS AND INIT FUNCTIONS
struct NMEAent  {

  const char *line
  struct idname talker,
  struct idname message,
  void *data

};

void init_nmea( struct *const NMEAent nmea,
    const char *line )  {

  nmea->line = line;
  nmea->talker.id = talkerINIT;
  memset( nmea->talker.name, '\0', TALKMSG_SIZE );
  nmea->message.id = messageINIT;
  memset( nmea->message.name, '\0', TALKMSG_SIZE );
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

} // TODO check this one last time



// END OF NMEA STRUCTS AND INIT FUNCTIONS
/////////////////////////////////////////////
// PRINTING FUNCTIONS

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

// END OF PRINTING FUNCTIONS
///////////////////////////////////////////////
// READING FUNCTIONS AND DATA UPLOAD


// look for name in the talker or message idname array
// if matches nmea one set the ID
int setid( struct idname *const nmea_idn,
    const struct idname *const idn,
    const size_t idn_len )  {

  for( size_t i = 0; i < idn_len; i++ )  {

    if( ! strcmp( nmea_idn->name, idn_array[i].name ) )  {

      //set the ID and return
      strcpy( nmea_idn->id, idn_array[i].id );
      return 0;

    }

  }

  // this should not happen
  return -1;

}

// extract data from nmea line
// this function sets null in buff
int readmem( char **const nmealinep,
    char *const datastr, const size_t cplen )  {

  // Get it, then use it and set new location
  char *nmealine = *nmealinep;
  for( size_t i = 0; i < cplen; i++ )  {

    // nul should never appear in data this function uses
    if( nmealine[i] == '\0' )  {

      errno = 0;
      perror( "Reached nul too early, missign data" );
      return -1;

    }
    datastr[i] = nmealine[i];

  }

  // No need to set nul because it is done in nmea init function.
  // Finaly move to place where we finished coping data..
  *nmealinep += cplen;
  return 0;

}


// This function extracts whole data from nmea->line
const size_t nmeatalker_len = 2;  
const size_t nmeamessage_len = 3;
int readnmea( struct NMEAent *const nmea )  {

  char *nmealine = nmea->line;
  // $ should always start NMEA line
  if( nmealine[0] != '$' )  {

    errno = 0;
    perror( "The nmea line has no $ at start" );
    return -1;

  }
  nmealine++; // move forward, after $.

  // set nmea talker name
  if( readmem( &nmealine, nmea->talker.name ,
      TALKERNAME_LEN ) == -1 )
    return -1;

  if( setid( &( nmea->talker ), talker_idname,
       talker_idname_len ) == -1 )  {

    fprintf( stderr, "Unknown talker ID: %s\n",
      nmea->talker.name );
    return -1;

  }

  // set nmea message name
  if( readmem( &nmealine, nmea->message.name,
      MESSAGENAME_LEN ) == -1 )
    return -1;

  if( setid( &( nmea->message ), message_idname,
       message_idname_len ) == -1 )  {

    fprintf( stderr, "Unknown message ID: %s\n",
      nmea->message.name );
    return -1;

  }


  // Now we should be standing on ',' - check it
  if( nmealine[0] != ',' )  {

    perror( "No ',' after message ID which has static size" );
    return -1;

  }



  // TODO
  // finished here last time


}


// END OF  READING FUNCTIONS AND DATA UPLOAD
///////////////////////////////////////////////////////////////
//  MAINa PROGRAM

int main( const int argc, const char *const argv[] )  {

  #define OPT_POS 1
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
