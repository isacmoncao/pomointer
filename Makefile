CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic
INCLUDE_DIR = include
SRCS != find src -name '*.c'
OBJS = ${SRCS:.c=.o}
EXAMPLES = examples

PREFIX = /usr/local

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
	cp build/${PROGRAM_NAME} ${PREFIX}/bin

uninstall:
	rm ${PREFIX}/bin/${PROGRAM_NAME}

clean:
	rm -rf build ${OBJS}

.PHONY: all run run_many install uninstall clean
