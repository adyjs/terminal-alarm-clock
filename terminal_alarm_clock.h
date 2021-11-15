#include <stdlib.h>
#include <pthread.h>


#define A_DAY_SEC (86400)
#define BUF_SIZE 10
#define ALARM_LIST_SIZE 30
#define ALARM_STRING_SIZE 20



time_t setted_time_to_epochtime_format(char *);

time_t clocktime_to_sec(char *);

void alarm_list_sorting(void);

int alarm_goes_off_trigger(time_t);

void *alarm_player_routine(void *);

void *get_current_time(void *);

int alarm_list_format_validate(char *);

void get_epochtime_alarm_list(void);

void *read_list_routine(void *);

void *print_list_routine(void *);

void mark_goes_off_one(int);