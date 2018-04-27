#ifndef _MSG_PUB_H_
#define _MSG_PUB_H_

#define MSG_SUCCESS_RET              (0)
#define MSG_FAILURE_RET              (-1)

//-------按键触发状态定义---------
#define KEY_SHORT_UP					0x00
#define KEY_HOLD						0x4000
#define KEY_LONG						0x8000
#define KEY_LONG_UP						0xc000
#define KEY_DOUBLE_CLICK 				0x2000
#define KEY_IRDA_LONG					0x1000
#define KEY_IRDA_SHORT					0x800

/* message table*/
#define KEY_MSG_GP                      (0x00000000)    /* Attention: special msg*/
#define SDMUSIC_MSG_GP                  (0x10000000)
#define UDISK_MSG_GP                    (0x20000000)
#define LINEIN_MSG_GP                   (0x30000000)
#define OTHER_MSG_GP                    (0x40000000)

/* Name format: MSG_module_messageInformation
   assume: message number is less than 65535 at one module
*/
enum
{
    MSG_NULL = 0, 
    /* Attention: special msg for key press, from 0x00000000--0x00000fff*/
 //-------按键消息值定义---------
	MSG_KEY_0							 = MSG_NULL + 0x1,
	MSG_KEY_1,
	MSG_KEY_2,
	MSG_KEY_3,
	MSG_KEY_4,
	MSG_KEY_5,
	MSG_KEY_6,
	MSG_KEY_7,
	MSG_KEY_8,
	MSG_KEY_9,
	MSG_BT_MIX_KEY,
	MSG_KEY_PLAY,
	MSG_KEY_STOP,
	MSG_KEY_PLUS,
	MSG_KEY_MINUS,
	MSG_KEY_PREV,
	MSG_KEY_NEXT,
	MSG_KEY_VOL_DOWN,
	MSG_KEY_VOL_UP,
	MSG_KEY_MODE,
	MSG_KEY_POWER,
	MSG_KEY_CALL,
	MSG_NO_KEY = 0xff,
    
    /*sd music msg*/
    MSG_SD_INIT                      = SDMUSIC_MSG_GP   + 0x0000,
    MSG_SD_ATTACH,            /* sd attach or detach*/
	MSG_SD_DETACH,

	
    /*usb disk msg*/
    MSG_UDISK_INIT                   = UDISK_MSG_GP     + 0x0000,
	MSG_USB_ATTACH,
	MSG_USB_DETATCH,
   
    /*linein msg*/
    MSG_LINEIN_INIT                  = LINEIN_MSG_GP  + 0x0000,
    MSG_LINEIN_ATTACH,
    MSG_LINEIN_DETACH,
    
    /*other msg*/   
    MSG_LED_INIT                     = OTHER_MSG_GP   + 0x0000,
    MSG_DEBUG_UART_RX,               /* debug uart gets datum*/
    MSG_SDADC,                       /* sdadc*/
    MSG_MUSIC_PLAY,
    MSG_MUSIC_STOP,
    MSG_POWER_DOWN,
    MSG_POWER_UP,
    MSG_IRDA_RX,
    MSG_LOWPOWER_DETECT,
    MSG_MEDIA_READ_ERR,      /* mp3-mode,SD/Udisk read Err */
    MSG_INPUT_TIMEOUT,
    MSG_HALF_SECOND,
    
    MSG_1S_TIMER
};

#endif
// EOF
