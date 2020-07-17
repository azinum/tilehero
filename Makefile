# Makefile

include config.mk

all: prepare compile run

clean:
	rm -dr ${BUILD_DIR}/*

prepare:
	mkdir -p ${BUILD_DIR}
	cp -rp ${RES_DIR} ${BUILD_DIR}

compile:
	${CC} ${FLAGS} ${LIBS}

run:
	./${BUILD_DIR}/${PROG_NAME}
