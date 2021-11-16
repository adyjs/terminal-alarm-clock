#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <ncurses.h>
#include <errno.h>

#ifndef __TERMINAL_ALARM_CLOCK_H__
#define __TERMINAL_ALARM_CLOCK_H__
#include "../include/terminal_alarm_clock.h"
#include "../include/clock.h"
#include "../include/audio_player.h"
#endif /*__TERMINAL_ALARM_CLOCK_H__*/


// #define _BSD_SOURCE
// #define A_DAY_SEC (86400)
// #define BUF_SIZE 10
// #define ALARM_LIST_SIZE 30
// #define ALARM_STRING_SIZE 10

// char current_time_formatted_string[100];
// char *alarm_list[ALARM_LIST_SIZE] = {NULL};
// time_t alarm_list_epochtime[ALARM_LIST_SIZE] = {0};
// int is_alarm_list_read = 0;

// pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


time_t setted_time_to_epochtime_format(char *alarm_setted_time_string){
    struct tm t;
    time_t setted_time_in_sec;
    char week[20], time_zone[20];
    int year, month, day, h, m, s;
    int DST = 0;   // daylight saving time, yes = 1, no = 0, unknown = -1
    int status;

    // scan format : 2021-11-05 Friday 01:15:19 CST
    status = sscanf(
        current_time_formatted_string, 
        "%d-%d-%d %s %d:%d:%d %s", 
        &year, &month, &day, week, &h, &m, &s, time_zone
    );
    if(status != 8){
        printf("status : %d\n", status);
        printf("ERROR sscanf current_time_formatted_string failed, %s, %d\n", __FUNCTION__, __LINE__);
        exit(1);
    }
    // scan format : 01:15:19
    status = sscanf(alarm_setted_time_string, "%d:%d:%d", &h, &m, &s);
    if(status != 3){
        printf("status : %d\n", status);
        printf("ERROR sscanf alarm_setted_time_string failed, %s, %d\n", __FUNCTION__, __LINE__);
        exit(1);
    }
    t.tm_year = year-1900;  // Year - 1900
    t.tm_mon = month - 1;   // month start from 0 == Jan
    t.tm_mday = day;        
    t.tm_hour = h;
    t.tm_min = m;
    t.tm_sec = s;
    t.tm_isdst = DST;       
    setted_time_in_sec = mktime(&t);
    // printf("t of day : %ld\n", setted_time_in_sec);
    return setted_time_in_sec;
}

time_t clocktime_to_sec(char *clocktime){
    int h,m,s;
    int status = sscanf(clocktime, "%d:%d:%d", &h, &m, &s);
    if(status != 3){
        printf("ERROR clock to sec parse failed, %s, %d\n", __FUNCTION__, __LINE__);
        exit(1);
    }
    return ((h*3600) + (m*60) + s);
}

void alarm_list_sorting(){
    unsigned long int temp_time_1, temp_time_2;
    char temp[10] = {'\0'};
    for(int i=0 ; i<alarm_count-1 ; i++){
        for(int j=0 ; j<alarm_count-1-i ; j++){
            temp_time_1 = clocktime_to_sec(alarm_list[j]);
            temp_time_2 = clocktime_to_sec(alarm_list[j+1]);
            if(temp_time_1 > temp_time_2){
                strcpy(temp, alarm_list[j]);
                strcpy(alarm_list[j], alarm_list[j+1]);
                strcpy(alarm_list[j+1], temp);
            }
        }

    }
}

int main(int argc, char *argv[]){
    initscr();
    curs_set(0);

    pthread_t time, read_alarm_list;
    int status;

    status = pthread_create(&time, NULL, get_current_time, NULL);
    if(status != 0){
        printf("ERROR pthread_create get_current_time failed\n");
        exit(1);
    }

    status = pthread_create(&read_alarm_list, NULL, read_list_routine, NULL);
    if(status != 0){
        printf("ERROR pthread_create read_list_routine failed\n");
        exit(1);
    }
    
    
    pthread_exit(NULL);
    endwin();   
    return 0;
}