/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "config.h"
#include "pal.h"
#include "pal_platform.h"
#include "cJSON.h"
#include "aos/aos.h"
#include "runloop.h"
#include "opus_codec.h"
#include "cjson_utils.h"

#ifdef  MCU_XR871
#include "ff.h"
#include "oled.h"
#include "xplayer.h"
#include "xplayer_i.h"
#define OPUS_SAMPLE_FILE  "0:/sample.opus"
#define PCM_SAMPLE_FILE  "0:/sample.wav"
#else
int volume_sumulate=49;
#define OPUS_SAMPLE_FILE  "example/linkvoiceapp/sample.opus"
#define PCM_SAMPLE_FILE  "example/linkvoiceapp/sample.wav"
#endif



#define LOG_TAG "palapp"
#define log_debug(format, ...) LOGI(LOG_TAG, format"\r\n",##__VA_ARGS__)
#define log_info(format, ...) LOGI(LOG_TAG, format"\r\n",##__VA_ARGS__)
#define log_error(format, ...) LOGE(LOG_TAG, format"\r\n",##__VA_ARGS__)

/* 设备状态 */
#define PLAY_STATUS_PLAY 0
#define PLAY_STATUS_PAUSE 1
#define PLAY_STATUS_STOP 2

#define TAG "linkvoiceapp"
char *cur_url=NULL;
static int play_status = PLAY_STATUS_STOP;

static int play_duration = 3;//120;
static int play_progress_time = 0;
//static int next_done_once = 0;

static runloop_t *device_runloop = NULL;

enum test_device_event {
    TEST_DEVICE_EVENT_PLAYER_STATUS_CHANGE_PLAY = 0,
    TEST_DEVICE_EVENT_PLAYER_STATUS_CHANGE_PAUSE,
    TEST_DEVICE_EVENT_PLAYER_STATUS_CHANGE_STOP,
	TEST_DEVICE_EVENT_BUTTON_NEXT,
    TEST_DEVICE_EVENT_BUTTON_PREVIOUS,
    TEST_DEVICE_EVENT_BUTTON_LOVE_AUDIO,
    TEST_DEVICE_EVENT_PLAYER_VOLUME_CHANGE,
    TEST_DEVICE_EVENT_PLAY_DONE,
	TEST_DEVICE_EVENT_PLAY_PROGRESS_TIME
};
typedef void (*get_recode_stream_fn)(char * date,int size);
extern int pal_pcm_recode_with_cb(get_recode_stream_fn fn);

static int get_volume_for_pal(){
#ifdef MCU_XR871
    return volume*100/31;
#else
    return volume_sumulate;
#endif
 }
static void set_volume_for_device(int v){
#ifdef MCU_XR871
    return volume=v*31/100;
#else
    return volume_sumulate=v;
#endif
 }

static void test_post_player_volume_change(int delay)
{
	const char *format = "{\"jsonrpc\": \"2.0\", \"method\": \"player_volume_change\", \"params\": {\"volume\": %d}}";
    char *buffer = pal_malloc(128);	
    sprintf(buffer, format, get_volume_for_pal());
    runloop_event_t *e = pal_malloc(sizeof(runloop_event_t));
    memset(e, 0, sizeof(runloop_event_t));
    e->name = TEST_DEVICE_EVENT_PLAYER_VOLUME_CHANGE;
    e->timestamp = tic();
	e->delay_ms = delay * 1000;
    e->param = buffer;
    blocking_queue_add(device_runloop->q, e, 1);
}

