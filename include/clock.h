#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <ncurses.h>
#include <unistd.h>

#ifndef __CLOCK_H__
#define __CLOCK_H__
#include "list_io.h"
#include "mutex.h"
#include "audio_player.h"
#endif /*__CLOCK_H__*/


#define ALARM_LIST_SIZE 30

extern time_t alarm_list_epochtime[ALARM_LIST_SIZE];

void *get_current_time(void *arg);