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
#include <unistd.h>

#define USER_AGENT "--user-agent='Mozilla/5.0" \
" (Windows NT 10.0; Win64; x64; " \
"rv:79.0) Gecko/20100101 Firefox/79.0'"

#define WGET "torsocks wget " USER_AGENT "-q -O"

#define PROXY "-e use_proxy=yes https_proxy="

#define ANDROID_LIST "https://developers.what" \
"ismybrowser.com/useragents/explore/" \
"operating_system_name/android/"
#define IOS_LIST "https://developers.what" \
"ismybrowser.com/useragents/explore/" \
"operating_system_name/ios/"
#define WINDOWS_LIST "https://developers.what" \
"ismybrowser.com/useragents/explore/" \
"operating_system_name/windows/"
#define LINUX_LIST "https://developers.what" \
"ismybrowser.com/useragents/explore/" \
"operating_system_name/linux/"
#define MACOSX_LIST "https://developers.what" \
"ismybrowser.com/useragents/explore/" \
"operating_system_name/mac-os-x/"
#define MACOS_LIST "https://developers.what" \
"ismybrowser.com/useragents/explore/" \
"operating_system_name/macos/"
#define SYMBIAN_LIST "https://developers.what" \
"ismybrowser.com/useragents/explore/" \
"operating_system_name/symbian/"
#define FIREOS_LIST "https://developers.what" \
"ismybrowser.com/useragents/" \
"explore/operating_system_name/fire-os/"
#define CHROMEOS_LIST "https://developers.what" \
"ismybrowser.com/useragents/" \
"explore/operating_system_name/chrome-os/"

#define BUFF_SIZE 8192

void fail( char *estr )  {

  perror( estr );
  exit( EXIT_FAILURE );

}

int extract_ua( char *filename, char *buff ) {

  FILE *webpage = fopen( filename, "r" );
  if( webpage == NULL )  return -1;

  errno = 0;
  while( fgets( buff, BUFF_SIZE, webpage ) != NULL )  {

    char *start = strstr( buff, "class=\"useragent\"><a href=" );
    if( start == NULL )  continue;

    start = strchr( start, '>' );
    if( start == NULL )  continue;
    start++;
    start = strchr( start, '>' );
    if( start == NULL )  continue;
    start++;

    char *end = strchr( start, '<' );
    if( end == NULL )  continue;
    *end = '\0';

    puts( start );

  }

  if( errno )  return -1;
  fclose( webpage );

  return 0;

}

int main( int argc, char *argv[]  )  {

  if( argc != 2 )  fail( "You must provide ip:port string" );

  char *tempname = tmpnam( NULL );
  if( tempname == NULL )  fail( "Could not get temp name" );

  char buff[ BUFF_SIZE ];
  memset( buff, 0, BUFF_SIZE );

  sprintf( buff, "%s '%s' %s%s '%s'", WGET, tempname, PROXY, argv[1],  ANDROID_LIST );
  system( buff );
  extract_ua( tempname, buff );
  sleep( 10 );
  /* They will ban if you download too fast, possibly they will ban
   * after time anyway, so we are forced to use tor I understand they are
   * afraid of scrapping but it's not purpose of this program,
   * it's for grabbing only a little UA most popular list.
   * GOOD */


  sprintf( buff, "%s '%s' %s%s '%s'", WGET, tempname, PROXY, argv[1], IOS_LIST );
  system( buff );
  extract_ua( tempname, buff );
  sleep( 10 );

  sprintf( buff, "%s '%s' %s%s '%s'", WGET, tempname, PROXY, argv[1], WINDOWS_LIST );
  system( buff );
  extract_ua( tempname, buff );
  sleep( 10 );

  sprintf( buff, "%s '%s' %s%s '%s'", WGET, tempname, PROXY, argv[1], LINUX_LIST );
  system( buff );
  extract_ua( tempname, buff );
  sleep( 10 );

  sprintf( buff, "%s '%s' %s%s '%s'", WGET, tempname, PROXY, argv[1], MACOSX_LIST );
  system( buff );
  extract_ua( tempname, buff );
  sleep( 10 );

  sprintf( buff, "%s '%s' %s%s '%s'", WGET, tempname, PROXY, argv[1], MACOS_LIST );
  system( buff );
  extract_ua( tempname, buff );
  sleep( 10 );

  sprintf( buff, "%s '%s' %s%s '%s'", WGET, tempname, PROXY, argv[1], SYMBIAN_LIST );
  system( buff );
  extract_ua( tempname, buff );
  sleep( 10 );

  sprintf( buff, "%s '%s' %s%s '%s'", WGET, tempname, PROXY, argv[1], FIREOS_LIST );
  system( buff );
  extract_ua( tempname, buff );
  sleep( 10 );

  sprintf( buff, "%s '%s' %s%s '%s'", WGET, tempname, PROXY, argv[1], CHROMEOS_LIST );
  system( buff );
  extract_ua( tempname, buff );

  remove( tempname );
  return 0;

}
