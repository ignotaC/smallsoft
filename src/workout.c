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

#include "../ignotalib/src/ig_file/igf_write.h"
#include "../ignotalib/src/ig_file/igf_read.h"

#include <sys/stat.h>

#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <string.h>

#include <stdint.h>
#include <inttypes.h>

#define WORKOUT_FILE_NAME ".workout"

#ifdef DEBUG_PROGRAM

  #define dmsg(x) ( fprintf( stderr, "%s\n", x ) )
  #define drun(x) ( x )
  #define dret(x) return x

#else /* DEBUG_PROGRAM */

  #define dmsg(x) 
  #define drun(x)
  #define dret(x)

#endif /* DEBUG_PROGRAM */
// Procent chance
#define MIX_CHANCE 25
#define DAY_TIME ( 60 * 60 * 24 )

#define FPERM ( ( mode_t ) ( S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH ) )

enum {

  false,
  true

};

int get_year( void )  {

  time_t current_time = time( NULL );
  struct tm *ct_tm = localtime( &current_time );
  return ( 1900 + ct_tm->tm_year );

}

int get_day( void ) {

  time_t current_time = time( NULL );
  struct tm *ct_tm = localtime( &current_time );
  return 1 + ct_tm->tm_yday;

}

int create_workout_file( const char *const workout_file_path )  {

  int fd = open( workout_file_path, O_RDWR | O_CREAT, FPERM );
  if( fd == -1 )  return -1;
  if( close( fd ) == -1 )  perror( "Error on closing workoutfile" );
  return 0;

}

void fail( const char *const estr )  {

  perror( estr );
  exit( EXIT_FAILURE );

}

void stdout_nobuf( void )  {

  if( setvbuf( stdout, NULL, _IONBF, 0 ) != 0 )  fail( "Could not set buff" );

}


char *get_homepath( const char *const filename )  {

  const char *const entry = getenv( "HOME" );
  if( entry == NULL )  return NULL;
  const size_t homepath_size = strlen( entry ) + strlen( "/" ) + strlen( filename ) + 1;
  // last is nu

  char *const retpath = malloc( homepath_size );
  if( retpath == NULL )  return NULL;

  if( sprintf( retpath, "%s/%s", entry, filename ) < 0 )  {
    
    free( retpath );
    return NULL;
  
  }
  return retpath;

}

// pass percent chance
int get_roll_ans( int chance_percent )  {

  int ans = rand() % 100;
  if ( ans < chance_percent ) ans = 1;
  else  ans = 0;
  return ans;

}

int roll_num( const int min, const int max )  {

  int ans = rand() % ( max + 1 - min );
  ans += min;
  return ans;

}

// use X macro for string representation.

#define X_EXERCISE \
  X(EX_START) \
  X(EX_SHOULDERS) \
  X(EX_BICEPS) \
  X(EX_TRICEPS) \
  X(EX_THIGHS_BUTTOCKS) \
  X(EX_CALVES) \
  X(EX_BACK) \
  X(EX_FOREARMS) \
  X(EX_STOMACH) \
  X(EX_CHEST) \
  X(EX_END) \
  X(EX_NO_EXERCISE )
  // special value for second exercise if none
  // also used when exercise file does not exist.

enum  {

  #define X(name)  name,
  X_EXERCISE
  #undef X
  EX_ENUM_EXERCISE_END

};

int exercise_intval[] = {

  #define X(name) name,
  X_EXERCISE
  #undef X
  0

};


char *exercise_names[] = {

  #define X(name) #name,
  X_EXERCISE
  #undef X
  NULL

};

char *getexname( int exer )  {

  if( exer == EX_NO_EXERCISE )  return "";
  return exercise_names[ exer ];

}

int count_exercise( void ) {

  return EX_END - 1;

}

