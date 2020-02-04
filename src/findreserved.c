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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// globals get zero value so no need to init.
int found;
char *lineptr;
char **ignorelist;

void fail( str_e )  {

  perror( str_e );
  exit( EXIT_FAILURE );

}

int chk_ignorelist( char *name )  {

  if( ignorelist == NULL )  return 0;
  size_t i = 0;
  while( ignorelist[i] != NULL )  {

    if( ! strcmp( name, ignorelist[i] ) )  return 1;
    i++;

  }

}

char *getname( char *name_pos )  {

  size_t namelen = 0;
  while( isalnum( name_pos[ namelen ] ) ) namelen++;
  char *full_name = malloc( namelen + 1 ) // 1 is our nul
  if( full_name ==  NULL )  fail( "Getname allocation fail" );
  memcpy( full_name, name_pos, namelen );
  return full_name;

}

void process_found( char *found_pos, char *str_to_find, void( *function )( char*, char* ) )  {

  char *full_name = getname( found_pos );
  size_t namelen = getname( full_name );

  int ans = chk_ignorelist( full_name );
  if( ans == 0 )  {
	  
    add_name( full_name );
    found = 1;

  }

  found_pos += strlen( full_name );
  free( full_name );
  function( found_pos, str_to_find );

}


char *find_suffix_in( char *str_line, char *str_to_find )  {

  char *str_pos = strstr( str_line, str_to_find );
  if( str_pos == NULL )  return NULL;
  if( str_pos == lineptr )  return str_pos;
  if( isalnum( *( str_pos - 1 ) ) )  return NULL;  // it's part of variable already 
  return str_pos;

}

void find_suffix( char *str_line, char *str_to_find )  {

  char str_pos = find_suffix_in( str_line, str_to_find );
  if( str_pos == NULL )  return;
  process_found( str_pos, str_to_find, find_suffix );

}

// IT MUST RESTART IF islower not mached since furter might be also something
void find_suffix_lowalp( char *str_line, char *str_to_find )  {
  
  char *str_pos = NULL;
  for(;;)  {

    str_pos = find_suffix_in( str_line, str_to_find );
    if( str_pos == NULL )  return;
    if( islower( *( str_pos + strlen( str_to_find ) ) ) )  break;
    while( isalnum( *str_line ) ) str_line++;
  
  }

  process_found( str_line, str_to_find, find_suffix_lowalp );

}


void find_suffix_uppalp( char *str_line, char *str_to_find )  {
  
  char *str_pos = find_suffix_in( str_line, str_to_find );
  if( str_pos == NULL )  return;
  if( ! isupper( *( str_pos + strlen( str_to_find ) ) )  return;
  process_found( str_line, str_to_fine, find_suffix_up

}


char *find_suffix_digit( char *str_line, char *str_to_find )  {
  
  char *str_pos = find_suffix_in( str_line, str_to_find );
  if( str_pos == NULL )  return NULL;
  if( isdigit( *( str_pos + strlen( str_to_find ) ) )  return  str_pos;
  return NULL;

}



char *find_suffix_digituppalp( char *str_line, char *str_to_find )  {
  
  char *str_pos = find_suffix_in( str_line, str_to_find );
  if( str_pos == NULL )  return NULL;
  if( isdigit( *( str_pos + strlen( str_to_find ) ) )  return  str_pos;
  if( isupper( *( str_pos + strlen( str_to_find ) ) )  return  str_pos;
  return NULL;

}


int chk_undscr( char *str_line )  {

  if( find_suffix( str_line, "_" ) != NULL )  return 1;
  return 0;

}


int chk_ercode( char *str_line )  {

  if( find_suffix_diggituppalp( str_line, "E" ) != NULL )  return 1;
  return 0;

}

int chk_is( char *str_line )  {

  if( find_suffix_lowalp( str_line, "is" ) != NULL )  return 1;
  return 0;

}

int chk_to( char *str_line )  {

  if( find_suffix_lowalp( str_line, "to" ) != NULL )  return 1;
  return 0;

}

int chk_LC( char *str_line )  {

  if( find_suffix_uppalp( str_line, "LC_" ) != NULL )  return 1;
  return 0;

}

int chk_math( char *str_line )  {

  return 0; // NEEDS A BIT MORE WORK

}

int chk_predirent( char *str_line )  {

  if( find_suffix( str_line, "d_" ) != NULL )  return 1;
  return 0;

}



int chk_prefcntl( char *str_line )  {

  if( find_suffix( str_line, "l_" ) != NULL )  return 1;
  if( find_suffix( str_line, "F_" ) != NULL )  return 1;
  if( find_suffix( str_line, "O_" ) != NULL )  return 1;
  if( find_suffix( str_line, "S_" ) != NULL )  return 1;
  return 0;

}


int chk_pregrp( char *str_line )  {

  if( find_suffix( str_line, "gr_" ) != NULL )  return 1;
  return 0;

}



int chk_prepwd( char *str_line )  {

  if( find_suffix( str_line, "pw_" ) != NULL )  return 1;
  return 0;

}



int chk_presignal( char *str_line )  {

  if( find_suffix( str_line, "sa_" ) != NULL )  return 1;
  if( find_suffix( str_line, "SA_" ) != NULL )  return 1;
  return 0;

}


int chk_prestat( char *str_line )  {

  if( find_suffix( str_line, "st_" ) != NULL )  return 1;
  if( find_suffix( str_line, "S_" ) != NULL )  return 1;
  return 0;

}


int chk_pretimes( char *str_line )  {

  if( find_suffix( str_line, "tms_" ) != NULL )  return 1;
  return 0;

}


int chk_pretermios( char *str_line )  {

  if( find_suffix( str_line, "c_" ) != NULL )  return 1;
  if( find_suffix( str_line, "V" ) != NULL )  return 1;
  if( find_suffix( str_line, "I" ) != NULL )  return 1;
  if( find_suffix( str_line, "O" ) != NULL )  return 1;
  if( find_suffix( str_line, "TC" ) != NULL )  return 1;
  
  return 0;

}

int main( void )  {

  uint64_t count_line = 0;
  for( ;getline( &lineptr, &linesize, stdin ) != NULL; count_line++ )  {

    found = 0;

    chk_undscr( lineptr );
    chk_ercode( lineptr );
    chk_is( lineptr );
    chk_to( lineptr );
    chk_LC( lineptr );
    chk_math( lineptr );
    chk_SIG( lineptr );
    chk_SIG2( lineptr );
    chk_str( lineptr );
    chk_mem( lineptr );
    chk_wcs( lineptr );
    chk_tend( lineptr );
    chk_predirnet( lineptr );
    chk_prefcntl( lineptr );
    chk_pregrp( lineptr );
    chk_suflimits( lineptr );
    chk_prepwd( lineptr );
    chk_presignal( lineptr );
    chk_prestat( lineptr );
    chk_pretimes( lineptr );
    chk_pretermios( lineptr );

    if( found )  printf( "%" PRIu64 ":%s", count_line, lineptr )
    free( lineptr );
    lineptr = NULL;

  }

  if( ferror( stdin ) )  fail( "\n\nERROR on line read\n" );

}
