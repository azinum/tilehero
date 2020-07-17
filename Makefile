# Makefile

include config.mk

all: prepare compile run

mac: prepare compile_mac run

compile_mac:
	${CC} ${FLAGS} ${LIBS_MAC}

clean:
	rm -dr ${BUILD_DIR}/*

prepare:
	mkdir -p ${BUILD_DIR}
	cp -rp ${RES_DIR} ${BUILD_DIR}

compile:
	${CC} ${FLAGS} ${LIBS}

run:
	./${BUILD_DIR}/${PROG_NAME}