// Rules of exercise you can never do after.
// First is exercise we are doing today, everything till NULL
// is forbiden to be done yesterday.
// two NULLS signal end of array.
int *exer_rules_tab[] = {

  NULL,
  &exercise_intval[EX_THIGHS_BUTTOCKS],
  &exercise_intval[EX_BACK],
  &exercise_intval[EX_CALVES],
  NULL,
  &exercise_intval[EX_CHEST],
  &exercise_intval[EX_BICEPS],
  NULL,
  &exercise_intval[EX_TRICEPS],
  &exercise_intval[EX_BICEPS],
  &exercise_intval[EX_FOREARMS],
  NULL,
  &exercise_intval[EX_BICEPS],
  &exercise_intval[EX_FOREARMS],
  NULL,
  &exercise_intval[EX_BACK],
  &exercise_intval[EX_BICEPS],
  NULL,
  NULL

};

int canbenext( const int today_exer, const int yesterday_exer )  {

  if( yesterday_exer == EX_NO_EXERCISE )
    return 1;

  int **exe_rule = exer_rules_tab;
  int **te = NULL;
  while( true )  {

    if( *exe_rule == NULL )  {

      exe_rule++;
      if( *exe_rule == NULL )  return 1;
      te = exe_rule;
      // This is not 
      if( **te != today_exer )  {

        while( *exe_rule != NULL )  exe_rule++;
	continue;

      }
      exe_rule++;
      continue;

    }
    if( **exe_rule == yesterday_exer )  return 0;
    exe_rule++;

  }

}

struct exercise_check {

  int today_ex1;
  int today_ex2;
  int yesterday_ex1;
  int yesterday_ex2;

};

void print_ec( struct exercise_check *ec )  {

  printf( "Yesterday exercise: %d %s, %d %s\n",
	  ec->yesterday_ex1, getexname( ec->yesterday_ex1 ),
	  ec->yesterday_ex2, getexname( ec->yesterday_ex2 ) );
  printf( "Today exercise: %d %s, %d %s\n",
	  ec->today_ex1, getexname( ec->today_ex1 ),
	  ec->today_ex2, getexname( ec->today_ex2 ) );

}

int check_exercise( const struct exercise_check *const ec )  {

  if( ! canbenext( ec->today_ex1, ec->yesterday_ex1 ) )  return 0;
  if( ! canbenext( ec->today_ex1, ec->yesterday_ex2 ) )  return 0;
  if( ! canbenext( ec->today_ex2, ec->yesterday_ex1 ) )  return 0;
  if( ! canbenext( ec->today_ex2, ec->yesterday_ex2 ) )  return 0;
  return 1;

}

void move_exercise( struct exercise_check *const ec )  {

  ec->yesterday_ex1 = ec->today_ex1;
  ec->yesterday_ex2 = ec->today_ex2;

}

// Exercise that can never be mixed with other exercise

int *nomix_tab[] = {

  &exercise_intval[EX_THIGHS_BUTTOCKS],
  &exercise_intval[EX_BACK],
  NULL

};

int canmix( int exercise )  {

  int **mixtabptr = NULL;
  for( mixtabptr = nomix_tab; *mixtabptr != NULL; mixtabptr++ )
    if( **mixtabptr == exercise )  return 0;

  return 1;

}

void print_exarr( int ( *exarr )[2] )  {

  
  puts( "Exercise array contents" );
  int i = 0;
  while( true )  {
    
    printf( "Pos %d, val0 =  %d -> exercise: %s, val1 = %d\n", \
	    i, ( *exarr )[0], getexname( ( *exarr )[0] ), ( *exarr )[1] );
    if( ( *exarr )[1] == -1 )  return;
    i++;
    exarr++;

  }

}


int( *make_exercise_array( void ) )[2]  {

  dmsg( "Entering make_exercise_array fnction " );
  size_t array_len = count_exercise() + 1;
  /* We put NULL at the end so START element which is counted is our space for NULL */

  int( *array )[2] = malloc( sizeof( *array ) * array_len );
  if( array == NULL )  return NULL;

  // Fill array with exercise, second value is used further in program to know if exercise was
  // previously used. So we set all by default as 0.
  int position = 0;
  size_t array_pos = 0;
  for( position = EX_START + 1; position != EX_END; position++, array_pos++ )  {

    array[ array_pos ][0] = position;
    array[ array_pos ][1] = 0;

  }

  array[ array_pos ][0] = EX_NO_EXERCISE;
  array[ array_pos ][1] = -1;
  drun( print_exarr( array ) );
  return array;

}


