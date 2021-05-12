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

#include <stdio.h>

int main( void )  {

  puts( "Welcome to miodpitny proportions" );

  double honey = 0.0, water = 0.0, lemon_ammount = 0.0, fosfamon = 0.0;
  double  miodpitny_volume = 0.0;

  puts( "Tell me the miodpitny ( eng. - MEAD )  volume, I will count proportions.." );
  puts( "Honey and water are in liters [l], fosfamon in grams [g]" );
  puts( "Lemons are simply counted - it's quantity" );
  printf( "Miodpitny volume in liters: " );
  scanf( "%lf", &miodpitny_volume );
  puts("");

  puts( "Poltorak:" );
  honey = miodpitny_volume / 1.5;
  water = miodpitny_volume - honey;
  lemon_ammount = 3.5 * miodpitny_volume / 3;
  fosfamon = 0.3 * miodpitny_volume;
  printf( "%.2f[l] honey + %.2f[l] water\n", honey, water );
  printf( "Squeeze juice from %.2f lemons\n", lemon_ammount );
  printf( "Possibly %.2f[g] fosfamon\n", fosfamon );
  puts( "Open bottle after 12 years\n" );

  puts( "Dwojniak:" );
  honey = miodpitny_volume / 2;
  water = miodpitny_volume - honey;
  lemon_ammount = 3 * miodpitny_volume / 3;
  fosfamon = 0.35 * miodpitny_volume;
  printf( "%.2f[l] honey + %.2f[l] water\n", honey, water );
  printf( "Squeeze juice from %.2f lemons\n", lemon_ammount );
  printf( "Possibly %.2f[g] fosfamon\n", fosfamon );
  puts( "Open bottle after 8 years\n" );

  puts( "Trojniak:" );
  honey = miodpitny_volume / 3;
  water = miodpitny_volume - honey;
  lemon_ammount = 2.5 * miodpitny_volume / 3;
  fosfamon = 0.4 * miodpitny_volume;
  printf( "%.2f[l] honey + %.2f[l] water\n", honey, water );
  printf( "Squeeze juice from %.2f lemons\n", lemon_ammount );
  printf( "Possibly %.2f[g] fosfamon\n", fosfamon );
  puts( "Open bottle after 4 years\n" );

  puts( "Czworniak:" );
  honey = miodpitny_volume / 4;
  water = miodpitny_volume - honey;
  lemon_ammount = 2 * miodpitny_volume / 3;
  fosfamon = 0.45 * miodpitny_volume;
  printf( "%.2f[l] honey + %.2f[l] water\n", honey, water );
  printf( "Squeeze juice from %.2f lemons\n", lemon_ammount );
  printf( "Possibly %.2f[g] fosfamon\n", fosfamon );
  puts( "Open bottle after 1 year\n" );

  puts( "Fosfamon can be ignored - but use yeast for mead" );
  puts( "Lemon is better to be added on end" );
  puts( "Good luck" );

}
