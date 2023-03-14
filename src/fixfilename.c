/*

Copyright (c) 2023 Piotr Trzpil  p.trzpil@protonmail.com

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

#include "../../ignota/src/ig_file/igf_dir.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argc, char *argv[] )  {

  struct igds_strarr filenames;

  if( igf_getdirfnames( '.', &filenames ) == -1 )
    fail( "Crashed on igf_getdirfnames" );

  for( size_t i = 0; i < filenames.listlen; i++ )  {

    bool has_argstrs = true;
    for( size_t j = 1; j < argc; j++ )  {

      if( strstr( filenames.list[i], argv[j] ) == NULL )
        has_argstrs = false;

      if( ! has_argstrs )  break;

    }

    if( ! has_argstrs )  continue;

    if( printf( "Do you want to rename file: %s\n",
        filenames.list[i] ) > 0 )
      fail( "Printf failure" );

    int ans = get_yn_ans();



  }

  return 0;

}