enum power {

  POW_LOW,
  POW_MEDIUM,
  POW_HIGH,
  POW_COUNT

};

char *getpowerstr( uint32_t powerval )  {

  switch( powerval )  {

    case POW_LOW:  return "low";
    case POW_MEDIUM:  return "medium";
    case POW_HIGH:  return "high";
    default: return "error";

  }

}

int getpower( void )  {

  if( get_roll_ans( 40 ) )  return POW_MEDIUM;
  return rand() % POW_COUNT;

}

#define EE_ENTRY_NUMBER 8
struct exercise_entry {

  uint32_t exercise;
  uint32_t second_exercise;

  uint32_t year;
  uint32_t day_of_year;

  uint32_t how_heavy;
  uint32_t series_ammount;
  uint32_t how_hard;
  uint32_t exer_types_count;

};

void print_ee( struct exercise_entry *ee )  {
  
  dmsg( "Entering print_ee function" );
  printf( "Day of year %" PRIu32 " Year %" PRIu32 "\n", ee->day_of_year, ee->year );
  printf( "Exercise: %s %s\n", getexname( ee->exercise ), getexname( ee->second_exercise ) );
  printf( "Weight level: %s. Series count: %" PRIu32 ".\n" \
	  "Hardness level: %s. Count of exercise types: %" PRIu32 "\n",
	  getpowerstr( ee->how_heavy ), ee->series_ammount,
	  getpowerstr( ee->how_hard ), ee->exer_types_count );

}


struct exercise_entry *alloc_fresh_ee( void )  {

  dmsg( "Creating entry" );
  struct exercise_entry *ee = malloc( sizeof *ee );
  if( ee == NULL )  return NULL;
  ee->exercise = EX_NO_EXERCISE;
  ee->second_exercise = EX_NO_EXERCISE;
  ee->year = get_year();
  ee->day_of_year = get_day();
  ee->how_heavy = getpower();
  ee->series_ammount = roll_num( 2, 5 );
  ee->how_hard = getpower();
  ee->exer_types_count = roll_num( 2, 5 );
  return ee;  

}

time_t get_nextday_time( int day, int year )  {

  struct tm tm_noon = { 0 }; // NULL is 0 in std so fine. Also January is 0 and so are seconds and minutes
  tm_noon.tm_hour = 12;
  tm_noon.tm_mday = 1;
  tm_noon.tm_year = year;

  time_t newtime = mktime( &tm_noon );
  newtime += DAY_TIME * ( day + 1 );
  return newtime;

}

void set_ee_time( time_t eetime, struct exercise_entry *ee )  {

  struct tm *ee_tm = localtime( &eetime );
  ee->year = ee_tm->tm_year;
  ee->day_of_year = ee_tm->tm_yday;

}


struct workout_entries  {

  struct exercise_entry *ee;
  uint32_t ee_count;

};


void print_we( struct workout_entries *we )  {

  for( size_t i = 0; i < we->ee_count; i++ )
    print_ee( &( we->ee[i] ) );

}


struct workout_entries *init_we( void )  {

  struct workout_entries *we = malloc( sizeof *we );
  if( we == NULL )  return NULL;
  we->ee = NULL;
  we->ee_count = 0;
  return we;

}

int push_ee_to_we( struct workout_entries *we, struct exercise_entry *ee )  {

  ( we->ee_count )++;
  void *newmem = realloc( we->ee, we->ee_count * sizeof *( we->ee ) );
  if( newmem == NULL )  {

    ( we->ee_count )--;
    return -1;

  }
  we->ee = newmem;
  memcpy( &( we->ee[ we->ee_count -1 ] ), ee, sizeof *( we->ee ) );
  return 0;

}

int push_we_to_we( struct workout_entries *we, struct workout_entries *we_to_add )  {

  for( size_t i = 0; i < we_to_add->ee_count; i++ )  {

    if( push_ee_to_we( we, &( we_to_add->ee[i] ) ) == -1 )
      return -1;

  }

  return 0;

}

