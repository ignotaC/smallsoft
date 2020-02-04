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


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void fail( char *str_fail )  {

  perror( str_fail );
  exit( EXIT_FAILURE );

}

int main( void )  {

  if( setvbuf( stdin, NULL, _IONBF, 0 ) != 0 )
    fail( "Could not set stdin buff" );

  char *line = NULL;
  size_t linesize = 0;
  long long int cur_time;
  while( getline( &line, &linesize, stdin ) != -1 )  {

    cur_time = ( long long int ) time(NULL);
    printf( "%lld %s", cur_time, line );
    free( line );
    line = NULL;

  }
  
  if( ferror( stdin ) )  fail( "Failed on getline function" );
  return 0;

}
