CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic
INCLUDE_DIR = include
SRCS != find src -name '*.c'
OBJS = ${SRCS:.c=.o}
EXAMPLES = examples

PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

PROGRAM_NAME = pomointer
VERSION = 1.0.1

%.o: %.c
	${CC} -c ${CFLAGS} -I${INCLUDE_DIR} $< -o $@

all: ${PROGRAM_NAME}

${PROGRAM_NAME}: ${OBJS}
	mkdir -p build
	${CC} -o build/${PROGRAM_NAME} ${OBJS}

run_many:
	build/${PROGRAM_NAME} ${EXAMPLES}/feb*

run:
	build/${PROGRAM_NAME} ${EXAMPLES}/feb1.pf

install: all
	cp -f build/${PROGRAM_NAME} ${PREFIX}/bin
	chmod 755 ${PREFIX}/bin/${PROGRAM_NAME}
	sed  "s/VERSION/${VERSION}/g" < doc/man/man1/pomointer.1 > ${MANPREFIX}/man1/pomointer.1
	sed  "s/VERSION/${VERSION}/g" < doc/man/man5/pomofile.5 > ${MANPREFIX}/man5/pomofile.5
	chmod 644 ${MANPREFIX}/man1/pomointer.1
	chmod 644 ${MANPREFIX}/man5/pomofile.5

dist: clean
	mkdir -p ${PROGRAM_NAME}-${VERSION}
	cp -R LICENSE Makefile README doc examples include src ${PROGRAM_NAME}-${VERSION}
	tar -cf ${PROGRAM_NAME}-${VERSION}.tar ${PROGRAM_NAME}-${VERSION}
	xz ${PROGRAM_NAME}-${VERSION}.tar
	rm -rf ${PROGRAM_NAME}-${VERSION}

uninstall:
	rm ${PREFIX}/bin/${PROGRAM_NAME}
	rm ${MANPREFIX}/man1/pomointer.1 ${MANPREFIX}/man5/pomofile.5

clean:
	rm -rf build ${OBJS}

.PHONY: all run run_many install uninstall clean