int rmv_first_ee_from_we( struct workout_entries *we )  {

  if( we->ee_count == 0 )  return 0;
  if( we->ee_count == 1 )  {

    free( we->ee );
    we->ee = NULL;
    we->ee_count = 0;
    return 0;

  }

  const size_t new_size = ( we->ee_count - 1 ) * sizeof( *( we->ee ) );
  void *newmem = malloc( new_size );
  if( newmem == NULL )  return -1;
  
  memcpy( newmem, &( we->ee[1] ), new_size );
  free( we->ee );
  we->ee = newmem;
  ( we->ee_count )--;
  return 0;
 
}

void free_we( struct workout_entries *we )  {

  free( we->ee );
  free( we );

}


ssize_t readfd( const int fd, void *buff, size_t read_size )  {

  ssize_t igf_ret = igf_read( fd, buff, read_size );
  if( igf_ret == -1 )  return -1;
  if( read_size != ( size_t )igf_ret )  {

    dmsg( "We did not read full data" );
    errno = 0;
    return -1;

  }

  return 0;

}


int write_ee( struct exercise_entry *ee, int binfile_fd )  {

  uint32_t data[ EE_ENTRY_NUMBER ];
  size_t data_pos = 0;

  data[ data_pos++ ] = ee->exercise;
  data[ data_pos++ ] = ee->second_exercise;
  data[ data_pos++ ] = ee->year;
  data[ data_pos++ ] = ee->day_of_year;
  data[ data_pos++ ] = ee->how_heavy;
  data[ data_pos++ ] = ee->series_ammount;
  data[ data_pos++ ] = ee->how_hard;
  data[ data_pos++ ] = ee->exer_types_count;

  return igf_write( binfile_fd, data, sizeof data );

}

int write_we( struct workout_entries *we, char *workout_file_name )  {

  int binfile_fd = open( workout_file_name, O_RDWR | O_TRUNC );
  if( binfile_fd == -1 )  return -1;

  uint32_t ee_count = we->ee_count;
  if( igf_write( binfile_fd, &ee_count, sizeof ee_count ) == -1 )  goto failexit;
  
  for( size_t i = 0; i < ee_count; i++ )
    if( igf_write( binfile_fd, &( we->ee[i] ), sizeof *( we->ee ) ) == -1 )  goto failexit;

  close( binfile_fd );
  return 0;

failexit:
  close( binfile_fd );
  return -1;

}

struct exercise_entry* getentry( int binfile_fd )  {

  struct exercise_entry *ee = NULL;
  ee = malloc( sizeof( *ee ) );
  uint32_t data[ EE_ENTRY_NUMBER ];
  if( ee == NULL )  return NULL;
  if( readfd( binfile_fd, data, sizeof data ) == -1 )  {

    free( ee );
    if( errno == 0 )  perror( "Broken file entry - ignoring not complet data" );
    return NULL;

  }

  size_t data_pos = 0;
  ee->exercise = data[ data_pos++ ];
  ee->second_exercise = data[ data_pos++ ];
  ee->year = data[ data_pos++ ];
  ee->day_of_year = data[ data_pos++];
  ee->how_heavy = data[ data_pos++ ];
  ee->series_ammount = data[ data_pos++ ];
  ee->how_hard = data[ data_pos++ ];
  ee->exer_types_count = data[ data_pos++ ];

  return ee;

}

void set_we_days( struct workout_entries *we )  {

  if( we == NULL )  return;
  if( we->ee_count <= 0 )  return;
  
  for( size_t i = 0; i < ( we->ee_count - 1 ); i++ )  {
    
    int day = we->ee[i].day_of_year;
    int year = we->ee[i].year;
    time_t newtime = get_nextday_time( day, year );
    set_ee_time( newtime, &( we->ee[ i + 1 ] ) );

  }

}


struct workout_entries *load_workout( const char *const workout_file_path )  {
  
  dmsg( "Enetring workout_entries function" );
  
