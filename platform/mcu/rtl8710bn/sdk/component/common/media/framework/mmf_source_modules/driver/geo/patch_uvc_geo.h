#ifndef _PATCH_UVC_GEO_H_
#define _PATCH_UVC_GEO_H_

#include "videodev2.h"
#include "uvc_osdep.h"
#if 0
#define be32_to_cpu(x)  ((((x) >> 24) & 0xff) | \
             (((x) >> 8) & 0xff00) | \
             (((x) << 8) & 0xff0000) | \
             (((x) << 24) & 0xff000000))
#define be24_to_cpu(a)   \
    ((((a)>>16)&0xFF) |                                                     \
    ((a)&0xFF00) |                                                          \
    (((a)<<16)&0xFF0000))
#define be16_to_cpu(x)  ((((x) >> 8) & 0xff) | \
             (((x) << 8) & 0xff00))
#endif
//#define BUFFER_COUNT 8
#define V4L2_CID_PRIV_EXT_BASE             (0x04000000)
#define V4L2_CID_XU_BASE                   (V4L2_CID_PRIVATE_BASE|V4L2_CID_PRIV_EXT_BASE)

#define V4L2_CID_XU_AVC_PROFILE            (V4L2_CID_XU_BASE + 0)
#define V4L2_CID_XU_AVC_LEVEL              (V4L2_CID_XU_BASE + 1)
#define V4L2_CID_XU_PICTURE_CODING         (V4L2_CID_XU_BASE + 2)
#define V4L2_CID_XU_RESOLUTION             (V4L2_CID_XU_BASE + 3)
#define V4L2_CID_XU_RESOLUTION2            (V4L2_CID_XU_BASE + 4)
#define V4L2_CID_XU_GOP_STRUCTURE          (V4L2_CID_XU_BASE + 5)
#define V4L2_CID_XU_GOP_LENGTH             (V4L2_CID_XU_BASE + 6)
#define V4L2_CID_XU_FRAME_RATE             (V4L2_CID_XU_BASE + 7)
#define V4L2_CID_XU_BITRATE                (V4L2_CID_XU_BASE + 8)
#define V4L2_CID_XU_FORCE_I_FRAME          (V4L2_CID_XU_BASE + 9)
#define V4L2_CID_XU_GET_VERSION            (V4L2_CID_XU_BASE + 10)
#define V4L2_CID_XU_MAX_NAL                (V4L2_CID_XU_BASE + 11)

#define V4L2_CID_XU_VUI_ENABLE             (V4L2_CID_XU_BASE + 30)
#define V4L2_CID_XU_PIC_TIMING_ENABLE      (V4L2_CID_XU_BASE + 31)
#define V4L2_CID_XU_AV_MUX_ENABLE          (V4L2_CID_XU_BASE + 32)
#define V4L2_CID_XU_MAX_FRAME_SIZE         (V4L2_CID_XU_BASE + 33)

//mux channel xu's
//mux1 h264
#define V4L2_CID_MUX_CH1_XU_RESOLUTION      (V4L2_CID_XU_BASE + 0)
#define V4L2_CID_MUX_CH1_XU_FRAMEINTRVL     (V4L2_CID_XU_BASE + 1)
#define V4L2_CID_MUX_CH1_XU_COMPRESSION_Q   (V4L2_CID_XU_BASE + 2)
#define V4L2_CID_MUX_CH1_XU_GOP_HIERARCHY_LEVEL (V4L2_CID_XU_BASE + 3)
#define V4L2_CID_MUX_CH1_XU_ZOOM        (V4L2_CID_XU_BASE + 4)
#define V4L2_CID_MUX_CH1_XU_BITRATE     (V4L2_CID_XU_BASE + 5)
#define V4L2_CID_MUX_CH1_XU_FORCE_I_FRAME   (V4L2_CID_XU_BASE + 6)
#define V4L2_CID_MUX_CH1_XU_VUI_ENABLE      (V4L2_CID_XU_BASE + 7)
#define V4L2_CID_MUX_CH1_XU_CHCOUNT     (V4L2_CID_XU_BASE + 8)
#define V4L2_CID_MUX_CH1_XU_CHTYPE      (V4L2_CID_XU_BASE + 9)
#define V4L2_CID_MUX_CH1_XU_GOP_LENGTH      (V4L2_CID_XU_BASE + 10)
#define V4L2_CID_MUX_CH1_XU_AVC_PROFILE     (V4L2_CID_XU_BASE + 11)
#define V4L2_CID_MUX_CH1_XU_AVC_MAX_FRAME_SIZE  (V4L2_CID_XU_BASE + 12)
#define V4L2_CID_MUX_CH1_XU_AVC_LEVEL       (V4L2_CID_XU_BASE + 13)
#define V4L2_CID_MUX_CH1_XU_PIC_TIMING_ENABLE   (V4L2_CID_XU_BASE + 14)
#define V4L2_CID_MUX_CH1_XU_VFLIP       (V4L2_CID_XU_BASE + 15)
#define V4L2_CID_MUX_CH1_XU_AUDIO_BITRATE   (V4L2_CID_XU_BASE + 16)

//mux2 h264/mjpeg
#define V4L2_CID_MUX_CH2_XU_RESOLUTION      (V4L2_CID_XU_BASE + 17)
#define V4L2_CID_MUX_CH2_XU_FRAMEINTRVL     (V4L2_CID_XU_BASE + 18)
#define V4L2_CID_MUX_CH2_XU_PIC_TIMING_ENABLE   (V4L2_CID_XU_BASE + 19)
#define V4L2_CID_MUX_CH2_XU_GOP_HIERARCHY_LEVEL (V4L2_CID_XU_BASE + 20)
#define V4L2_CID_MUX_CH2_XU_ZOOM        (V4L2_CID_XU_BASE + 21)
#define V4L2_CID_MUX_CH2_XU_BITRATE     (V4L2_CID_XU_BASE + 22)
#define V4L2_CID_MUX_CH2_XU_FORCE_I_FRAME   (V4L2_CID_XU_BASE + 23)
#define V4L2_CID_MUX_CH2_XU_VUI_ENABLE      (V4L2_CID_XU_BASE + 24)
#define V4L2_CID_MUX_CH2_XU_COMPRESSION_Q   (V4L2_CID_XU_BASE + 25)
#define V4L2_CID_MUX_CH2_XU_CHTYPE      (V4L2_CID_XU_BASE + 26)
#define V4L2_CID_MUX_CH2_XU_GOP_LENGTH      (V4L2_CID_XU_BASE + 27)
#define V4L2_CID_MUX_CH2_XU_AVC_PROFILE     (V4L2_CID_XU_BASE + 28)
#define V4L2_CID_MUX_CH2_XU_AVC_MAX_FRAME_SIZE  (V4L2_CID_XU_BASE + 29)
#define V4L2_CID_MUX_CH2_XU_AVC_LEVEL       (V4L2_CID_XU_BASE + 30)

//mux3 h264/mjpeg
#define V4L2_CID_MUX_CH3_XU_RESOLUTION      (V4L2_CID_XU_BASE + 33)
#define V4L2_CID_MUX_CH3_XU_FRAMEINTRVL     (V4L2_CID_XU_BASE + 34)
#define V4L2_CID_MUX_CH3_XU_PIC_TIMING_ENABLE   (V4L2_CID_XU_BASE + 35)
#define V4L2_CID_MUX_CH3_XU_GOP_HIERARCHY_LEVEL (V4L2_CID_XU_BASE + 36)
#define V4L2_CID_MUX_CH3_XU_ZOOM        (V4L2_CID_XU_BASE + 37)
#define V4L2_CID_MUX_CH3_XU_BITRATE     (V4L2_CID_XU_BASE + 38)
#define V4L2_CID_MUX_CH3_XU_FORCE_I_FRAME   (V4L2_CID_XU_BASE + 39)
#define V4L2_CID_MUX_CH3_XU_VUI_ENABLE      (V4L2_CID_XU_BASE + 40)
#define V4L2_CID_MUX_CH3_XU_COMPRESSION_Q   (V4L2_CID_XU_BASE + 41)
#define V4L2_CID_MUX_CH3_XU_CHTYPE      (V4L2_CID_XU_BASE + 42)
#define V4L2_CID_MUX_CH3_XU_GOP_LENGTH      (V4L2_CID_XU_BASE + 43)
#define V4L2_CID_MUX_CH3_XU_AVC_PROFILE     (V4L2_CID_XU_BASE + 44)
#define V4L2_CID_MUX_CH3_XU_AVC_MAX_FRAME_SIZE  (V4L2_CID_XU_BASE + 45)
#define V4L2_CID_MUX_CH3_XU_AVC_LEVEL       (V4L2_CID_XU_BASE + 46)

