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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFF_SIZE;
char buff[ BUFF_SIZE ];
char *html_name;
char **dirs;
FILE *conf_file;
long long int line_count = 1;
int cmdinbuff;

struct link  {

  char *name;
  char *file_name;

};

struct section {

  char *name;
  struct link *links;

} section;


void wrong_input( void )  {

  puts( "Please provide file name of config file generation" );
  puts( "Blank lines and lines begining with # in generation file are ignored" );
  puts( "\nExample of setting up output name file:" );
  puts( "HTMLNAME" );
  puts( "Name of generated html file" );
  puts( "\nExample of entry in generating file:" );
  puts( "ENTRY" );
  puts( "Entry name that will be visible in html file" );
  puts( "Name of file that is the entry" );
  puts( "Section.name" );
  puts( "Next Section name" );
  puts( "You can provide as many sections as you want" );
  puts( "\nExample of path where files will be searched:" );
  puts( "PATH" );
  puts( "Directory name" );
  puts( "Directory name" );
  puts( "Next directory name" );
  exit( EXIT_SUCCESS );

}

void failline( void )  {

  fprintf( stderr, "Problem at line %lld", linecount );
  exit( EXIT_FAILURE );

}

void fail( str_err )  

  perror( str_err );
  abort();

}

void nonewline( void )  {

  char *bp = buff;
  while( *bp != '\0' )  {

    if( *bp == '\n' )  {

      *bp = '\0';
      return;

    }

    bp++;

  }

}
int ignore_line( void )  {

  if( buff[0] == '#' )  return 1;
  char bp = buff;
  for(; *bp != '\0'; bp++ )
    if( ! isspace( bp ) )  return 0;
  return 1;

}

int getbfline( void )  {

  for(;;)  {

    linecount++;
    if( fgets( buff, BUFF_SIZE, conf_file ) == NULL )  {

      if( fgets == NULL )  {

        if( ferror( conf_file ) )
          fail( "Failed on reading conf_file" );
	return 1;

      }

      if( ! ignore_line() ) return 0;
      if( strstr( buff, "\n" ) == NULL )  {

        fprintf( stderr, "Too long line" );
	linefail();

      }
      nonewline();

    } 

  }

}



void in_getbfline( void )  {

  if( getbufline() )  {  

    fprintf( stderr, "Missing line\n" );	  
    failline();

  }

}

struct command  {

  char *name
  void ( *function )( void );

}

void iscommand( void );

void setname( void )  {

  if( html_name != NULL )  {

    fprintf( stderr, "There can be only one name provided\n" \
		     "Second definition here:" );
    linefail();

  }

  in_getbfline();
  size_t string_size = 1 + strlen( buff );
  html_name = malloc( string_size );
  if( html_name == NULL )  fail( "Failed on allocating html_name" );
  strcpy( html_name, buff );
  atexit( free( html_name ) );
  cmdinbuff = 0;

}

size_t paths_count( void )  {

  size_t count = 0;
  if( dirs == NULL )  return count;
  for(; dirs[ count ] != NULL; count++ );
  return count;

}


void savepath( void )  {

  int path_loaded = 0;

  for(;;)  {
  
    if( ! getbfline() )  break;
    if( iscommand() )  {

      cmdinbuff = 1;
      break

    }
    
    path_loaded++;
    size_t pc = paths_count();
    dirs = realloc( dirs, ( pc + 2 ) * sizeof *dirs );
    if( dirs == NULL )  fail( "Failed on reallocing dirs" );
    dirs[ pc ] = malloc( strlen( buff ) + 1 );
    strcpy( dirs[ pc ], buff );
    dirs[ pc + 1 ] = NULL;

  }

  if( path_loaded == 0 )  {

    fprintf( "At least one directory name must be specified" );
    linefail();

  }

}

void loadent( void )  {

  
  in_getbfline();
  struct link *lptr = malloc( sizeof *lptr );
  if( lptr == NULL )  fail( "Failed on Allocating link" );
  lptr->name = malloc( strlen( buff ) + 1 );
  if( lptr->name == NULL )  fail( "Failed on Alocating link name" );
  strcpy( lptr->name, buff );
  in_getbfline();
  lptr->file_name = malloc( strlen( buff ) + 1 );
  if( lptr->file_name == NULL )  fail( "Failed on file name allocation" );

  int sec_count = 0;

  for(;;)  {

    if( !getbfline() )  break;
    if( iscommand() )  {

      cmdinbuff = 1;
      break

    }

    add_section();
 
  }

  if( sec_count == 0 )  {

    fprintf( stderr, "Entry must have at least one section" );
    linefail();

  }

}

struct command cmd[] =  {

  { "PATH", savepath },
  { "ENTRY", loadent },
  { "HTMLNAME", setname }

}

int cmdlen( void )  {

  return sizeof cmd / sizeof *cmd;

}

int iscommand( void )  {

  for( size_t i = 0; i < cmdlen(); i++ )  {

    if( ! strncmp( cmd[i].name, buff, strlen( cmd[i].name ) ) )
      return 1;

  }

  return 0;

}

void chkcommand( void )  {

  if( ! iscommand() )  {

    fprintf( stderr, "Wrong command used" );
    linefail();

  }	 

}


void usecmd( void )  {

  

}


int main( int argc, char *argv[] )  {

  if( argc != 2 )  wrong_input();

  conf_file = fopen( argv[1] );
  if( conf_file == NULL )  fail( "Failed on opening configure file" );

  for(;;)  {

    if( ! cmdinbuff )  {
	    
      if( ! getfbline() )  break;

    }
    usecmd();

  }

  // check html name before creating

}
