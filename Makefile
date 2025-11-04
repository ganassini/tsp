CC:=gcc
CFLAGS:=-Wall -Wextra -std=c99 -pedantic -g
LIBS:=
BIN:=tsp

SRC:=tsp.c
OBJ:=${SRC:.c=.o}

all: ${OBJ}
	${CC} ${LIBS} ${OBJ} -o ${BIN}

%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@

.PHONY: clean
clean:
	@rm -rfv ${OBJ} ${BIN}