//mux4 h264/mjpeg
#define V4L2_CID_MUX_CH4_XU_RESOLUTION      (V4L2_CID_XU_BASE + 47)
#define V4L2_CID_MUX_CH4_XU_FRAMEINTRVL     (V4L2_CID_XU_BASE + 48)
#define V4L2_CID_MUX_CH4_XU_PIC_TIMING_ENABLE   (V4L2_CID_XU_BASE + 49)
#define V4L2_CID_MUX_CH4_XU_GOP_HIERARCHY_LEVEL (V4L2_CID_XU_BASE + 50)
#define V4L2_CID_MUX_CH4_XU_ZOOM        (V4L2_CID_XU_BASE + 51)
#define V4L2_CID_MUX_CH4_XU_BITRATE     (V4L2_CID_XU_BASE + 52)
#define V4L2_CID_MUX_CH4_XU_FORCE_I_FRAME   (V4L2_CID_XU_BASE + 53)
#define V4L2_CID_MUX_CH4_XU_VUI_ENABLE      (V4L2_CID_XU_BASE + 54)
#define V4L2_CID_MUX_CH4_XU_COMPRESSION_Q   (V4L2_CID_XU_BASE + 55)
#define V4L2_CID_MUX_CH4_XU_CHTYPE      (V4L2_CID_XU_BASE + 56)
#define V4L2_CID_MUX_CH4_XU_GOP_LENGTH      (V4L2_CID_XU_BASE + 57)
#define V4L2_CID_MUX_CH4_XU_AVC_PROFILE     (V4L2_CID_XU_BASE + 58)
#define V4L2_CID_MUX_CH4_XU_AVC_MAX_FRAME_SIZE  (V4L2_CID_XU_BASE + 59)
#define V4L2_CID_MUX_CH4_XU_AVC_LEVEL       (V4L2_CID_XU_BASE + 60)

//mux5
#define V4L2_CID_MUX_CH5_XU_RESOLUTION      (V4L2_CID_XU_BASE + 62)
#define V4L2_CID_MUX_CH5_XU_FRAMEINTRVL     (V4L2_CID_XU_BASE + 63)
#define V4L2_CID_MUX_CH5_XU_PIC_TIMING_ENABLE   (V4L2_CID_XU_BASE + 64)
#define V4L2_CID_MUX_CH5_XU_GOP_HIERARCHY_LEVEL (V4L2_CID_XU_BASE + 65)
#define V4L2_CID_MUX_CH5_XU_ZOOM        (V4L2_CID_XU_BASE + 66)
#define V4L2_CID_MUX_CH5_XU_BITRATE     (V4L2_CID_XU_BASE + 67)
#define V4L2_CID_MUX_CH5_XU_FORCE_I_FRAME   (V4L2_CID_XU_BASE + 68)
#define V4L2_CID_MUX_CH5_XU_VUI_ENABLE      (V4L2_CID_XU_BASE + 69)
#define V4L2_CID_MUX_CH5_XU_COMPRESSION_Q   (V4L2_CID_XU_BASE + 70)
#define V4L2_CID_MUX_CH5_XU_CHTYPE      (V4L2_CID_XU_BASE + 71)
#define V4L2_CID_MUX_CH5_XU_GOP_LENGTH      (V4L2_CID_XU_BASE + 72)
#define V4L2_CID_MUX_CH5_XU_AVC_PROFILE     (V4L2_CID_XU_BASE + 73)
#define V4L2_CID_MUX_CH5_XU_AVC_MAX_FRAME_SIZE  (V4L2_CID_XU_BASE + 74)
#define V4L2_CID_MUX_CH5_XU_AVC_LEVEL       (V4L2_CID_XU_BASE + 75)
//mux6
#define V4L2_CID_MUX_CH6_XU_RESOLUTION      (V4L2_CID_XU_BASE + 78)
#define V4L2_CID_MUX_CH6_XU_FRAMEINTRVL     (V4L2_CID_XU_BASE + 79)
#define V4L2_CID_MUX_CH6_XU_PIC_TIMING_ENABLE   (V4L2_CID_XU_BASE + 80)
#define V4L2_CID_MUX_CH6_XU_GOP_HIERARCHY_LEVEL (V4L2_CID_XU_BASE + 81)
#define V4L2_CID_MUX_CH6_XU_ZOOM        (V4L2_CID_XU_BASE + 82)
#define V4L2_CID_MUX_CH6_XU_BITRATE     (V4L2_CID_XU_BASE + 83)
#define V4L2_CID_MUX_CH6_XU_FORCE_I_FRAME   (V4L2_CID_XU_BASE + 84)
#define V4L2_CID_MUX_CH6_XU_VUI_ENABLE      (V4L2_CID_XU_BASE + 85)
#define V4L2_CID_MUX_CH6_XU_COMPRESSION_Q   (V4L2_CID_XU_BASE + 86)
#define V4L2_CID_MUX_CH6_XU_CHTYPE      (V4L2_CID_XU_BASE + 87)
#define V4L2_CID_MUX_CH6_XU_GOP_LENGTH      (V4L2_CID_XU_BASE + 88)
#define V4L2_CID_MUX_CH6_XU_AVC_LEVEL       (V4L2_CID_XU_BASE + 89)

//mux7
#define V4L2_CID_MUX_CH7_XU_RESOLUTION      (V4L2_CID_XU_BASE + 95)
#define V4L2_CID_MUX_CH7_XU_FRAMEINTRVL     (V4L2_CID_XU_BASE + 96)
#define V4L2_CID_MUX_CH7_XU_PIC_TIMING_ENABLE   (V4L2_CID_XU_BASE + 97)
#define V4L2_CID_MUX_CH7_XU_GOP_HIERARCHY_LEVEL (V4L2_CID_XU_BASE + 98)
#define V4L2_CID_MUX_CH7_XU_ZOOM        (V4L2_CID_XU_BASE + 99)
#define V4L2_CID_MUX_CH7_XU_BITRATE     (V4L2_CID_XU_BASE + 100)
#define V4L2_CID_MUX_CH7_XU_FORCE_I_FRAME   (V4L2_CID_XU_BASE + 101)
#define V4L2_CID_MUX_CH7_XU_VUI_ENABLE      (V4L2_CID_XU_BASE + 102)
#define V4L2_CID_MUX_CH7_XU_COMPRESSION_Q   (V4L2_CID_XU_BASE + 103)
#define V4L2_CID_MUX_CH7_XU_CHTYPE      (V4L2_CID_XU_BASE + 104)
#define V4L2_CID_MUX_CH7_XU_GOP_LENGTH      (V4L2_CID_XU_BASE + 105)
#define V4L2_CID_MUX_CH7_XU_AVC_LEVEL       (V4L2_CID_XU_BASE + 106)

//common mux xu
#define V4L2_CID_MUX_XU_START_CHANNEL       (V4L2_CID_XU_BASE + 107)
#define V4L2_CID_MUX_XU_STOP_CHANNEL        (V4L2_CID_XU_BASE + 108)

#define V4L2_CID_PU_XU_ANF_ENABLE          (V4L2_CID_XU_BASE + 110)
#define V4L2_CID_PU_XU_NF_STRENGTH         (V4L2_CID_XU_BASE + 111)
// Following are new define for Condor ISP API's
#define V4L2_CID_PU_XU_SINTER_MODE     (V4L2_CID_XU_BASE + 112)
#define V4L2_CID_PU_XU_SINTER_MIN_NRSTR    (V4L2_CID_XU_BASE + 113)
#define V4L2_CID_PU_XU_SINTER_MAX_NRSTR    (V4L2_CID_XU_BASE + 114)
#define V4L2_CID_PU_XU_SINTER_MIN_THR      (V4L2_CID_XU_BASE + 115)
#define V4L2_CID_PU_XU_SINTER_MAX_THR      (V4L2_CID_XU_BASE + 116)
#define V4L2_CID_PU_XU_SINTER_TRIGPT       (V4L2_CID_XU_BASE + 117)



