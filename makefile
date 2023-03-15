DBG=-DNDEBUG
CC=cc -Wall -Wextra -pedantic -O2 -std=c99 -D_POSIX_C_SOURCE=200809L ${DBG}
CCOBJ=${CC} -c
AR=ar rcs
SRC=src
BIN=bin
IG=ignotalib/src
IG_OBJ=ignota_obj
LIBIG_OBJ=-Lignota_obj
SS_OBJ=ss_obj
make:

	sh code_update.sh

#compile external static libs ( ignotalib submodule )
	mkdir -p ${IG_OBJ}

	${CCOBJ} ${IG}/ig_file/igf_write.c -o ${IG_OBJ}/igf_write.o
	${AR} ${IG_OBJ}/libigf_write.a ${IG_OBJ}/igf_write.o

	${CCOBJ} ${IG}/ig_file/igf_read.c -o ${IG_OBJ}/igf_read.o
	${AR} ${IG_OBJ}/libigf_read.a ${IG_OBJ}/igf_read.o

	${CCOBJ} ${IG}/ig_file/igf_readword.c -o ${IG_OBJ}/igf_readword.o
	${AR} ${IG_OBJ}/libigf_readword.a ${IG_OBJ}/igf_readword.o

	${CCOBJ} ${IG}/ig_file/igf_search.c -o ${IG_OBJ}/igf_search.o
	${AR} ${IG_OBJ}/libigf_search.a ${IG_OBJ}/igf_search.o

	${CCOBJ} ${IG}/ig_file/igf_open.c -o ${IG_OBJ}/igf_open.o
	${AR} ${IG_OBJ}/libigf_open.a ${IG_OBJ}/igf_open.o

	${CCOBJ} ${IG}/ig_file/igf_offset.c -o ${IG_OBJ}/igf_offset.o
	${AR} ${IG_OBJ}/libigf_offset.a ${IG_OBJ}/igf_offset.o

	${CCOBJ} ${IG}/ig_net/ign_unixserv.c -o ${IG_OBJ}/ign_unixserv.o
	${AR} ${IG_OBJ}/libign_unixserv.a ${IG_OBJ}/ign_unixserv.o

	${CCOBJ} ${IG}/ig_net/ign_inetserv.c -o ${IG_OBJ}/ign_inetserv.o
	${AR} ${IG_OBJ}/libign_inetserv.a ${IG_OBJ}/ign_inetserv.o

	${CCOBJ} ${IG}/ig_net/ign_strtoport.c -o ${IG_OBJ}/ign_strtoport.o
	${AR} ${IG_OBJ}/libign_strtoport.a ${IG_OBJ}/ign_strtoport.o

	${CCOBJ} ${IG}/ig_event/igev_signals.c -o ${IG_OBJ}/igev_signals.o
	${AR} ${IG_OBJ}/libigev_signals.a ${IG_OBJ}/igev_signals.o

	${CCOBJ} ${IG}/ig_print/igp_double.c -o ${IG_OBJ}/igp_double.o
	${AR} ${IG_OBJ}/libigp_double.a ${IG_OBJ}/igp_double.o

	${CCOBJ} ${IG}/ig_memory/igm_search.c -o ${IG_OBJ}/igm_search.o
	${AR} ${IG_OBJ}/libigm_search.a ${IG_OBJ}/igm_search.o

	${CCOBJ} ${IG}/ig_math/igmath_geopos.c -o ${IG_OBJ}/igmath_geopos.o
	${AR} ${IG_OBJ}/libigmath_geopos.a ${IG_OBJ}/igmath_geopos.o

	${CCOBJ} ${IG}/ig_file/igf_opt.c -o ${IG_OBJ}/igf_opt.o
	${AR} ${IG_OBJ}/libigf_opt.a ${IG_OBJ}/igf_opt.o

	${CCOBJ} ${IG}/ig_datastructure/igds_strarr.c -o\
		${IG_OBJ}/igds_strarr.o
	${AR} ${IG_OBJ}/libigds_strarr.a ${IG_OBJ}/igds_strarr.o

	${CCOBJ} ${IG}/ig_file/igf_dir.c -o ${IG_OBJ}/igf_dir.o
	${AR} ${IG_OBJ}/libigf_dir.a ${IG_OBJ}/igf_dir.o

	${CCOBJ} ${IG}/ig_miscellaneous/igmisc_getans.c\
		-o ${IG_OBJ}/igmisc_getans.o
	${AR} ${IG_OBJ}/libigmisc_getans.a ${IG_OBJ}/igmisc_getans.o



#compile and link external libs
	mkdir -p ${BIN}
	mkdir -p ${SS_OBJ}
#1
	${CC} ./${SRC}/coin.c -o ./${BIN}/coin
