
DBG=#-DNDEBUG
CC=cc -Wall -Wextra -pedantic -O2 -std=c99 -D_POSIX_C_SOURCE=200809L ${DBG}
CCOBJ=${CC} -c
AR=ar rcs
SRC=../src
BIN=../bin
IG=../ignotalib/src


# TODO try to detect what kind of make is used so we 
# can with some set of macros not create variables for other make files
# set up src path so make can find files.c
# openbsd make
.PATH.c: ../src
# gmake
VPATH=../src


# TODO write in manual what does do
.PHONY: clean clear clearobj cleanobj make setup


# program list
make: coin \
  getproxies \
  novena \
  workout \
  logtime \
  recev \
  playev \
  gethostipv \
  repeatstr \
  lext \
  getlimits \
  fstrswp \
  parsekdump \
  rmvmb \
  randstr \
  sndlog_data \
  httpreq_addcrlf \
  flinemem \
  urlcode \
  sh_getopt \
  miodpitny \
  faddlines \
  cutfile \
  htmlentities \
  httphb_split \
  fixedxargs \
  listwords \
  fext \
  readNMEA \
  getfileinode \
  evalline \
  in2outerr

# setup requred directories and get ignota
setup:
	@git submodule update --init --recursive
	@mkdir -p ${BIN}


# This handles file compilation of source that does not use ignota.
# We copy because the binary is what we check in obj for remake if needed
# so we don't recompile all. After we are dun we can run make clearobj
.SUFFIXES: .o .c


.c:
	${CCOBJ} $< -o $*.o


.o:
	${CC} $< -o $*
	cp $* ${BIN}/$*


workout: libigf_read.a libigf_write.a $@.o
	${CC} $@.o -L. -ligf_read -ligf_write -o $@ 
	cp $@ ${BIN}/$@

gethostipv: libigmisc_opts.a $@.o
	${CC} $@.o -L. -ligmisc_opts -o $@
	cp $@ ${BIN}/$@

getlimits: libigp_double.a $@.o
	${CC} $@.o -L. -ligp_double -o $@
	cp $@ ${BIN}/$@

sndlog_data: libign_unixserv.a libign_inetserv.a \
	libign_strtoport.a libigev_signals.a $@.o
	${CC} $@.o -L. \
		-lign_unixserv -lign_inetserv -lign_strtoport \
		-ligev_signals -o $@
	cp $@ ${BIN}/$@

sh_getopt: libigmisc_opts.a $@.o
	${CC} $@.o -L. -ligmisc_opts -o $@
	cp $@ ${BIN}/$@

# irc logger is revisited and this will wait TODO
#irclog:  irclog.o
#	${CC} irclog.o -pthread \
#	  -L. -ligxxx -o irclog
#	cp $@ ${BIN}/$@
#

# TODO this segfaults
listwords: libigf_readword.a libigf_open.a libigf_offset.a \
	libigf_read.a $@.o
	${CC} $@.o -L. -ligf_readword -ligf_open \
		-ligf_offset -ligf_read -o $@
	cp $@ ${BIN}/$@

fext: libigf_search.a libigf_open.a libigf_read.a \
	libigf_write.a libigf_offset.a \
	libigm_search.a $@.o
	${CC} $@.o -L. -ligf_search -ligf_open \
		-ligf_read -ligf_write -ligf_offset \
		-ligm_search -o $@
	cp $@ ${BIN}/$@

readNMEA: libigmath_geopos.a $@.o
	${CC} $@.o -L. -ligmath_geopos -lm -o $@
	cp $@ ${BIN}/$@

getfileinode: libigf_dir.a libigmisc_getans.a \
	libigds_strarr.a $@.o
	${CC} $@.o -L. -ligf_dir \
		-ligmisc_getans -ligds_strarr -o $@
	cp $@ ${BIN}/$@

in2outerr: libigf_read.a libigf_write.a $@.o
	${CC} $@.o -L. -ligf_read -ligf_write -o $@
	cp $@ ${BIN}/$@