#define V4L2_CID_PU_XU_TF_STRENGTH         (V4L2_CID_XU_BASE + 118)
#define V4L2_CID_PU_XU_ADAPTIVE_WDR_ENABLE (V4L2_CID_XU_BASE + 119)
#define V4L2_CID_PU_XU_WDR_STRENGTH        (V4L2_CID_XU_BASE + 120)
#define V4L2_CID_PU_XU_AUTO_EXPOSURE       (V4L2_CID_XU_BASE + 121)
#define V4L2_CID_PU_XU_EXPOSURE_TIME       (V4L2_CID_XU_BASE + 122)
#define V4L2_CID_PU_XU_AUTO_WHITE_BAL      (V4L2_CID_XU_BASE + 123)
#define V4L2_CID_PU_XU_WHITE_BAL_TEMP      (V4L2_CID_XU_BASE + 124)
#define V4L2_CID_PU_XU_VFLIP               (V4L2_CID_XU_BASE + 125)
#define V4L2_CID_PU_XU_HFLIP               (V4L2_CID_XU_BASE + 126)
#define V4L2_CID_PU_XU_WB_ZONE_SEL_ENABLE  (V4L2_CID_XU_BASE + 127)
#define V4L2_CID_PU_XU_WB_ZONE_SEL         (V4L2_CID_XU_BASE + 128)
#define V4L2_CID_PU_XU_EXP_ZONE_SEL_ENABLE (V4L2_CID_XU_BASE + 129)
#define V4L2_CID_PU_XU_EXP_ZONE_SEL        (V4L2_CID_XU_BASE + 130)
#define V4L2_CID_PU_XU_MAX_ANALOG_GAIN     (V4L2_CID_XU_BASE + 131)
#define V4L2_CID_PU_XU_HISTO_EQ            (V4L2_CID_XU_BASE + 132)
#define V4L2_CID_PU_XU_SHARPEN_FILTER      (V4L2_CID_XU_BASE + 133)
#define V4L2_CID_PU_XU_GAIN_MULTIPLIER     (V4L2_CID_XU_BASE + 134)
#define V4L2_CID_PU_XU_CROP_CHANNEL        (V4L2_CID_XU_BASE + 135)
#define V4L2_CID_PU_XU_CROP_ENABLE         (V4L2_CID_XU_BASE + 136)
#define V4L2_CID_PU_XU_CROP_WIDTH          (V4L2_CID_XU_BASE + 137)
#define V4L2_CID_PU_XU_CROP_HEIGHT         (V4L2_CID_XU_BASE + 138)
#define V4L2_CID_PU_XU_CROP_X              (V4L2_CID_XU_BASE + 139)
#define V4L2_CID_PU_XU_CROP_Y              (V4L2_CID_XU_BASE + 140)
#define V4L2_CID_PU_XU_EXP_MIN_FR_RATE     (V4L2_CID_XU_BASE + 141)
#define V4L2_CID_PU_XU_SATURATION_MODE     (V4L2_CID_XU_BASE + 142)
#define V4L2_CID_PU_XU_BRIGHTNESS_MODE     (V4L2_CID_XU_BASE + 143)
#define V4L2_CID_PU_XU_CONTRAST_MODE       (V4L2_CID_XU_BASE + 144)

//dewarp xu's
#define V4L2_CID_MUX_CH1_XU_DEWRP_CHANNEL  (V4L2_CID_XU_BASE + 150)
#define V4L2_CID_MUX_CH2_XU_DEWRP_CHANNEL  (V4L2_CID_XU_BASE + 158)
#define V4L2_CID_MUX_CH3_XU_DEWRP_CHANNEL  (V4L2_CID_XU_BASE + 161)
#define V4L2_CID_MUX_CH4_XU_DEWRP_CHANNEL  (V4L2_CID_XU_BASE + 164)
#define V4L2_CID_MUX_CH5_XU_DEWRP_CHANNEL  (V4L2_CID_XU_BASE + 167)
#define V4L2_CID_MUX_CH6_XU_DEWRP_CHANNEL  (V4L2_CID_XU_BASE + 170)
#define V4L2_CID_MUX_CH7_XU_DEWRP_CHANNEL  (V4L2_CID_XU_BASE + 173)
#define V4L2_CID_MUX_CH1_XU_CROP_CHANNEL   (V4L2_CID_XU_BASE + 174)
//dewarp xu end

#define V4L2_CID_PU_XU_DEWARP_CHANNEL      (V4L2_CID_XU_BASE + 176)
#define V4L2_CID_PU_XU_DEWARP_PANEL        (V4L2_CID_XU_BASE + 177)
#define V4L2_CID_PU_XU_DEWARP_MODE         (V4L2_CID_XU_BASE + 178)
#define V4L2_CID_PU_XU_DEWARP_PARAM0       (V4L2_CID_XU_BASE + 179)
#define V4L2_CID_PU_XU_DEWARP_PARAM1       (V4L2_CID_XU_BASE + 180)
#define V4L2_CID_PU_XU_DEWARP_PARAM2       (V4L2_CID_XU_BASE + 181)
#define V4L2_CID_PU_XU_DEWARP_PARAM3       (V4L2_CID_XU_BASE + 182)
#define V4L2_CID_PU_XU_DEWARP_PARAM4       (V4L2_CID_XU_BASE + 183)
#define V4L2_CID_PU_XU_DEWARP_PARAM5       (V4L2_CID_XU_BASE + 184)
#define V4L2_CID_PU_XU_DEWARP_PARAM6       (V4L2_CID_XU_BASE + 185)
#define V4L2_CID_PU_XU_DEWARP_PARAM7       (V4L2_CID_XU_BASE + 186)
#define V4L2_CID_PU_XU_DEWARP_PARAM8       (V4L2_CID_XU_BASE + 187)
#define V4L2_CID_PU_XU_DEWARP_PARAM9       (V4L2_CID_XU_BASE + 188)
//dewarp xu end

#define V4L2_CID_PU_XU_COMPOSITOR_CHANNEL      (V4L2_CID_XU_BASE + 189)
#define V4L2_CID_PU_XU_COMPOSITOR_PANEL_ID     (V4L2_CID_XU_BASE + 190)
#define V4L2_CID_PU_XU_COMPOSITOR_MODE         (V4L2_CID_XU_BASE + 191)
#define V4L2_CID_PU_XU_COMPOSITOR_PARAM0       (V4L2_CID_XU_BASE + 192)
#define V4L2_CID_PU_XU_COMPOSITOR_PARAM1       (V4L2_CID_XU_BASE + 193)
#define V4L2_CID_PU_XU_COMPOSITOR_PARAM2       (V4L2_CID_XU_BASE + 194)
#define V4L2_CID_PU_XU_COMPOSITOR_PARAM3       (V4L2_CID_XU_BASE + 195)
#define V4L2_CID_PU_XU_COMPOSITOR_PARAM4       (V4L2_CID_XU_BASE + 196)

#define V4L2_CID_PU_XU_CONFIG_PARAM0      (V4L2_CID_XU_BASE + 197)
#define V4L2_CID_PU_XU_CONFIG_PARAM1       (V4L2_CID_XU_BASE + 198)
#define V4L2_CID_PU_XU_CONFIG_PARAM2       (V4L2_CID_XU_BASE + 199)
#define V4L2_CID_PU_XU_CONFIG_PARAM3       (V4L2_CID_XU_BASE + 200)
#define V4L2_CID_PU_XU_CONFIG_PARAM4       (V4L2_CID_XU_BASE + 201)
#define V4L2_CID_PU_XU_CONFIG_PARAM5       (V4L2_CID_XU_BASE + 202)
#define V4L2_CID_PU_XU_CONFIG_PARAM6       (V4L2_CID_XU_BASE + 203)
#define V4L2_CID_PU_XU_CONFIG_PARAM7       (V4L2_CID_XU_BASE + 204)

