#include "../include/clock.h"


time_t alarm_list_epochtime[ALARM_LIST_SIZE] = {0};




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
            int alarm_went_off_index = alarm_goes_off_trigger(now);
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