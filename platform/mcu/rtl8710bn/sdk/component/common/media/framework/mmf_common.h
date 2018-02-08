#ifndef _EXCHBUF_H
#define _EXCHBUF_H
#include "cmsis_os.h"

/*service task state*/
#define S_STOP 					0x00
#define S_RUN					0x01
#define S_FROZEN                                0X02

#define CMD_SET_HEIGHT			0x00
#define CMD_SET_WIDTH			0x01
#define CMD_SET_BITRATE			0x02
#define CMD_SET_FRAMERATE		0x03
#define CMD_SET_CPZRATIO		0x04
#define CMD_SET_FRAMETYPE		0x05
#define CMD_SET_SAMPLERATE		0x06
#define CMD_SET_CHANNEL			0x07
#define CMD_SET_CODEC			0x08
#define CMD_SET_STREAMNUM		0x09
#define CMD_SET_SPS				0x0a
#define CMD_SET_PPS				0x0b
#define CMD_SET_LEVEL			0x0c
#define CMD_SET_APPLY			0x1f
#define CMD_SET_STREAMMING		0x20
#define CMD_SET_INPUT_QUEUE 	0x21
#define CMD_SET_OUTPUT_QUEUE 	0x22
#define CMD_SET_PRIV_BUF                0x23
#define CMD_SET_TASK_ON                 0x24
#define CMD_SET_TASK_FROZEN             0x25
#define CMD_SET_TASK_OFF                0x26
#define CMD_SELECT_CHANNEL		0x30
#define CMD_ENABLE_CHANNEL              0X31
#define CMD_DISABLE_CHANNEL             0x32
#define CMD_SET_CB_START		0x40
#define CMD_SET_CB_STOP			0x41
#define CMD_SET_CB_PAUSE		0x42
#define CMD_SET_CB_CUSTOMCMD	0x43
#define CMD_FLUSH				0x50
#define CMD_SET_FLAG                    0X51
#define CMD_GET_STREAM_READY            0x52
#define CMD_GET_STREAM_STATUS           0x53

#define STAT_INIT		0
#define STAT_USED		1
#define STAT_READY		2
#define STAT_RESERVED           3

#define TIME_SYNC_EN            0
#define TIME_SYNC_DIS           1

#define FMT_V_MJPG			0x00
#define FMT_V_H264			0x01
#define FMT_V_MP4V_ES		0x02
#define FMT_A_PCMU			0x10
#define FMT_A_PCMA			0x11
#define FMT_A_MP4A_LATM		0x12
#define FMT_AV_UNKNOWN		0xFF

/*mp4 storage*/
#define CMD_SET_ST_PERIOD               0X60
#define CMD_SET_ST_TOTAL                0X61
#define CMD_SET_ST_TYPE                 0X62
#define CMD_SET_ST_FILENAME             0x63
#define CMD_SET_ST_START                0x64
/*mp4 storage*/
#define STORAGE_ALL     0
#define STORAGE_VIDEO   1
#define STORAGE_AUDIO   2

#define MFT_CMD	0
#define MFT_DATA 1
typedef struct _exch_buf{
	//_list 		node;		// linking node
	uint32_t	type;			// CMD, or DATA
	uint32_t	cmd;			// command 
	uint32_t	arg;			// command arg
	
	uint8_t* 	data;			//
	uint32_t	index;
	uint32_t	len;
	uint32_t 	timestamp;		// 0: not set
	uint32_t	codec_fmt;		// FMT_V_xx or FMT_A_xx
	
	uint32_t	state;		
	void* 		priv;			// private use
}exch_buf_t;

typedef int (*mmf_cb_t)(void*);

/* common error code for MMF */
#define EAGAIN          11
#define	EFAULT          14
#define	EINVAL          22

#endif
