CC = gcc
CPPFLAGS = -MMD
CFLAGS = -Wall -Wextra -O0 `pkg-config --cflags gtk+-3.0` -g3
LDFLAGS =
LDLIBS = `pkg-config --libs gtk+-3.0`

SRC = application.c networking.c message_analysis.c
DEP = ${SRC:.c=.d}
PRG = ${SRC:.c=.o}

all: application

application: ${PRG}

-include ${DEP}

clean:
	rm -f ${DEP}
	rm -f ${PRG}
	rm -f application
