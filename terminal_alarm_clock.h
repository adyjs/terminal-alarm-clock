#include <stdlib.h>
#include <pthread.h>


#define A_DAY_SEC (86400)
#define BUF_SIZE 10
#define ALARM_LIST_SIZE 128
#define ALARM_STRING_SIZE 10


char current_time_formatted_string[100];
char *alarm_list[ALARM_LIST_SIZE] = {NULL};
time_t alarm_list_epochtime[ALARM_LIST_SIZE] = {0};
int alarm_count = 0;
int is_alarm_list_read = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


time_t setted_time_to_epochtime_format(char *);

time_t clocktime_to_sec(char *);

void alarm_list_sorting(void);

int alarm_go_off_trigger(time_t);

void *alarm_player_routine(void *);

void *get_current_time(void *);

int alarm_list_format_validate(char *);

void get_epochtime_alarm_list(void);

void *read_list_routine(void *);

void *print_list_routine(void *);
