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

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// basic error funtion - finish program
void fail( const char *const estr )  {

  assert( esrt != NULL );

  perror( estr );
  exit( EXIT_FAILURE );

}

void etalk( const char *const estr )  {

  fprintf( stderr, "%s\n", estr );

}

//////////////////////////////////////
//  PROGRAM OPTIONS / ARGUMENTS FUNCTIONS

// global option we set in chkoption
unsigned char readgps_opt;

// Set the option and check for bugs
  #define OPTSTR_POS 1
int setoption( const char *const options )  {

  assert( options != NULL );

  // '-' is a must
  if( options[0] != '-' )  return -1;
  // Size of option string is expected static
  if( strlen( options ) != 2 )  return -1;

  switch( options[ OPTSTR_POS ] )  {

     // information
     case 'i':
     // Check the nmealine and chksum
     case 'c':
      readgps_opt = options[ OPTSTR_POS ];
      return 0;

     default :
      return -1;

  }

}

int chkarg( const int argc )  {

  assert( argc >= 0 );

  switch( readgps_opt )  {

   case 'c':
   case 'i':
     if( argc != 3 )  return -1;
     return 0;

   default:
     return -1;

  }

}


// END OF PROGRAM OPTIONS / ARGUMENTS FUNCTIONS
////////////////////////////////////////////////
//  CORE DEFINITIONS

// it is used for keeping message ID or talker ID
#define TALKMSG_SIZE 16
struct idname {

  int id;
  char name[ TALKMSG_SIZE ];

};

#define TALKERNAME_LEN 2
#define MESSAGENAME_LEN 3

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

struct idname talker_idname[] = {

#define X( VARNAME ) { talker##VARNAME, #VARNAME }
  GENERATE_TALKER
#undef X  

};

// get the lengt of it 
const size_t talker_idname_len =
  sizeof talker_idname / sizeof talker_idname[0];

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


struct idname message_idname[] = {

#define X( VARNAME ) { message##VARNAME, #VARNAME }
  GENERATE_MESSAGE
#undef X  

};
const size_t message_idname_len =
  sizeof message_idname / sizeof message_idname[0];

// END OF CORE DEFINITIONS
//////////////////////////////////////////////////
// NMEA STRUCTS AND INIT FUNCTIONS
struct NMEAent  {

  char *line;
  struct idname talker;
  struct idname message;
  char **entries;
  size_t entries_len;
  uint8_t chksum;

};

void init_nmea( struct NMEAent *const nmea,
    const char *const line )  {

  assert( nmea != NULL );
  assert( line != NULL );

  nmea->line = ( char* )line;
  nmea->talker.id = talkerINIT;
  memset( nmea->talker.name, '\0', TALKMSG_SIZE );
  nmea->message.id = messageINIT;
  memset( nmea->message.name, '\0', TALKMSG_SIZE );
  nmea->entries = NULL;
  nmea->entries_len = 0;
  nmea->chksum = 0;

}

struct nmeatime {

  int hour;
  int minute;
  double sec;

};

struct nmeadate {

  int day;
  int month;
  int year;

};


/////////////////////
// MESSAGE ID STRUCTS
// Minimal parameter len since new versions of NMEA
// introduce new parameters so only below minimal
// will trigger error.

#define GGA_MIN_PARAMLEN 14
struct GGAmsg {

  struct nmeadate time; // UTC hh:mm:ss.ss
  double lat; //  degres
  char lat_NS; // N / S  latitude hemisphere
  double lon; // degres
  char lon_NE; // W / E longtitude hemisphere 
  int quality; // small numbers
  double satelite_num; // small numbers
  double hddp; // horizontal Dilution of Precision
  double alt; // Altitude above mean sea level
  char alt_units; // Seems only M, as for meters appear
  double sep; // geoid separation differance between 
	      // elipsoid and mean sea level
  char sep_units; // ^ units, seems M for meters is only choice
  double diff_age; // Age of differental corretions
		   // blank if DGPS is not used
  double diff_station; // ID of station providing differential
		      // corrections - blank when DGPS not used

