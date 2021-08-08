
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void fail( const char *const estr )  {

  perror( estr );
  exit( EXIT_FAILURE );

}

int main( const int argc, const char *const argv[] )  {

  if( argc < 2 )
    fail( "No files passed to add their lines" );

  const int filenum = argc - 1;
  FILE *filelist[ filenum ];
  for( int i = 0; i < filenum; i++ )  {

    filelist[i] = fopen( argv[ i + 1 ], "r" );
    if( filelist[i] == NULL )
      fail( "Failure on file opening" );

  }

  int canread = 1;
  char *lines[ filenum ];
  // 0 equvalent in C to NULL
  size_t lineslen[ filenum ];
  // we start with zero size;

  for( int i = 0; i < filenum; i++ )  {

    lines[i] = NULL;
    lineslen[i] = 0;

  }

  char *newline_pos = NULL;
  while( canread )  {

    canread = 0;
    for( int i = 0; i < filenum; i++ )  {

      if( getline( &lines[i], &lineslen[i],
          filelist[i] ) == -1 )  {

        if( feof( filelist[i] ) )  continue;
	fail( "Getline failure" );

      }

      canread = 1;
      newline_pos = strchr( lines[i], '\n' );
      if( newline_pos != NULL )
        *newline_pos = '\0';
      
      printf( "%s", lines[i] );
      if( i != filenum - 1 )
        printf( " " );

    }

    printf( "\n" );

  }

  for( int i = 0; i < filenum; i++ )  {

    free( lines[i] );
    fclose( filelist[i] );

  }

  return 0;

}
