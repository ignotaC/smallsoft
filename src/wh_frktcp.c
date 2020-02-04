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

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void error_msg( const char *const estr )  {

  perror( estr );
  exit( EXIT_FAILURE );

}

int whiteline( const char *line )  {

  for(;; line++)  {

    if( isspace( *line ) )  continue;
    if( *line == 0 )  return 1;
    return 0;

  }

}

int main( void )  {

  puts( "Welcome to fork torsocks process." );
  puts( "Type in number of forks." );
  int fork_num;
  if( scanf( "%d", &fork_num ) != 1 )  error_msg( "Sacanf fail" );
  if( ( fork_num <= 0 ) )  error_msg( "fork number value is not acceptable" );
  
  size_t linebuff_size = 8192; 
  char linebuff[ linebuff_size ];
  
  puts( "\nPut in command line to fork, torsocks is added by default" );
  puts( "For example: wget -q 'www.site.com'" );
  puts( "Remember to qoute website link, torsocks part is added as default always" );
  puts( "So no need to type it" );
  strncpy( linebuff, "torsocks ", linebuff_size );
  size_t usedsize = strnlen( linebuff, linebuff_size );
  while( 1 )  {
  
    if( fgets( linebuff + usedsize, linebuff_size - usedsize, stdin ) == NULL )
      error_msg( "fgets fail" );

    // get rid of LF garbage
    if( whiteline( linebuff + usedsize ) )  continue;
    break;

  }

  puts( linebuff );
  puts( "Are you fine with this? If *yes* press y," );
  puts( "anything other is *no* and progrm will exit" );

  char ans;
  scanf( "%c", &ans );
  if( ans != 'y' )  {

    puts( "Exiting..." );
    return 0;

  }

  puts( "Populating children" );

  for( int i = 0; i < fork_num; i++ )  {

    pid_t forkpid = fork();
    if( forkpid == -1 )  error_msg( "fork creation fail" );
    if( forkpid == 0 )  {

      // child work here;
      for(;;)  system( linebuff );
      return 0;

    }

  }

  puts( "Parent is exiting now, children will loop their job" );

  return 0;

}
