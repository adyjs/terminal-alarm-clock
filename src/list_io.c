#include "../include/list_io.h"

int alarm_count = 0;
int is_alarm_list_read = 0;
char current_time_formatted_string[100];
char *alarm_list[ALARM_LIST_SIZE] = {NULL};

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
