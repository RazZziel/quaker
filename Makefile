CC=g++
SRC=main.cpp CEntity.cpp CModel.cpp CScene.cpp CVector.cpp CShader.cpp vector.c
OUT=quaker
CFLAGS=-Wall -ansi -std=c++0x -g3
LDFLAGS=-lm -lGL -lGLU -lglut -lCg -lCgGL `sdl-config --cflags --libs`

all:
	make wave.asm -C cg
	make wave.asm2 -C cg
	$(CC) ${SRC} ${CFLAGS} -o ${OUT} ${LDFLAGS}

run: all
	./${OUT}

clean:
	@echo Cleaning up...
	@rm -f ${OUT}
	make clean -C cg
	@echo Done.
