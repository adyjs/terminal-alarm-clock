#include <stdlib.h>
#include <pthread.h>


#define A_DAY_SEC (86400)
// #define ALARM_STRING_SIZE 20

#define BUF_SIZE 10

// extern int alarm_count = 0;

// extern char current_time_formatted_string[100];

time_t setted_time_to_epochtime_format(char *);

time_t clocktime_to_sec(char *);

void alarm_list_sorting(void);
