CC=cc -Wall -Wextra -pedantic \
   -O2 -std=c99 -D_POSIX_C_SOURCE=200809L
CCOBJ=${CC} -c
CCOBJ_ND=${CCOBJ} -DNDEBUG
AR=ar rcs
SRC=src
BIN=bin
IG=ignota/src
IG_OBJ=ignota_obj
LIBIG_OBJ=-Lignota_obj

make:

	${CCOBJ_ND} ${IG}/ig_fileio/igf_write.c -o ${IG_OBJ}/igf_write.o
	${AR} ${IG_OBJ}/libigf_write.a ${IG_OBJ}/igf_write.o

	${CCOBJ_ND} ${IG}/ig_fileio/igf_read.c -o ${IG_OBJ}/igf_read.o
	${AR} ${IG_OBJ}/libigf_read.a ${IG_OBJ}/igf_read.o

	mkdir -p bin

	${CC} ${SRC}/coin.c -o ${BIN}/coin

	${CC} ${SRC}/getproxies.c -o ${BIN}/getproxies # TODO huge revision for future remove wget crap etc

	${CC} ${SRC}/novena.c -o ${BIN}/novena

	${CCOBJ} ${SRC}/workout.c -o ${BIN}/workout.o
	${CC} ${BIN}/workout.o ${LIBIG_OBJ} -ligf_read -ligf_write -o ${BIN}/workout 

	${CC} ${SRC}/logtime.c -o ${BIN}/logtime

	${CC} ${SRC}/recev.c -o ${BIN}/recev

	${CC} ${SRC}/playev.c -o ${BIN}/playev

	${CC} ${SRC}/strext.c -o ${BIN}/strext

	${CC} ${SRC}/linsaftrdiff.c -o ${BIN}/linsaftrdiff
	${CC} ${SRC}/fstrswp.c -o ${BIN}/fstrswp
	${CC} ${SRC}/parsekdump.c -o ${BIN}/parsekdump
	${CC} ${SRC}/ualist.c -o ${BIN}/ualist
	${CC} ${SRC}/randstr.c -o ${BIN}/randstr
	${CC} ${SRC}/logdata.c -o ${BIN}/logdata
	${CC} ${SRC}/runprog.c -o ${BIN}/runprog
	${CC} ${SRC}/httpreq_addcrlf.c -o ${BIN}/httpreq_addcrlf
	${CC} ${SRC}/flinemem.c -o ${BIN}/flinemem
	${CC} ${SRC}/urlcode.c -o ${BIN}/urlcode
	${CC} ${SRC}/givetask.c -o ${BIN}/givetask
	${CC} ${SRC}/miodpitny.c -o ${BIN}/miodpitny
	${CC} -pthread ${SRC}/irclog.c -o ${BIN}/irclog
	${CC} ${SRC}/faddlines.c -o ${BIN}/faddlines
	${CC} ${SRC}/cutfile.c -o ${BIN}/cutfile
	${CC} ${SRC}/htmlentities.c -o ${BIN}/htmlentities
	${CC} ${SRC}/httphb_split.c -o ${BIN}/httphb_split
	${CC} ${SRC}/fixedxarg.c -o ${BIN}/fixedxarg

clear:
	rm -f ${BIN}/*
	rm -f ${IG_OBJ}/*