static void test_post_player_progress_time_event(int delay)
{
	runloop_event_t *e = pal_malloc(sizeof(runloop_event_t));
    memset(e, 0, sizeof(runloop_event_t));
    e->name = TEST_DEVICE_EVENT_PLAY_PROGRESS_TIME;
    e->timestamp = tic();
    e->delay_ms = delay * 1000;
    blocking_queue_add(device_runloop->q, e, 1);
}
static void device_dispatcher_fn(runloop_event_t *e, void *extra_params)
{
    if (!e) {
        return;
    }
    switch (e->name) {
        case TEST_DEVICE_EVENT_PLAYER_STATUS_CHANGE_PLAY:
        case TEST_DEVICE_EVENT_PLAYER_STATUS_CHANGE_PAUSE:
        case TEST_DEVICE_EVENT_PLAYER_STATUS_CHANGE_STOP:
        {
            char *msg = (char *)e->param;
            log_debug(">>>>>>>>>>>>>>>>>>>>>>>> msg %s", msg);
            pal_notify_msg(msg);
            pal_free(msg);
            break;
        }
        case TEST_DEVICE_EVENT_BUTTON_NEXT:
        {
            char *msg = (char *)e->param;
            log_debug(">>>>>>>>>>>>>>>>>>>>>>>> msg %s", msg);
            pal_notify_msg(msg);
            pal_free(msg);
            break;
        }
        case TEST_DEVICE_EVENT_PLAY_DONE:
        {
            char *msg = (char *)e->param;
            log_debug(">>>>>>>>>>>>>>>>>>>>>>>> msg %s", msg);
            pal_notify_msg(msg);
            pal_free(msg);
            break;
        }
		case TEST_DEVICE_EVENT_PLAY_PROGRESS_TIME:
		{
			if (play_status == PLAY_STATUS_PLAY) {
				play_progress_time += 5;
			}
			test_post_player_progress_time_event(play_progress_time);
			break;
        }
		case TEST_DEVICE_EVENT_PLAYER_VOLUME_CHANGE:
		{
			char *msg = (char *)e->param;
			log_debug(">>>>>>>>>>>>>>>>>>>>>>>> msg %s", msg);
			pal_notify_msg(msg);
			pal_free(msg);
			//test_post_player_volume_change(5);
			break;
		}
        default:
            break;
    }
}

static void device_destroy_handler_fn(void *entry)
{
	if (!entry) {
		return;
	} 
	runloop_event_t *e = (runloop_event_t*)entry;
	if (e->param) {
		pal_free(e->param);
	}
	pal_free(e);
}

static void test_post_player_status_change_event(int status)
{
    const char *format = "{\"jsonrpc\": \"2.0\", \"method\": \"player_state_change\", \"params\": {\"state\": %d, \"duration\":%d}}";
    char *buffer = pal_malloc(128);
    sprintf(buffer, format, status, play_duration * 1000);
    runloop_event_t *e = pal_malloc(sizeof(runloop_event_t));
    memset(e, 0, sizeof(runloop_event_t));
    e->name = status;
    e->timestamp = tic();
    e->param = buffer;
    blocking_queue_add(device_runloop->q, e, 1);
}



static void test_post_play_done(char* url,int state,int delay)
{
    const char *format = "{\"jsonrpc\": \"2.0\", \"method\": \"play_done\", \"params\": {\"uri\":%s \"\", \"status\":%d}}";
    char *buffer = pal_malloc(512);
    sprintf(buffer, format, url,state );
    runloop_event_t *e = pal_malloc(sizeof(runloop_event_t));
    memset(e, 0, sizeof(runloop_event_t));
    e->name = TEST_DEVICE_EVENT_PLAY_DONE;
    e->timestamp = tic();
    e->delay_ms = delay * 1000;
    e->param = buffer;
    blocking_queue_add(device_runloop->q, e, 1);
}
static void post_play_done(int state,int delay)
{
    if(cur_url){
        test_post_play_done(cur_url,state,delay);
        pal_free(cur_url);
        cur_url=NULL;
    }
}