#define V4L2_CID_MUX_CH1_XU_VBR_MIN_BITRATE  (V4L2_CID_XU_BASE + 205)
#define V4L2_CID_MUX_CH2_XU_VBR_MIN_BITRATE  (V4L2_CID_XU_BASE + 206)
#define V4L2_CID_MUX_CH3_XU_VBR_MIN_BITRATE  (V4L2_CID_XU_BASE + 207)
#define V4L2_CID_MUX_CH4_XU_VBR_MIN_BITRATE  (V4L2_CID_XU_BASE + 208)
#define V4L2_CID_MUX_CH5_XU_VBR_MIN_BITRATE  (V4L2_CID_XU_BASE + 209)
#define V4L2_CID_MUX_CH6_XU_VBR_MIN_BITRATE  (V4L2_CID_XU_BASE + 210)
#define V4L2_CID_MUX_CH7_XU_VBR_MIN_BITRATE  (V4L2_CID_XU_BASE + 211)
#define V4L2_CID_XU_END                    (V4L2_CID_XU_BASE + 212)

/* GUIDs */
/* {303B461D-BC63-44c3-8230-6741CAEB5D77} */
#define GUID_VIDCAP_EXT \
    {0x1d, 0x46, 0x3b, 0x30, 0x63, 0xbc, 0xc3, 0x44,\
     0x82, 0x30, 0x67, 0x41, 0xca, 0xeb, 0x5d, 0x77}
/* {6DF18A70-C113-428e-88C5-4AFF0E286AAA} */
#define GUID_VIDENC_EXT \
    {0x70, 0x8a, 0xf1, 0x6d, 0x13, 0xc1, 0x8e, 0x42,\
     0x88, 0xc5, 0x4a, 0xff, 0x0e, 0x28, 0x6a, 0xaa}
/* {ba2b92d9-26f2-4294-ae42-06dd684debe4} */
#define AVC_XU_GUID \
    {0xd9, 0x92, 0x2b, 0xba, 0xf2, 0x26, 0x94, 0x42,\
     0x42, 0xae, 0xe4, 0xeb, 0x4d, 0x68, 0xdd, 0x06}
#define MUX1_XU_GUID AVC_XU_GUID
#define MUX2_XU_GUID \
    {0xd9, 0x92, 0x2b, 0xbb, 0xf2, 0x26, 0x94, 0x42,\
     0x42, 0xae, 0xe4, 0xeb, 0x4d, 0x68, 0xdd, 0x06}
#define MUX3_XU_GUID \
    {0xd9, 0x92, 0x2b, 0xbc, 0xf2, 0x26, 0x94, 0x42,\
     0x42, 0xae, 0xe4, 0xeb, 0x4d, 0x68, 0xdd, 0x06}
#define MUX4_XU_GUID \
    {0xd9, 0x92, 0x2b, 0xbd, 0xf2, 0x26, 0x94, 0x42,\
     0x42, 0xae, 0xe4, 0xeb, 0x4d, 0x68, 0xdd, 0x06}
#define MUX5_XU_GUID \
    {0xd9, 0x92, 0x2b, 0xbe, 0xf2, 0x26, 0x94, 0x42,\
     0x42, 0xae, 0xe4, 0xeb, 0x4d, 0x68, 0xdd, 0x06}
#define MUX6_XU_GUID \
    {0xd9, 0x92, 0x2b, 0xbf, 0xf2, 0x26, 0x94, 0x42,\
     0x42, 0xae, 0xe4, 0xeb, 0x4d, 0x68, 0xdd, 0x06}
#define MUX7_XU_GUID \
    {0xd9, 0x92, 0x2b, 0xc0, 0xf2, 0x26, 0x94, 0x42,\
     0x42, 0xae, 0xe4, 0xeb, 0x4d, 0x68, 0xdd, 0x06}

#define PU_XU_GUID \
    {0x12, 0xcd, 0x5d, 0xdf, 0x5f, 0x7d, 0xba, 0x4b,\
     0xbb, 0x6d, 0x4b, 0x62, 0x5a, 0xdd, 0x52, 0x72}


#define V4L2_CID_DIGITIAL_MULTIPLIER    (V4L2_CID_CAMERA_CLASS_BASE+32)

// framerate in 100 nsec units //TBD correctly
#define FRI(x)  ((float)10000000/x) //TBD
#define FRR(x)  ((float)10000000/(x+0.0))

/** Image Name Length */
#define IH_NMLEN                32          

enum MUX_XU_CTRL {
    //for all video channels
    MUX_XU_RESOLUTION = 1,
    MUX_XU_FRAMEINTRVL,
    MUX_XU_PIC_TIMING_ENABLE,
    MUX_XU_GOP_HIERARCHY_LEVEL,
    MUX_XU_ZOOM,

    //for H264 based channels only
    MUX_XU_BITRATE,
    MUX_XU_FORCE_I_FRAME,
    MUX_XU_VUI_ENABLE,
    MUX_XU_AVC_LEVEL,

    //for MJPEG channel only
    MUX_XU_COMPRESSION_Q,

    MUX_XU_CHCOUNT,
    MUX_XU_CHTYPE,
    MUX_XU_GOP_LENGTH,
    MUX_XU_AVC_PROFILE,
    MUX_XU_AVC_MAX_FRAME_SIZE,
    MUX_XU_START_CHANNEL,
    MUX_XU_STOP_CHANNEL,
    MUX_XU_HFLIP,
    MUX_XU_VFLIP,

    //Audio bitrate
    MUX_XU_AUDIO_BITRATE,

    //dewarp xu's
    MUX_XU_DEWRP_CHANNEL,

    // vbr params for avc   
    MUX_XU_VBR_MIN_BITRATE,
    MUX_XU_VBR_SETTLING_TIME,           //Not used
    
    MUX_XU_NUM_CTRLS,
};

/* Controls in the XU */
enum PU_XU_CTRL {
    PU_XU_ANF_ENABLE = 1,
    PU_XU_NF_STRENGTH,
    PU_XU_TF_STRENGTH,
    PU_XU_SINTER,
    PU_XU_ADAPTIVE_WDR_ENABLE,
    PU_XU_WDR_STRENGTH,
    PU_XU_AE_ENABLE,
    PU_XU_EXPOSURE_TIME,
    PU_XU_AWB_ENABLE,
    PU_XU_WB_TEMPERATURE,
    PU_XU_VFLIP,
    PU_XU_HFLIP,
    PU_XU_WB_ZONE_SEL_ENABLE,
    PU_XU_WB_ZONE_SEL,
    PU_XU_EXP_ZONE_SEL_ENABLE,
    PU_XU_EXP_ZONE_SEL,
    PU_XU_MAX_ANALOG_GAIN,
    PU_XU_HISTO_EQ,
    PU_XU_SHARPEN_FILTER,
    PU_XU_GAIN_MULTIPLIER,
    PU_XU_CROP,
    PU_XU_EXP_MIN_FR_RATE,
    PU_XU_DEWARP_PARAMS1,
    PU_XU_DEWARP_PARAMS2,
    PU_XU_COMPOSITOR_PARAMS,
    PU_XU_CONFIG_PARAMS,
    PU_XU_SATURATION_MODE,
    PU_XU_BRIGHTNESS_MODE,
    PU_XU_CONTRAST_MODE,
    PU_XU_MVMT_QUERY,
    PU_XU_SENSOR_FRAMERATE,
    PU_XU_AEROI,
};

