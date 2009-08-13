CC = g++
OUT = lesson37
CFLAGS=-Wall -ansi -std=c++0x
LDFLAGS=-lm -lGL -lGLU `sdl-config --cflags --libs`

all:
	$(CC) lesson37.cpp CVector.cpp  ${CFLAGS} -o ${OUT} ${LDFLAGS}

run: all
	./${OUT}

clean:
	@echo Cleaning up...
	@rm ${OUT}
	@echo Done.