static int simulation_device_handle_sdk_cmd_nop(const char *cmd)
{
    
    log_debug("-------[simulation device] in NOTIFICATION_CMD_NOP : %s------", cmd ? cmd : "");
    cJSON *params = cJSON_Parse(cmd);
    if (!params) {
        return -1;
    }
    cJSON *method_obj = cJSON_GetObjectItem(params, "method");
    log_info("method : %s", (method_obj && method_obj->valuestring) ? method_obj->valuestring : "");
    if (method_obj && method_obj->valuestring)  {
        if (0 == strcmp(method_obj->valuestring, "play")) {
            LOGI(TAG,"～～～～～～～～～play～～～～～～～～～～～～～");
            cJSON *uri_obj = get_service_data(params, 2, "params", "uri");
            if (uri_obj) {
                if (uri_obj->valuestring) {
                    log_debug("uri : %s", uri_obj->valuestring);
#ifdef  MCU_XR871                
                    if(strncmp(uri_obj->valuestring,"http",strlen("http"))!=0){
                        char path[256]={0};
                        if(cur_url){
                            pal_free(cur_url);
                        }
                        cur_url=pal_malloc(strlen(uri_obj->valuestring)+1);
                        if(cur_url){
                            memcpy(cur_url,uri_obj->valuestring,strlen(uri_obj->valuestring)+1);
                        }

                        sprintf(path, "%s/%s", "file://music", uri_obj->valuestring);                       
                        xPlayer_add_to_queue(path);
                    }else{
                        xPlayer_add_to_queue(uri_obj->valuestring);
                        
                    }
#endif 
                    play_status = PLAY_STATUS_PLAY;
					play_progress_time = 0;
                    test_post_player_status_change_event(TEST_DEVICE_EVENT_PLAYER_STATUS_CHANGE_PLAY);
                    //TEST play done in 120 sec later
                  // test_post_play_done(uri_obj->valuestring,0,120);
                }
            }
            else {
                log_error("no uri decoded");
            }
        }
        else if (0 == strcmp(method_obj->valuestring, "update_cache")) {
        
        } else if (0 == strcmp(method_obj->valuestring, "pause")) {
#ifdef  MCU_XR871             
            xPlayerPause();
#endif            
            play_status = PLAY_STATUS_PAUSE;
            test_post_player_status_change_event(TEST_DEVICE_EVENT_PLAYER_STATUS_CHANGE_PAUSE);
            
        }
        else if (0 == strcmp(method_obj->valuestring, "resume")) {
#ifdef  MCU_XR871             
            xPlayerResume();
#endif              
            play_status = PLAY_STATUS_PLAY;
            test_post_player_status_change_event(TEST_DEVICE_EVENT_PLAYER_STATUS_CHANGE_PLAY);
        }
        else if (0 == strcmp(method_obj->valuestring, "pause_toggle")) {
            
        }
        else if (0 == strcmp(method_obj->valuestring, "seek_pos")) {
            
        }
        else if (0 == strcmp(method_obj->valuestring, "seek_time")) {
            
        }
        else if (0 == strcmp(method_obj->valuestring, "set_volume")) {
            //cmd {"method":"set_volume","params":{"volume":30},"jsonrpc":"2.0"}
            cJSON* vol_obj = get_service_data(params, 2, "params","volume");
            if (vol_obj) {
                int vol_value = vol_obj->valueint;
                if (vol_value >= 0 && vol_value <= 100) {
                    set_volume_for_device(vol_value);
                    LOGI(TAG,"~~~~volume=%d~~~~~~",vol_value);
                    #ifdef  MCU_XR871  
                    aud_mgr_handler(AUDIO_DEVICE_MANAGER_VOLUME, volume);
                    #endif
                }
            }
        }
        else if (0 == strcmp(method_obj->valuestring, "set_led")) {
            ;
        }
        else if (0 == strcmp(method_obj->valuestring, "update_screen")) {
            ;
        }
        else if (0 == strcmp(method_obj->valuestring, "play_voice_prompt")) { }
        else if (0 == strcmp(method_obj->valuestring, "net_state_change")) {
            
        } else {
        }
    }
    
    cJSON_Delete(params);

    return 0;
}

void test_post_asr_context()
{
        cJSON *reply = cJSON_CreateObject();
        cJSON_AddStringToObject(reply, "jsonrpc", "2.0");
        cJSON_AddNumberToObject(reply, "id", 0);
        cJSON *result_obj = cJSON_CreateObject();
        #define ds "{\"speed\": {\"min\": 1, \"max\": 36, \"plus\": 2, \"current\": 9, \"reduction\": 2, \"safety\": 20}, \"mode\": \"beach\", \"on\": [\"music\",\"switch\"],\"slope\": {\"min\": 0,\"max\": 20,\"plus\": 1,\"current\": 5,\"reduction\": 1}}"
            log_error("xxxxxxxxxxxxxxxxxxxxxxxx   ds %s", ds);
		cJSON *ob = cJSON_Parse(ds);
		cJSON_AddItemToObject(result_obj, "context", ob);
		cJSON_AddStringToObject(result_obj, "device_status", ds);

        cJSON_AddItemToObject(reply, "result", result_obj);
        char *reply_str = cJSON_PrintUnformatted(reply);
		pal_notify_msg(reply_str);
        cJSON_Delete(reply);
        aos_free(reply_str);
}