  int workout_fd = open( workout_file_path, O_RDONLY );
  if( workout_fd == -1 )  {
	  
    if( errno == ENOENT )  errno = 0;
    return NULL;

  }
  dmsg( "Found workout file" );

  struct workout_entries *we = NULL;
  we = init_we();
  if( we == NULL )  goto closefd;
  dmsg( "Workout entries initalized" );

  if( readfd( workout_fd, &(we->ee_count), sizeof( we->ee_count ) ) == -1 )  {

    if( errno != 0 )  goto freedata;
    return we;

  }
  dmsg( "Workout entires count aquaried" );

  we->ee = malloc( sizeof( *( we->ee ) ) * we->ee_count );
  if( we->ee == NULL )  goto freedata;
  
  for( size_t i = 0; i < we->ee_count; i++ )  {

    struct exercise_entry *temp_ee = getentry( workout_fd );
    
    if( temp_ee == NULL )  {

      if( errno == 0 )  {

	perror( "broken data in exercise file" );
        we->ee_count = i;
	size_t correct_size = i * sizeof *temp_ee;
	temp_ee = malloc( correct_size );
	if( temp_ee != NULL )  {

	  memcpy( temp_ee, we->ee, correct_size );
	  free( we->ee );
	  we->ee = temp_ee;
	  break;

	}

      }

      free( we->ee );
      goto freedata;

    }
    
    memcpy( &( we->ee[i] ), temp_ee, sizeof *temp_ee );
    free( temp_ee );

  } 
  
  close( workout_fd );
  return we;

freedata:
  free( we );
closefd:
  close( workout_fd );
  return NULL;

}


int rmv_outdated_exer( struct workout_entries *we )  {

  uint32_t cur_day = get_day();
  uint32_t cur_year = get_year();

  while( we->ee_count )  {

    if( ( we->ee[0] ).year < cur_year )  {

      if( rmv_first_ee_from_we( we ) == -1 )
        return -1;
      continue;

    }

    if( ( we->ee[0] ).day_of_year < cur_day )  {

      if( rmv_first_ee_from_we( we ) == -1 )
        return -1;
      continue;

    }

    break;

  }

  return 0;

}

// get unused  exercise
int pick_exercise( int( *exercise_array )[2], struct exercise_check *ec )  {

  int exer_pos_val = 1; // must be 1 if all 0.
  int( *ea_end )[2] = exercise_array;
  if( ea_end[0][1] )  return -1;  // error the array has no place left
  while( ( *ea_end )[1] != -1 )  {

    if( ( *ea_end )[1] != 0 )  {

      exer_pos_val = ( *ea_end )[1] + 1;
      break;

    }
    ea_end++;

  }

  ptrdiff_t ea_len = ea_end - exercise_array;
  ec->today_ex2 = EX_NO_EXERCISE;
  if( ea_len == 1 )  {

    ec->today_ex1 = exercise_array[0][0];
    return exer_pos_val;

  }

  ea_len--;
  int roll = roll_num( 0, ea_len );
  ec->today_ex1 = exercise_array[ roll ][0];

  if( ! canmix( exercise_array[ roll ][0] ) )
    return exer_pos_val;
  
  if( get_roll_ans( 80 ) )
    return exer_pos_val;

  roll = roll_num( 0, ea_len );
  if( ec->today_ex1 == exercise_array[ roll ][0] )
    return exer_pos_val;

  if( ! canmix( exercise_array[ roll ][0] ) )
    return exer_pos_val;

  ec->today_ex2 = exercise_array[ roll ][0];
  return exer_pos_val;

}

int swap_element( void *const elem1, void *const elem2, const size_t e_size )  {

  void *space = malloc( e_size );
  if( space == NULL )  return -1;
  memmove( space, elem1, e_size ); // we are safe no matter what we do 
  memmove( elem1, elem2, e_size );
  memmove( elem2, space, e_size );
  free( space );
  return 0;

}

