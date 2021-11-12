#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <ncurses.h>
#include <errno.h>
#include <vlc/vlc.h>
#include "terminal_alarm_clock.h"

// #define _BSD_SOURCE

// #define A_DAY_SEC (86400)

// #define BUF_SIZE 10
// #define ALARM_LIST_SIZE 30
// #define ALARM_STRING_SIZE 10



char current_time_formatted_string[100];
char *alarm_list[ALARM_LIST_SIZE] = {NULL};
time_t alarm_list_epochtime[ALARM_LIST_SIZE] = {0};
int alarm_count = 0;
int is_alarm_list_read = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


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

int alarm_go_off_trigger(time_t epochtime){
    for(int i=0 ; i<alarm_count ; i++){
        if(alarm_list_epochtime[i] == 0){
            break;
        }
        if(epochtime == alarm_list_epochtime[i]){
            return i;
        }
    }
    return -1;
}


void *alarm_player_routine(void *arg){
    int status = pthread_detach(pthread_self());
    if(status != 0){
        printf("ERROR pthread_detach failed %s %d\n", __FUNCTION__, __LINE__);
        exit(1);
    }
    libvlc_instance_t *inst;
    libvlc_media_player_t *mp;
    libvlc_media_t *m;

    inst = libvlc_new(0, NULL);
    m = libvlc_media_new_path(inst, "./alarm_ringtone/ringtone_example");
    mp = libvlc_media_player_new_from_media(m);
    libvlc_media_release(m);
    libvlc_media_player_play(mp);
    // sleep(10);
    // libvlc_media_player_stop(mp);
    // libvlc_media_player_release(mp);
    // libvlc_release(inst);
    return NULL;
}

void *get_current_time(void *arg){
    
    time_t now;
    struct tm *time_info;
    pthread_t alarm_player, refresh_list_print;

    // struct timespec timeout;
    // timeout.tv_sec = time(NULL) + 1;
    // timeout.tv_nsec = 0;

    int status;
    while(1){
        status = pthread_mutex_lock(&mutex);
        if(status != 0 ){
            printf("get_current_time pthread_mutex_lock failed %s %d\n", __FUNCTION__, __LINE__);
            exit(1);
        }

        
        // status = pthread_cond_timedwait(&cond, &mutex, &timeout);
        // if(status != ETIMEDOUT && status != 0 ){
        //     printf("status %d\n", status);
        //     printf("get_current_time pthread_cond_timedwait failed %s %d\n", __FUNCTION__, __LINE__);
        //     exit(1);
        // }

        time_t time_status = time(&now);
        if(time_status == -1){
            printf("time_status %ld\n", time_status);
            printf("time(&now) failed %s %d\n", __FUNCTION__, __LINE__);
            exit(1);
        }
        
        
        time_info = localtime(&now);
        if(time_info == NULL){
            printf("localtime(&now) failed %s %d\n", __FUNCTION__, __LINE__);
            exit(1);
        }
        if(is_alarm_list_read){
            int alarm_went_off_index = alarm_go_off_trigger(now);
            if(alarm_went_off_index != (-1)){
                mark_goes_off_one(alarm_went_off_index);
                pthread_create(&refresh_list_print, NULL, print_list_routine, NULL);
                pthread_create(&alarm_player, NULL, alarm_player_routine, NULL);
            }
        }
        strftime(
            current_time_formatted_string, 
            sizeof(current_time_formatted_string), 
            "%Y-%m-%d %A %H:%M:%S %Z", time_info
        );
        move(0, 0);
        clrtoeol();
        printw("%s\n", current_time_formatted_string);
        refresh();

        status = pthread_mutex_unlock(&mutex);
        if(status != 0){
            printf("get_current_time mutex unlock failed\n");
            exit(1);
        }
        sleep(1);
    }
}



int alarm_list_format_validate(char *alarm_list_time_string){
    int h, m, s, valid_num;
    valid_num = sscanf(alarm_list_time_string, "%d:%d:%d", &h, &m, &s);
    if(valid_num != 3){
        return -1;
    }
    if(h < 0 || h > 23 || m < 0 || m > 59 || s < 0 || s > 59){
        return -1;
    }
    return 1;
}

void get_epochtime_alarm_list(void){
    for(int i=0 ; i<alarm_count ; i++){
        alarm_list_epochtime[i] = setted_time_to_epochtime_format(alarm_list[i]);
    }
}

