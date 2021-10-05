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
#include <stdio.h>
#include <stdlib.h>

int main( const int argc, const char *const[] argv  )  {

  if( argc != 3 )  {

    fprintf( stderr, "You need to pass version file and version string\n" );
    fprintf( stderr, "For exanpmple version 'libver' 'X.X.X.X'\n" );
    fprintf( stderr, "This will create file libver with version in it 0.0.0.1\n" );
    fprintf( stderr, "If the file already exist the incrementation of version number\n" );
    fprintf( stderr, "Will look like this: version 'libver' '^^' \n" );
    fprintf( stderr, "Which will cause lower version numbers to get zeroed\n" );
    fprintf( stderr, "And we will get 0.1.0.0\n" );
    fprintf( stderr, "Than we could do this: version 'libver' '^' \n");
    fprintf( stderr, "This will resoult in:  0.1.1.0\n" );
    return -1

  }

}
