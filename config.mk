# config.mk

CC=gcc

PROG_NAME=tilehero

INCLUDE_DIR=include

RES_DIR=resource

SRC=src/*.c

BUILD_DIR=build

LIBS=-lportaudio -lpng -lm -lGLEW -lglfw -lGL

LIBS_MAC=-lportaudio -lpng -lm -lGLEW -lglfw -framework OpenGL

FLAGS=${SRC} -I${INCLUDE_DIR} -o ${BUILD_DIR}/${PROG_NAME} -O2 -Wall -W -Wno-missing-braces -std=c99
