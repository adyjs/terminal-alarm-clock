CC = gcc
CFLAGS = -Wall -Werror -std=c99 -ggdb
LDFLAGS = -lpthread -lncurses -lvlc

all: terminal_alarm_clock
	
terminal_alarm_clock:	terminal_alarm_clock.c
	${CC} $< -o $@ ${CFLAGS} ${LDFLAGS}

.PHONE:clean

clean:
	rm ./terminal_alarm_clock