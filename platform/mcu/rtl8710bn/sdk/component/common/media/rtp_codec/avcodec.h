#ifndef _AV_CODEC_H_
#define _AV_CODEC_H_

/* Is this the place to include different codec header for rtp usage?*/


struct codec_info
{
       int codec_id;
       const char codec_name[8];
       u8 pt;
       u32 clock_rate;
       u8 audio_channels;
       void *priv;
};

/* media type list -- range from 0-255 stored in 1 BYTE*/
#define AVMEDIA_TYPE_VIDEO 0
#define AVMEDIA_TYPE_AUDIO 1
#define AVMEDIA_TYPE_SUBTITLE  2
#define AVMEDIA_TYPE_UNKNOWN 255

/*codec id list -- id must match its placing order (starting from 0) in av_codec_tables*/

#define AV_CODEC_ID_MJPEG 0
#define AV_CODEC_ID_H264  1
#define AV_CODEC_ID_PCMU  2
#define AV_CODEC_ID_PCMA  3
#define AV_CODEC_ID_MP4A_LATM 4
#define AV_CODEC_ID_MP4V_ES 5
#define AV_CODEC_ID_UNKNOWN -1

/*rtp payload type mapping and standard rtp payload type table -- range from 0-255 in 1 BYTE*/
#define RTP_PT_PCMU     0
#define RTP_PT_GSM      3
#define RTP_PT_G723     4
#define RTP_PT_DVI4_R8000        5
#define RTP_PT_DVI4_R16000       6
#define RTP_PT_LPC      7
#define RTP_PT_PCMA     8
#define RTP_PT_G722     9
#define RTP_PT_L16_C2   10
#define RTP_PT_L16_C1   11
#define RTP_PT_QCELP    12
#define RTP_PT_CN       13
#define RTP_PT_MPA      14
#define RTP_PT_G728     15
#define RTP_PT_DVI4_R11025      16
#define RTP_PT_DVI4_R22050      17
#define RTP_PT_G719     18
#define RTP_PT_CELB     25
#define RTP_PT_JPEG     26
#define RTP_PT_NV       28
#define RTP_PT_H261     31
#define RTP_PT_MPV      32
#define RTP_PT_MP2T     33
#define RTP_PT_H263     34
#define RTP_PT_RTCP_BASE        72
#define RTP_PT_DYN_BASE         96
#define RTP_PT_UNKNOWN          255
/* AAC PROFILE */
#define AV_PROFILE_AAC_MAIN 0
#define AV_PROFILE_AAC_LOW  1
#define AV_PROFILE_AAC_SSR  2
#define AV_PROFILE_AAC_LTP  3
#define AV_PROFILE_AAC_HE   4
#define AV_PROFILE_AAC_HE_V2 28
#define AV_PROFILE_AAC_LD   22
#define AV_PROFILE_AAC_ELD  38
#define AV_PROFILE_MPEG2_AAC_LOW 128
#define AV_PROFILE_MPEG2_AAC_HE  131

#if 0
/* MPEG4 VIDEO PROFILE */
#define AV_PROFILE_MPEG4_SIMPLE                     0
#define AV_PROFILE_MPEG4_SIMPLE_SCALABLE            1
#define AV_PROFILE_MPEG4_CORE                       2
#define AV_PROFILE_MPEG4_MAIN                       3
#define AV_PROFILE_MPEG4_N_BIT                      4
#define AV_PROFILE_MPEG4_SCALABLE_TEXTURE           5
#define AV_PROFILE_MPEG4_SIMPLE_FACE_ANIMATION      6
#define AV_PROFILE_MPEG4_BASIC_ANIMATED_TEXTURE     7
#define AV_PROFILE_MPEG4_HYBRID                     8
#define AV_PROFILE_MPEG4_ADVANCED_REAL_TIME         9
#define AV_PROFILE_MPEG4_CORE_SCALABLE             10
#define AV_PROFILE_MPEG4_ADVANCED_CODING           11
#define AV_PROFILE_MPEG4_ADVANCED_CORE             12
#define AV_PROFILE_MPEG4_ADVANCED_SCALABLE_TEXTURE 13
#define AV_PROFILE_MPEG4_SIMPLE_STUDIO             14
#define AV_PROFILE_MPEG4_ADVANCED_SIMPLE           15
/* DTS */
#define AV_PROFILE_DTS         20
#define AV_PROFILE_DTS_ES      30
#define AV_PROFILE_DTS_96_24   40
#define AV_PROFILE_DTS_HD_HRA  50
#define AV_PROFILE_DTS_HD_MA   60
#define AV_PROFILE_DTS_EXPRESS 70
/* MPEG2 */
#define AV_PROFILE_MPEG2_422    0
#define AV_PROFILE_MPEG2_HIGH   1
#define AV_PROFILE_MPEG2_SS     2
#define AV_PROFILE_MPEG2_SNR_SCALABLE  3
#define AV_PROFILE_MPEG2_MAIN   4
#define AV_PROFILE_MPEG2_SIMPLE 5
/* H264 */
#define AV_PROFILE_H264_CONSTRAINED  (1<<9)  // 8+1; constraint_set1_flag
#define AV_PROFILE_H264_INTRA        (1<<11) // 8+3; constraint_set3_flag
 
#define AV_PROFILE_H264_BASELINE             66
#define AV_PROFILE_H264_CONSTRAINED_BASELINE (66|FF_PROFILE_H264_CONSTRAINED)
#define AV_PROFILE_H264_MAIN                 77
#define AV_PROFILE_H264_EXTENDED             88
#define AV_PROFILE_H264_HIGH                 100
#define AV_PROFILE_H264_HIGH_10              110
#define AV_PROFILE_H264_HIGH_10_INTRA        (110|FF_PROFILE_H264_INTRA)
#define AV_PROFILE_H264_HIGH_422             122
#define AV_PROFILE_H264_HIGH_422_INTRA       (122|FF_PROFILE_H264_INTRA)
#define AV_PROFILE_H264_HIGH_444             144
#define AV_PROFILE_H264_HIGH_444_PREDICTIVE  244
#define AV_PROFILE_H264_HIGH_444_INTRA       (244|FF_PROFILE_H264_INTRA)
#define AV_PROFILE_H264_CAVLC_444            44
#endif

/*av codec tables*/
//#include "section_config.h"
//SDRAM_DATA_SECTION
static const struct codec_info av_codec_tables[] = {
  {AV_CODEC_ID_MJPEG, "MJPEG", RTP_PT_JPEG, 90000, 0, 0},
  {AV_CODEC_ID_H264, "H264", RTP_PT_DYN_BASE, 90000, 0, 0},
  {AV_CODEC_ID_PCMU, "PCMU", RTP_PT_PCMU, 8000, 1, 0},
  {AV_CODEC_ID_PCMA, "PCMA", RTP_PT_PCMA, 8000, 1, 0},
  {AV_CODEC_ID_MP4A_LATM, "MP4A", RTP_PT_DYN_BASE, 16000, 2, 0},
  {AV_CODEC_ID_MP4V_ES, "MP4V", RTP_PT_DYN_BASE, 90000, 0, 0},
};
#define AVCODEC_SIZE 6

void get_codec_by_id(struct codec_info *c, int id);

#endif  //_AV_CODEC_H_