CC = gcc
CFLAGS = -Wall -Werror -std=c99 -ggdb
LDFLAGS = -lpthread -lncurses -lvlc

SRCs = clock.c list_io.c mutex.c audio_player.c terminal_alarm_clock.c 
# SRCs = $(ls ./*.c) 
OBJs = $(patsubst %.c, %.o, $(SRCs))

build = ../
# vpath: ../
all: terminal_alarm_clock 
	mv terminal_alarm_clock ../

terminal_alarm_clock: $(OBJs)
	$(CC) $(OBJs) -o $@ $(CFLAGS) $(LDFLAGS) && make clean_obj


clock.o: clock.c
	$(CC) -c clock.c

audio_player.o:	audio_player.c
	$(CC) -c audio_player.c

list_io.o: list_io.c
	$(CC) -c list_io.c

mutex.o: mutex.c
	$(CC) -c mutex.c



.PHONE:clean

clean_obj:
	rm *.o

clean:
	rm ./terminal_alarm_clock *.o