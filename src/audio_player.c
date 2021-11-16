#include "../include/audio_player.h"


int alarm_goes_off_trigger(time_t epochtime){
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