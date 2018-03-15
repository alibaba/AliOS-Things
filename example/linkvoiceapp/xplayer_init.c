/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <netmgr.h>
#include "hal/soc/soc.h"
#include "ff.h"
#include "xplayer.h"
#include "xplayer_i.h"
#include "audio/manager/audio_manager.h"

int volume = 20;
#define TAG "xplayer-init"
enum {
	CODE_EVENT_EXT_HEADPHONE = 0xf001,
	CODE_EVENT_EXT_SPEAKER = 0xf002,
};

static int audio_out_select=1;

extern aos_sem_t key_event_sem;
void xplayer_key_process(input_event_t *eventinfo, void *priv_data)
{
	uint16_t msg;
	kbuf_queue_t   *bufque = priv_data;
    if (eventinfo->type != EV_KEY) {
        return;
    }

	LOG("xplayer key event: %d", eventinfo->code);
	switch (eventinfo->code) {
		case CODE_RECORD_PRE:
	        if (eventinfo->value == VALUE_KEY_CLICK) {
	            LOG("CODE_RECORD_PRE VALUE_KEY_CLICK: ");
				aos_sem_signal(&key_event_sem);
	           // krhino_buf_queue_send(bufque, &eventinfo->code, 2);
	        } else if(eventinfo->value == VALUE_KEY_LTCLICK) {
	            LOG("CODE_RECORD_PRE VALUE_KEY_LTCLICK: ");
	        } else if(eventinfo->value == VALUE_KEY_LLTCLICK) {
	            LOG("CODE_RECORD_PRE VALUE_KEY_LLTCLICK: ");
	        }
			break;
		case CODE_NEXT:
	        if (eventinfo->value == VALUE_KEY_CLICK) {
	            LOG("CODE_NEXT VALUE_KEY_CLICK: ");
	            krhino_buf_queue_send(bufque, &eventinfo->code, 2);
	        } else if(eventinfo->value == VALUE_KEY_LTCLICK) {
	            LOG("CODE_NEXT VALUE_KEY_LTCLICK: ");
	        } else if(eventinfo->value == VALUE_KEY_LLTCLICK) {
	            LOG("CODE_NEXT VALUE_KEY_LLTCLICK: ");
	        }
			break;
		case CODE_PLAY_PAUSE:
	        if (eventinfo->value == VALUE_KEY_CLICK) {
	            LOG("CODE_PLAY_PAUSE VALUE_KEY_CLICK: ");
	            krhino_buf_queue_send(bufque, &eventinfo->code, 2);
	        } else if(eventinfo->value == VALUE_KEY_LTCLICK) {
	            LOG("CODE_PLAY_PAUSE VALUE_KEY_LTCLICK: ");
	        } else if(eventinfo->value == VALUE_KEY_LLTCLICK) {
	            LOG("CODE_PLAY_PAUSE VALUE_KEY_LLTCLICK: ");
	        }
			break;
		case CODE_VOLUME:
	        if (eventinfo->value == VALUE_KEY_CLICK) {
	            LOG("CODE_VOLUME VALUE_KEY_CLICK: ");
	            krhino_buf_queue_send(bufque, &eventinfo->code, 2);
	        } else if(eventinfo->value == VALUE_KEY_LTCLICK) {
	            LOG("CODE_VOLUME VALUE_KEY_LTCLICK: ");
	        } else if(eventinfo->value == VALUE_KEY_LLTCLICK) {
	            LOG("CODE_VOLUME VALUE_KEY_LLTCLICK: ");
	        }
			break;
		case CODE_BOOT:
	        if (eventinfo->value == VALUE_KEY_CLICK) {
	            LOG("CODE_BOOT VALUE_KEY_CLICK: ");
	        } else if(eventinfo->value == VALUE_KEY_LTCLICK) {
	            LOG("CODE_BOOT VALUE_KEY_LTCLICK: ");
				
				if (audio_out_select == 0) {
					audio_out_select = 1;
					msg = CODE_EVENT_EXT_SPEAKER;
				} else {
					audio_out_select = 0;
					msg = CODE_EVENT_EXT_HEADPHONE;
				}
	            krhino_buf_queue_send(bufque, &msg, 2);
	        } else if(eventinfo->value == VALUE_KEY_LLTCLICK) {
	            LOG("CODE_BOOT VALUE_KEY_LLTCLICK: ");
	        }
			break;
		case CODE_ELINK:
	        if (eventinfo->value == VALUE_KEY_CLICK) {
	            LOG("CODE_ELINK VALUE_KEY_CLICK: ");
	        } else if(eventinfo->value == VALUE_KEY_LTCLICK) {
	            LOG("CODE_ELINK VALUE_KEY_LTCLICK: ");
	        } else if(eventinfo->value == VALUE_KEY_LLTCLICK) {
	            LOG("CODE_ELINK VALUE_KEY_LLTCLICK: ");
	        }
			break;
	}
}
static int old_state=0;
aos_queue_t player_cached_queue;
#define MSG_MAX_LEN 256
void xplayer_run(void)
{
	xplayer_t *music_player;
	kstat_t stat;
	uint16_t msg;
	uint32_t readlen;
    char rec_cached[256];
	int aud_mgr_handler(int event, int val);
	aud_mgr_handler(AUDIO_DEVICE_MANAGER_VOLUME, 50);
	
	LOG("Create xplayer...");
	music_player = xPlayer_create();
	// create message queue
	stat = krhino_buf_queue_dyn_create(&music_player->bufque, "xplayer-q", 10 , 2);
	if(stat != RHINO_SUCCESS) {
		LOGE(TAG,"create xpalyer message queue failed\n");
		xPlayerDestroy();
		return;
	}
	char *msg_start = (char*)malloc(MSG_MAX_LEN * 2);
    if (msg_start == NULL) {
		LOGE(TAG,"=======malloc failed========\n");
        return ;
    }
	stat = aos_queue_new(&player_cached_queue,msg_start, 2*MSG_MAX_LEN, MSG_MAX_LEN);
	if(stat != RHINO_SUCCESS) {
		LOGE(TAG,"=======create player_cached_queue failed========\n");
		xPlayerDestroy();
		return;
	}

    aos_register_event_filter(EV_KEY, xplayer_key_process, music_player->bufque);
	LOG("show music list...");
	if (xPlayerShowMusicList() == -1) {
		krhino_buf_queue_dyn_del(&music_player->bufque);
		xPlayerDestroy();
		krhino_task_dyn_del(NULL);
		LOG("xplaer thread exit...");
		return;
	}
   // draw_text(0,3,0,"HEADPHONE");
    if(audio_out_select){
	    aud_mgr_handler(AUDIO_DEVICE_MANAGER_PATH, CODE_EVENT_EXT_SPEAKER);
	}
	else{
	    aud_mgr_handler(AUDIO_DEVICE_MANAGER_PATH, AUDIO_DEVICE_HEADPHONE);
	}

	while (1) {
		readlen = 0;
		stat = krhino_buf_queue_recv(music_player->bufque, 10, &msg, &readlen);
		if (stat != RHINO_BLK_TIMEOUT && readlen) {
			switch (msg) {
				case CODE_RECORD_PRE:
		            xPlayerPre();
		            break;
				case CODE_NEXT:
		            xPlayerNext();
		            break;
				case CODE_PLAY_PAUSE:
		            if (xPlayerStatus() == STATUS_PLAYING)
		            	xPlayerPause();
		            else if (xPlayerStatus() == STATUS_PAUSED){
						LOG("====resume===");
		            	xPlayerPlay(NULL);
					}
		            break;
				case CODE_VOLUME:
					volume ++;
					if (volume == 32)
						volume = 0;
					aud_mgr_handler(AUDIO_DEVICE_MANAGER_VOLUME, volume);
		            break;
				case CODE_EVENT_EXT_HEADPHONE:
				    //draw_text(0,3,0,"HEADPHONE");
					aud_mgr_handler(AUDIO_DEVICE_MANAGER_PATH, AUDIO_DEVICE_HEADPHONE);
		            break;
				case CODE_EVENT_EXT_SPEAKER:
				    //draw_text(0,3,0,"        ");
					aud_mgr_handler(AUDIO_DEVICE_MANAGER_PATH, AUDIO_DEVICE_SPEAKER);
		            break;	
                

				default:
					LOG("unknown message");
					continue;
			}
		}
		if (/*xPlayerStatus() == STATUS_STOPPED ||*/ xPlayerStatus() == STATUS_PLAYEND) {
			LOG("stop xplayer..");
			xPlayerStop();
		}
        int state =xPlayerStatus();
		if(state!=old_state){
			old_state=state;
            play_stats_changed(state);
		}
		//LOG("state=%d",state);
        if(state==STATUS_STOPPED){
			stat= aos_queue_recv(&player_cached_queue,10, rec_cached, &readlen);
			if(stat==0&&readlen!=0){
				xPlayerPlay(rec_cached);
			}
		}

		aos_msleep(200);
	}
	aos_queue_free(&player_cached_queue);
	aos_free(msg_start);
}

int  xPlayer_add_to_queue(const char * url)
{
    return aos_queue_send(&player_cached_queue,url,strlen(url)+1);
}

