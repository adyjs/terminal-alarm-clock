#include <stdlib.h>
#include <vlc/vlc.h>
#include <pthread.h>
#include <time.h>

#include "list_io.h"
#include "clock.h"

void *alarm_player_routine(void *arg);

int alarm_goes_off_trigger(time_t epochtime);