  size_t min_paramlen;  // minimal expected entries

};


#define RMC_MIN_PARAMLEN 14
struct RMCAmsg {

  struct nmeadate time; // UTC hh:mm:ss.ss
  char status; // data validator
  double lat; //  degres
  char lat_NS; // N / S  latitude hemisphere
  double lon; // degres
  char lon_NE; // W / E longtitude hemisphere 
  double velocity; // speed over ground in knots - change to km/h or m/s
  double course; // Course over ground.
  struct nmeadate date; // dd mmy yyy		 
  double mv; // Magnetic variation value 
	    // Only supported in ADR 4.10 and above
  char mv_EW; // Magnetic variation E / W indicator
	    // Only supported in ADR 4.10 and above
  char pos_mod; // Mode indicator NMEA v 2.3 and above only  // TODO wtf is this
  char nav_stat; // Navigation indicator status NMEA v 4.1 and above
		// For examples tells us equipment does not provide
		// navigational status information.

  size_t min_paramlen;  // minimal expected entries

};



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
    etalk( "Talker ID is unknown\n" );
    return -1;
	
  }

}

int print_messageID( const int messageID )  {

  switch( messageID )  {

   case messageGGA:
     printf( "Message ID: Global positioning system fix data\n" );
     return 0;

   case messageRMC:
     printf( "Message ID: Recommended Minimum data\n" );
     return 0;

   default:
     etalk( "Message ID is unknown\n" );
     return -1;

  }

}

// this function prints nmea data,
// it's more meant for debugging or when the Message
// type is unknown
void print_nmeadata( const struct NMEAent *const nmea )  {

  print_talkerID( nmea->talker.id );
  print_messageID( nmea->message.id );
  for( size_t i = 0; i < nmea->entries_len; i++)
    printf( "%s\n", ( nmea->entries )[i] );
  printf( "Counted checksum in hex: %" PRIX8 "\n", nmea->chksum );

}

// END OF PRINTING FUNCTIONS
///////////////////////////////////////////////
// READING FUNCTIONS AND DATA UPLOAD


