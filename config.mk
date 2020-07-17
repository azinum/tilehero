# config.mk

CC=gcc

PROG_NAME=tilehero2

INCLUDE_DIR=src

RES_DIR=resources

SRC=src/*.c

BUILD_DIR=build

LIBS=-lpng -lm -lGLEW -lglfw -lGL

FLAGS=${SRC} -I${INCLUDE_DIR} -o ${BUILD_DIR}/${PROG_NAME} -O2 -Wall -W
