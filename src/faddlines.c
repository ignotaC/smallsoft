
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void fail_showmanual( void )  {

  fprintf( stderr, "You need to pass at least two files to program.\n" );
  fprintf( stderr, "Example: faddlines file1 file2\n" );
  exit( EXIT_FAILURE );

}

void fail( const char *const estr )  {

  perror( estr );
  exit( EXIT_FAILURE );

}

int main( const int argc, const char *const argv[] )  {

  if( argc < 3 )
    fail_showmanual();

  const int filenum = argc - 1;
  FILE *filelist[ filenum ];
  for( int i = 0; i < filenum; i++ )  {

    filelist[i] = fopen( argv[ i + 1 ], "r" );
    if( filelist[i] == NULL )  {

      fprintf( stderr, "Problem with file: %s\n", argv[ i + 1 ] );
      fail( "Could not open it" );

    }

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