// look for name in the talker or message idname array
// if matches nmea one set the ID
int setid( struct idname *const nmea_idn,
    const struct idname *const idn_array,
    const size_t idn_len )  {

  assert( nmea_idn != NULL );
  assert( idn_array != NULL );
  assert( idn_len != 0 );

  for( size_t i = 0; i < idn_len; i++ )  {

    if( ! strcmp( nmea_idn->name, idn_array[i].name ) )  {

      //set the ID and return
      nmea_idn->id = idn_array[i].id;
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

  assert( nmealinep != NULL );
  assert( *nmealinep != NULL );
  assert( datastr != NULL );
  assert( cplen != 0 );

  // Get it, then use it and set new location
  char *nmealine = *nmealinep;
  for( size_t i = 0; i < cplen; i++ )  {

    // nul should never appear in data this function uses
    if( nmealine[i] == '\0' )  {

      etalk( "Reached nul too early, missign data" );
      return -1;

    }
    datastr[i] = nmealine[i];

  }

  // No need to set nul because it is done in nmea init function.
  // Finaly move to place where we finished coping data..
  *nmealinep += cplen;
  return 0;

}

// create entries. in NMEA we can simply strtok on  ','
// since each entry is separated by ','
// checksum needs to be done before this function call since it
// changes the nmea line
int loadent( struct NMEAent *const nmea )  {

  assert( nema != NULL );
  assert( nmea->line != NULL );
  assert( nmea->entres == NULL );
  assert( nmea->entries_len == 0 );

  // TODO check if * should not be treated as last entry indicator
  // we do not look into first return of strtok
  // because it's the $<TALKER><MESSAGE> data
  // and we already have that obtained
  char *ent = strchr( nmea->line, ',' );
  if( ent == NULL )  {

    //we actualy check it partialy elsewhere but we check it again
    //here since this function is not bound to other one
    //so it might be used in situation the other is not used.
    etalk( "the nmea line seems to be broken\n"
      "Could not find any comma" );
    return -1;

  }

  // now create the entry list
  bool chksumsign_found = false;
  bool lastent = false;
  for(;;)  {

    ent++; // move after comma ( in this loop comma is changed to nul btw )
    char *newent = strchr( ent, ',' );
    if( newent == NULL )  {

      newent = strchr( ent, '*' );
      if( newent == NULL )  {

        etalk( "Check sum indycator never appeared in NMEA line" );
	return -1;

      }

      chksumsign_found = true;

    }


   finished:  // goto for last entry loop
    // Now since we found the separator so we set it to null.
    *newent = '\0';
    
    void *aloc_data =
      realloc( nmea->entries,
      ( nmea->entries_len + 1 ) * sizeof *( nmea->entries ) );
    if( aloc_data == NULL )  return -1;

    ( nmea->entries_len )++;
    nmea->entries = aloc_data;
    char **newent_ptr = &( ( nmea->entries )[ nmea->entries_len - 1 ] );
    size_t entsize = strlen( ent ) + 1;
    *newent_ptr = malloc( entsize ); // + 1 for nul
    if( *newent_ptr == NULL )  return -1;
    // ^ even if we return -1 
    // it is fine NULL is left, free( NULL ) wont cause problems
    strncpy( *newent_ptr, ent, entsize );

    // pass found entry
    ent = newent;

    // we have all entries
    if( lastent )  return 0;
    if( chksumsign_found == true )  {

      // pass the checksum sign - also turned to nul earlier
      ent++;
      newent = ent;
      for(; isxdigit( *newent ); newent++);
      lastent = true;
      goto finished;

    }

  }

  // We should never appear here
  etalk( "loadent fatal error" );
  return -1;

}

// This function extracts whole data from nmea->line
const size_t nmeatalker_len = 2;  
const size_t nmeamessage_len = 3;
int readnmea( struct NMEAent *const nmea )  {

  assert( nmea != NULL );
  assert( nmea->line != NULL );
  assert( nmea->entries == NULL );
  assert( nmea->entries_len == 0 );
  assert( nmea->chksum == 0 );

  char *nmealine = nmea->line;
  // $ should always start NMEA line
  if( nmealine[0] != '$' )  {

    etalk( "The nmea line has no $ at start" );
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

    etalk( "No ',' after message ID which has static size" );
    return -1;

  }

  // Move at start and do the checksum
  nmealine = nmea->line;
  nmealine++; // pass the $
  for(; *nmealine != '*'; nmealine++ )  {

    if( *nmealine == '\0' )  {

      etalk( "There is no check sum sign indicator.\n"
        "nmea line appears to be broken." );
      return -1;

    }

    nmea->chksum ^= *nmealine;

  }

  // load everything else
  if( loadent( nmea ) != 0 )  return -1;
  // at this point all data was processed
  return 0;

}


//It expects one character
//if no nul after one char this is an error!
int get_nmeachr( const char *const ent,
    char *const nmeachr )  {

  // we expect character and fin after it
  if( ent[0] == '\0' )  return -1; 
  if( ent[1] != '\0' )  return -1;
  *nmeachr = ent[0];
  return 0;

}

int chk_nmeachr_NS( const char *const nmeachr )  {

  if( ( *nmeachr == 'N' ) || ( *nmeachr == 'S' ) )
    return 0;
  return -1;

}

int chk_nmeachr_EW( const char *const nmeachr )  {

  if( ( *nmeachr == 'E' ) || ( *nmeachr == 'W' ) )
    return 0;
  return -1;

}

int chk_nmeachr_M( const char *const nmeachr )  {

  if(  *nmeachr == 'M' )  return 0;
  return -1;

}

// check digits and dot, else error
int chk_nmeafloat( const char *const ent )  {

  int count_dot = 0;
  for( size_t i = 0; ent[i] != '\0'; i++ )  {

    if( ent[i] == '.' )  {

      // obviously we can have only one dot
      if( count_dot != 0 )  return -1;
      count_dot++;
      continue;

    }
    if( isdigit( ent[i] ) )  continue;
    return -1;

  }

  return 0;

}

//  date slen should at least be six characters
//  dot appears if - always after six character
//  hhmmss.ss
int chk_nmeadate( const char *const ent )  {

  size_t i = 0;
  for(; i < 6; i++ )  {

    if( isdigit( ent[i] ) )  continue;
    return -1;

  }

  if( ent[i] == '\0' )  return 0;
  if( ent[i] != '.' )  return -1;
  i++; // pass the dot

  for(; ent[i] == '\0'; i++ )  {

    if( isdigit( ent[i] ) )  continue;
    return -1;

  }

  return 0;

}

int get_nmeafloat( const char *const ent,
    double *const nmeafloat )  {

  errno = 0;
  *nmeafloat = strtod( ent, NULL );
  if( errno != 0 )  return -1;
  return 0;

}

// 0-9 range is ok, rest is error
int chk_nmeadigit( const char *const ent )  {

  for( size_t i = 0; ent[i] != '\0'; i++ )  {

    if( isdigit( ent[i] ) )  continue;
    return -1;

  }

  return 0;

}

int get_nmeadigit( const char *const ent,
    long *const digit )  {

  errno = 0;
  *digit = strtol( ent, NULL, 10 );
  if( errno != 0 )  return -1;
  return 0;

}

// END OF  READING FUNCTIONS AND DATA UPLOAD
///////////////////////////////////////////////////////////////
//  MAIN PROGRAM

int main( const int argc, const char *const argv[] )  {

  #define OPT_POS 1
  // basic argument check, there must be something.
  if( argc < 2 )  fail( "Not enought arguments" );
  // Check the option and set it to the global value
  if( setoption( argv[ OPT_POS ] ) == -1 )
    fail( "Broken options" );
  // See if the number of arguments is correct
  if( chkarg( argc ) == -1 )
    fail( "Broken number of arguments" );

  // dependign on option do...
  switch( readgps_opt )  {

  /////////////////////////////////////////////////////////////////////////

  // output information of GPS data
case 'i': ;// expression for *goto* - case
  #define INFO_NMEALINE_ARGPOS 2

  // init nmea
  struct NMEAent nmea_info;
  init_nmea( &nmea_info, argv[ INFO_NMEALINE_ARGPOS ] );

  // read nmea data
  if( readnmea( &nmea_info ) == -1 )
    fail( "Could not read NMEA entry" );
  
  // print nmea with basic info and raw data entries
  print_nmeadata( &nmea_info );  

  return 0;

  /////////////////////////////////////////////////////////////////////

// Check the chksum
// It will return 0 on succes
// 1 means problem with passed commands and arguments - liek too many arguments, wrong commands
// 2 means check sum does not match or the nmea line is broken, corrupted data.
case 'c': ;
  #define CHKSUM_NMEALINE_ARGPOS 2
  #define NMEALINE_FAIL 2
  #define CHKSUM_BASE 16

  struct NMEAent nmea_chksum;
  init_nmea( &nmea_chksum, argv[ CHKSUM_NMEALINE_ARGPOS ] );
  if( readnmea( &nmea_chksum ) == -1 )  {

    etalk( "Could not read NMEA line" );    
    return NMEALINE_FAIL;

  }

  errno = 0;
  long int chksum = strtol( nmea_chksum.entries[ nmea_chksum.entries_len - 1 ],
    NULL, CHKSUM_BASE );
  if( ( uint8_t )chksum != nmea_chksum.chksum )
    return 2;  // Chksum fail, corrupted data
    
  // all fine
  return 0;

  // TODO free function

  /////////////////////////////////////////////////////////////////////
  // everythign else is bug
default:
    etalk( "No such option" );
    return -1;

  }

}