#2
	${CC} ${SRC}/getproxies.c -o ${BIN}/getproxies # TODO huge revision for future remove wget crap, we have now torgethhtp10
#3
	${CC} ${SRC}/novena.c -o ${BIN}/novena
#4
	${CCOBJ} ${SRC}/workout.c -o ${SS_OBJ}/workout.o
	${CC} ${SS_OBJ}/workout.o ${LIBIG_OBJ} -ligf_read -ligf_write -o ${BIN}/workout 
#5
	${CC} ${SRC}/logtime.c -o ${BIN}/logtime
#6
	${CC} ${SRC}/recev.c -o ${BIN}/recev
#7
	${CC} ${SRC}/playev.c -o ${BIN}/playev
#8
	${CC} ${SRC}/gethostipv.c -o ${BIN}/gethostipv
#9
	${CC} ${SRC}/repeatstr.c -o ${BIN}/repeatstr
#10
	${CC} ${SRC}/lext.c -o ${BIN}/lext
#11
	${CCOBJ} ${SRC}/getlimits.c -o ${SS_OBJ}/getlimits.o
	${CC} ${SS_OBJ}/getlimits.o ${LIBIG_OBJ} -ligp_double -o ${BIN}/getlimits
#12
	${CC} ${SRC}/fstrswp.c -o ${BIN}/fstrswp 
#13
	${CC} ${SRC}/parsekdump.c -o ${BIN}/parsekdump 
#14
	${CC} ${SRC}/rmvmb.c -o ${BIN}/rmvmb
#15
	${CC} ${SRC}/randstr.c -o ${BIN}/randstr
#16	
	${CCOBJ} ${SRC}/sndlog_data.c -o ${SS_OBJ}/sndlog_data.o
	${CC} ${SS_OBJ}/sndlog_data.o ${LIBIG_OBJ} \
	  -lign_unixserv -lign_inetserv -lign_strtoport \
	  -ligev_signals \
	  -o ${BIN}/sndlog_data
#17
	${CC} ${SRC}/httpreq_addcrlf.c -o ${BIN}/httpreq_addcrlf
#18
	${CC} ${SRC}/flinemem.c -o ${BIN}/flinemem
	#TODO make it possible to use OR and AND | & with () 
#19
	${CC} ${SRC}/urlcode.c -o ${BIN}/urlcode
	# TODO add + support with option ofc
#20
#####################################
#22
	${CC} ${SRC}/miodpitny.c -o ${BIN}/miodpitny
#23
#	${CC} -pthread ${SRC}/irclog.c \
	  ${LIBIG_OBJ} -ligf_opt -ligev_signals \
	  -o ${BIN}/irclog
#24
	${CC} ${SRC}/faddlines.c -o ${BIN}/faddlines
#25
	${CC} ${SRC}/cutfile.c -o ${BIN}/cutfile
	#TODO check httphb split for info
#26
	${CC} ${SRC}/htmlentities.c -o ${BIN}/htmlentities
#27
	${CC} ${SRC}/httphb_split.c -o ${BIN}/httphb_split
#  TODO cutfile shoudl do it just let it understand /r/n etc with
#  escape char program should be removed and instead be a script using cutfile
#28
	${CC} ${SRC}/fixedxargs.c -o ${BIN}/fixedxargs
#29
	${CCOBJ} ${SRC}/listwords.c -o ${SS_OBJ}/listwords.o
	${CC} ${SS_OBJ}/listwords.o ${LIBIG_OBJ} -ligf_readword\
		-ligf_open -ligf_offset -ligf_read\
		-o ${BIN}/listwords
#30  TODO I need more advanced searching
	${CCOBJ} ${SRC}/fext.c -o ${SS_OBJ}/fext.o
	${CC} ${SS_OBJ}/fext.o ${LIBIG_OBJ} -ligf_search\
		-ligf_open -ligf_read -ligf_write -ligf_offset\
		-ligm_search\
		-o ${BIN}/fext
#31  readNMEA
	${CCOBJ} ${SRC}/readNMEA.c -o ${SS_OBJ}/readNMEA.o
	${CC} ${SS_OBJ}/readNMEA.o ${LIBIG_OBJ} -ligmath_geopos\
		-lm \
		-o ${BIN}/readNMEA
#
#32  find reserved
#33 version
#fixfilename
	${CCOBJ} ${SRC}/fixfilename.c -o ${SS_OBJ}/fixfilename.o
	${CC} ${SS_OBJ}/fixfilename.o ${LIBIG_OBJ}\
		-ligmisc_getans -ligds_strarr\
		-ligf_dir\
		-o ${BIN}/fixfilename
#

clear:
	rm -f ${BIN}/*
	rm -f ${SS_OBJ}/*
	rm -f ${IG_OBJ}/*
