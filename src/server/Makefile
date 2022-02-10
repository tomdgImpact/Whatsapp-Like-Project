# Simple Makefile
# Compilers vars
CC=gcc -fsanitize=address
CPPFLAGS= -MMD
CFLAGS= -std=c99 -Wall -Wextra -pthread -O3
LDFLAGS= -pthread
LDLIBS=

SRC = server_tools.c server.c ../encryption/diffie-hellman.c
DEP= ${SRC:.c=.d}
PRG= ${SRC:.c=.o}

all: server

server: ${PRG}

-include ${DEP}

clean:
	rm -f *.o
	rm -f ${DEP}
	rm -f ${PRG}
	rm -f server

# END Makefile
