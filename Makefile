CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic
INCLUDE_DIR = include
SRCS != find src -name '*.c'
OBJS = ${SRCS:.c=.o}
EXAMPLES = examples

PREFIX = /usr/local

TARGET = pomointer

%.o: %.c
	${CC} -c ${CFLAGS} -I${INCLUDE_DIR} $< -o $@

all: ${TARGET}

${TARGET}: ${OBJS}
	mkdir -p build
	${CC} -o build/${TARGET} ${OBJS}

run_many:
	build/${TARGET} ${EXAMPLES}/example*

run:
	build/${TARGET} ${EXAMPLES}/example1.txt

install: all
	cp build/${TARGET} ${PREFIX}/bin

uninstall:
	rm ${PREFIX}/bin/${TARGET}

clean:
	rm -rf build ${OBJS}

.PHONY: clean run all
