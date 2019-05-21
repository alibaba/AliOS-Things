#ifndef __JPEG_ENCODER_PUB_H__
#define __JPEG_ENCODER_PUB_H__

#if CFG_USE_CAMERA_INTF
#define EJPEG_FAILURE                (1)
#define EJPEG_SUCCESS                (0)

#define EJPEG_DEV_NAME               "ejpeg"

#define EJPEG_CMD_MAGIC              (0x0EBC0000)

#include "uart_pub.h"
#define EJPEG_DEBUG
#ifdef EJPEG_DEBUG
#define EJPEG_PRT                    os_printf
#else
#define EJPEG_PRT                    null_prf
#define EJPEG_WPRT                   null_prf
#endif

typedef struct ejpeg_desc
{
    UINT8 *rxbuf;
    
    void (*node_full_handler)(void *curptr, UINT32 newlen, UINT32 is_eof, UINT32 frame_len);
    void (*data_end_handler)(void);   

    UINT16 rxbuf_len;
    UINT16 rx_read_len;
    UINT32 node_len;

    UINT16 x_pixel;
    UINT16 y_pixel;
    void (*start_frame_handler)(void);
    void (*end_frame_handler)(void);
#if CFG_GENERAL_DMA
    void (*dma_rx_handler)(UINT32);
    UINT32 dma_channel;
#endif
    //CTIMER_T delay_timer;
} DJPEG_DESC_ST, *DJPEG_DESC_PTR;

enum
{
    EJPEG_CMD_SET_START_FRAME_INT = EJPEG_CMD_MAGIC + 1,
    EJPEG_CMD_SET_END_FRAME_INT,
    EJPEG_CMD_SET_MCLK_DIV,
    EJPEG_CMD_SET_VIDEO_BYTE_REVERSE,
    EJPEG_CMD_SELECT_YUV_FORMAT,
    EJPEG_CMD_SET_ENCODER_ENABLE,
    EJPEG_CMD_SET_X_PIXEL,
    EJPEG_CMD_ENABLE_ENC_SIZE,
    EJPEG_CMD_ENABLE_BITRATE_CTRL,    
    EJPEG_CMD_SET_BITRATE_STEP,
    EJPEG_CMD_SET_HSYNC_REVERSE,
    EJPEG_CMD_SET_VSYNC_REVERSE,
    EJPEG_CMD_SET_Y_PIXEL,
    EJPEG_CMD_GET_TARGET_HIGH_BYTE,
    EJPEG_CMD_GET_TARTGE_LOW_BYTE,
    EJPEG_CMD_GET_FRAME_LEN,
};


#define Y_PIXEL_480                                  (60)  // Y * 8
#define X_PIXEL_640                                  (80)  // X * 8

void ejpeg_init(void);
void ejpeg_exit(void);

#endif // CFG_USE_JPEG_ENCODER
#endif // __JPEG_ENCODER_PUB_H__
