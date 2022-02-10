CC=gcc -fsanitize=address
CPPFLAGS= -MMD
CFLAGS= -Wall -Wextra -std=c99 -O0 -g3
LDFLAGS=
LDLIBS= -lm

SRC = genkey.c encrypt.c decryption.c sha.c diffie-hellman.c test.c
DEP= ${SRC:.c=.d}

all: decryption sha

encrypt: genkey.o encrypt.o

decryption: genkey.o decryption.o

sha: sha.o

genkey: genkey.o

diffie-hellman: diffie-hellman.o

test: test.o
-include ${DEP}

clean:
	rm -f *.o
	rm -f ${DEP}
	rm -f encrypt decryption sha
	rm -f client test

# END Makefile
