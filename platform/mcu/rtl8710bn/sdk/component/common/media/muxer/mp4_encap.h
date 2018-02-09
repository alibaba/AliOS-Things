#ifndef _MP4_ENCAP_H_
#define _MP4_ENCAP_H_

#include "basic_types.h"
#include "osdep_service.h"

#define MAX_BOX_CHILD 8
#define RBUF_SIZE 1024

typedef struct _mp4root{
	unsigned int video_len;
        unsigned int audio_len;
	unsigned int total;
	unsigned int keyindex;
}MP4root,*PMP4root;

#define STORAGE_IDLE            0
#define STORAGE_START           1
#define STORAGE_WRITE_ALL       2
#define STORAGE_WRITE_VIDEO     3
#define STORAGE_WRITE_AUDIO     4
#define STORAGE_STOP            5
#define STORAGE_END             6


#define STORAGE_ALL     0
#define STORAGE_VIDEO   1
#define STORAGE_AUDIO   2

typedef struct _mp4_payload{
  unsigned char *addr;
  unsigned int  len;
}mp4_payload,pmp4_payload;

typedef struct _mp4_write_info{
  int mp4_start_position;
  int mp4_end_position;
  int mp4_occupy_size;
  int mp4_empty_size;
  int mp4_write_start_position;
  int mp4_write_end_position;
  int mp4_write_occupy_size;
  int mp4_write_empty_size;
  int mp4_write_length;
}mp4_winfo,*pmp4_winfo;

typedef struct _mp4_context{
    FATFS       m_fs;
    int         drv_num;
    char        _drv[4];
    FIL         m_file;
    char        filename[32];
    int         Fatfs_ok;
    int         filecount;
    int         width;
    int         height;
    int         sample_rate;
    int         channel_count;
    int         frame_rate;
    u32         start_time;//rtw_get_current_time()
    u32         file_name_index;
    u32         storage_state;
    MP4root     root;
    MP4root     root_reserve;
    int         period_time;
    int         file_total;
    int         sps_start;
    int         type;
    int         sps_len;
    int         pps_len;
    int         sps_pps_mark;
    unsigned    int *video_buffer_index;
    unsigned    int *video_buffer_size;
    int         video_size;
    unsigned    int *audio_buffer_index;
    unsigned    int *audio_buffer_size;
    int         audio_size;
    unsigned    char *moov_box;
    int         moov_box_size;
    unsigned    char *h264_buf;
    int         H264_LENGTH;
    mp4_payload payload;
    mp4_payload reserved_payload;
    int         reserved_start_index;
    int         iframe;
    int         reserved_type;
    _sema       start_fatfs_write;
    int         write_status;
    int         audio_write_status;
    int         video_write_status;
    int         buffer_write_status;//0: real write 1: virtual write 
    mp4_winfo   winfo;
    int         nal_len;
    int         h264_extra_len;
    int         mp4_muxer_enable;
}mp4_context,*pmp4_context;
#define IDLE            0
#define FATFS_WRITING   1
#define FATFS_DONE      2

void mp4_handle(pmp4_context mp4_ctx,unsigned char *buf,unsigned int size,int type);
void set_mp4_audio_buffer(pmp4_context mp4_ctx,unsigned int *audio_index,unsigned int *audio_size,int size);
void set_mp4_video_buffer(pmp4_context mp4_ctx,unsigned int *video_index,unsigned int *video_size,int size);
void set_mp4_moov_buffer(pmp4_context mp4_ctx,unsigned char *moov,int size);
int mp4_set_record(pmp4_context mp4_ctx,int num);
void mp4_muxer_init(pmp4_context mp4_ctx);
void mp4_muxer_close(pmp4_context mp4_ctx);
int mp4_set_start_status(pmp4_context mp4_ctx);

#endif