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
	mkdir -p ${PROF_DIR}
	cp -rp ${RES_DIR} ${BUILD_DIR}

compile:
	${CC} ${FLAGS} ${LIBS} ${O_RELEASE}

debug:
	${CC} ${FLAGS} ${LIBS} ${O_DEBUG}
	gdb ./${BUILD_DIR}/${PROG_NAME}

debug_profile:
	${CC} ${FLAGS} ${LIBS} ${O_DEBUG_PROFILE}
	./${BUILD_DIR}/${PROG_NAME}
	gprof -b ./${BUILD_DIR}/${PROG_NAME} gmon.out > ./${PROF_DIR}/analysis.out
	mv gmon.out ./${PROF_DIR}

debug_mem:
	${CC} ${FLAGS} ${LIBS} -O2 -g
	valgrind --tool=memcheck --leak-check=full --leak-resolution=med --track-origins=yes ${BUILD_DIR}/${PROG_NAME}

run:
	./${BUILD_DIR}/${PROG_NAME}