typedef enum {
    /** Success (no error) */
    MXCAM_OK = 0,
    /** Started EEPROM FW Upgrade */
    MXCAM_STAT_EEPROM_FW_UPGRADE_START,//1
    /** Completed  EEPROM FW Upgrade */
    MXCAM_STAT_EEPROM_FW_UPGRADE_COMPLETE,//2
    /** Started SNOR FW Upgrade */
    MXCAM_STAT_SNOR_FW_UPGRADE_START,//3
    /** Completed SNOR FW Upgrade */
    MXCAM_STAT_SNOR_FW_UPGRADE_COMPLETE,//4
    /** Completed FW Upgrade */
    MXCAM_STAT_FW_UPGRADE_COMPLETE,//5
    /** EEPROM Erase in progress */
    MXCAM_STAT_EEPROM_ERASE_IN_PROG,//6
    /** EEPROM config save in progress */
    MXCAM_STAT_EEPROM_SAVE_IN_PROG,//7
    /** ERR numbers starts here */
    MXCAM_ERR_FAIL = 128,
    /** FW Image is corrupted */
    MXCAM_ERR_FW_IMAGE_CORRUPTED,//129
    /** SNOR FW upgrade failed */
    MXCAM_ERR_FW_SNOR_FAILED,//130
    /** Unsupported Flash memory */
    MXCAM_ERR_FW_UNSUPPORTED_FLASH_MEMORY,//131
    /** Erase size exceeds MAX_VEND_SIZE */
    MXCAM_ERR_ERASE_SIZE,//132
    /** Unknown area to erase */
    MXCAM_ERR_ERASE_UNKNOWN_AREA,//133
    /** Unknown area to save */
    MXCAM_ERR_SAVE_UNKNOWN_AREA,//134
    /** Not enough memory to save new key on memory */
    MXCAM_ERR_SETKEY_OVER_FLOW_NO_MEM,//135
    /** Unknown area to set key */
    MXCAM_ERR_SETKEY_UNKNOWN_AREA,//136
    /** Unknown area to remove key */
    MXCAM_ERR_REMOVE_KEY_UNKNOWN_AREA,//137
    /** Unknown area to get key */
    MXCAM_ERR_GETVALUE_UNKNOWN_AREA,//138
    /** Value not found for given key */
    MXCAM_ERR_GETVLAUE_KEY_NOT_FOUND,//139
    /** Failed to read TCW from flash */
    MXCAM_ERR_TCW_FLASH_READ,//140
    /** Failed to write TCW on flash */
    MXCAM_ERR_TCW_FLASH_WRITE,//141
    /** Failed to allocate memory on camera*/
    MXCAM_ERR_MEMORY_ALLOC,//142
    /** Vendor area is not initialized */
    MXCAM_ERR_VEND_AREA_NOT_INIT,//143
    /** Json syntax error */
    MXCAM_ERR_VEND_ERR_JSON_SYNTAX_ERR, //144
    MXCAM_ERR_SETKEY_UNSUPPORTED, //145
    /** Ispcfg syntax error */
    MXCAM_ERR_VEND_ERR_ISPCFG_SYNTAX_ERR, //146
    //Don't change the above values
    /** Unknown area to get config size */
    MXCAM_ERR_GET_CFG_SIZE_UNKNOWN_AREA = 150,
    /** Invalid parameter(s) */
    MXCAM_ERR_INVALID_PARAM,//151
    /** Not a valid device */
    MXCAM_ERR_INVALID_DEVICE,//152
    /** Failed to send image */
    MXCAM_ERR_IMAGE_SEND_FAILED,//153
    /** File not found */
    MXCAM_ERR_FILE_NOT_FOUND,//154
    /** Not enough memory */
    MXCAM_ERR_NOT_ENOUGH_MEMORY,//155
    /** Not a valid image */
    MXCAM_ERR_NOT_VALID_IMAGE,//156
    /** vid and pid already registered */
    MXCAM_ERR_VID_PID_ALREADY_REGISTERED,//157
    /** device not found */
    MXCAM_ERR_DEVICE_NOT_FOUND,//158
    /** vendor area not initialized*/
    MXCAM_ERR_UNINITIALIZED_VENDOR_MEMORY,//159
    /** feature not supported*/
    MXCAM_ERR_FEATURE_NOT_SUPPORTED,//160
    /** i2c read error */
    MXCAM_ERR_I2C_READ = 180,
    /** i2c write error */
    MXCAM_ERR_I2C_WRITE,
    /** spi read-write error */
    MXCAM_ERR_SPI_RW,
    /** Invalid bootloader used */
    MXCAM_ERR_INVALID_BOOTLOADER,
    /** PWMLED is active **/
    MXCAM_ERR_PWMLED_ACTIVE
} MXCAM_STAT_ERR;

typedef enum  {
    RUNNING_FW_HEADER = 0,
    SNOR_FW_HEADER,
    BOOTLOADER_HEADER,
    UNDEFINED,
} IMG_HDR_TYPE;

typedef struct image_header {
    /** Image Header Magic Number 0x27051956 */
    uint32_t        ih_magic;
    /** Image Header CRC Checksum */
    uint32_t        ih_hcrc;
    /** Image Creation Timestamp in  in ctime format*/
    uint32_t        ih_time;
    /** Image Data Size           */            
    uint32_t        ih_size;
    /** Data  Load  Address       */         
    uint32_t        ih_load;
    /** Entry Point Address       */  
    uint32_t        ih_ep;
    /** Image Data CRC Check sum   */
    uint32_t        ih_dcrc;
    /** Operating System          */
    uint8_t         ih_os;
    /** CPU architecture          */              
    uint8_t         ih_arch;
    /** Image Type                */
    uint8_t         ih_type;
    /** Compression Type          */
    uint8_t         ih_comp;
    /** Image Name                */     
    uint8_t         ih_name[IH_NMLEN]; 
} image_header_t;


#define QMED_BOX_TYPE 0x716d6564
#define QMED_MAJOR_MEDIA_TYPE_AAC 0x1
#define QMED_MAJOR_MEDIA_TYPE_H264 0x2
#define QMED_MAJOR_MEDIA_TYPE_PCM 0x3
#define QMED_MAJOR_MEDIA_TYPE_MP2 0x6
#define QMED_MAJOR_MEDIA_TYPE_JPEG 0x7
#define QMED_MAJOR_MEDIA_TYPE_Q711 0x9
#define QMED_MAJOR_MEDIA_TYPE_Q728 0xa
#define QMED_MAJOR_MEDIA_TYPE_Q722 0xb
#define QMED_MAJOR_MEDIA_TYPE_Q726 0xc
#define QMED_MAJOR_MEDIA_TYPE_QOPUS 0xd
#define QMED_MAJOR_MEDIA_TYPE_MAX 0xe

#define QMED_MINOR_MEDIA_TYPE_Q711_ALAW 0x0
#define QMED_MINOR_MEDIA_TYPE_Q711_ULAW 0x1
#define QMED_MINOR_MEDIA_TYPE_Q726_ITU_BYTE_ORDER 0x0
#define QMED_MINOR_MEDIA_TYPE_Q726_IETF_BYTE_ORDER 0x1

#define QMED_VERSION (A) ((A >> 24) && 0xff)

#define QMED_SHA_SIZE            8

/** enum of video format used in a particular channel */
typedef enum {
    FIRST_VID_FORMAT       = 0,
    /** H264 in elementary stream format */
    VID_FORMAT_H264_RAW    = 0,
    /** H264 in transport stream format */
    VID_FORMAT_H264_TS     = 1,
    /** MJPEG in elmentary stream format */
    VID_FORMAT_MJPEG_RAW   = 2,
    /** YUV stream in YUY2 format */
    VID_FORMAT_YUY2_RAW    = 3,
    /** YUV stream in NV12 format */
    VID_FORMAT_NV12_RAW    = 4,
    /** Luma stream format */
    VID_FORMAT_GREY_RAW    = 5,
    /** H264 and AAC in transport stream format */
    VID_FORMAT_H264_AAC_TS = 6,
    /** mux data */
    VID_FORMAT_MUX         = 7,
    
    VID_FORMAT_AAC_RAW       = 8,
    /** total number of video formats supported */
    NUM_VID_FORMAT
} video_format_t;

/** enum to indicate the H264 profile used for encoding */
typedef enum {
    /** baseline profile */
    PROFILE_BASELINE = 0,
    /** main profile */
    PROFILE_MAIN     = 1,
    /** high profile */
    PROFILE_HIGH     = 2,
    /** number of h264 profiles supported */
    NUM_PROFILE
} video_profile_t;

/** enum to turn on/off video flip */
typedef enum {
    /** turn off flip */
    FLIP_OFF    = 0,
    /** trun on flip */
    FLIP_ON     = 1,
    /** number of flip mode supported */
    NUM_FLIP
} video_flip_t;

/** enum to set the sensor exposure mode */
typedef enum {
    /** auto exposure */
    EXP_AUTO = 0,
    /** manual exposure */
    EXP_MANUAL  = 1,
    /** number of exposure mode supported */
    NUM_EXP
} exp_set_t;

typedef enum {
    SATURATION_AUTO = 0,
    SATURATION_MANUAL  = 1,
    NUM_SATURATION
} saturation_mode_t;

typedef enum {
    BRIGHTNESS_AUTO = 0,
    BRIGHTNESS_MANUAL  = 1,
    NUM_BRIGHTNESS
} brightness_mode_t;

typedef enum {
    CONTRAST_AUTO = 0,
    CONTRAST_MANUAL  = 1,
    NUM_CONTRAST
} contrast_mode_t;

