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

	${CCOBJ} ${IG}/ig_print/igp_double.c -o ${IG_OBJ}/igp_double.o
	${AR} ${IG_OBJ}/libigp_double.a ${IG_OBJ}/igp_double.o

	${CCOBJ} ${IG}/ig_memory/igm_search.c -o ${IG_OBJ}/igm_search.o
	${AR} ${IG_OBJ}/libigm_search.a ${IG_OBJ}/igm_search.o

	${CCOBJ} ${IG}/ig_math/igmath_geopos.c -o ${IG_OBJ}/igmath_geopos.o
	${AR} ${IG_OBJ}/libigmath_geopos.a ${IG_OBJ}/igmath_geopos.o

#compile and link external libs
	mkdir -p ${BIN}
	mkdir -p ${SS_OBJ}
#1
	${CC} ./${SRC}/coin.c -o ./${BIN}/coin
#2
	${CC} ${SRC}/getproxies.c -o ${BIN}/getproxies # TODO huge revision for future remove wget crap etc
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
	${CC} ${SRC}/rmvmb.c -o ${BIN}/rmvmb #TODO - turn allocations to simple buff
#15
	${CC} ${SRC}/randstr.c -o ${BIN}/randstr
#16	
#  We need to remove the socket after gentle fin so it does not sleave shit in system	
#  ^ TODO
	${CCOBJ} ${SRC}/sndlog_data.c -o ${SS_OBJ}/sndlog_data.o # TODO add net socket support
	${CC} ${SS_OBJ}/sndlog_data.o ${LIBIG_OBJ} \
	  -lign_unixserv -lign_inetserv -lign_strtoport \
	  -o ${BIN}/sndlog_data
#17
# THis probably should stay as it is.
	${CCOBJ} ${SRC}/runprog.c -o ${SS_OBJ}/runprog.o
	${CC} ${SS_OBJ}/runprog.o ${LIBIG_OBJ} -lign_unixserv -o ${BIN}/runprog
#18
	${CC} ${SRC}/httpreq_addcrlf.c -o ${BIN}/httpreq_addcrlf
#19
	${CC} ${SRC}/flinemem.c -o ${BIN}/flinemem
#20
	${CC} ${SRC}/urlcode.c -o ${BIN}/urlcode
#21

# PROGRAM REMOVED
#
#22
	${CC} ${SRC}/miodpitny.c -o ${BIN}/miodpitny
#23
	${CC} -pthread ${SRC}/irclog.c -o ${BIN}/irclog
#24
	${CC} ${SRC}/faddlines.c -o ${BIN}/faddlines
#25
	${CC} ${SRC}/cutfile.c -o ${BIN}/cutfile
#26
	${CC} ${SRC}/htmlentities.c -o ${BIN}/htmlentities
#27
	${CC} ${SRC}/httphb_split.c -o ${BIN}/httphb_split
#28
	${CC} ${SRC}/fixedxarg.c -o ${BIN}/fixedxarg
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

clear:
	rm -f ${BIN}/*
	rm -f ${SS_OBJ}/*
	rm -f ${IG_OBJ}/*
