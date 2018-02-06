/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <netmgr.h>
#include "libwebsockets.h"
#include "hal/soc/soc.h"
#include "ff.h"
#include "xplayer.h"
#include "xplayer_i.h"
#include "audio/manager/audio_manager.h"

extern void xplayer_key_process(input_event_t *eventinfo, void *priv_data);

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

void xplayer_run(void)
{
	xplayer_t *music_player;
	kstat_t stat;
	uint16_t msg;
	uint32_t readlen;
	uint8_t volume = 50;

	int aud_mgr_handler(int event, int val);
//	aud_mgr_handler(AUDIO_DEVICE_MANAGER_PATH, AUDIO_DEVICE_HEADPHONE);
	aud_mgr_handler(AUDIO_DEVICE_MANAGER_VOLUME, 5);
	
	LOG("Create xplayer...");
	music_player = xPlayer_create();
	// create message queue
	stat = krhino_buf_queue_dyn_create(&music_player->bufque, "xplayer-q", 10 , 2);
	if(stat != RHINO_SUCCESS) {
		LOG("create xpalyer message queue failed\n");
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
	//LOG("now playing...");
	//xPlayerPlay(NULL);

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
		            else if (xPlayerStatus() == STATUS_PAUSED)
		            	xPlayerPlay(NULL);
		            break;
				case CODE_VOLUME:
					volume ++;
					if (volume == 32)
						volume = 0;
					aud_mgr_handler(AUDIO_DEVICE_MANAGER_VOLUME, volume);
		            break;
				default:
					LOG("unknown message");
					continue;
			}
		}
		if (/*xPlayerStatus() == STATUS_STOPPED ||*/ xPlayerStatus() == STATUS_PLAYEND) {
			LOG("stop to next...");
			xPlayerNext();
		}
		aos_msleep(200);
	}
}


// int application_start(int argc, char *argv[])
// {
// #ifdef CSP_LINUXHOST
//     signal(SIGPIPE, SIG_IGN);
// #endif
//     netmgr_ap_config_t apconfig;
//     memset(&apconfig, 0, sizeof(apconfig));
//     // strcpy(apconfig.ssid,"TT");
//     // strcpy(apconfig.pwd, "tian2211");
//     strcpy(apconfig.ssid,"aos_test_01");
//     strcpy(apconfig.pwd, "Alios@Embedded");
//     netmgr_set_ap_config(&apconfig);

//    aos_set_log_level(AOS_LL_DEBUG);
//    LOG("application_start!!");
//    	krhino_task_dyn_create(&g_player, "xplayer", 0, 10, 0, 2048, xplayer_run, 1);

//    //aos_post_delayed_action(5000, app_delayed_action, NULL);
//    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);
//     netmgr_init();
// 	netmgr_start(0);

//     aos_loop_run();

//     return 0;
// }