## TODO smallsoft LIST
# 1. getproxies need huge revision for future remove wget crap, we have now torgethhtp10
# 2. gethostipv we need to have working -4 -6 or -6 -4 and also
# option for udp proto etc    and for showing simply
# interface we can connect to so for example ipv6 will be
# not shown etc
# 3. flinemem make it possible to use OR and AND | & with () 
# 4. urlcode add + support with option ofc
# 5. httphb split  should be script using cutfile which must support \r\n
# 6. Upgrade fext I need more advanced searching
# 7. Unfinished programms: find reserved, version
# 8. finish documentation
# 9. hint in dosc how to pick up compiler with make -opt gcc for example
# 10. add to make running readme to html ( only for my use )
# 11. add for my own use ignota update in make


#How to compile external static libs rules ( ignotalib submodule )

libigf_write.a:
	${CCOBJ} ${IG}/ig_file/igf_write.c -o igf_write.o
	${AR} libigf_write.a igf_write.o

libigf_read.a:
	${CCOBJ} ${IG}/ig_file/igf_read.c -o igf_read.o
	${AR} libigf_read.a igf_read.o

libigf_readword.a:
	${CCOBJ} ${IG}/ig_file/igf_readword.c -o igf_readword.o
	${AR} libigf_readword.a igf_readword.o

libigf_search.a:
	${CCOBJ} ${IG}/ig_file/igf_search.c -o igf_search.o
	${AR} libigf_search.a igf_search.o

libigf_open.a:
	${CCOBJ} ${IG}/ig_file/igf_open.c -o igf_open.o
	${AR} libigf_open.a igf_open.o

libigf_offset.a:
	${CCOBJ} ${IG}/ig_file/igf_offset.c -o igf_offset.o
	${AR} libigf_offset.a igf_offset.o

libign_unixserv.a:
	${CCOBJ} ${IG}/ig_net/ign_unixserv.c -o ign_unixserv.o
	${AR} libign_unixserv.a ign_unixserv.o

libign_inetserv.a:
	${CCOBJ} ${IG}/ig_net/ign_inetserv.c -o ign_inetserv.o
	${AR} libign_inetserv.a ign_inetserv.o

libign_strtoport.a:
	${CCOBJ} ${IG}/ig_net/ign_strtoport.c -o ign_strtoport.o
	${AR} libign_strtoport.a ign_strtoport.o

libigev_signals.a:
	${CCOBJ} ${IG}/ig_event/igev_signals.c -o igev_signals.o
	${AR} libigev_signals.a igev_signals.o

libigp_double.a:
	${CCOBJ} ${IG}/ig_print/igp_double.c -o igp_double.o
	${AR} libigp_double.a igp_double.o

libigm_search.a:
	${CCOBJ} ${IG}/ig_memory/igm_search.c -o igm_search.o
	${AR} libigm_search.a igm_search.o

libigmath_geopos.a:
	${CCOBJ} ${IG}/ig_math/igmath_geopos.c -o igmath_geopos.o
	${AR} libigmath_geopos.a igmath_geopos.o

libigf_opt.a:
	${CCOBJ} ${IG}/ig_file/igf_opt.c -o igf_opt.o
	${AR} libigf_opt.a igf_opt.o

libigds_strarr.a:
	${CCOBJ} ${IG}/ig_datastructure/igds_strarr.c -o igds_strarr.o
	${AR} libigds_strarr.a igds_strarr.o

libigf_dir.a:
	${CCOBJ} ${IG}/ig_file/igf_dir.c -o igf_dir.o
	${AR} libigf_dir.a igf_dir.o


libigmisc_getans.a:
	${CCOBJ} ${IG}/ig_miscellaneous/igmisc_getans.c -o igmisc_getans.o
	${AR} libigmisc_getans.a igmisc_getans.o

libigmisc_opts.a:
	${CCOBJ} ${IG}/ig_miscellaneous/igmisc_opts.c -o igmisc_opts.o
	${AR} libigmisc_opts.a igmisc_opts.o


clean clear:
	rm -f ${BIN}/*
	find . -type f ! -name makefile -exec rm -f {} \;

cleanobj clearobj:
	find . -type f ! -name makefile -exec rm -f {} \;



