#ifndef __VIDEO_TRANS_H__
#define __VIDEO_TRANS_H__

typedef enum {
    TVIDEO_SND_NONE         = 0LU,
    TVIDEO_SND_UDP,
    TVIDEO_SND_TCP,
} TVIDEO_SND_TYPE;

typedef struct tvideo_desc
{
    UINT8 *rxbuf;
    
    void (*node_full_handler)(void *curptr, UINT32 newlen, UINT32 is_eof, UINT32 frame_len);
    void (*data_end_handler)(void);   

    UINT16 rxbuf_len;
    UINT16 rx_read_len;
    UINT32 node_len;
} TVIDEO_DESC_ST, *TVIDEO_DESC_PTR;

#if (CFG_USE_SPIDMA || CFG_USE_CAMERA_INTF)
void tvideo_intfer_send_msg(UINT32 new_msg);
UINT32 video_transfer_init(UINT32 tcp_udp_mode);
UINT32 video_transfer_deinit(void);
#endif

#endif // __VIDEO_TRANS_H__