static int simulation_device_handle_sdk_cmd_query(const char* cmd, char *buffer, int buffer_capacity)
{
    cJSON *params = cJSON_Parse(cmd);

    if (!params) {
        log_error("[simulation device] cmd decode error");
        return -1;
    }
    log_debug("-------[simulation device] in NOTIFICATION_CMD_NOP : %s------", cmd ? cmd : "");
    cJSON *method_obj = cJSON_GetObjectItem(params, "method");
    cJSON *id_obj = cJSON_GetObjectItem(params, "id");
    if (method_obj && method_obj->valuestring && id_obj)  {
        int message_id = id_obj->valueint;
        cJSON *reply = cJSON_CreateObject();
        cJSON_AddStringToObject(reply, "jsonrpc", "2.0");
        cJSON_AddNumberToObject(reply, "id", message_id);
        cJSON *result_obj = cJSON_CreateObject();
        if (0 == strcmp(method_obj->valuestring, "is_attaching")) {
            cJSON_AddStringToObject(result_obj, "attach", "true");
        }
        else if (0 == strcmp(method_obj->valuestring, "get_setup_wifi_state")) {
            cJSON_AddNumberToObject(result_obj, "state", -1);
        }
        else if (0 == strcmp(method_obj->valuestring, "get_mac_address")) {
            cJSON_AddStringToObject(result_obj, "mac_address", "00:00:00:00:00:00");
        }
        else if (0 == strcmp(method_obj->valuestring, "get_volume")) {
            log_debug(">>>>>> get_volume");
            cJSON_AddNumberToObject(result_obj, "volume", get_volume_for_pal());
        }
        else if (0 == strcmp(method_obj->valuestring, "get_time")) {
            cJSON_AddNumberToObject(result_obj, "second", play_progress_time);
            log_debug(">>>>>> get_time");
        }
        else if (0 == strcmp(method_obj->valuestring, "get_pos")) {
            cJSON_AddNumberToObject(result_obj, "percent", 20);
        }
        else if (0 == strcmp(method_obj->valuestring, "get_play_state")) {
            cJSON_AddNumberToObject(result_obj, "state", play_status);
            log_debug(">>>>>> get_play_state");
        }
        else if (0 == strcmp(method_obj->valuestring, "get_uri")) {
            cJSON_AddStringToObject(result_obj, "uri", "");
        }
        else if (0 == strcmp(method_obj->valuestring, "get_filename")) {
            cJSON_AddStringToObject(result_obj, "filename", "");
        } else if (0 == strcmp(method_obj->valuestring, "get_asr_context")) {
           #define ds "{\"speed\": {\"min\": 1, \"max\": 36, \"plus\": 2, \"current\": 9, \"reduction\": 2, \"safety\": 20}, \"mode\": \"beach\", \"on\": [\"music\",\"switch\"],\"slope\": {\"min\": 0,\"max\": 20,\"plus\": 1,\"current\": 5,\"reduction\": 1}}"
			//cJSON *ob = cJSON_Parse(ds);
			//cJSON_AddItemToObject(result_obj, "asr_context", ob);
			//cJSON_AddStringToObject(result_obj, "device_status", ds);
        } else {
            log_error("[simulation device] unknown query");
        }
        cJSON_AddItemToObject(reply, "result", result_obj);
        char *reply_str = cJSON_PrintUnformatted(reply);
        log_info("[simulation device] query reply : %s", reply_str ? reply_str : "");
        strncpy(buffer, reply_str, buffer_capacity);
        if (reply_str) {
            aos_free(reply_str);
        }
        cJSON_Delete(reply);
    }
    cJSON_Delete(params);
    return 0;
}

static void pal_callback_fn(const char *cmd, int cmd_type, char *buffer, int buffer_capacity, void *user)
{
	log_debug("cmd %s\n", cmd);
    switch (cmd_type) {
        case PAL_CALLBACK_CMD_NOTIFY:
            simulation_device_handle_sdk_cmd_nop(cmd);
            break;
        case PAL_CALLBACK_CMD_QUERY:
            simulation_device_handle_sdk_cmd_query(cmd, buffer, buffer_capacity);
            break;
        default:
            log_error("unknow cmd type");
    }
	log_debug("==== done");
}

#ifdef  MCU_XR871

static int start=0;
static void get_recode_stream(char * data,int size)
{
    if(start){
        int ret = pal_asr_send_buffer(data, size * sizeof(unsigned char));
       // aos_msleep(10);
        if (ret == PAL_VAD_STATUS_STOP) {
            start=0;
            log_error("!!!!!!!!!!!!!Detect VAD STOP!!!!!!!!!!!!\n");
        }
    }
}

