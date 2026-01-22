CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic
INCLUDE_DIR = include
SRCS != find src -name '*.c'
OBJS = ${SRCS:.c=.o}
EXAMPLES = examples

TARGET = pomointer

%.o: %.c
	${CC} -c ${CFLAGS} -I${INCLUDE_DIR} $< -o $@

all: ${TARGET}

${TARGET}: ${OBJS}
	mkdir -p build
	${CC} -o build/${TARGET} ${OBJS}

run:
	build/${TARGET} ${EXAMPLES}/example1.txt

clean:
	rm -rf build ${OBJS}

.PHONY: clean run all
