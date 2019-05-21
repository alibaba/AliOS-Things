#include "key_handle.h"
#include "error.h"
#include "include.h"
#include "typedef.h"
#include "arm_arch.h"
#include "gpio_pub.h"
#include "uart_pub.h"
#include "music_msg_pub.h"

#if(CONFIG_APP_MP3PLAYER == 1)
#define SD_DEBOUNCE_COUNT 			    50
#define KEY_SCAN_INTERVAL_10MS		    10

#if (CFG_SOC_NAME == SOC_BK7221U)
#define SD_CARD_DETECT_PIN			    31
#else
#define SD_CARD_DETECT_PIN			    25
#endif

#define KEY_PLAY_PAUSE_PIN			    28

#define SD_CARD_OFFLINE				    0
#define SD_CARD_ONLINE				    1

#define KEY_SHORTUP_COUNTER				3				//30ms
#define KEY_LONG_COUNTER				80				//800ms
#define KEY_LONGUP_COUNTER				84				//840ms
#define KEY_HOLD_COUNTER				110				//1100ms		

static void key_scan_callback( void * arg ); 
extern void bmsg_music_sender(void *arg);

static UINT16 backup_keyval = MSG_NO_KEY;
static UINT16 keycount = 0;
static UINT16 sd_online = SD_CARD_OFFLINE;
static UINT16 detect_cnt = 0;
static beken_timer_t key_handle_timer = {0};


void key_init(void)
{
 	UINT32 param;
	OSStatus err;
		
	param = GPIO_CFG_PARAM(SD_CARD_DETECT_PIN, GMODE_INPUT_PULLUP);
	sddev_control(GPIO_DEV_NAME, CMD_GPIO_CFG, &param);

	param = GPIO_CFG_PARAM(KEY_PLAY_PAUSE_PIN, GMODE_INPUT_PULLUP);
	sddev_control(GPIO_DEV_NAME, CMD_GPIO_CFG, &param);
	

	err = rtos_init_timer(&key_handle_timer, 
					KEY_SCAN_INTERVAL_10MS, 
					key_scan_callback, 
					(void *)0);
    ASSERT(kNoErr == err);
    err = rtos_start_timer(&key_handle_timer);
	ASSERT(kNoErr == err); 	
	
	KEY_PRT("==key init==\r\n");
}

uint8 sd_is_attached(void)
{
    return (sd_online);
}

static void sd_detect_handle()
{
	UINT32 sd_pin_level;
	UINT32 sd_detect_pin = SD_CARD_DETECT_PIN;
	 
	sd_pin_level = sddev_control(GPIO_DEV_NAME, CMD_GPIO_INPUT, &sd_detect_pin);
	UINT32 msg;
 	if(sd_pin_level)
	{
		if(sd_online == SD_CARD_ONLINE)
		{
		 	detect_cnt = 0;
			sd_online  = SD_CARD_OFFLINE;
			msg = MSG_SD_DETACH;
			//?detach???
			bmsg_music_sender((void*)msg);
		 }
	}
	else
	{
		if(sd_online == SD_CARD_OFFLINE)
		{
		 	if(detect_cnt < SD_DEBOUNCE_COUNT)
				detect_cnt++;
			else
			{
				sd_online = SD_CARD_ONLINE;
				msg = MSG_SD_ATTACH;
				//?attach???
				bmsg_music_sender((void*)msg);
			}
		 } 
	}
}

static UINT16 keyIOdetect(void)
{
	UINT16 key_pin_level;
	UINT16 keyValue= MSG_NO_KEY;;
	UINT32 key_detect_pin = KEY_PLAY_PAUSE_PIN;
	 
	key_pin_level = sddev_control(GPIO_DEV_NAME, CMD_GPIO_INPUT, &key_detect_pin);
	if(key_pin_level==0)
		keyValue = MSG_KEY_PLAY;

	return keyValue;
}


static void key_detect_handle(void)
{
	UINT32 msg;
	UINT16 keyval = keyIOdetect();

	if(keyval == MSG_NO_KEY)
	{
		if(keycount >= KEY_SHORTUP_COUNTER)
		{
			if(keycount > KEY_LONG_COUNTER)
				msg = KEY_LONG_UP|backup_keyval;
			else
				msg = backup_keyval;
			bmsg_music_sender((void*)msg);
		}
		keycount = 0;
		backup_keyval = MSG_NO_KEY;	
	}
	else
	{
		if(keyval!=backup_keyval)
		{
			keycount = 0;
			backup_keyval = keyval;
		}

		keycount++;
		
		if((keycount == KEY_LONG_COUNTER)||(keycount == KEY_HOLD_COUNTER))
		{
			if(keycount == KEY_LONG_COUNTER)
			{
				msg = KEY_LONG|keyval;
			}
			else
			{
				keycount = KEY_LONGUP_COUNTER;
				msg = KEY_HOLD|keyval;
			}
			bmsg_music_sender((void*)msg);
		}
	}
}


static void key_scan_callback( void * arg )  
{
	sd_detect_handle();
	key_detect_handle();
} 
#endif