int set_array_val( int ( *exercise_array )[2], int ( *ea_pos )[2], int passed_ex, int ex_val )  {

  drun( fprintf( stderr, "Setting up array with passed_ex = %d -> %s, ex_val = %d\n",
		         passed_ex, getexname( passed_ex ), ex_val ) );
  int ( *cur_ex )[2] = exercise_array;
  while( true )  {

    if( ( *cur_ex )[1] == -1 )  {

      errno = 0;
      return -1;

    }

    if( ( *cur_ex )[0] == passed_ex )  {

      ( *cur_ex )[1] = ex_val;
      if( swap_element( ea_pos, cur_ex, sizeof *cur_ex ) == -1 )
        return -1;

      return 0;
	
    }
    cur_ex++;

  } 

}

int save_exercise( int ( *exercise_array )[2] , struct exercise_check *const ec, const int ex_val ) {

  drun( fprintf( stderr, "Passed ex_val is %d\n", ex_val ) );
  int ( *ea_pos )[2] = exercise_array;
  if( ea_pos[0][1] == -1 )  {
	  
    errno = 0;
    return -1;

  }
  while( ( *ea_pos )[1] != -1 )  ea_pos++; // go to the end

  while( ea_pos >= exercise_array  )  {

    if( ( *ea_pos )[1] == 0 )  {

      if( set_array_val( exercise_array, ea_pos, ec->today_ex1, ex_val ) == -1 )
        return -1;

      if( ec->today_ex1 == ec->today_ex2 )  return 0;
      if( ec->today_ex2 == EX_NO_EXERCISE )  return 0;

      ea_pos--;
      if( ea_pos == exercise_array )  {

        errno = 0;
	return -1;

      }
      if( set_array_val( exercise_array, ea_pos, ec->today_ex2, ex_val ) == -1 )
        return -1;

      return 0;

    }
    ea_pos--;

  }

  errno = 0;
  return -1;

}


struct workout_entries *we_from_ea( int( *exercise_entries )[2] )  {

  struct workout_entries *we = init_we();
  if( we == NULL )  return NULL;

  int ( *cur_ent )[2] = exercise_entries;
  while( ( *cur_ent )[1] != -1 ) cur_ent++; // go at the end of exercise_entires

  cur_ent--;
  while( true )  {

    struct exercise_entry *ee = alloc_fresh_ee();
    if( ee == NULL )  return NULL;

    ee->exercise = ( *cur_ent )[0];
    ee->second_exercise = EX_NO_EXERCISE;

    if( cur_ent != exercise_entries )  {

      if( ( *cur_ent )[1] == ( *( cur_ent - 1 ) )[1] )  {

	cur_ent--;
        ee->second_exercise = ( *cur_ent )[0];

      }

    }

    if( push_ee_to_we( we, ee ) == -1 )  {

      free( ee );
      free_we( we );
      return NULL;

    }
   
    free( ee );
    if( cur_ent == exercise_entries )  return we;
    cur_ent--;

  }

}

// one full circle of exercise will be generated
// If no errno and NULL -> should not happen
struct workout_entries *gen_exercise_row( struct exercise_entry *last_ee )  {

  dmsg( "Entering exercise generator function" );

  int counter = 0;
  struct exercise_entry *last_ent = alloc_fresh_ee();
  if( last_ent == NULL )  return NULL;
  if( last_ee ==  NULL )  {
	
    dmsg( "last entry did not exist" );
  
  } else  memcpy( last_ent, last_ee, sizeof( *last_ent ) );
  drun( print_ee( last_ent ) );

