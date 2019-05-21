#ifndef _BK_JPEG_ENCODER_H_
#define _BK_JPEG_ENCODER_H_

#define JPEG_BASE                                    (0x0080A000)

#define JPEG_REG0                                    (JPEG_BASE + 0 * 4)
#define START_FRM_INT                                (1 << 2)
#define END_FRM_INT                                  (1 << 3) 
#define DIV_POSI                                     (4)
#define DIV_MASK                                     (0x3)
#define DIV_24M                                      (0)
#define DIV_16M                                      (1)
#define DIV_12M                                      (2)
#define DIV_24M_1                                    (3)

#define JPEG_REG1                                    (JPEG_BASE + 1 * 4)
#define VIDEO_BYTE_REV                               (1 << 1)
#define YUV_FMT_SEL_POSI                             (2) 
#define YUV_FMT_SEL_MASK                             (0x3)
#define YUV_FMT_YUYV                                 (0)
#define YUV_FMT_UYVY                                 (1)
#define YUV_FMT_YYUV                                 (2)
#define YUV_FMT_UVYY                                 (3)
#define JPEG_ENC_EN                                  (1 << 4)
#define X_PIXEL_POSI                                 (8) 
#define X_PIXEL_MASK                                 (0xFF) 
#define JPEG_ENC_SIZE                                (1 << 16)
#define BIT_RATE_CTRL                                (1 << 17)
#define BIT_RATE_STEP_POSI                           (18)
#define BIT_RATE_STEP_MASK                           (0x7)
#define HSYNC_REV                                    (1 << 21)
#define VSYNC_REV                                    (1 << 22)
#define Y_PIXEL_POSI                                 (24)
#define Y_PIXEL_MASK                                 (0xF)

#define JPEG_REG2                                    (JPEG_BASE + 2 * 4)
#define TARGET_BYTE_H_POSI                           (0)
#define TARGET_BYTE_H_MASK                           (0xFFFFFFFF)

#define JPEG_REG3                                    (JPEG_BASE + 3 * 4)
#define TARGET_BYTE_L_POSI                           (0)
#define TARGET_BYTE_L_MASK                           (0xFFFFFFFF)

#define JPEG_REG5                                    (JPEG_BASE + 5 * 4)
#define RX_FIFO_DATE_POSI                            (0)
#define RX_FIFO_DATE_MASK                            (0xFFFFFFFF)

#define JPEG_REG6                                    (JPEG_BASE + 6 * 4)
#define START_FRM_INT_STATUS                         (1 << 0)
#define END_FRM_INT_STATUS                           (1 << 1) 

#define JPEG_REG7                                    (JPEG_BASE + 7 * 4)
#define BYTE_CNT_PFRM_POSI                           (0)
#define BYTE_CNT_PFRM_MASK                           (0xFFFFFFFF)

#define JPEG_REG20_QUANT_TAB                         (JPEG_BASE + 0x20 * 4)
#define JPEG_QUANT_TAB_LEN                           (32)

static UINT32 ejpeg_open(UINT32 op_flag);
static UINT32 ejpeg_close(void);
static UINT32 ejpeg_ctrl(UINT32 cmd, void *param);

#endif // _BK_JPEG_H_
