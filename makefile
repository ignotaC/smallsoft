CC=cc -Wall -Wextra -pedantic -O2 -std=c99
SRC=src
BIN=bin

make:
	${CC} ${SRC}/coin.c -o ${BIN}/coin
	${CC} ${SRC}/getproxies.c -o ${BIN}/getproxies
	${CC} ${SRC}/novena.c -o ${BIN}/novena
	${CC} ${SRC}/workout.c -o ${BIN}/workout
	${CC} ${SRC}/logtime.c -o ${BIN}/logtime
	${CC} ${SRC}/recev.c -o ${BIN}/recev
	${CC} ${SRC}/playev.c -o ${BIN}/playev
	${CC} ${SRC}/wh_frktcp.c -o ${BIN}/wh_frktcp
	${CC} ${SRC}/wh_inchttpext.c -o ${BIN}/wh_inchttpext
	${CC} ${SRC}/wh_httpext.c -o ${BIN}/wh_httpext
	${CC} ${SRC}/linsaftrdiff.c -o ${BIN}/linsaftrdiff
	${CC} ${SRC}/fstrswp.c -o ${BIN}/fstrswp
	${CC} ${SRC}/parsekdump.c -o ${BIN}/parsekdump
	${CC} ${SRC}/ualist.c -o ${BIN}/ualist

clearbin:
	rm ${BIN}/*