/** enum to set the white balance mode in the sensor */
typedef enum {
    /** auto mode */
    WB_MODE_AUTO = 0,
    /** manual mode */
    WB_MODE_MANUAL = 1,
    /** number of white balance modes supported */
    NUM_WB
}white_balance_mode_t;

/** enum to set power line frequency mode */
typedef enum {
    /** disable power line frequency */
    PWR_LINE_FREQ_MODE_DISABLE = 0,
    /** 50Hz power line frequency */
    PWR_LINE_FREQ_MODE_50HZ = 1,
    /** 60Hz power line frequency */
    PWR_LINE_FREQ_MODE_60HZ = 2
}pwr_line_freq_mode_t;


/** Structure containing the information regarding an encoding channel */
typedef struct
{
    /** format of the video used in the channel */
    video_format_t  format;

    /** width of the video in the channel */
    uint16_t    width;

    /** height of the video in the channel */
    uint16_t    height;

    /** frame Rate of the video in the channel */
    uint32_t    framerate;

    /** GOP size of video to be used in the channel, applicable only for
    VID_FORMAT_H264_RAW,
    VID_FORMAT_H264_TS,
    VID_FORMAT_H264_AAC_TS */
    uint32_t    goplen;

    /** H264 profile used for encoding in the channel, applicable only for
    VID_FORMAT_H264_RAW,
    VID_FORMAT_H264_TS,
    VID_FORMAT_H264_AAC_TS */
    video_profile_t profile;

    /** bitrate of the video in the channel */
    uint32_t    bitrate;

    /** compression quality in terms of the QP factor set for the video
     on this channel, applicable only for VID_FORMAT_MJPEG_RAW */
    uint32_t    compression_quality;
} video_channel_info_t;

typedef struct{
  unsigned int v : 8;
  unsigned int f : 24;
}QMedFlags;

typedef struct {
  unsigned int boxSize;
  unsigned int boxType;
  union {
    unsigned int value;
    QMedFlags field;
  } boxFlags;
  unsigned int majorMediaType;
  unsigned int minorMediaType;
} QMedStruct;


typedef struct
{
    unsigned long version;
    unsigned long width;
    unsigned long height;
    unsigned long sampleTicks;
    unsigned long motionCounter;
    unsigned long motionBitmapSize;
} QMedH264Struct;

typedef struct {
    char *qmedExt;
    unsigned int total_len;
}metadata_t;

typedef struct {
  unsigned int hashSize;
  unsigned int hashPayload[QMED_SHA_SIZE];
}QMedVer1InfoStruct;



/** Video Information structure used for processing the video data received 
from the camera in the callback function */
#ifdef RTW_PACK_STRUCT_USE_INCLUDES
#  include "pack_begin.h"
#endif
RTW_PACK_STRUCT_BEGIN
typedef struct {
    /** format of the video frame received */
    video_format_t format;

    /** video frame timestamp in terms of ticks of 90kHz clock, where each tick
     corresponds to 1/(90 * 1000) sec or 1/90 ms */
    uint64_t       ts;
#if 0
    /** motion vector statistics information of the video frame,
     only in case of
     VID_FORMAT_YUY2_RAW,
     VID_FORMAT_NV12_RAW
     VID_FORMAT_GREY_RAW */
    motion_stat_t  stats;
#endif
    /** physical buffer index of the video frame dequeued by the V4L.
     This needs to be used by MXUVC application to queue back the video frame
     after processing, in the mxuvc_video_cb_buf_done() function described
     later */
    int            buf_index;
#if 0
    /** TBD */
    pme_info_t     pme;
    /** TBD */
    rectangle_info_t  rect;
    /** TBD */
#endif
    metadata_t     metadata;

}/*__attribute__((packed))*/video_info_t;
RTW_PACK_STRUCT_STRUCT;
RTW_PACK_STRUCT_END
#ifdef RTW_PACK_STRUCT_USE_INCLUDES
#  include "pack_end.h"
#endif

typedef void (*mxuvc_video_cb_t)(unsigned char *buffer, unsigned int size,
                                    video_info_t info, void *user_data);

struct uvc_xu_data {
    __u8    entity[16]; /* Extension unit GUID */
    __u8    selector;   /* UVC control selector */
    __u8    size;       /* V4L2 control size (in bits) */
    __u8    offset;     /* V4L2 control offset (in bits) */
    __u32   id;     /* V4L2 control identifier */
    __u8    name[32];   /* V4L2 control name */
};

//static unsigned int buffer_count = BUFFER_COUNT;
static unsigned int mux_channel_count = 1; //minimum mux channel count has to be one else CH1 init logic will fail.
static int videoInit=0;     //Flag indicating if video is initialized
#if 0
struct buffer_info {
    void   *start;
    size_t  length;
};
#endif
static struct video_stream {
    int fd;
    int enabled;
    int started;

    /* V4l2 buffer/settings */
    //struct buffer_info *buffers;
    //unsigned int n_buffers;
    int n_buffers;
    struct v4l2_format fmt;

    /* Callback */
    mxuvc_video_cb_t cb;
    void *cb_user_data;

    /* Current stream format settings.
     * Those settings are cached in order to populate the SECS
     * StreamFormat with the current values when doing a StreamFormatProbe */
    video_format_t cur_vfmt;
    uint16_t cur_width;
    uint16_t cur_height;
    uint32_t cur_bitrate;
    uint32_t cur_framerate;

    /* Captured frame count */
    unsigned int frame_count;

    /* AAC muxing enabled ? */
    unsigned int mux_aac_ts;

    /* SECS */
    //unsigned int secs_supported; /* SECS mode supported but not enabled */
    //unsigned int secs_enabled; /* SECS mode enabled */

} *video_stream;

typedef struct {
    int fd;
    int enabled;
    int started;
    int waiting;
    int ref_count;
    /* V4l2 buffer/settings */
    //struct buffer_info *buffers;
    unsigned int n_buffers;
    void *priv;
}mux_stream_t;

/** enum to indicate the video channel number */
typedef enum channel {
    FIRST_VID_CHANNEL = 0,
    /** channels for ip camera */
    /** video channel1 */
    CH1  = 0,
    /** video channel2 */
    CH2  = 1,
    /** video channel3 */
    CH3  = 2,
    /** video channel4 */
    CH4  = 3,
    /** video channel5 */
    CH5  = 4,
    /** video channel6 */
    CH6  = 5,
    /** video channel7 */
    CH7  = 6,
    /** number of mux channels in ip camera */
    NUM_MUX_VID_CHANNELS  = CH7+1,
    /** video channel for RAW stream */
    CH_RAW            = NUM_MUX_VID_CHANNELS,
    /** total number of video channels in ip camera */
    NUM_IPCAM_VID_CHANNELS
} video_channel_t;

int channel_init(video_channel_t ch);

/**
 * @brief This API is used to initialize the Linux video interface on the Host
 * system.
 * 
 * @return 0 on Success, -1 on Failure
 */

int mxuvc_video_init();