aos_sem_t key_event_sem;
extern void pcm_cfg_init();
extern int pal_pcm_recode();
//extern int pal_pcm_read_from_mmc(get_recode_stream_fn fn);
extern void xplayer_run(void);
ktask_t *g_player;
static void douglas_asr_recode_test()
{
 
    FRESULT fs_ret;
    if(aos_sem_new(&key_event_sem,0)==-1){
        return;
    }
    pcm_cfg_init();
    for (int i = 0; i < 100; i++) {
        draw_text(0,0,1,"press AK2 to start a conversation");
        LOGI(TAG,"========================press AK2 to contiune============================== begin\n");
        aos_sem_wait(&key_event_sem, AOS_WAIT_FOREVER);
        xPlayerStop();//先关闭播放再识别
        aos_msleep(2*1000);
        int ret = pal_asr_start();
        if(ret!=0){
            LOGE(TAG,"pal_asr_start failed\n");
            continue;
        }

        start=1;
        pal_pcm_recode_with_cb(get_recode_stream);

        struct pal_rec_result *result = pal_asr_stop();

        if (result) {
            if(result->tts){
                log_debug("~~~~~ttsurl=%s size=%d",result->tts,strlen(result->tts));
                if(xPlayer_add_to_queue(result->tts)){
                  LOGI(TAG,"!!!!!xPlayer_add_to_queue faile!!!!!");
                }
            }
            pal_rec_result_destroy(result);
        }

        LOGI(TAG,"===================================================== end \n");
    }
    aos_sem_free(&key_event_sem);
}


#define OPUS_BUFF_SIZE 1024*8
#define ONCE_LEN 640

static void douglas_asr_test(int format)
{
    
    FRESULT fs_ret;
    for (int i = 0; i < 3; i++) {
        int ret = pal_asr_start();
        FIL fp;
        //FILE *fp = NULL;
        if (format == PAL_AUDIO_FORMAT_OPUS) {
            fs_ret = f_open(&fp, OPUS_SAMPLE_FILE,FA_READ);
        } else if (format == PAL_AUDIO_FORMAT_PCM){
            fs_ret = f_open(&fp, PCM_SAMPLE_FILE,FA_READ);
        } else {
            LOGE(TAG,"========unsupport format======\n");
        }
        if (fs_ret!=FR_OK) {
            LOGE(TAG,"===========No file opened==fs_ret=%d===\n",fs_ret);
            return;
        }
        int file_size;
        f_lseek(&fp,  SEEK_END);
        file_size = f_tell(&fp);
        f_lseek(&fp,  SEEK_SET);
        int raw_len = 640; 
    
		printf("====================================================== begin\n");
        unsigned char *raw = aos_malloc(raw_len * sizeof(unsigned char));
        int read;
        int sum = 0;
		int count = 0;
         //f_read(&fp, &c, 1, &len)
        while ((f_read(&fp,raw, raw_len,&read)) == FR_OK) {
            sum += read;
           // aos_msleep(100);
            
            int ret = pal_asr_send_buffer((char *)raw, raw_len * sizeof(unsigned char));
			if (ret == PAL_VAD_STATUS_STOP) {
                printf("Detect VAD STOP\n");
                break;
            }
           if(read<raw_len){
               break;
           }
        }
		printf("===================================================== end sum: %d\n", sum);
        aos_free(raw);
        f_close(&fp);
        struct pal_rec_result *result = pal_asr_stop();
        if (result) {
            log_debug("~~~~~ttsurl=%s",result->tts);
            if(xPlayer_add_to_queue(result->tts)){
                log_error("!!!!!xPlayer_add_to_queue faile!!!!!");
            }
            test_post_play_done(result->tts,0,10);
            pal_rec_result_destroy(result);
        }
		aos_msleep(10*1000);
    }
}

int play_stats_changed(int state)
{
   log_debug("==========================play_stats_changed=========================== end sum: %d\n", state);
    switch(state){
        case STATUS_STOPPED:
            play_status=PLAY_STATUS_STOP;
            
            test_post_player_status_change_event(TEST_DEVICE_EVENT_PLAYER_STATUS_CHANGE_STOP);
            post_play_done(0,0);
            break;
        case STATUS_PLAYING:
            play_status=PLAY_STATUS_PLAY;
            test_post_player_status_change_event(TEST_DEVICE_EVENT_PLAYER_STATUS_CHANGE_PLAY);
            break;
        case STATUS_PAUSED:
            play_status=PLAY_STATUS_PAUSE;
            test_post_player_status_change_event(TEST_DEVICE_EVENT_PLAYER_STATUS_CHANGE_PAUSE);
            break;
    }
}