  int ( *exercise_array )[2] = NULL;
  while( true )  {

    dmsg( "Inside main exercise generator loop" );
    counter++;
    exercise_array = make_exercise_array();
    if( exercise_array == NULL )  goto freelastent;
    

    int left = count_exercise();
    drun( fprintf( stderr, "Left exercise: %d\n", left ) ); 
    struct exercise_check ec;
    ec.yesterday_ex1 = last_ent->exercise;
    ec.yesterday_ex2 = last_ent->second_exercise;
    ec.today_ex1 = EX_NO_EXERCISE;
    ec.today_ex2 = EX_NO_EXERCISE;
    drun( print_ec( &ec ) );

    int chances = 3;
    int picked_num = 0;
    while( true )  {

      dmsg( "Inside random exercise generation loop" );
      drun( fprintf( stderr, "Left exercise: %d\n", left ) );
      if( left == 0 )  break;
      picked_num = pick_exercise( exercise_array, &ec );
      drun( fprintf( stderr, "Picked number for exercise: %d\n", picked_num ) );
      if( picked_num == -1 )  {

        errno = 0;  // not std error.
	goto freeexerarr;

      }

      drun( print_ec( &ec ) );
      if( ! check_exercise( &ec ) )  {

	dmsg( "Exercise can't be used" );
        if( chances < 1 )  break;
        chances--;
	continue;

      }

      if( ec.today_ex2 == EX_NO_EXERCISE )
        left -= 1;
      else left -= 2; // we have two exercise
      
      save_exercise( exercise_array, &ec, picked_num );
      drun( print_exarr( exercise_array ) );
      move_exercise( &ec );

    }


    if( left == 0 )  break;
    free( exercise_array );
    // seems we could not generate our *we* so we will restart

  }

  struct workout_entries *we = init_we();
  if( we == NULL ) goto freeexerarr;
  if( ( we = we_from_ea( exercise_array ) ) == NULL )  goto freeexerarr;
  
  free( exercise_array );
  free( last_ent );
  return we;

freeexerarr:
  free( exercise_array );
freelastent:
  free( last_ent );
  return NULL;

}

int main( void )  {

  drun( stdout_nobuf() );
  
  // seed randomness useless on obsd but other systems respect
  // on obsd could be simply srand -> deterministic.
  srand( ( unsigned int ) time( NULL ) ); 
  dmsg( "Seed was set" );

  char *const workout_file_path = get_homepath( WORKOUT_FILE_NAME );
  if( workout_file_path == NULL )  fail( "Failed on path string creation" );
  dmsg( "Created work file name:" );
  dmsg( workout_file_path );

  struct workout_entries *we = NULL;
  we = load_workout( workout_file_path );
  dmsg( "workout file loaded" );
  if( we == NULL ) {
    
    if( errno != 0 )  fail( "Workout entries loading fail" );

    dmsg( "There was no workout file" );
    if( create_workout_file( workout_file_path ) == -1 )
      fail( "Can't create workour file" );
    dmsg( "Created workout file" );
    if( ( we = init_we() ) == NULL )  fail( "Could not init we" );
    dmsg( "Workout entries initalized" );

  } 

  dmsg( "Workout file checked" );

  if( rmv_outdated_exer( we ) == -1 )  {

    free_we( we );
    fail( "Falure on removing outdated exercise" );

  }

  dmsg( "Outdated exercise where removed" );

  while( we->ee_count < 30 )  {

    drun( fprintf( stderr, "Inside workout entries generator,\n"\
			   "currently we have: %" PRIu32 "\n", we->ee_count ) );
    struct exercise_entry *last_ee = NULL;
    if( we->ee_count > 0 )  last_ee = &( we->ee[ we->ee_count - 1 ] );
    struct workout_entries *exercise_row = NULL;
    dmsg( "Exercise generating" );
    exercise_row = gen_exercise_row( last_ee );
    if( exercise_row == NULL )  {

      free_we( we );
      fail( "Failure on generation of exercise" );

    }

    dmsg( "\nAdding generated exercise to workout entries" );
    if( push_we_to_we( we, exercise_row ) == -1 )  {

      free_we( we );
      free_we( exercise_row );

    }

    set_we_days( we );
    drun( print_we( we ) );
    free( exercise_row );

  }

  dmsg( "\n\nLoop for exercise generation was left" );
  drun( print_we( we ) );

  if( write_we( we, workout_file_path ) == -1 )  {

    free_we( we );
    fail( "Fail on writing workout entries to file" );

  }
  drun( fprintf( stderr, "At end workout entries have: %" PRIu32 " entries\n", we->ee_count ) );
  
  puts( "Today exercise is:" );
  print_ee( we->ee );
  free_we( we );

}