/**
 * @brief This API is used to free all the memory allocated by the MXUVC.
 * This function automatically calls @ref mxuvc_video_stop if the video
 * has not been stopped yet, it is therefore not necessary to explicitly call
 * @ref mxuvc_video_stop before calling @ref mxuvc_video_deinit.
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_deinit();


/**
 * @brief This API is used to start the capture of video data from the specified
 * channel in the camera.
 * 
 * @param[in] ch video channel on which the video capture needs to be started
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_start(video_channel_t ch);

/**
 * @brief This API is used to stop the capture of video data from a channel in
 * the camera.
 * 
 * @param[in] ch video channel on which the video capture needs to be stopped
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_stop(video_channel_t ch);

/**
 * @brief This API is used to get the number of video channels supported by the
 * GEO Camera. The number of video channels supported by the camera is configuration
 * dependent.
 * 
 * @param[out] count number of video channels supported. This parameter is returned
 *                   from the camera based on the configuration in which it is running.
 *                   
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_get_channel_count(uint32_t *count);

/**
 * @brief This API gets the information regarding the video parameters set on the
 * channel in the Camera.
 * 
 * @param[in]  ch    video channel from which the information is needed
 * @param[out] info  pointer to the video channel information structure returned
 *                   from the camera
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_get_channel_info(video_channel_t ch, video_channel_info_t *info);

/**
 * @brief This API is used to force an I frame in the specified video channel.
 * Applicable only for VID_FORMAT_H264_RAW, VID_FORMAT_H264_TS, VID_FORMAT_H264_AAC_TS.
 * 
 * @param[in] ch video channel on which I frame needs to be forced
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_force_iframe(video_channel_t ch);

/**
 * @brief This API is used to get the format of the video data on the specified channel.
 * 
 * @param[in]   ch  video channel on which the format is queried
 * @param[out]  fmt pointer to receive the format of the video
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_get_format(video_channel_t ch, video_format_t *fmt);

/**
 * @brief This API is used to set the resolution of the video data on the specified channel.
 * 
 * @param[in] ch        video channel on which the resolution is to set
 * @param[in] width     width of the video in the specified channel
 * @param[in] height    height of the video in the specified channel
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_set_resolution(video_channel_t ch, uint16_t width, uint16_t height);

/**
 * @brief This API is used to get the resolution of the video data on the specified channel.
 * 
 * @param[in]  ch        video channel on which the resolution is queried
 * @param[out] width     pointer to receive the width of the video
 * @param[out] height    pointer to receive the height of the video
 * 
 * @return 0 on Success, -1 on Failure
 */
 int mxuvc_video_get_resolution(video_channel_t ch, uint16_t *width, uint16_t *height);

/**
 * @brief This api is used to set the frame rate on the video channel specified.
 * Typical Range: 1 to 30.
 * 
 * Not applicable for VID_FORMAT_YUY2_RAW,VID_FORMAT_NV12_RAW,VID_FORMAT_GREY_RAW.
 * 
 * @param[in] ch        video channel for which framerate is to set
 * @param[in] framerate framerate to set on the channel
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_set_framerate(video_channel_t ch, uint32_t framerate);

/**
 * @brief This api is used to get the frame rate on the video channel specified.
 * 
 * Not applicable for VID_FORMAT_YUY2_RAW,VID_FORMAT_NV12_RAW,VID_FORMAT_GREY_RAW.
 * 
 * @param[in]  ch        video channel for which framerate is to get
 * @param[out] framerate pointer to receive the framerate
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_get_framerate(video_channel_t ch, uint32_t *framerate);

/**
 * @brief This api is used to set GOP length on the video channel specified.
 * Typical Range: 0 to Max Integer (2147483647).
 * 
 * Applicable only for VID_FORMAT_H264_RAW, VID_FORMAT_H264_TS, VID_FORMAT_H264_AAC_TS.
 * 
 * @param[in] ch     video channel for which GOP length is to set
 * @param[in] value  GOP length to set on the channel
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_set_goplen(video_channel_t ch, uint32_t value);

/**
 * @brief This api is used to get GOP length on the video channel specified.
 * 
 * Applicable only for VID_FORMAT_H264_RAW, VID_FORMAT_H264_TS, VID_FORMAT_H264_AAC_TS.
 * 
 * @param[in]   ch      video channel for which GOP length is to get
 * @param[out]  value   pointer to receive the GOP length
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_get_goplen(video_channel_t ch, uint32_t *value);

/**
 * @brief This api is used to set bitrate on the video channel specified.
 * Typical Range: 100000 (100 kbps) to 2000000 (2 Mbps)
 * 
 * Not applicable for VID_FORMAT_YUY2_RAW,VID_FORMAT_NV12_RAW,VID_FORMAT_GREY_RAW.
 * 
 * @param[in] ch     video channel for which bitrate is to set
 * @param[in] value  bitrate to set on the channel
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_set_bitrate(video_channel_t ch, uint32_t value);

/**
 * @brief This api is used to get bitrate on the video channel specified.
 * 
 * Not applicable for VID_FORMAT_YUY2_RAW,VID_FORMAT_NV12_RAW,VID_FORMAT_GREY_RAW.
 * 
 * @param[in]  ch     video channel for which bitrate is to get
 * @param[out] value  pointer to receive the bitrate
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_get_bitrate(video_channel_t ch, uint32_t *value);

/**
 * @brief This api is used to set vertical flip of the given video channel.
 * 
 * Values: 1 Enable, 0 Disable
 * 
 * @param[in] ch video channel on which the parameter is set
 * @param[in] value value to be set in the camera
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_set_flip_vertical(video_channel_t ch, video_flip_t value);

/**
 * @brief This api is used to get vertical flip status of the given video channel.
 * 
 * Values: 1 Enable, 0 Disable
 * 
 * @param[in] ch video channel on which the parameter is set
 * @param[out] value pointer to parameter value updated by the camera
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_get_flip_vertical(video_channel_t ch, video_flip_t *value);

/**
 * @brief This api is used to set horizontal flip of the given video channel.
 * 
 * Values: 1 Enable, 0 Disable
 * 
 * @param[in] ch video channel on which the parameter is set
 * @param[in] value value to be set in the camera
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_set_flip_horizontal(video_channel_t ch, video_flip_t value);

/**
 * @brief This api is used to get horizontal flip status of the given video channel.
 * 
 * Values: 1 Enable, 0 Disable
 * 
 * @param[in] ch video channel on which the parameter is set
 * @param[out] value pointer to parameter value updated by the camera
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_get_flip_horizontal(video_channel_t ch, video_flip_t *value);

/**
 * @brief This api is used to set image contrast of the given video channel.
 * 
 * Typical Range: 0 to 200
 * 
 * @param[in] ch video channel on which the parameter is set
 * @param[in] value value to be set in the camera
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_set_contrast(video_channel_t ch, uint16_t value);

/**
 * @brief This api is used to get image contrast of the given video channel.
 * 
 * Typical Range: 0 to 200
 * 
 * @param[in] ch video channel on which the parameter is set
 * @param[out] value pointer to parameter value updated by the camera
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_get_contrast(video_channel_t ch, uint16_t *value);

/**
 * @brief This api is used to set image brightness of the given video channel.
 * 
 * Typical Range: -255 to 255
 * 
 * @param[in] ch video channel on which the parameter is set
 * @param[in] value value to be set in the camera
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_set_brightness(video_channel_t ch, int16_t value);

/**
 * @brief This api is used to get image brightness of the given video channel.
 * 
 * Typical Range: -255 to 255
 * 
 * @param[in] ch video channel on which the parameter is set
 * @param[out] value pointer to parameter updated by the camera
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_get_brightness(video_channel_t ch, int16_t *value);

/**
 * @brief This api is used to set image saturation of the given video channel.
 * 
 * Typical Range: 0 to 200
 * 
 * @param[in] ch video channel on which the parameter is set
 * @param[in] value value to be set in the camera
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_set_saturation(video_channel_t ch, uint16_t value);

/**
 * @brief This api is used to get image saturation of the given video channel.
 * 
 * Typical Range: 0 to 200
 * 
 * @param[in] ch video channel on which the parameter is set
 * @param[out] value pointer to parameter updated by the camera
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_get_saturation(video_channel_t ch, uint16_t *value);

/**
 * @brief This api is used to set image gain of the given video channel.
 * 
 * Typical Range: 1 to 100
 * 
 * @param[in] ch video channel on which the parameter is set
 * @param[in] value value to be set in the camera
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_set_gain(video_channel_t ch, uint16_t value);

/**
 * @brief This api is used to get image gain of the given video channel.
 * 
 * Typical Range: 1 to 100
 * 
 * @param[in] ch video channel on which the parameter is set
 * @param[out] value pointer to parameter updated by the camera
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_get_gain(video_channel_t ch, uint16_t *value);

/**
 * @brief This api is used to set image sharpness of the given video channel.
 * 
 * Typical Range: 1 to 100
 * 
 * @param[in] ch video channel on which the parameter is set
 * @param[in] value value to be set in the camera
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_set_sharpness(video_channel_t ch, uint16_t value);

/**
 * @brief This api is used to get image sharpness of the given video channel.
 * 
 * Typical Range: 1 to 100
 * 
 * @param[in] ch video channel on which the parameter is set
 * @param[out] value pointer to parameter updated by the camera
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_get_sharpness(video_channel_t ch, uint16_t *value);

/**
 * @brief This api is used to set the exposure mode (auto or manual) and
 * the exposure time in the manual mode.
 * 
 * @param[in] ch video channel on which the parameter is set
 * @param[in] sel selects to exposure mode (Auto or Manual)
 * @param[in] value exposure time value in the Manual mode - Range 0 to 255
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_set_exp(video_channel_t ch, exp_set_t sel, uint16_t value);

/**
 * @brief This api is used to get the exposure mode (auto or manual) and
 * the exposure time in the manual mode.
 * 
 * @param[in] ch video channel on which the parameter is set
 * @param[out] sel selects to exposure mode (Auto or Manual)
 * @param[out] value pointer to parameter updated by the camera
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_get_exp(video_channel_t ch, exp_set_t *sel, uint16_t *value);

/**
 * @brief This api is used to set the white balance mode (auto or manual) and
 * the white balance temperature in Kelvin (works in auto mode only).
 * 
 * @param[in] ch video channel on which the parameter is set
 * @param[in] sel selects to white balance mode (Auto or Manual)
 * @param[in] value white balance temperature in either Mode - Range 2000 to 8000
 *                  if 0 is passed, no change will be done
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_set_wb(video_channel_t ch, white_balance_mode_t sel, uint16_t value);

/**
 * @brief This api is used to get the white balance mode (auto or manual) and
 * the white balance temperature in the manual mode.
 * 
 * @param[in] ch video channel on which the parameter is set
 * @param[out] sel selects to white balance mode (Auto or Manual)
 * @param[out] value pointer to parameter updated by the camera
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_get_wb(video_channel_t ch, white_balance_mode_t *sel, uint16_t *value);

/**
 * @brief This API is used to select the power line frequency of the operating region.
 * Sensor exposure value under the auto-exposure algorithm will be adjusted to avoid
 * flickering caused by power level oscillation. 
 * 0 disables this function, and the values of 1 and 2 represents 50 and 60Hz power
 * line frequency respectively.
 * 
 * @param[in] ch video channel on which the parameter is set
 * @param[in] mode value indicating power line frequency mode
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_set_pwr_line_freq(video_channel_t ch,pwr_line_freq_mode_t mode);

/**
 * @brief This api is used to get the status of power line frequency of the operating
 * region.
 *
 * @param[in] ch video channel on which the parameter is set
 * @param[out] mode pointer to parameter updated by the camera
 * 0 \u2013 disable, 1 \u2013 50Hz, 2 \u2013 60Hz
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_video_get_pwr_line_freq(video_channel_t ch,pwr_line_freq_mode_t *mode);

int mxuvc_video_set_format(video_channel_t ch, video_format_t fmt);
int mxuvc_video_set_brightness_mode(video_channel_t ch, brightness_mode_t mode);
int mxuvc_video_get_brightness_mode(video_channel_t ch, brightness_mode_t *mode);
int mxuvc_video_set_contrast_mode(video_channel_t ch, contrast_mode_t mode);
int mxuvc_video_get_contrast_mode(video_channel_t ch, contrast_mode_t *mode);
int mxuvc_video_set_saturation_mode(video_channel_t ch, saturation_mode_t mode);
int mxuvc_video_get_saturation_mode(video_channel_t ch, saturation_mode_t *mode);


/**
 * @brief This API is used to set the microphone gain.
 * This will work only when AGC is off.
 * 
 * @param[in] volume microphone gain level. Range is from 0 to 100 without any units.
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_audio_set_volume(int volume);

/**
 * @brief This API is used to mute or unmute the micro phone.
 * It will mute or unmute all the audio input channels.
 * 
 * @param[in] bMute microphone mute value. 0 - unmute, 1 - mute
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_audio_set_mic_mute(int bMute);

/**
 * @brief This API is used to mute or unmute only the left microphone
 * in case of stereo. This will work only when ASP is off (in JSON).
 * 
 * @param[in] bMute microphone mute value. 0 - unmute, 1 - mute
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_audio_set_left_mic_mute(int bMute);

/**
 * @brief This API is used to mute or unmute only the right microphone
 * in case of stereo. This will work only when ASP is off (in JSON).
 * 
 * @param[in] bMute microphone mute value. 0 - unmute, 1 - mute
 * 
 * @return 0 on Success, -1 on Failure
 */
