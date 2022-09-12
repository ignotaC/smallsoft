/*

Copyright (c) 2021 Piotr Trzpil  p.trzpil@protonmail.com

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
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main( void )  {

  puts( "Welcome to miodpitny ( mead ) proportions" );

#define BUFF_SIZE 1024
  char miodpitny_usrvol[ BUFF_SIZE ] = {0};
  char format[ BUFF_SIZE ] = {0};
  // +1 for nul

  puts( "Tell me the miodpitny ( eng. - MEAD )  volume, I will count proportions.." );
  puts( "Honey and water are in liters [l], fosfamon = nutrient medium for yeast in grams [g]" );
  puts( "Lemons are simply counted - it's quantity" );
  printf( "Miodpitny volume in liters: " );

  snprintf( format, BUFF_SIZE - 1, "%%%ds", BUFF_SIZE - 1 );
  scanf( format,  miodpitny_usrvol );
  size_t miodpitny_usrvol_len = strlen( miodpitny_usrvol );

  int dot = 0;
  for( size_t i = 0; i < miodpitny_usrvol_len; i++ )  {

    if( isdigit( miodpitny_usrvol[i] ) )  continue;
    if( miodpitny_usrvol[i] == '.' ) {

      // can't have two dots
      if( dot )  goto broken_usrent;
      dot++;
      continue;

    }

    if( miodpitny_usrvol[i] == '\0' )  break;
    
broken_usrent:
    fprintf( stderr, "Broken mead liters number\n" );
    exit( EXIT_FAILURE );
 
  }

  double honey = 0.0, water = 0.0, lemon_ammount = 0.0, fosfamon = 0.0;
  errno = 0;
  double  miodpitny_volume = strtod( miodpitny_usrvol, NULL );
  if( errno )  {

    perror( "Error on strtod" );
    exit( EXIT_FAILURE );

  }

  puts( "\nPoltorak:" );
  honey = miodpitny_volume / 1.5;
  water = miodpitny_volume - honey;
  lemon_ammount = 3.5 * miodpitny_volume / 3;
  fosfamon = 0.3 * miodpitny_volume;
  printf( "%.2f[l] honey + %.2f[l] water\n", honey, water );
  printf( "Squeeze juice from %.2f lemons\n", lemon_ammount );
  printf( "%.2f[g] fosfamon\n", fosfamon );
  puts( "Keep it in ballon 12 years before putting into bottles\n" );

  puts( "Dwojniak:" );
  honey = miodpitny_volume / 2;
  water = miodpitny_volume - honey;
  lemon_ammount = 3 * miodpitny_volume / 3;
  fosfamon = 0.35 * miodpitny_volume;
  printf( "%.2f[l] honey + %.2f[l] water\n", honey, water );
  printf( "Squeeze juice from %.2f lemons\n", lemon_ammount );
  printf( "%.2f[g] fosfamon\n", fosfamon );
  puts( "Keep it in ballon 8 years before putting into bottles\n" );

  puts( "Trojniak:" );
  honey = miodpitny_volume / 3;
  water = miodpitny_volume - honey;
  lemon_ammount = 2.5 * miodpitny_volume / 3;
  fosfamon = 0.4 * miodpitny_volume;
  printf( "%.2f[l] honey + %.2f[l] water\n", honey, water );
  printf( "Squeeze juice from %.2f lemons\n", lemon_ammount );
  printf( "%.2f[g] fosfamon\n", fosfamon );
  puts( "Keep it in ballon 4 years before putting into bottles\n" );

  puts( "Czworniak:" );
  honey = miodpitny_volume / 4;
  water = miodpitny_volume - honey;
  lemon_ammount = 2 * miodpitny_volume / 3;
  fosfamon = 0.45 * miodpitny_volume;
  printf( "%.2f[l] honey + %.2f[l] water\n", honey, water );
  printf( "Squeeze juice from %.2f lemons\n", lemon_ammount );
  printf( "%.2f[g] fosfamon\n", fosfamon );
  puts( "Keep it in ballon one year before putting into bottles\n" );

  puts( "Fosfamon should not be ignred" );
  puts( "Lemon is better to be added after effervescent fermentation" );
  puts( "Good luck and be patient" );

}