#else //linuxhost
#ifdef DUMP_MM
extern uint32_t dumpsys_mm_info_func(uint32_t len);
#endif
static void douglas_asr_test(int format)
{
    for (int i = 0; i < 10; i++) {
        int ret = pal_asr_start();
        if(ret!=0){
            LOGE(TAG,"pal_asr_start failed\n");
            continue;
        }
        aos_msleep(1000);
        FILE *fp = NULL;
        if (format == PAL_AUDIO_FORMAT_OPUS) {
            fp = fopen(OPUS_SAMPLE_FILE, "rb");
        } else if (format == PAL_AUDIO_FORMAT_PCM){
            fp = fopen(PCM_SAMPLE_FILE, "rb");
        } else {
            LOGE(TAG,"unsupport format\n");
        }
        if (!fp) {
            LOGE(TAG,"No file opened\n");
            return;
        }
        // int file_size;
        // fseek(fp, 0L, SEEK_END);
        // file_size = ftell(fp);
        // fseek(fp, 0L, SEEK_SET);
        int raw_len = 640; 
		printf("====================================================== begin\n");
        unsigned char *raw = aos_malloc(raw_len * sizeof(unsigned char));
        int read;
        int sum = 0;
        while ((read = fread(raw, sizeof(unsigned char), raw_len, fp)) != 0) {
            sum += read;
            aos_msleep(10);
		   
            int ret = pal_asr_send_buffer((char *)raw, raw_len * sizeof(unsigned char));
			if (ret == PAL_VAD_STATUS_STOP) {
                printf("Detect VAD STOP\n");
                break;
            }
        }
		printf("===================================================== end sum: %d\n", sum);
        aos_free(raw);
        fclose(fp);
        struct pal_rec_result *result = pal_asr_stop();
        if (result) {
            pal_rec_result_destroy(result);      
        }
#ifdef DUMP_MM
        test_post_player_status_change_event(TEST_DEVICE_EVENT_BUTTON_NEXT);
        dumpsys_mm_info_func(0);
#endif
		aos_msleep(3000);
    }
}

#endif

static void douglas_tts_test(){
    for(int i=0;i<3;i++){
        log_info("~~~~~~douglas_tts_test~~~~~");
        struct pal_rec_result* result =  pal_get_tts("播放刘德华的歌");
        if (result) {
            LOG("===== %s\n", result->raw);
#ifdef  MCU_XR871
            xPlayer_add_to_queue(result->tts);
#endif
            pal_rec_result_destroy(result);
        }
        aos_msleep(15*1000);
    }
}




void pal_sample(void *p) {
    
    struct pal_config config;

   if (!device_runloop) {
        device_runloop = pal_malloc(sizeof(runloop_t));
        if (!device_runloop) {
            log_error("Malloc failed");
            return -1;
        }
    }
    runloop_init(device_runloop, device_dispatcher_fn, NULL, device_destroy_handler_fn, "device_thread", 1024);
   	test_post_player_progress_time_event(0);

    memset(&config, 0, sizeof(struct pal_config));
    config.ca_file_path =NULL;//"ca.pem"; //ca.pem路径，如/tmp／ca.pem
    //int format = PAL_AUDIO_FORMAT_OPUS;
    int format = PAL_AUDIO_FORMAT_PCM;
    config.format = format;
    config.sample_rate = 16000;
    config.channels = 1;
    config.bits_per_sample = 16;
   	config.callback = pal_callback_fn;
    int ret = pal_init(&config);
    if(ret!=0){
        LOGE(TAG,"pal_init failed\n");
        return;
    }
    test_post_asr_context();
    test_post_player_volume_change(0);
    aos_msleep(1000);
    #ifdef  MCU_XR871
    draw_text(0,0,1,"linkvoice initializing...");
    //krhino_task_dyn_create(&g_player, "xplayer_init", 0, 10, 0, 768, xplayer_run, 1);
    aos_task_new("xplayer_init", xplayer_run, NULL,3072);
    //pal_factory_reset();
    douglas_asr_recode_test(); 
    //send_opus();
    #else
    douglas_asr_test(format);
    #endif
    
    aos_msleep(5000);
    douglas_tts_test();
    while (1) {
        aos_msleep(1000);
    }
	opus_codec_destroy();

}

