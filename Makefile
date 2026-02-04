CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic
INCLUDE_DIR = include
SRCS != find src -name '*.c'
OBJS = ${SRCS:.c=.o}
EXAMPLES = examples

PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

PROGRAM_NAME = pomointer

%.o: %.c
	${CC} -c ${CFLAGS} -I${INCLUDE_DIR} $< -o $@

all: ${PROGRAM_NAME}

${PROGRAM_NAME}: ${OBJS}
	mkdir -p build
	${CC} -o build/${PROGRAM_NAME} ${OBJS}

run_many:
	build/${PROGRAM_NAME} ${EXAMPLES}/example*

run:
	build/${PROGRAM_NAME} ${EXAMPLES}/example1.txt

install: all
	cp -f build/${PROGRAM_NAME} ${PREFIX}/bin
	cp -f doc/man/man1/pomointer.1 ${MANPREFIX}/man1/pomointer.1
	cp -f doc/man/man5/pomofile.5 ${MANPREFIX}/man5/pomofile.5

uninstall:
	rm ${PREFIX}/bin/${PROGRAM_NAME}
	rm ${MANPREFIX}/man1/pomointer.1 ${MANPREFIX}/man5/pomofile.5

clean:
	rm -rf build ${OBJS}

.PHONY: all run run_many install uninstall clean
