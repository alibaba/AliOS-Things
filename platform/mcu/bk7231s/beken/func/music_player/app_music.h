#ifndef _APP_MUSIC_H_
#define _APP_MUSIC_H_
#include "sys_rtos.h"
#include "rtos_pub.h"

#define APP_MUSIC_DEBUG

#ifdef APP_MUSIC_DEBUG
#define APP_MUSIC_PRT       os_printf
#else
#define APP_MUSIC_PRT       os_null_printf
#endif

typedef struct _app_player_ctrl_s
{
    fat_file_info_t file_info;        // cur file info
    UINT32           file_index;      // file handle
    UINT32          block_played;

    UINT32          player_flag;
    UINT32          schedule_cmd; 
    UINT8          media_err_status; 

}app_player_ctrl;

typedef struct _app_player_ctrl_backup_s
{
    UINT32          file_index;      // file handle
    UINT32          file_size_blks;
    UINT32          block_played;
}app_player_ctrl_backup;


#define MEDIA_STACK_SIZE 4096
#define MEDIA_QITEM_COUNT 16

typedef struct
{
    uint32_t id;
} MEDIA_MSG_T, *MEDIA_MSG_PTR;


typedef struct _media_core_
{
    uint32_t queue_item_count;
    beken_queue_t io_queue;
    xTaskHandle handle;
    uint32_t stack_size;
} MEDIA_CORE_T;

#define MEDIA_ERR_CNT_THRE         15

enum
{
    APP_PLAYER_SCHEDULE_CMD_NEXT_SONG = 1,
    APP_PLAYER_SCHEDULE_CMD_PREV_SONG = 2,
    APP_PLAYER_SCHEDULE_CMD_NEXT_DIR = 3,
    APP_PLAYER_SCHEDULE_CMD_MODE_CHANGE = 4,
    APP_PLAYER_SCHEDULE_CMD_PREV_DIR = 5,
    APP_PLAYER_SCHEDULE_CMD_FIX_SONG = 8,
};

#define APP_PLAY_SCHEDULE_CMD_PLAY_FLAG      0x40000000
#define APP_PLAYER_FLAG_HW_ATTACH                0x1
#define APP_PLAYER_FLAG_PLAY_PAUSE               0x2
#define APP_PLAYER_FLAG_PLAY_CONTINOUS           0x4
#define APP_PLAYER_FLAG_PLAY_CYCLE               0x8
#define APP_PLAYER_FLAG_PLAY_END                 0x10

// not implemented yet
#define APP_PLAYER_FLAG_RANDOM_ORDER          0x2000

#define APP_PLAYER_HW_STANDBY_TIME              100
#define APP_PLAYER_WAVE_READ_BLOCK_NUM          4
// before setting audio corretly, we need wait for at least 4 corret
// frame decode.
#define APP_PLAYER_AUDIO_INITION_BEGIN          4


void Convert_Mono(short *buffer, int outputSamps);

enum
{
    APP_PLAYER_MODE_PLAY_ALL_CYCLE = 0, //所有歌曲循环
    APP_PLAYER_MODE_PLAY_DIR_CYCLE = 1, //文件夹循环
    APP_PLAYER_MODE_PLAY_ONE = 2,        //单曲循环
    APP_PLAYER_MODE_PLAY_RANDOM = 3,     //随即播放
    APP_PLAYER_MODE_END
};

#endif

