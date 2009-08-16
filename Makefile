CC=g++
SRC=main.cpp CEntity.cpp CModel.cpp CScene.cpp CVector.cpp vector.c
OUT=quaker
CFLAGS=-Wall -ansi -std=c++0x -g3
LDFLAGS=-lm -lGL -lGLU `sdl-config --cflags --libs`

all:
	$(CC) ${SRC} ${CFLAGS} -o ${OUT} ${LDFLAGS}

run: all
	./${OUT}

clean:
	@echo Cleaning up...
	@rm ${OUT}
	@echo Done.