void mark_goes_off_one(int index){
    size_t str_len = strlen(alarm_list[index]);
    *( alarm_list[index] + str_len - 1 ) = '\t';
    *( alarm_list[index] + str_len     ) = '*';
    *( alarm_list[index] + str_len + 1 ) = '\n';
}

void *read_list_routine(void *arg){

    pthread_t print_alarm_list;
    struct timespec timeout;
    timeout.tv_sec = time(NULL) + 1;
    timeout.tv_nsec = 0;

    int status = pthread_mutex_lock(&mutex);
    if(status != 0){
        printf("ERROR pthread_mutex_lock failed, %s, %d\n", __FUNCTION__, __LINE__);
        exit(1);
    }

    status = pthread_cond_timedwait(&cond, &mutex, &timeout);
    if(status != ETIMEDOUT && status != 0 ){
        printf("status %d\n", status);
        printf("ERROR pthread_cond_timedwait failed %s %d\n", __FUNCTION__, __LINE__);
        exit(1);
    }

    char buf[BUF_SIZE];
    FILE * alarm_list_file = fopen("./alarm_list", "r");
    if(alarm_list_file == NULL){
        printf("ERROR alarm-list open failed %s %d\n", __FUNCTION__, __LINE__);
        exit(1);
    }

    char *input_alarm_str;
    size_t allocate_str_buf;
    size_t str_len;
    do{
        input_alarm_str = fgets(buf, BUF_SIZE, alarm_list_file);
        if(input_alarm_str == NULL){
            break;
        }
        if(alarm_list_format_validate(input_alarm_str) == -1){
            continue;
        }
        allocate_str_buf = strlen(input_alarm_str) + 12;
        alarm_list[alarm_count] = malloc((allocate_str_buf) * sizeof(char));
        strncpy(alarm_list[alarm_count], input_alarm_str, allocate_str_buf);
        str_len = strlen(alarm_list[alarm_count]);

        if( *( alarm_list[alarm_count] + str_len - 1 ) != '\n'){
            *( alarm_list[alarm_count] + str_len ) = '\n';
        }

        alarm_count++;
    }while(input_alarm_str != NULL);

    fclose(alarm_list_file);
    alarm_list_sorting();
    get_epochtime_alarm_list();
    is_alarm_list_read = 1;
    
    status = pthread_create(&print_alarm_list, NULL, print_list_routine, NULL);
    if(status != 0){
        printf("ERROR pthread_create print_list_routine failed %s %d\n", __FUNCTION__, __LINE__);
        exit(1);
    }

    status = pthread_mutex_unlock(&mutex);
    if(status != 0){
        printf("ERROR pthread_mutex_unlock failed, %s, %d\n", __FUNCTION__, __LINE__);
        exit(1);
    }
    return NULL;
}


void *print_list_routine(void *arg){
    
    int status;
    status = pthread_detach(pthread_self());
    if(status != 0){
        printf("ERROR pthread_detach failed, %s, %d\n", __FUNCTION__, __LINE__);
        exit(1);
    }


    status = pthread_mutex_lock(&mutex);
    if(status != 0){
        printf("ERROR pthread_mutex_lock failed, %s, %d\n", __FUNCTION__, __LINE__);
        exit(1);
    }


    // struct timespec timeout;
    // timeout.tv_sec = time(NULL);
    // timeout.tv_nsec = 0;

    // status = pthread_cond_timedwait(&cond, &mutex, &timeout);
    // if(status != ETIMEDOUT && status != 0 ){
    //     printf("status %d\n", status);
    //     printf("get_current_time pthread_cond_timedwait failed %s %d\n", __FUNCTION__, __LINE__);
    //     exit(1);
    // }

    int x = 0;
    int y = 5;
    move(y, x);
    printw("Alarm Setting List :\n");
    y += 2;
    for(int i=0 ; i<alarm_count ; i++, y++){
        move(y, x);
        printw("%s", alarm_list[i]);
    }
    refresh();

    status = pthread_mutex_unlock(&mutex);
    if(status != 0){
        printf("ERROR pthread_mutex_unlock failed, %s, %d\n", __FUNCTION__, __LINE__);
        exit(1);
    }

    return NULL;
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