int mxuvc_audio_set_right_mic_mute(int bMute);

/**
 * @brief This API resets the camera
 *
 * @return 0 on Success, -1 on Failure
 */
int mxcam_reset(void);

/**
 * @brief This API gets VALUE for given KEY from camera's volatile memory
 *
 * @param[in]   keyname KEY
 * @param[out] value Value returned by camera for the given key
 *
 * @return 0 on Success, -1 on Failure
 */
int mxcam_get_value(const char* keyname, char** value);

/**
 * @brief This API frees the resource allocated by mxcam_get_value
 *
 * @param[in]   value_mem:  pointer to a memory,allocated by mxcam_get_value
 *
 * @return MXCAM_OK upon Success or following on Failure:
 * MXCAM_ERR_INVALID_PARAM - if value_mem is NULL
 * MXCAM_ERR_GETVALUE_UNKNOWN_AREA -  Unknown area to get value
 * MXCAM_ERR_GETVLAUE_KEY_NOT_FOUND -  Value not found for given KEY
 */
int mxcam_free_get_value_mem(char* value_mem);

/**
 * @brief This API adds/modifies a KEY=VALUE pair on camera's volatile memory
 *
 * @param[in] keyname :KEY
 * @param[in] value       :VALUE
 *
 * @return 0 upon Success or following on Failure:
 * MXCAM_ERR_INVALID_PARAM - if any one of the following condition meet
 * - keyname is NULL
 * - value is NULL
 * MXCAM_ERR_SETKEY_OVER_FLOW_NO_MEM - Not enough memory to save new
 * key on memory
 *  MXCAM_ERR_SETKEY_UNKNOWN_AREA -  Unkown area to set key
 */

int mxcam_set_key(const char* keyname, const char* value);

/**
 * @brief This API check the json syntax
 *
 * @param[in] ispcfg:      ISP config file name
 *
 * @return MXCAM_OK upon Success or following on Failure:
 * MXCAM_ERR_FILE_NOT_FOUND - if given ispcfg file is not found
 * MXCAM_ERR_VEND_ERR_ISPCFG_SYNTAX_ERR - if syntax is wrong
 */
int mxcam_check_ispcfg_syntax(const char *ispcfg);

/**
 * @brief This API stores given ISP configuration binary on camera's non volatile memory.
 *
 * @param[in] ispcfg:      ISP config file name
 *
 * @return 0 on Success, -1 on Failure
 */
int mxcam_write_ispcfg(const char *ispcfgfile);

/**
 * @brief This API saves the given JSON on camera persistent storage memory
 *
 * @param[in] jsonfile:	  JSON file name
 *
 * @return MXCAM_OK upon Success or following on Failure:
 * MXCAM_ERR_INVALID_PARAM - if invalid JSON is provided
 */
int mxcam_save_eeprom_config(const char *jsonfile);

/**
  * @brief This API does QCC read operation in camera
  *
  * @param[in] bid:	  qcc block id
  * @param[in] addr:  qcc register address
  * @param[in] length: length of the register
  * @param[out]  value     : return value
  *
  * @return MXCAM_OK upon Success or following on Failure:
  * MXCAM_ERR_INVALID_PARAM - length is  not 1, 2 or 4
  * MXCAM_ERR_INVALID_DEVICE -  Device not booted completely
  */
int mxcam_qcc_read(uint16_t bid, uint16_t addr, uint16_t length, uint32_t *value);

/**
 * @brief This API reads 64 bytes flash image header. This image header carries
 * camera information such as firmware version, etc.
 *
 * @param[in] header  :pointer to image_header_t structure
 * @param[in] hdr_type : possible value of this field is  
 * 0, for running fw image header
 * 1, if fw image hdr need to be read from sno
 * 2, for bootloader header
 *
 * @return MXCAM_OK upon Success or following on Failure:
 * MXCAM_ERR_INVALID_PARAM - if header is NULL
 */
int mxcam_read_flash_image_header(image_header_t *header, IMG_HDR_TYPE hdr_type);

/**
 * @brief TBD.
 *
 * @param[in] TBD
 *
 * @return 0 on Success, -1 on Failure
 */
int mxcam_rw_gpio (int gpio_no, int value, int gpio_write, int *status);


void get_adts_header(int size, unsigned char *adtsHeader);

#endif /* _PATCH_UVC_GEO_H_ */