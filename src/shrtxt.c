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



void bad( char *str_bad )  {

  perror( str_bad );
  exit( EXIT_SUCCESS ); 

}

struct comand  {

  char *namestr;
  size_t count;

}

int main( int ac, char *av[] )  {

  int rmv_newlines = 0;
  int reduce_space = 0;
  struct command *cmd = NULL;
  size_t cmd_len = 0;


  while( int i = 1; i < ac; i++ )  {

    if( av[i][0] != '-' )  bad( "Missing '-' this is not a command" );

    switch( av[i][1] )  {

      case 'n':  // remove new lines
        rmv_newlines = 1;
	break;
      case 's':  // turn multiplie white spaces to one
	reduce_space = 1;
	break;
      case 'i'  // don't do changes between those strings + escape string
	if( ( ac - 1 - i ) < 2 )  // there may be no escape strings.
          bad( "Not enought arguments for ignore command" );
        int isnew = 1;
        for(; i < ac; i++)  {

	  if( av[i][0] != '-' )  bad( "Broken -i command args" );
	  if( av[i][1] != '-' )  {

	    i--;  //so we reread command
	    break;

	  }

	  pushcmd( &cmd, &cmd_len, isnew );
	  isnew = 0;

	}

    }

  }

}
