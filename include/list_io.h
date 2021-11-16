#include <errno.h>
#include <string.h>

#ifndef __LIST_IO_H__
#define __LIST_IO_H__

#include "clock.h"
#include "terminal_alarm_clock.h"


#endif /*__LIST_IO_H__*/


#define ALARM_LIST_SIZE 30

extern char *alarm_list[ALARM_LIST_SIZE];

extern int alarm_count;

extern int is_alarm_list_read;

extern char current_time_formatted_string[100];

int alarm_list_format_validate(char *alarm_list_time_string);

void get_epochtime_alarm_list(void);

void mark_goes_off_one(int index);

void *read_list_routine(void *arg);

void *print_list_routine(void *arg);