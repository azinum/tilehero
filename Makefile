# Makefile

include config.mk

all: prepare compile run

mac: prepare compile_mac run

compile_mac:
	${CC} ${FLAGS} ${LIBS_MAC} ${O_RELEASE}

clean:
	rm -dr ${BUILD_DIR}/*

prepare:
	mkdir -p ${BUILD_DIR}
	cp -rp ${RES_DIR} ${BUILD_DIR}

compile:
	${CC} ${FLAGS} ${LIBS} ${O_RELEASE}

debug:
	${CC} ${FLAGS} ${LIBS} ${O_DEBUG}
	gdb ./${BUILD_DIR}/${PROG_NAME}

run:
	./${BUILD_DIR}/${PROG_NAME}
