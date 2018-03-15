
/*
* uvc video capture example for GEO gc6500
*/

#include "FreeRTOS.h"
#include "task.h"
#include <osdep_service.h>
#include "patch_uvc_geo.h"
//uvc/v4l2 header files
#include "videodev2.h"
#include "uvcvideo.h"
#include "v4l2_intf.h"
#include "ff.h"

// in v4l2_driver.c
extern streaming_state stream_state;

static Mutex lock;

#define CLEAR(x) memset(&(x), 0, sizeof(x))

/* -----------------------------imported from Qbox.c-------------------------*/

#define QBOX_TYPE   (('q' << 24) | ('b' << 16) | ('o' << 8) | ('x'))

#define QBOX_SAMPLE_TYPE_AAC 0x1
#define QBOX_SAMPLE_TYPE_QAC 0x1
#define QBOX_SAMPLE_TYPE_H264 0x2
#define QBOX_SAMPLE_TYPE_QPCM 0x3
#define QBOX_SAMPLE_TYPE_DEBUG 0x4
#define QBOX_SAMPLE_TYPE_H264_SLICE 0x5
#define QBOX_SAMPLE_TYPE_QMA 0x6
#define QBOX_SAMPLE_TYPE_VIN_STATS_GLOBAL 0x7
#define QBOX_SAMPLE_TYPE_VIN_STATS_MB 0x8
#define QBOX_SAMPLE_TYPE_Q711 0x9
#define QBOX_SAMPLE_TYPE_Q722 0xa
#define QBOX_SAMPLE_TYPE_Q726 0xb
#define QBOX_SAMPLE_TYPE_Q728 0xc
#define QBOX_SAMPLE_TYPE_JPEG 0xd
#define QBOX_SAMPLE_TYPE_MPEG2_ELEMENTARY 0xe
#define QBOX_SAMPLE_TYPE_USER_METADATA 0xf
#define QBOX_SAMPLE_TYPE_MB_STAT_LUMA 0x10
#define QBOX_SAMPLE_TYPE_MB_STAT_NV12 0x11
#define QBOX_SAMPLE_TYPE_PME_STAT_LUMA 0x12
#define QBOX_SAMPLE_TYPE_PME_STAT_NV12 0x13
#define QBOX_SAMPLE_TYPE_MB_PME_STAT_LUMA 0x14
#define QBOX_SAMPLE_TYPE_MB_PME_STAT_NV12 0x15
#define QBOX_SAMPLE_TYPE_LUMA 0x16
#define QBOX_SAMPLE_TYPE_NV12 0x17
#define QBOX_SAMPLE_TYPE_QOPUS 0x18
#define QBOX_SAMPLE_TYPE_REC_STAT_LUMA 0x19
#define QBOX_SAMPLE_TYPE_REC_STAT_NV12 0x1a
#define QBOX_SAMPLE_TYPE_MAX 0x1a

//#define QBOX_SAMPLE_FLAGS_CONFIGURATION_INFO 0x0001
#define QBOX_SAMPLE_FLAGS_CTS_PRESENT        0x0002
//#define QBOX_SAMPLE_FLAGS_SYNC_POINT         0x0004
#define QBOX_SAMPLE_FLAGS_DISPOSABLE         0x0008
#define QBOX_SAMPLE_FLAGS_MUTE               0x0010
#define QBOX_SAMPLE_FLAGS_BASE_CTS_INCREMENT 0x0020
#define QBOX_SAMPLE_FLAGS_META_INFO          0x0040
#define QBOX_SAMPLE_FLAGS_END_OF_SEQUENCE    0x0080
#define QBOX_SAMPLE_FLAGS_END_OF_STREAM      0x0100
#define QBOX_SAMPLE_FLAGS_QMED_PRESENT       0x0200
#define QBOX_SAMPLE_FLAGS_PKT_HEADER_LOSS    0x0400
#define QBOX_SAMPLE_FLAGS_PKT_LOSS           0x0800
#define QBOX_SAMPLE_FLAGS_120HZ_CLOCK        0x1000
#define QBOX_SAMPLE_FLAGS_TS                 0x2000
#define QBOX_SAMPLE_FLAGS_TS_FRAME_START     0x4000
#define QBOX_SAMPLE_FLAGS_PADDING_MASK       0xFF000000

#define QBOX_VERSION(box_flags)                                         \
    ((box_flags) >> 24)
#define QBOX_BOXFLAGS(box_flags)                                        \
    (((box_flags) << 8) >> 8)
#define QBOX_FLAGS(v, f)                                                \
    (((v) << 24) | (f))
#define QBOX_SAMPLE_PADDING(sample_flags)                               \
    (((sample_flags) & QBOX_SAMPLE_FLAGS_PADDING_MASK) >> 24)
#define QBOX_SAMPLE_FLAGS_PADDING(sample_flags, padding)                \
    ((sample_flags) | ((padding) << 24))

//qmed functions
#define GetExtPtr(pHdr)  \
    ((GetQMedBaseVersion(pHdr) == 1) ? (pHdr + sizeof(QMedStruct) + sizeof(QMedVer1InfoStruct)) : \
                                      (pHdr + sizeof(QMedStruct)))

static int gMicVolume=0;

#define AUDIO_CTRL_INTERACE		0x04

#define AC_MUTE_CONTROL		0x01
#define AC_VOLUME_CONTROL		0x02
#define EP0_TIMEOUT            1000	/* in millisecond */

/* Control request type */
#define REQ_SET         0x21
#define REQ_GET         0xa1

/* Probe requests */
#define SET_CUR		0x01
#define GET_CUR		0x81
#define GET_MIN		0x82
#define GET_MAX		0x83
#define GET_RES		0x84
#define GET_LEN		0x85
#define GET_INFO		0x86
#define GET_DEF		0x87

#define MIC_GAIN	45 /*JSON value*/
#define GAIN_FACTOR	256
#define MIC_GAIN_MIN	(0*GAIN_FACTOR)
#define MIC_GAIN_MAX	(MIC_GAIN*GAIN_FACTOR)
#define MXCAM_I2C_MAX_RETRIES 10 
#define MAX_ISPCFG_FILE_SIZE (64*1024)
#define FWPACKETSIZE 4088   /*3072  2048 4096 */
#define MAX_JSON_FILE_SIZE (200*1024)


typedef enum  {
	CTRL_NONE   = 0,
	CTRL_MUTE,
	CTRL_LEFT_MUTE,
	CTRL_RIGHT_MUTE,
	CTRL_VOLUME,
} ctrl_id_t;

typedef struct {
	ctrl_id_t id;
	int unit;
	int cs;
	int cn;
	int len;
	int16_t min;
	int16_t max;
	uint16_t res;
} AUDIO_CTRL;

static AUDIO_CTRL uac_controls[] = {
    {CTRL_MUTE,        2,  AC_MUTE_CONTROL,    0,  1,  0,  1,  1},
    {CTRL_LEFT_MUTE,   2,  AC_MUTE_CONTROL,    1,  1,  0,  1,  1},
    {CTRL_RIGHT_MUTE,  2,  AC_MUTE_CONTROL,    2,  1,  0,  1,  1},
    {CTRL_VOLUME,      2,  AC_VOLUME_CONTROL,  0,  2,  MIC_GAIN_MIN,  MIC_GAIN_MAX,  GAIN_FACTOR},
    {CTRL_NONE,        0,  0,                  0,  0,  0,  0,  0}
};

typedef enum
{
    STREAM_STATE_UNDEFINED,
    STREAM_STATE_RUN,
    STREAM_STATE_STOP
} stream_state_t;

typedef enum {
    GET_SNOR_IMG_HEADER=0x8,
    REQ_GET_EEPROM_VALUE=0xB,
    GET_EEPROM_VALUE=0xC,
    SET_EEPROM_KEY_VALUE=0xD,
    SET_OR_REMOVE_KEY_STATUS=0xF,
    SEND_JSON_FILE=0x11,
    RESET_BOARD=0x12,
    GET_IMG_HEADER=0x25,
    GET_BOOTLOADER_HEADER=0x26,
    GPIO_READ=0x41,
    GPIO_WRITE=0x42,
    GPIO_RW_STATUS=0x43,
    GET_AV_STREAMING_STATE=0x49,
    QHAL_VENDOR_REQ_START = 0x60,	
    QCC_READ = QHAL_VENDOR_REQ_START,
    WRITE_ISPCFG_FILE=0x7D,
    VENDOR_REQ_LAST = 0xFF
} VEND_CMD_LIST;


#define BE8(a) a
#ifdef RTW_PACK_STRUCT_USE_INCLUDES
#include "pack_begin.h"
#endif
RTW_PACK_STRUCT_BEGIN
typedef struct qbox_header {
    uint32_t box_size;
    uint32_t box_type;
    /* TODO test on BE and LE machines */
    struct {
        uint32_t version : 8;
        uint32_t flags   : 24;
    } box_flags;
    uint16_t stream_type;
    uint16_t stream_id;
    uint32_t sample_flags;
    uint32_t sample_cts;
    uint32_t sample_cts_low;
}/*__attribute__((__packed__)) */qbox_header_t;
RTW_PACK_STRUCT_STRUCT;
RTW_PACK_STRUCT_END
#ifdef RTW_PACK_STRUCT_USE_INCLUDES
#include "pack_end.h"
#endif

static int set_ctrl(ctrl_id_t id, uint8_t bRequest, void *val);
static AUDIO_CTRL* get_ctrl_by_id(ctrl_id_t id);
static int mxcam_get_av_stream_status(void);
static const char * get_status(const int status);
static void cJSON_Minify(char *json);


uint32_t GetQMedBaseVersion(unsigned char *pQMed)
{
    QMedStruct *pQMedBase = (QMedStruct *) pQMed;

    return BE8(pQMedBase->boxFlags.field.v);
}

/* QBOX header parser
 * buf: data buffer passed by application
 * channel_id: channel id
 * fmt: data format of the channel
 * data_buf: data starts here
 * size: size of data
 * ts: timestamp; only 32 bit timestamps are passed to application
 * Returns 0 if QBOX header was parsed successfully
 * Returns 1 if the frame is not a valid qbox frame
 */
int qbox_parse_header(uint8_t *buf, int *channel_id, video_format_t *fmt,
                      uint8_t **data_buf, uint32_t *size,
                      uint64_t *ts, /*uint32_t *analytics,*/
                      metadata_t *metadata)
{
    int ret = 0;
    unsigned char *p = buf;
    qbox_header_t *qbox = (qbox_header_t*)buf;
    int hdr_len;
    int qmed_len=0;
    uint32_t flags;

    if (be32_to_cpu(qbox->box_type) != QBOX_TYPE) {
        ret = -EINVAL;
        return ret;
    }

    hdr_len = sizeof(qbox_header_t);
#if 1
    if (qbox->box_flags.version == 1) {
        *ts = ((uint64_t)be32_to_cpu(qbox->sample_cts) << 32) | be32_to_cpu(qbox->sample_cts_low);
        //*ts = be32_to_cpu(qbox->sample_cts);
    } else {
        *ts = be32_to_cpu(qbox->sample_cts);
        hdr_len -= sizeof(uint32_t);
    }   
#endif
    //map qbox header type to video_format_t type before passing to user
    switch (be16_to_cpu(qbox->stream_type)) {
    case QBOX_SAMPLE_TYPE_AAC:
        //printf("AAC\n");
        *fmt = VID_FORMAT_AAC_RAW;
		if (be32_to_cpu(qbox->sample_flags) & QBOX_SAMPLE_FLAGS_QMED_PRESENT) {
            QMedStruct *pQMed;
            int qmedVersion;
            // skip the qbox header
            p += hdr_len;
            qmedVersion = GetQMedBaseVersion(p);
            pQMed = (QMedStruct *) p;
            qmed_len = be32_to_cpu(pQMed->boxSize);
            p = (unsigned char *)(GetExtPtr(p) + sizeof(QMedH264Struct));
            metadata->qmedExt = (char *) p;
            metadata->total_len = qmed_len
                - sizeof(QMedStruct)
                - (qmedVersion == 1 ? sizeof(QMedVer1InfoStruct) : 0)
                - sizeof(QMedH264Struct);
        }
		//uint8_t *temp_data = ((uint8_t*)buf + hdr_len + qmed_len);
		//printf("\r\nA:%x %x %x %x %x %x %x %x %x %x", temp_data[0], temp_data[1], temp_data[2], temp_data[3], temp_data[4], temp_data[5], temp_data[6], temp_data[7], temp_data[8], temp_data[9]);
		hdr_len-=7;//AAC: preserve storage space for ADTS header. Because the space before AAC raw data is not used anymore, it could be borrowed for ADTS header. 
		
        break;
    case QBOX_SAMPLE_TYPE_QOPUS:
        *fmt = VID_FORMAT_H264_AAC_TS;
        break;
    case QBOX_SAMPLE_TYPE_H264:
        flags = be32_to_cpu(qbox->sample_flags);
        if (0 != (flags & QBOX_SAMPLE_FLAGS_TS)) {
            *fmt = VID_FORMAT_H264_TS;
        } else {
            *fmt = VID_FORMAT_H264_RAW;
#if 1
            if (be32_to_cpu(qbox->sample_flags) & QBOX_SAMPLE_FLAGS_QMED_PRESENT) {
                QMedStruct *pQMed;
                int qmedVersion;
                // skip the qbox header
                p += hdr_len;
                qmedVersion = GetQMedBaseVersion(p);
                pQMed = (QMedStruct *) p;
                qmed_len = be32_to_cpu(pQMed->boxSize);
                p = GetExtPtr(p) + sizeof(QMedH264Struct);
                metadata->qmedExt = (char *) p;
                metadata->total_len = qmed_len
                    - sizeof(QMedStruct)
                    - (qmedVersion == 1 ? sizeof(QMedVer1InfoStruct) : 0)
                    - sizeof(QMedH264Struct);
            }
#endif
        }
        break;        
    case QBOX_SAMPLE_TYPE_JPEG:
        *fmt = VID_FORMAT_MJPEG_RAW;
#if 0
        if (be32_to_cpu(qbox->sample_flags) & QBOX_SAMPLE_FLAGS_QMED_PRESENT) {
            QMedStruct *pQMed;
            int qmedVersion;
            // skip the qbox header
            p += hdr_len;
            qmedVersion = GetQMedBaseVersion(p);
            pQMed = (QMedStruct *) p;
            qmed_len = be32_to_cpu(pQMed->boxSize);
            p = GetExtPtr(p) + sizeof(QMedJPEGStruct);
            metadata->qmedExt = (char *) p;
            metadata->total_len = qmed_len
                - sizeof(QMedStruct)
                - (qmedVersion == 1 ? sizeof(QMedVer1InfoStruct) : 0)
                - sizeof(QMedJPEGStruct);
        }
#endif
        break;
#if 0        
    case QBOX_SAMPLE_TYPE_LUMA:
        *fmt = VID_FORMAT_GREY_RAW;
        *analytics = 0;
        break;
    case QBOX_SAMPLE_TYPE_MB_STAT_LUMA:
        *fmt = VID_FORMAT_GREY_RAW;
        *analytics = 1;
        break;
    case QBOX_SAMPLE_TYPE_PME_STAT_LUMA:
        *fmt = VID_FORMAT_GREY_RAW;
        *analytics = 2;
        break;
    case QBOX_SAMPLE_TYPE_MB_PME_STAT_LUMA:
        *fmt = VID_FORMAT_GREY_RAW;
        *analytics = 3;
        break;
    case QBOX_SAMPLE_TYPE_NV12:
        *fmt = VID_FORMAT_NV12_RAW;
        *analytics = 0;
        break;
    case QBOX_SAMPLE_TYPE_MB_STAT_NV12:
        *fmt = VID_FORMAT_NV12_RAW;
        *analytics = 1;
        break;
    case QBOX_SAMPLE_TYPE_PME_STAT_NV12:
        *fmt = VID_FORMAT_NV12_RAW;
        *analytics = 2;
        break;
    case QBOX_SAMPLE_TYPE_MB_PME_STAT_NV12:
        *fmt = VID_FORMAT_NV12_RAW;
        *analytics = 3;
        break;
    case QBOX_SAMPLE_TYPE_REC_STAT_LUMA:
        *fmt = VID_FORMAT_GREY_RAW;
        *analytics = 4;
        break;
    case QBOX_SAMPLE_TYPE_REC_STAT_NV12:
        *fmt = VID_FORMAT_NV12_RAW;
        *analytics = 4;
        break;
#endif
    default:
        V4L2_ERROR("unsupport Qbox format\n");
        ret = -EINVAL;
        break;
    }
    *channel_id = be16_to_cpu(qbox->stream_id);
    //*fmt = be16_to_cpu(qbox->stream_type);
    *data_buf = ((uint8_t*)buf + hdr_len + qmed_len);
    //*data_buf = ((uint8_t*)buf + hdr_len);
    *size = be32_to_cpu(qbox->box_size) - hdr_len - qmed_len;
    //*size = be32_to_cpu(qbox->box_size) - hdr_len;
    return ret;
}

/* -----------------------------end of Qbox.c---------------------------------*/

mux_stream_t mux_stream;

static int is_mux_stream(struct video_stream *vstream)
{
    if (vstream->fd == mux_stream.fd)
        return 1;

    return 0;
}

static int vidformat2fourcc(video_format_t fmt, uint32_t *fourcc)
{
    switch (fmt) {
    case VID_FORMAT_H264_RAW:
        *fourcc = v4l2_fourcc('H','2','6','4');
        return 0;
    case VID_FORMAT_H264_TS:
        *fourcc = V4L2_PIX_FMT_MPEG;
        return 0;
    case VID_FORMAT_H264_AAC_TS:
        *fourcc = V4L2_PIX_FMT_MPEG;
        return 0;
    case VID_FORMAT_MJPEG_RAW:
        *fourcc = v4l2_fourcc('M','J','P','G');
        return 0;
    case VID_FORMAT_YUY2_RAW:
        *fourcc = v4l2_fourcc('Y','U','Y','V');
        return 0;
    case VID_FORMAT_NV12_RAW:
        *fourcc = v4l2_fourcc('N','V','1','2');
        return 0;
    case VID_FORMAT_GREY_RAW:
        *fourcc = V4L2_PIX_FMT_GREY;
        return 0;
    case VID_FORMAT_MUX:
        *fourcc = v4l2_fourcc('M','U','X',' ');
        return 0;
    case VID_FORMAT_AAC_RAW:
	*fourcc = v4l2_fourcc('A','A','C',' ');
	return 0;
    default:
        V4L2_ERROR("\n\rUnknown video format %d.", fmt);
        return 0;
    }
}

static int fourcc2vidformat(uint32_t fourcc, video_format_t *fmt)
{
    int f;
    uint32_t fcc;
    for (f=FIRST_VID_FORMAT; f<NUM_VID_FORMAT; f++) {
        vidformat2fourcc(f, &fcc);
        if (fcc == fourcc) {
            *fmt = f;
            return 0;
        }
    }
    V4L2_ERROR("\n\runknown fourcc 0x%x", fourcc);
    return 0;
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------

#include "section_config.h"
SDRAM_DATA_SECTION
static struct uvc_xu_data mux_xu_data[] = {
    /*{GUID_VIDCAP_EXT, OLD_XU_BITRATE,        32,  0, V4L2_CID_XU_BITRATE,        "Bitrate"},
    {GUID_VIDCAP_EXT, OLD_XU_AVC_PROFILE,    32,  0, V4L2_CID_XU_AVC_PROFILE,    "Profile"},
    {GUID_VIDCAP_EXT, OLD_XU_AVC_LEVEL,      32,  0, V4L2_CID_XU_AVC_LEVEL,      "Level"},
    {GUID_VIDCAP_EXT, OLD_XU_PICTURE_CODING, 32,  0, V4L2_CID_XU_PICTURE_CODING, "Picture Coding"},
    {GUID_VIDCAP_EXT, OLD_XU_GOP_STRUCTURE,  32,  0, V4L2_CID_XU_GOP_STRUCTURE,  "GOP Structure"},
    {GUID_VIDCAP_EXT, OLD_XU_GOP_LENGTH,     32,  0, V4L2_CID_XU_GOP_LENGTH,     "GOP Length"},
    {GUID_VIDCAP_EXT, OLD_XU_RESOLUTION,     32,  0, V4L2_CID_XU_RESOLUTION,     "Resolution"},
    {GUID_VIDCAP_EXT, OLD_XU_FORCE_I_FRAME,  32,  0, V4L2_CID_XU_FORCE_I_FRAME,  "Force I Frame"},
    {GUID_VIDCAP_EXT, OLD_XU_GET_VERSION,    32,  0, V4L2_CID_XU_GET_VERSION,    "Version"},

    {GUID_VIDENC_EXT, OLD_XU_BITRATE,        32,  0, V4L2_CID_XU_BITRATE,        "Bitrate"},
    {GUID_VIDENC_EXT, OLD_XU_AVC_PROFILE,    32,  0, V4L2_CID_XU_AVC_PROFILE,    "Profile"},
    {GUID_VIDENC_EXT, OLD_XU_AVC_LEVEL,      32,  0, V4L2_CID_XU_AVC_LEVEL,      "Level"},
    {GUID_VIDENC_EXT, OLD_XU_PICTURE_CODING, 32,  0, V4L2_CID_XU_PICTURE_CODING, "Picture Coding"},
    {GUID_VIDENC_EXT, OLD_XU_GOP_STRUCTURE,  32,  0, V4L2_CID_XU_GOP_STRUCTURE,  "GOP Structure"},
    {GUID_VIDENC_EXT, OLD_XU_GOP_LENGTH,     32,  0, V4L2_CID_XU_GOP_LENGTH,     "GOP Length"},
    {GUID_VIDENC_EXT, OLD_XU_RESOLUTION,     32,  0, V4L2_CID_XU_RESOLUTION,     "Resolution"},
    {GUID_VIDENC_EXT, OLD_XU_FORCE_I_FRAME,  32,  0, V4L2_CID_XU_FORCE_I_FRAME,  "Force I Frame"},
    {GUID_VIDENC_EXT, OLD_XU_GET_VERSION,    32,  0, V4L2_CID_XU_GET_VERSION,    "Version"},*/

    {MUX1_XU_GUID, MUX_XU_RESOLUTION,        32,  0,  V4L2_CID_MUX_CH1_XU_RESOLUTION,      "Resolution"},
    {MUX1_XU_GUID, MUX_XU_FRAMEINTRVL,       32,  0,  V4L2_CID_MUX_CH1_XU_FRAMEINTRVL,     "frame interval"},
    {MUX1_XU_GUID, MUX_XU_COMPRESSION_Q,     32,  0,  V4L2_CID_MUX_CH1_XU_COMPRESSION_Q,   "compression quality"},
    {MUX1_XU_GUID, MUX_XU_GOP_HIERARCHY_LEVEL,32, 0,  V4L2_CID_MUX_CH1_XU_GOP_HIERARCHY_LEVEL, "Gop hierarchy level"},
    {MUX1_XU_GUID, MUX_XU_ZOOM,              32,  0,  V4L2_CID_MUX_CH1_XU_ZOOM,        "Zoom"},
    {MUX1_XU_GUID, MUX_XU_BITRATE,           32,  0,  V4L2_CID_MUX_CH1_XU_BITRATE,         "Bitrate"},
    {MUX1_XU_GUID, MUX_XU_FORCE_I_FRAME,     32,  0,  V4L2_CID_MUX_CH1_XU_FORCE_I_FRAME,   "Force I Frame"},
    {MUX1_XU_GUID, MUX_XU_VUI_ENABLE,        32,  0,  V4L2_CID_MUX_CH1_XU_VUI_ENABLE,       "VUI Enable"},
    {MUX1_XU_GUID, MUX_XU_CHCOUNT,       32,  0,  V4L2_CID_MUX_CH1_XU_CHCOUNT,         "mux channel count"},
    {MUX1_XU_GUID, MUX_XU_CHTYPE,        32,  0,  V4L2_CID_MUX_CH1_XU_CHTYPE,          "channel type"},
    {MUX1_XU_GUID, MUX_XU_GOP_LENGTH,        32,  0,  V4L2_CID_MUX_CH1_XU_GOP_LENGTH,      "GOP Length"},
    {MUX1_XU_GUID, MUX_XU_AVC_PROFILE,       32,  0,  V4L2_CID_MUX_CH1_XU_AVC_PROFILE,     "AVC Profile"},
    {MUX1_XU_GUID, MUX_XU_AVC_MAX_FRAME_SIZE,32,  0,  V4L2_CID_MUX_CH1_XU_AVC_MAX_FRAME_SIZE,"AVC Max Frame Size"},
    {MUX1_XU_GUID, MUX_XU_AVC_LEVEL,         32,  0,  V4L2_CID_MUX_CH1_XU_AVC_LEVEL,       "AVC Level"},
    {MUX1_XU_GUID, MUX_XU_PIC_TIMING_ENABLE, 32,  0,  V4L2_CID_MUX_CH1_XU_PIC_TIMING_ENABLE,"pic timing enable"},
    {MUX1_XU_GUID, MUX_XU_VFLIP,         32,  0,  V4L2_CID_MUX_CH1_XU_VFLIP,       "vflip"},
    {MUX1_XU_GUID, MUX_XU_AUDIO_BITRATE,     32,  0,  V4L2_CID_MUX_CH1_XU_AUDIO_BITRATE,   "audio bitrate"},

    {MUX1_XU_GUID, MUX_XU_START_CHANNEL,     32,  0,  V4L2_CID_MUX_XU_START_CHANNEL,       "Channel start"},
    {MUX1_XU_GUID, MUX_XU_STOP_CHANNEL,      32,  0,  V4L2_CID_MUX_XU_STOP_CHANNEL,        "Channel stop"},

    {MUX1_XU_GUID, MUX_XU_DEWRP_CHANNEL,     32,  0,  V4L2_CID_MUX_CH1_XU_DEWRP_CHANNEL,    "Dewarp channel"},

    {MUX2_XU_GUID, MUX_XU_RESOLUTION,        32,  0,  V4L2_CID_MUX_CH2_XU_RESOLUTION,      "Resolution"},
    {MUX2_XU_GUID, MUX_XU_FRAMEINTRVL,       32,  0,  V4L2_CID_MUX_CH2_XU_FRAMEINTRVL,     "frame interval"},
    {MUX2_XU_GUID, MUX_XU_COMPRESSION_Q,     32,  0,  V4L2_CID_MUX_CH2_XU_COMPRESSION_Q,   "compression quality"},
    {MUX2_XU_GUID, MUX_XU_PIC_TIMING_ENABLE, 32,  0,  V4L2_CID_MUX_CH2_XU_PIC_TIMING_ENABLE,"pic timing enable"},
    {MUX2_XU_GUID, MUX_XU_GOP_HIERARCHY_LEVEL,32, 0,  V4L2_CID_MUX_CH2_XU_GOP_HIERARCHY_LEVEL, "Gop hierarchy level"},
    {MUX2_XU_GUID, MUX_XU_BITRATE,           32,  0,  V4L2_CID_MUX_CH2_XU_BITRATE,         "Bitrate"},
    {MUX2_XU_GUID, MUX_XU_FORCE_I_FRAME,     32,  0,  V4L2_CID_MUX_CH2_XU_FORCE_I_FRAME,   "Force I Frame"},
    {MUX2_XU_GUID, MUX_XU_VUI_ENABLE,        32,  0,  V4L2_CID_MUX_CH2_XU_VUI_ENABLE,       "vui enable"},
    {MUX2_XU_GUID, MUX_XU_CHTYPE,        32,  0,  V4L2_CID_MUX_CH2_XU_CHTYPE,          "channel type"},
    {MUX2_XU_GUID, MUX_XU_GOP_LENGTH,        32,  0,  V4L2_CID_MUX_CH2_XU_GOP_LENGTH,      "GOP Length"},
    {MUX2_XU_GUID, MUX_XU_AVC_PROFILE,       32,  0,  V4L2_CID_MUX_CH2_XU_AVC_PROFILE,     "AVC Profile"},
    {MUX2_XU_GUID, MUX_XU_AVC_MAX_FRAME_SIZE,32,  0,  V4L2_CID_MUX_CH2_XU_AVC_MAX_FRAME_SIZE,"AVC Max Frame Size"},
    {MUX2_XU_GUID, MUX_XU_AVC_LEVEL,         32,  0,  V4L2_CID_MUX_CH2_XU_AVC_LEVEL,       "AVC Level"},

    {MUX2_XU_GUID, MUX_XU_DEWRP_CHANNEL,     32,  0,  V4L2_CID_MUX_CH2_XU_DEWRP_CHANNEL,    "Dewarp channel"},
    {MUX2_XU_GUID, MUX_XU_VBR_MIN_BITRATE,   32,  0,  V4L2_CID_MUX_CH2_XU_VBR_MIN_BITRATE,     "vbr min bitrate"},

    {MUX3_XU_GUID, MUX_XU_RESOLUTION,        32,  0,  V4L2_CID_MUX_CH3_XU_RESOLUTION,      "Resolution"},
    {MUX3_XU_GUID, MUX_XU_FRAMEINTRVL,       32,  0,  V4L2_CID_MUX_CH3_XU_FRAMEINTRVL,     "frame interval"},
    {MUX3_XU_GUID, MUX_XU_COMPRESSION_Q,     32,  0,  V4L2_CID_MUX_CH3_XU_COMPRESSION_Q,   "compression quality"},
    {MUX3_XU_GUID, MUX_XU_PIC_TIMING_ENABLE, 32,  0,  V4L2_CID_MUX_CH3_XU_PIC_TIMING_ENABLE,"pic timing enable"},
    {MUX3_XU_GUID, MUX_XU_GOP_HIERARCHY_LEVEL,32, 0,  V4L2_CID_MUX_CH3_XU_GOP_HIERARCHY_LEVEL, "Gop hierarchy level"},
    {MUX3_XU_GUID, MUX_XU_BITRATE,           32,  0,  V4L2_CID_MUX_CH3_XU_BITRATE,         "Bitrate"},
    {MUX3_XU_GUID, MUX_XU_FORCE_I_FRAME,     32,  0,  V4L2_CID_MUX_CH3_XU_FORCE_I_FRAME,   "Force I Frame"},
    {MUX3_XU_GUID, MUX_XU_VUI_ENABLE,        32,  0,  V4L2_CID_MUX_CH3_XU_VUI_ENABLE,        "vui enable"},
    {MUX3_XU_GUID, MUX_XU_CHTYPE,        32,  0,  V4L2_CID_MUX_CH3_XU_CHTYPE,          "channel type"},
    {MUX3_XU_GUID, MUX_XU_GOP_LENGTH,        32,  0,  V4L2_CID_MUX_CH3_XU_GOP_LENGTH,      "GOP Length"},
    {MUX3_XU_GUID, MUX_XU_AVC_PROFILE,       32,  0,  V4L2_CID_MUX_CH3_XU_AVC_PROFILE,     "AVC Profile"},
    {MUX3_XU_GUID, MUX_XU_AVC_MAX_FRAME_SIZE,32,  0,  V4L2_CID_MUX_CH3_XU_AVC_MAX_FRAME_SIZE,"AVC Max Frame Size"},
    {MUX3_XU_GUID, MUX_XU_AVC_LEVEL,         32,  0,  V4L2_CID_MUX_CH3_XU_AVC_LEVEL,       "AVC Level"},

    {MUX3_XU_GUID, MUX_XU_DEWRP_CHANNEL,     32,  0,  V4L2_CID_MUX_CH3_XU_DEWRP_CHANNEL,    "Dewarp channel"},
    {MUX3_XU_GUID, MUX_XU_VBR_MIN_BITRATE,   32,  0,  V4L2_CID_MUX_CH3_XU_VBR_MIN_BITRATE,     "vbr min bitrate"},

    {MUX4_XU_GUID, MUX_XU_RESOLUTION,        32,  0,  V4L2_CID_MUX_CH4_XU_RESOLUTION,      "Resolution"},
    {MUX4_XU_GUID, MUX_XU_FRAMEINTRVL,       32,  0,  V4L2_CID_MUX_CH4_XU_FRAMEINTRVL,     "frame interval"},
    {MUX4_XU_GUID, MUX_XU_COMPRESSION_Q,     32,  0,  V4L2_CID_MUX_CH4_XU_COMPRESSION_Q,   "compression quality"},
    {MUX4_XU_GUID, MUX_XU_PIC_TIMING_ENABLE, 32,  0,  V4L2_CID_MUX_CH4_XU_PIC_TIMING_ENABLE,"pic timing enable"},
    {MUX4_XU_GUID, MUX_XU_GOP_HIERARCHY_LEVEL,32, 0,  V4L2_CID_MUX_CH4_XU_GOP_HIERARCHY_LEVEL, "Gop hierarchy level"},
    {MUX4_XU_GUID, MUX_XU_BITRATE,           32,  0,  V4L2_CID_MUX_CH4_XU_BITRATE,         "Bitrate"},
    {MUX4_XU_GUID, MUX_XU_FORCE_I_FRAME,     32,  0,  V4L2_CID_MUX_CH4_XU_FORCE_I_FRAME,   "Force I Frame"},
    {MUX4_XU_GUID, MUX_XU_VUI_ENABLE,        32,  0,  V4L2_CID_MUX_CH4_XU_VUI_ENABLE,       "vui enable"},
    {MUX4_XU_GUID, MUX_XU_CHTYPE,        32,  0,  V4L2_CID_MUX_CH4_XU_CHTYPE,          "channel type"},
    {MUX4_XU_GUID, MUX_XU_GOP_LENGTH,        32,  0,  V4L2_CID_MUX_CH4_XU_GOP_LENGTH,      "GOP Length"},
    {MUX4_XU_GUID, MUX_XU_AVC_PROFILE,       32,  0,  V4L2_CID_MUX_CH4_XU_AVC_PROFILE,     "AVC Profile"},
    {MUX4_XU_GUID, MUX_XU_AVC_MAX_FRAME_SIZE,32,  0,  V4L2_CID_MUX_CH4_XU_AVC_MAX_FRAME_SIZE,"AVC Max Frame Size"},
    {MUX4_XU_GUID, MUX_XU_AVC_LEVEL,         32,  0,  V4L2_CID_MUX_CH4_XU_AVC_LEVEL,       "AVC Level"},

    {MUX4_XU_GUID, MUX_XU_DEWRP_CHANNEL,     32,  0,  V4L2_CID_MUX_CH4_XU_DEWRP_CHANNEL,    "Dewarp channel"},
    {MUX4_XU_GUID, MUX_XU_VBR_MIN_BITRATE,   32,  0,  V4L2_CID_MUX_CH4_XU_VBR_MIN_BITRATE,     "vbr min bitrate"},
#if 0
    {MUX5_XU_GUID, MUX_XU_RESOLUTION,        32,  0,  V4L2_CID_MUX_CH5_XU_RESOLUTION,      "Resolution"},
    {MUX5_XU_GUID, MUX_XU_FRAMEINTRVL,       32,  0,  V4L2_CID_MUX_CH5_XU_FRAMEINTRVL,     "frame interval"},
    {MUX5_XU_GUID, MUX_XU_COMPRESSION_Q,     32,  0,  V4L2_CID_MUX_CH5_XU_COMPRESSION_Q,   "compression quality"},
    {MUX5_XU_GUID, MUX_XU_PIC_TIMING_ENABLE, 32,  0,  V4L2_CID_MUX_CH5_XU_PIC_TIMING_ENABLE,"pic timing enable"},
    {MUX5_XU_GUID, MUX_XU_GOP_HIERARCHY_LEVEL,32, 0,  V4L2_CID_MUX_CH5_XU_GOP_HIERARCHY_LEVEL, "Gop hierarchy level"},
    {MUX5_XU_GUID, MUX_XU_BITRATE,           32,  0,  V4L2_CID_MUX_CH5_XU_BITRATE,         "Bitrate"},
    {MUX5_XU_GUID, MUX_XU_FORCE_I_FRAME,     32,  0,  V4L2_CID_MUX_CH5_XU_FORCE_I_FRAME,   "Force I Frame"},
    {MUX5_XU_GUID, MUX_XU_VUI_ENABLE,        32,  0,  V4L2_CID_MUX_CH5_XU_VUI_ENABLE,       "vui enable"},
    {MUX5_XU_GUID, MUX_XU_CHTYPE,        32,  0,  V4L2_CID_MUX_CH5_XU_CHTYPE,          "channel type"},
    {MUX5_XU_GUID, MUX_XU_GOP_LENGTH,        32,  0,  V4L2_CID_MUX_CH5_XU_GOP_LENGTH,      "GOP Length"},
    {MUX5_XU_GUID, MUX_XU_AVC_PROFILE,       32,  0,  V4L2_CID_MUX_CH5_XU_AVC_PROFILE,     "AVC Profile"},
    {MUX5_XU_GUID, MUX_XU_AVC_MAX_FRAME_SIZE,32,  0,  V4L2_CID_MUX_CH5_XU_AVC_MAX_FRAME_SIZE,"AVC Max Frame Size"},
    {MUX5_XU_GUID, MUX_XU_AVC_LEVEL,         32,  0,  V4L2_CID_MUX_CH5_XU_AVC_LEVEL,       "AVC Level"},

    {MUX5_XU_GUID, MUX_XU_DEWRP_CHANNEL,     32,  0,  V4L2_CID_MUX_CH5_XU_DEWRP_CHANNEL,    "Dewarp channel"},
    {MUX6_XU_GUID, MUX_XU_VBR_MIN_BITRATE,   32,  0,  V4L2_CID_MUX_CH5_XU_VBR_MIN_BITRATE,     "vbr min bitrate"},

    {MUX6_XU_GUID, MUX_XU_RESOLUTION,        32,  0,  V4L2_CID_MUX_CH6_XU_RESOLUTION,      "Resolution"},
    {MUX6_XU_GUID, MUX_XU_FRAMEINTRVL,       32,  0,  V4L2_CID_MUX_CH6_XU_FRAMEINTRVL,     "frame interval"},
    {MUX6_XU_GUID, MUX_XU_COMPRESSION_Q,     32,  0,  V4L2_CID_MUX_CH6_XU_COMPRESSION_Q,   "compression quality"},
    {MUX6_XU_GUID, MUX_XU_PIC_TIMING_ENABLE, 32,  0,  V4L2_CID_MUX_CH6_XU_PIC_TIMING_ENABLE,"pic timing enable"},
    {MUX6_XU_GUID, MUX_XU_GOP_HIERARCHY_LEVEL,32, 0,  V4L2_CID_MUX_CH6_XU_GOP_HIERARCHY_LEVEL, "Gop hierarchy level"},
    {MUX6_XU_GUID, MUX_XU_BITRATE,           32,  0,  V4L2_CID_MUX_CH6_XU_BITRATE,         "Bitrate"},
    {MUX6_XU_GUID, MUX_XU_FORCE_I_FRAME,     32,  0,  V4L2_CID_MUX_CH6_XU_FORCE_I_FRAME,   "Force I Frame"},
    {MUX6_XU_GUID, MUX_XU_VUI_ENABLE,       32,  0,  V4L2_CID_MUX_CH6_XU_VUI_ENABLE,       "vui enable"},
    {MUX6_XU_GUID, MUX_XU_CHTYPE,        32,  0,  V4L2_CID_MUX_CH6_XU_CHTYPE,          "channel type"},
    {MUX6_XU_GUID, MUX_XU_GOP_LENGTH,        32,  0,  V4L2_CID_MUX_CH6_XU_GOP_LENGTH,      "GOP Length"},
    {MUX6_XU_GUID, MUX_XU_AVC_LEVEL,         32,  0,  V4L2_CID_MUX_CH6_XU_AVC_LEVEL,       "AVC Level"},

    {MUX6_XU_GUID, MUX_XU_DEWRP_CHANNEL,     32,  0,  V4L2_CID_MUX_CH6_XU_DEWRP_CHANNEL,    "Dewarp channel"},
    {MUX6_XU_GUID, MUX_XU_VBR_MIN_BITRATE,   32,  0,  V4L2_CID_MUX_CH6_XU_VBR_MIN_BITRATE,     "vbr min bitrate"},

    {MUX7_XU_GUID, MUX_XU_RESOLUTION,        32,  0,  V4L2_CID_MUX_CH7_XU_RESOLUTION,      "Resolution"},
    {MUX7_XU_GUID, MUX_XU_FRAMEINTRVL,       32,  0,  V4L2_CID_MUX_CH7_XU_FRAMEINTRVL,     "frame interval"},
    {MUX7_XU_GUID, MUX_XU_COMPRESSION_Q,     32,  0,  V4L2_CID_MUX_CH7_XU_COMPRESSION_Q,   "compression quality"},
    {MUX7_XU_GUID, MUX_XU_PIC_TIMING_ENABLE, 32,  0,  V4L2_CID_MUX_CH7_XU_PIC_TIMING_ENABLE,"pic timing enable"},
    {MUX7_XU_GUID, MUX_XU_GOP_HIERARCHY_LEVEL,32, 0,  V4L2_CID_MUX_CH7_XU_GOP_HIERARCHY_LEVEL, "Gop hierarchy level"},
    {MUX7_XU_GUID, MUX_XU_BITRATE,           32,  0,  V4L2_CID_MUX_CH7_XU_BITRATE,         "Bitrate"},
    {MUX7_XU_GUID, MUX_XU_FORCE_I_FRAME,     32,  0,  V4L2_CID_MUX_CH7_XU_FORCE_I_FRAME,   "Force I Frame"},
    {MUX7_XU_GUID, MUX_XU_VUI_ENABLE,        32,  0,  V4L2_CID_MUX_CH7_XU_VUI_ENABLE,       "vui enable"},
    {MUX7_XU_GUID, MUX_XU_CHTYPE,        32,  0,  V4L2_CID_MUX_CH7_XU_CHTYPE,          "channel type"},
    {MUX7_XU_GUID, MUX_XU_GOP_LENGTH,        32,  0,  V4L2_CID_MUX_CH7_XU_GOP_LENGTH,      "GOP Length"},
    {MUX7_XU_GUID, MUX_XU_AVC_LEVEL,         32,  0,  V4L2_CID_MUX_CH7_XU_AVC_LEVEL,       "AVC Level"},

    {MUX7_XU_GUID, MUX_XU_DEWRP_CHANNEL,     32,  0,  V4L2_CID_MUX_CH7_XU_DEWRP_CHANNEL,    "Dewarp channel"},
    {MUX7_XU_GUID, MUX_XU_VBR_MIN_BITRATE,   32,  0,  V4L2_CID_MUX_CH7_XU_VBR_MIN_BITRATE,     "vbr min bitrate"},

    {PU_XU_GUID, PU_XU_ANF_ENABLE,      32,  0,  V4L2_CID_PU_XU_ANF_ENABLE,     "ANF Enable"},
    {PU_XU_GUID, PU_XU_NF_STRENGTH,     32,  0,  V4L2_CID_PU_XU_NF_STRENGTH,    "NF Strength"},

    {PU_XU_GUID, PU_XU_ADAPTIVE_WDR_ENABLE, 32,  0,  V4L2_CID_PU_XU_ADAPTIVE_WDR_ENABLE,"Adaptive WDR Enable"},
    {PU_XU_GUID, PU_XU_WDR_STRENGTH,        32,  0,  V4L2_CID_PU_XU_WDR_STRENGTH,       "WDR Strength"},
    {PU_XU_GUID, PU_XU_AE_ENABLE,           32,  0,  V4L2_CID_PU_XU_AUTO_EXPOSURE,      "Auto Exposure"},
    {PU_XU_GUID, PU_XU_EXPOSURE_TIME,       32,  0,  V4L2_CID_PU_XU_EXPOSURE_TIME,      "Exposure Time"},
    {PU_XU_GUID, PU_XU_AWB_ENABLE,          32,  0,  V4L2_CID_PU_XU_AUTO_WHITE_BAL,     "Auto White Balance"},
    {PU_XU_GUID, PU_XU_WB_TEMPERATURE,      32,  0,  V4L2_CID_PU_XU_WHITE_BAL_TEMP,     "White Balance Temperature"},
    {PU_XU_GUID, PU_XU_VFLIP,               32,  0,  V4L2_CID_PU_XU_VFLIP,              "Vertical Flip"},
    {PU_XU_GUID, PU_XU_HFLIP,               32,  0,  V4L2_CID_PU_XU_HFLIP,              "Horizontal Flip"},
    {PU_XU_GUID, PU_XU_WB_ZONE_SEL_ENABLE,  32,  0,  V4L2_CID_PU_XU_WB_ZONE_SEL_ENABLE, "White Balance Zone Select"},
    {PU_XU_GUID, PU_XU_WB_ZONE_SEL,         32,  0,  V4L2_CID_PU_XU_WB_ZONE_SEL,        "White Balance Zone"},
    {PU_XU_GUID, PU_XU_EXP_ZONE_SEL_ENABLE, 32,  0,  V4L2_CID_PU_XU_EXP_ZONE_SEL_ENABLE,"Exposure Zone Select"},
    {PU_XU_GUID, PU_XU_EXP_ZONE_SEL,        32,  0,  V4L2_CID_PU_XU_EXP_ZONE_SEL,       "Exposure Zone"},
    {PU_XU_GUID, PU_XU_MAX_ANALOG_GAIN,     32,  0,  V4L2_CID_PU_XU_MAX_ANALOG_GAIN,    "Max Analog Gain"},
    {PU_XU_GUID, PU_XU_HISTO_EQ,            32,  0,  V4L2_CID_PU_XU_HISTO_EQ,           "Histogram Equalization"},
    {PU_XU_GUID, PU_XU_SHARPEN_FILTER,      32,  0,  V4L2_CID_PU_XU_SHARPEN_FILTER,     "Sharpen Filter Level"},
    {PU_XU_GUID, PU_XU_GAIN_MULTIPLIER,     32,  0,  V4L2_CID_PU_XU_GAIN_MULTIPLIER,    "Exposure Gain Multiplier"},
    {PU_XU_GUID, PU_XU_TF_STRENGTH,     32,  0,  V4L2_CID_PU_XU_TF_STRENGTH,        "Temporal filter strength"},
    /* Sinter Config */
    {PU_XU_GUID, PU_XU_SINTER,              16,   0, V4L2_CID_PU_XU_SINTER_MODE       , "Sinter Mode"},
    {PU_XU_GUID, PU_XU_SINTER,              16,  16, V4L2_CID_PU_XU_SINTER_MIN_NRSTR  , "Sinter Min strength"},
    {PU_XU_GUID, PU_XU_SINTER,              16,  32, V4L2_CID_PU_XU_SINTER_MAX_NRSTR  , "Sinter Max strength"},
    {PU_XU_GUID, PU_XU_SINTER,              16,  48, V4L2_CID_PU_XU_SINTER_MIN_THR    , "Sinter Min Thresh"},
    {PU_XU_GUID, PU_XU_SINTER,              16,  64, V4L2_CID_PU_XU_SINTER_MAX_THR    , "Sinter Max Thresh"},
    {PU_XU_GUID, PU_XU_SINTER,              16,  80, V4L2_CID_PU_XU_SINTER_TRIGPT     , "Sinter TriggerPt"},

    /* Crop mode */
    {PU_XU_GUID, PU_XU_CROP,                16,   0, V4L2_CID_PU_XU_CROP_CHANNEL   , "Crop Channel"},
    {PU_XU_GUID, PU_XU_CROP,                16,  16, V4L2_CID_PU_XU_CROP_ENABLE    , "Crop Enable"},
    {PU_XU_GUID, PU_XU_CROP,                16,  32, V4L2_CID_PU_XU_CROP_WIDTH     , "Crop Width"},
    {PU_XU_GUID, PU_XU_CROP,                16,  48, V4L2_CID_PU_XU_CROP_HEIGHT    , "Crop Height"},
    {PU_XU_GUID, PU_XU_CROP,                16,  64, V4L2_CID_PU_XU_CROP_X         , "Crop X"},
    {PU_XU_GUID, PU_XU_CROP,                16,  80, V4L2_CID_PU_XU_CROP_Y         , "Crop Y"},

    {PU_XU_GUID, PU_XU_DEWARP_PARAMS1,       32,  0, V4L2_CID_PU_XU_DEWARP_CHANNEL     , "channel"},
    {PU_XU_GUID, PU_XU_DEWARP_PARAMS1,       32,  32, V4L2_CID_PU_XU_DEWARP_PANEL    , "panel"},
    {PU_XU_GUID, PU_XU_DEWARP_PARAMS1,       32,  64, V4L2_CID_PU_XU_DEWARP_MODE    , "mode"},
    {PU_XU_GUID, PU_XU_DEWARP_PARAMS1,       32,  96, V4L2_CID_PU_XU_DEWARP_PARAM0         , "param0"},
    {PU_XU_GUID, PU_XU_DEWARP_PARAMS1,       32,  128, V4L2_CID_PU_XU_DEWARP_PARAM1         , "param1"},
    {PU_XU_GUID, PU_XU_DEWARP_PARAMS1,       32,  160, V4L2_CID_PU_XU_DEWARP_PARAM2         , "param2"},
    {PU_XU_GUID, PU_XU_DEWARP_PARAMS1,       32,  192, V4L2_CID_PU_XU_DEWARP_PARAM3         , "param3"},
    {PU_XU_GUID, PU_XU_DEWARP_PARAMS1,       32,  224, V4L2_CID_PU_XU_DEWARP_PARAM4         , "param4"},
    {PU_XU_GUID, PU_XU_DEWARP_PARAMS2,       32,  0, V4L2_CID_PU_XU_DEWARP_PARAM5         , "param5"},
    {PU_XU_GUID, PU_XU_DEWARP_PARAMS2,       32,  32, V4L2_CID_PU_XU_DEWARP_PARAM6         , "param6"},
    {PU_XU_GUID, PU_XU_DEWARP_PARAMS2,       32,  64, V4L2_CID_PU_XU_DEWARP_PARAM7         , "param7"},
    {PU_XU_GUID, PU_XU_DEWARP_PARAMS2,       32,  96, V4L2_CID_PU_XU_DEWARP_PARAM8         , "param8"},
    {PU_XU_GUID, PU_XU_DEWARP_PARAMS2,       32,  128, V4L2_CID_PU_XU_DEWARP_PARAM9         , "param9"},

    {PU_XU_GUID, PU_XU_COMPOSITOR_PARAMS,       32,  0,   V4L2_CID_PU_XU_COMPOSITOR_CHANNEL     , "channel"},
    {PU_XU_GUID, PU_XU_COMPOSITOR_PARAMS,       32,  32,  V4L2_CID_PU_XU_COMPOSITOR_PANEL_ID    , "panel"},
    {PU_XU_GUID, PU_XU_COMPOSITOR_PARAMS,       32,  64,  V4L2_CID_PU_XU_COMPOSITOR_MODE    , "mode"},
    {PU_XU_GUID, PU_XU_COMPOSITOR_PARAMS,       32,  96,  V4L2_CID_PU_XU_COMPOSITOR_PARAM0         , "param0"},
    {PU_XU_GUID, PU_XU_COMPOSITOR_PARAMS,       32,  128, V4L2_CID_PU_XU_COMPOSITOR_PARAM1         , "param1"},
    {PU_XU_GUID, PU_XU_COMPOSITOR_PARAMS,       32,  160, V4L2_CID_PU_XU_COMPOSITOR_PARAM2         , "param2"},
    {PU_XU_GUID, PU_XU_COMPOSITOR_PARAMS,       32,  192, V4L2_CID_PU_XU_COMPOSITOR_PARAM3         , "param3"},
    {PU_XU_GUID, PU_XU_COMPOSITOR_PARAMS,       32,  224, V4L2_CID_PU_XU_COMPOSITOR_PARAM4         , "param4"},

    {PU_XU_GUID, PU_XU_CONFIG_PARAMS,       32,  0, V4L2_CID_PU_XU_CONFIG_PARAM0    , "param0"},
    {PU_XU_GUID, PU_XU_CONFIG_PARAMS,       32,  32, V4L2_CID_PU_XU_CONFIG_PARAM1    , "param1"},
    {PU_XU_GUID, PU_XU_CONFIG_PARAMS,       32,  64, V4L2_CID_PU_XU_CONFIG_PARAM2    , "param2"},
    {PU_XU_GUID, PU_XU_CONFIG_PARAMS,       32,  96, V4L2_CID_PU_XU_CONFIG_PARAM3         , "param3"},
    {PU_XU_GUID, PU_XU_CONFIG_PARAMS,       32,  128, V4L2_CID_PU_XU_CONFIG_PARAM4         , "param4"},
    {PU_XU_GUID, PU_XU_CONFIG_PARAMS,       32,  160, V4L2_CID_PU_XU_CONFIG_PARAM5         , "param5"},
    {PU_XU_GUID, PU_XU_CONFIG_PARAMS,       32,  192, V4L2_CID_PU_XU_CONFIG_PARAM6         , "param6"},
    {PU_XU_GUID, PU_XU_CONFIG_PARAMS,       32,  224, V4L2_CID_PU_XU_CONFIG_PARAM7         , "param7"},
    {PU_XU_GUID, PU_XU_SATURATION_MODE,     32,   0, V4L2_CID_PU_XU_SATURATION_MODE    , "Saturation Mode"},
    {PU_XU_GUID, PU_XU_BRIGHTNESS_MODE,     32,   0, V4L2_CID_PU_XU_BRIGHTNESS_MODE    , "Brightness Mode"},
    {PU_XU_GUID, PU_XU_CONTRAST_MODE,       32,   0, V4L2_CID_PU_XU_CONTRAST_MODE    , "Contrast Mode"},
#endif
};

#if 0
#define DECLARE_SET(ctrl, ctrl_id, size_type) \
    int mxuvc_video_set_##ctrl (video_channel_t ch, size_type value)\
    {\
    struct v4l2_control control;\
    struct video_stream *vstream;\
    int ret = 0, id;\
    \
    control.id = ctrl_id;\
    control.value = value;\
    \
    vstream = &video_stream[ch];\
    if(vstream->enabled <= 0)\
    {\
        V4L2_ERROR("\n\rUnable to set " #ctrl "(channel not enabled)");\
        return -1;\
    }\
    \
    if((vstream->cur_vfmt == VID_FORMAT_MUX) && \
        (ctrl_id == V4L2_CID_DIGITIAL_MULTIPLIER || \
        ctrl_id == V4L2_CID_XU_BITRATE       || \
        ctrl_id == V4L2_CID_XU_GOP_LENGTH    || \
        ctrl_id == MUX_XU_VUI_ENABLE         || \
        ctrl_id == MUX_XU_COMPRESSION_Q      || \
        ctrl_id == V4L2_CID_XU_AVC_PROFILE   || \
        ctrl_id == MUX_XU_AVC_LEVEL      || \
        ctrl_id == MUX_XU_PIC_TIMING_ENABLE  || \
        ctrl_id == MUX_XU_GOP_HIERARCHY_LEVEL    || \
        ctrl_id == V4L2_CID_XU_MAX_FRAME_SIZE))     \
    { \
        if(ctrl_id == V4L2_CID_DIGITIAL_MULTIPLIER) \
            id = MUX_XU_ZOOM;           \
        else if(ctrl_id == V4L2_CID_XU_BITRATE)     \
            id = MUX_XU_BITRATE;            \
        else if(ctrl_id == V4L2_CID_XU_GOP_LENGTH)  \
            id = MUX_XU_GOP_LENGTH;         \
        else if(ctrl_id == V4L2_CID_XU_AVC_PROFILE) \
            id = MUX_XU_AVC_PROFILE;        \
        else if(ctrl_id == V4L2_CID_XU_MAX_FRAME_SIZE)  \
            id = MUX_XU_AVC_MAX_FRAME_SIZE;     \
        else                        \
            id = ctrl_id;               \
        ret = getset_mux_channel_param(ch,          \
                (void *)&value,         \
                sizeof(size_type),      \
                id,             \
                1);                 \
    } else { \
        ret = v4l_usr_ioctl(vstream->fd, VIDIOC_S_CTRL, &control);\
        if(ret < 0)\
        {\
            V4L2_ERROR("\n\rUnable to set " #ctrl "(VIDIOC_S_CTRL failed)");\
            return -1;\
        }\
    } \
    return ret;\
}

#define DECLARE_GET(ctrl, ctrl_id, size_type) \
    int mxuvc_video_get_##ctrl(video_channel_t ch, size_type *value)\
    {\
    struct v4l2_control control;\
    struct video_stream *vstream;\
    int ret = 0, id;\
    \
    control.id = ctrl_id;\
    \
    vstream = &video_stream[ch];\
    if(vstream->enabled <= 0)\
    {\
        V4L2_ERROR("\n\rUnable to get " #ctrl "(channel not enabled)");\
        return -1;\
    }\
    \
    if((vstream->cur_vfmt == VID_FORMAT_MUX) && \
        (ctrl_id == V4L2_CID_DIGITIAL_MULTIPLIER || \
        ctrl_id == V4L2_CID_XU_BITRATE       || \
        ctrl_id == V4L2_CID_XU_GOP_LENGTH    || \
        ctrl_id == MUX_XU_VUI_ENABLE         || \
        ctrl_id == MUX_XU_COMPRESSION_Q      || \
        ctrl_id == V4L2_CID_XU_AVC_PROFILE   || \
        ctrl_id == MUX_XU_AVC_LEVEL      || \
        ctrl_id == MUX_XU_PIC_TIMING_ENABLE  || \
        ctrl_id == MUX_XU_GOP_HIERARCHY_LEVEL    || \
        ctrl_id == V4L2_CID_XU_MAX_FRAME_SIZE))     \
    { \
        if(ctrl_id == V4L2_CID_DIGITIAL_MULTIPLIER) \
            id = MUX_XU_ZOOM;           \
        else if(ctrl_id == V4L2_CID_XU_BITRATE)     \
            id = MUX_XU_BITRATE;            \
        else if(ctrl_id == V4L2_CID_XU_GOP_LENGTH)  \
            id = MUX_XU_GOP_LENGTH;         \
        else if(ctrl_id == V4L2_CID_XU_AVC_PROFILE) \
            id = MUX_XU_AVC_PROFILE;        \
        else if(ctrl_id == V4L2_CID_XU_MAX_FRAME_SIZE)  \
            id = MUX_XU_AVC_MAX_FRAME_SIZE;     \
        else                        \
            id = ctrl_id;               \
        ret = getset_mux_channel_param(ch,          \
                (void *)value,          \
                sizeof(size_type),      \
                id,             \
                0);                 \
    } else {                        \
        ret = v4l_usr_ioctl(vstream->fd, VIDIOC_G_CTRL, &control);       \
        if(ret < 0)\
        {\
            V4L2_ERROR("\n\rUnable to get " #ctrl "(VIDIOC_G_CTRL failed)");\
            return -1;\
        }\
    \
        *value = control.value;\
    } \
    return ret;\
}
#define DECLARE_CTRL(ctrl, ctrl_id, size_type) \
    DECLARE_SET(ctrl, ctrl_id, size_type); \
    DECLARE_GET(ctrl, ctrl_id, size_type);

DECLARE_CTRL(brightness,        V4L2_CID_BRIGHTNESS,             int16_t);
DECLARE_CTRL(contrast,          V4L2_CID_CONTRAST,               uint16_t);
DECLARE_CTRL(saturation,        V4L2_CID_SATURATION,             uint16_t);
DECLARE_CTRL(gain,              V4L2_CID_GAIN,                   uint16_t);
DECLARE_CTRL(sharpness,         V4L2_CID_SHARPNESS,              uint16_t);
DECLARE_CTRL(goplen,            V4L2_CID_XU_GOP_LENGTH,          uint32_t);
DECLARE_CTRL(flip_vertical,     V4L2_CID_PU_XU_VFLIP,            video_flip_t);
DECLARE_CTRL(flip_horizontal,   V4L2_CID_PU_XU_HFLIP,            video_flip_t);
#endif

static int getset_mux_channel_param(video_channel_t ch,
                                    void *data,
                                    uint32_t data_size,
                                    enum MUX_XU_CTRL xu_name,
                                    uint32_t set)
{
    
    SDRAM_DATA_SECTION static uint32_t muxch_v4l2_mapping[MUX_XU_NUM_CTRLS+1][NUM_MUX_VID_CHANNELS] = {
        [MUX_XU_RESOLUTION][CH1]    = V4L2_CID_MUX_CH1_XU_RESOLUTION,
        [MUX_XU_RESOLUTION][CH2]    = V4L2_CID_MUX_CH2_XU_RESOLUTION,
        [MUX_XU_RESOLUTION][CH3]    = V4L2_CID_MUX_CH3_XU_RESOLUTION,
        [MUX_XU_RESOLUTION][CH4]    = V4L2_CID_MUX_CH4_XU_RESOLUTION,
        [MUX_XU_RESOLUTION][CH5]    = V4L2_CID_MUX_CH5_XU_RESOLUTION,
        [MUX_XU_RESOLUTION][CH6]    = V4L2_CID_MUX_CH6_XU_RESOLUTION,
        [MUX_XU_RESOLUTION][CH7]    = V4L2_CID_MUX_CH7_XU_RESOLUTION,

        [MUX_XU_FRAMEINTRVL][CH1]   = V4L2_CID_MUX_CH1_XU_FRAMEINTRVL,
        [MUX_XU_FRAMEINTRVL][CH2]   = V4L2_CID_MUX_CH2_XU_FRAMEINTRVL,
        [MUX_XU_FRAMEINTRVL][CH3]   = V4L2_CID_MUX_CH3_XU_FRAMEINTRVL,
        [MUX_XU_FRAMEINTRVL][CH4]   = V4L2_CID_MUX_CH4_XU_FRAMEINTRVL,
        [MUX_XU_FRAMEINTRVL][CH5]   = V4L2_CID_MUX_CH5_XU_FRAMEINTRVL,
        [MUX_XU_FRAMEINTRVL][CH6]   = V4L2_CID_MUX_CH6_XU_FRAMEINTRVL,
        [MUX_XU_FRAMEINTRVL][CH7]   = V4L2_CID_MUX_CH7_XU_FRAMEINTRVL,

        [MUX_XU_ZOOM][CH1]          = V4L2_CID_MUX_CH1_XU_ZOOM,
        [MUX_XU_ZOOM][CH2]          = V4L2_CID_MUX_CH2_XU_ZOOM,
        [MUX_XU_ZOOM][CH3]          = V4L2_CID_MUX_CH3_XU_ZOOM,
        [MUX_XU_ZOOM][CH4]          = V4L2_CID_MUX_CH4_XU_ZOOM,
        [MUX_XU_ZOOM][CH5]          = V4L2_CID_MUX_CH5_XU_ZOOM,
        [MUX_XU_ZOOM][CH6]          = V4L2_CID_MUX_CH6_XU_ZOOM,
        [MUX_XU_ZOOM][CH7]          = V4L2_CID_MUX_CH7_XU_ZOOM,

        [MUX_XU_BITRATE][CH1]       = V4L2_CID_MUX_CH1_XU_BITRATE,
        [MUX_XU_BITRATE][CH2]       = V4L2_CID_MUX_CH2_XU_BITRATE,
        [MUX_XU_BITRATE][CH3]       = V4L2_CID_MUX_CH3_XU_BITRATE,
        [MUX_XU_BITRATE][CH4]       = V4L2_CID_MUX_CH4_XU_BITRATE,
        [MUX_XU_BITRATE][CH5]       = V4L2_CID_MUX_CH5_XU_BITRATE,
        [MUX_XU_BITRATE][CH6]       = V4L2_CID_MUX_CH6_XU_BITRATE,
        [MUX_XU_BITRATE][CH7]       = V4L2_CID_MUX_CH7_XU_BITRATE,

        [MUX_XU_FORCE_I_FRAME][CH1] = V4L2_CID_MUX_CH1_XU_FORCE_I_FRAME,
        [MUX_XU_FORCE_I_FRAME][CH2] = V4L2_CID_MUX_CH2_XU_FORCE_I_FRAME,
        [MUX_XU_FORCE_I_FRAME][CH3] = V4L2_CID_MUX_CH3_XU_FORCE_I_FRAME,
        [MUX_XU_FORCE_I_FRAME][CH4] = V4L2_CID_MUX_CH4_XU_FORCE_I_FRAME,
        [MUX_XU_FORCE_I_FRAME][CH5] = V4L2_CID_MUX_CH5_XU_FORCE_I_FRAME,
        [MUX_XU_FORCE_I_FRAME][CH6] = V4L2_CID_MUX_CH6_XU_FORCE_I_FRAME,
        [MUX_XU_FORCE_I_FRAME][CH7] = V4L2_CID_MUX_CH7_XU_FORCE_I_FRAME,

        [MUX_XU_VUI_ENABLE][CH1]   = V4L2_CID_MUX_CH1_XU_VUI_ENABLE,
        [MUX_XU_VUI_ENABLE][CH2]   = V4L2_CID_MUX_CH2_XU_VUI_ENABLE,
        [MUX_XU_VUI_ENABLE][CH3]   = V4L2_CID_MUX_CH3_XU_VUI_ENABLE,
        [MUX_XU_VUI_ENABLE][CH4]   = V4L2_CID_MUX_CH4_XU_VUI_ENABLE,
        [MUX_XU_VUI_ENABLE][CH5]   = V4L2_CID_MUX_CH5_XU_VUI_ENABLE,
        [MUX_XU_VUI_ENABLE][CH6]   = V4L2_CID_MUX_CH6_XU_VUI_ENABLE,
        [MUX_XU_VUI_ENABLE][CH7]   = V4L2_CID_MUX_CH7_XU_VUI_ENABLE,

        [MUX_XU_COMPRESSION_Q][CH1] = V4L2_CID_MUX_CH1_XU_COMPRESSION_Q,
        [MUX_XU_COMPRESSION_Q][CH2] = V4L2_CID_MUX_CH2_XU_COMPRESSION_Q,
        [MUX_XU_COMPRESSION_Q][CH3] = V4L2_CID_MUX_CH3_XU_COMPRESSION_Q,
        [MUX_XU_COMPRESSION_Q][CH4] = V4L2_CID_MUX_CH4_XU_COMPRESSION_Q,
        [MUX_XU_COMPRESSION_Q][CH5] = V4L2_CID_MUX_CH5_XU_COMPRESSION_Q,
        [MUX_XU_COMPRESSION_Q][CH6] = V4L2_CID_MUX_CH6_XU_COMPRESSION_Q,
        [MUX_XU_COMPRESSION_Q][CH7] = V4L2_CID_MUX_CH7_XU_COMPRESSION_Q,

        [MUX_XU_CHCOUNT][CH1]       = V4L2_CID_MUX_CH1_XU_CHCOUNT,

        [MUX_XU_CHTYPE][CH1]        = V4L2_CID_MUX_CH1_XU_CHTYPE,
        [MUX_XU_CHTYPE][CH2]        = V4L2_CID_MUX_CH2_XU_CHTYPE,
        [MUX_XU_CHTYPE][CH3]        = V4L2_CID_MUX_CH3_XU_CHTYPE,
        [MUX_XU_CHTYPE][CH4]        = V4L2_CID_MUX_CH4_XU_CHTYPE,
        [MUX_XU_CHTYPE][CH5]        = V4L2_CID_MUX_CH5_XU_CHTYPE,
        [MUX_XU_CHTYPE][CH6]        = V4L2_CID_MUX_CH6_XU_CHTYPE,
        [MUX_XU_CHTYPE][CH7]        = V4L2_CID_MUX_CH7_XU_CHTYPE,

        [MUX_XU_GOP_LENGTH][CH1]    = V4L2_CID_MUX_CH1_XU_GOP_LENGTH,
        [MUX_XU_GOP_LENGTH][CH2]    = V4L2_CID_MUX_CH2_XU_GOP_LENGTH,
        [MUX_XU_GOP_LENGTH][CH3]    = V4L2_CID_MUX_CH3_XU_GOP_LENGTH,
        [MUX_XU_GOP_LENGTH][CH4]    = V4L2_CID_MUX_CH4_XU_GOP_LENGTH,
        [MUX_XU_GOP_LENGTH][CH5]    = V4L2_CID_MUX_CH5_XU_GOP_LENGTH,
        [MUX_XU_GOP_LENGTH][CH6]    = V4L2_CID_MUX_CH6_XU_GOP_LENGTH,
        [MUX_XU_GOP_LENGTH][CH7]    = V4L2_CID_MUX_CH7_XU_GOP_LENGTH,

        [MUX_XU_AVC_PROFILE][CH1]   = V4L2_CID_MUX_CH1_XU_AVC_PROFILE,
        [MUX_XU_AVC_PROFILE][CH2]   = V4L2_CID_MUX_CH2_XU_AVC_PROFILE,
        [MUX_XU_AVC_PROFILE][CH3]   = V4L2_CID_MUX_CH3_XU_AVC_PROFILE,
        [MUX_XU_AVC_PROFILE][CH4]   = V4L2_CID_MUX_CH4_XU_AVC_PROFILE,
        [MUX_XU_AVC_PROFILE][CH5]   = V4L2_CID_MUX_CH5_XU_AVC_PROFILE,

        [MUX_XU_AVC_MAX_FRAME_SIZE][CH1]   = V4L2_CID_MUX_CH1_XU_AVC_MAX_FRAME_SIZE,
        [MUX_XU_AVC_MAX_FRAME_SIZE][CH2]   = V4L2_CID_MUX_CH2_XU_AVC_MAX_FRAME_SIZE,
        [MUX_XU_AVC_MAX_FRAME_SIZE][CH3]   = V4L2_CID_MUX_CH3_XU_AVC_MAX_FRAME_SIZE,
        [MUX_XU_AVC_MAX_FRAME_SIZE][CH4]   = V4L2_CID_MUX_CH4_XU_AVC_MAX_FRAME_SIZE,
        [MUX_XU_AVC_MAX_FRAME_SIZE][CH5]   = V4L2_CID_MUX_CH5_XU_AVC_MAX_FRAME_SIZE,

        [MUX_XU_START_CHANNEL][CH1] = V4L2_CID_MUX_XU_START_CHANNEL,
        [MUX_XU_START_CHANNEL][CH2] = V4L2_CID_MUX_XU_START_CHANNEL,
        [MUX_XU_START_CHANNEL][CH3] = V4L2_CID_MUX_XU_START_CHANNEL,
        [MUX_XU_START_CHANNEL][CH4] = V4L2_CID_MUX_XU_START_CHANNEL,
        [MUX_XU_START_CHANNEL][CH5] = V4L2_CID_MUX_XU_START_CHANNEL,
        [MUX_XU_START_CHANNEL][CH6] = V4L2_CID_MUX_XU_START_CHANNEL,
        [MUX_XU_START_CHANNEL][CH7] = V4L2_CID_MUX_XU_START_CHANNEL,

        [MUX_XU_STOP_CHANNEL][CH1] = V4L2_CID_MUX_XU_STOP_CHANNEL,
        [MUX_XU_STOP_CHANNEL][CH2] = V4L2_CID_MUX_XU_STOP_CHANNEL,
        [MUX_XU_STOP_CHANNEL][CH3] = V4L2_CID_MUX_XU_STOP_CHANNEL,
        [MUX_XU_STOP_CHANNEL][CH4] = V4L2_CID_MUX_XU_STOP_CHANNEL,
        [MUX_XU_STOP_CHANNEL][CH5] = V4L2_CID_MUX_XU_STOP_CHANNEL,
        [MUX_XU_STOP_CHANNEL][CH6] = V4L2_CID_MUX_XU_STOP_CHANNEL,
        [MUX_XU_STOP_CHANNEL][CH7] = V4L2_CID_MUX_XU_STOP_CHANNEL,

        [MUX_XU_AVC_LEVEL][CH1] = V4L2_CID_MUX_CH1_XU_AVC_LEVEL,
        [MUX_XU_AVC_LEVEL][CH2] = V4L2_CID_MUX_CH2_XU_AVC_LEVEL,
        [MUX_XU_AVC_LEVEL][CH3] = V4L2_CID_MUX_CH3_XU_AVC_LEVEL,
        [MUX_XU_AVC_LEVEL][CH4] = V4L2_CID_MUX_CH4_XU_AVC_LEVEL,
        [MUX_XU_AVC_LEVEL][CH5] = V4L2_CID_MUX_CH5_XU_AVC_LEVEL,
        [MUX_XU_AVC_LEVEL][CH6] = V4L2_CID_MUX_CH6_XU_AVC_LEVEL,
        [MUX_XU_AVC_LEVEL][CH7] = V4L2_CID_MUX_CH7_XU_AVC_LEVEL,

        [MUX_XU_VFLIP][CH1] = V4L2_CID_MUX_CH1_XU_VFLIP,

        [MUX_XU_AUDIO_BITRATE][CH1] = V4L2_CID_MUX_CH1_XU_AUDIO_BITRATE,

        [MUX_XU_PIC_TIMING_ENABLE][CH1] = V4L2_CID_MUX_CH1_XU_PIC_TIMING_ENABLE,
        [MUX_XU_PIC_TIMING_ENABLE][CH2] = V4L2_CID_MUX_CH2_XU_PIC_TIMING_ENABLE,
        [MUX_XU_PIC_TIMING_ENABLE][CH3] = V4L2_CID_MUX_CH3_XU_PIC_TIMING_ENABLE,
        [MUX_XU_PIC_TIMING_ENABLE][CH4] = V4L2_CID_MUX_CH4_XU_PIC_TIMING_ENABLE,
        [MUX_XU_PIC_TIMING_ENABLE][CH5] = V4L2_CID_MUX_CH5_XU_PIC_TIMING_ENABLE,
        [MUX_XU_PIC_TIMING_ENABLE][CH6] = V4L2_CID_MUX_CH6_XU_PIC_TIMING_ENABLE,
        [MUX_XU_PIC_TIMING_ENABLE][CH7] = V4L2_CID_MUX_CH7_XU_PIC_TIMING_ENABLE,

        [MUX_XU_GOP_HIERARCHY_LEVEL][CH1] = V4L2_CID_MUX_CH1_XU_GOP_HIERARCHY_LEVEL,
        [MUX_XU_GOP_HIERARCHY_LEVEL][CH2] = V4L2_CID_MUX_CH2_XU_GOP_HIERARCHY_LEVEL,
        [MUX_XU_GOP_HIERARCHY_LEVEL][CH3] = V4L2_CID_MUX_CH3_XU_GOP_HIERARCHY_LEVEL,
        [MUX_XU_GOP_HIERARCHY_LEVEL][CH4] = V4L2_CID_MUX_CH4_XU_GOP_HIERARCHY_LEVEL,
        [MUX_XU_GOP_HIERARCHY_LEVEL][CH5] = V4L2_CID_MUX_CH5_XU_GOP_HIERARCHY_LEVEL,
        [MUX_XU_GOP_HIERARCHY_LEVEL][CH6] = V4L2_CID_MUX_CH6_XU_GOP_HIERARCHY_LEVEL,
        [MUX_XU_GOP_HIERARCHY_LEVEL][CH7] = V4L2_CID_MUX_CH7_XU_GOP_HIERARCHY_LEVEL,
#if 0
        [MUX_XU_DEWRP_CHANNEL][CH1] = V4L2_CID_MUX_CH1_XU_DEWRP_CHANNEL,
        [MUX_XU_DEWRP_CHANNEL][CH2] = V4L2_CID_MUX_CH2_XU_DEWRP_CHANNEL,
        [MUX_XU_DEWRP_CHANNEL][CH3] = V4L2_CID_MUX_CH3_XU_DEWRP_CHANNEL,
        [MUX_XU_DEWRP_CHANNEL][CH4] = V4L2_CID_MUX_CH4_XU_DEWRP_CHANNEL,
        [MUX_XU_DEWRP_CHANNEL][CH5] = V4L2_CID_MUX_CH5_XU_DEWRP_CHANNEL,
        [MUX_XU_DEWRP_CHANNEL][CH6] = V4L2_CID_MUX_CH6_XU_DEWRP_CHANNEL,
        [MUX_XU_DEWRP_CHANNEL][CH7] = V4L2_CID_MUX_CH7_XU_DEWRP_CHANNEL,

        [MUX_XU_VBR_MIN_BITRATE][CH1] = V4L2_CID_MUX_CH1_XU_VBR_MIN_BITRATE,
        [MUX_XU_VBR_MIN_BITRATE][CH2] = V4L2_CID_MUX_CH2_XU_VBR_MIN_BITRATE,
        [MUX_XU_VBR_MIN_BITRATE][CH3] = V4L2_CID_MUX_CH3_XU_VBR_MIN_BITRATE,
        [MUX_XU_VBR_MIN_BITRATE][CH4] = V4L2_CID_MUX_CH4_XU_VBR_MIN_BITRATE,
        [MUX_XU_VBR_MIN_BITRATE][CH5] = V4L2_CID_MUX_CH5_XU_VBR_MIN_BITRATE,
        [MUX_XU_VBR_MIN_BITRATE][CH6] = V4L2_CID_MUX_CH6_XU_VBR_MIN_BITRATE,
        [MUX_XU_VBR_MIN_BITRATE][CH7] = V4L2_CID_MUX_CH7_XU_VBR_MIN_BITRATE,
#endif
    };

    int ret = 0;
    struct v4l2_control control;

    struct video_stream *vstream;
    vstream = &video_stream[ch];
    control.id = muxch_v4l2_mapping[xu_name][ch];

    if (set == 1) {
        if (data_size == 1)
            control.value = *(uint8_t *)data;
        else if (data_size == 2)
            control.value = *(uint16_t *)data;
        else if (data_size == 4)
            control.value = *(uint32_t *)data;

        if((ret = v4l_usr_ioctl(vstream->fd, VIDIOC_S_CTRL, &control)) < 0)
            V4L2_ERROR("\n\rVIDIOC_S_CTRL failed!");  
    } else { //get
        ret = v4l_usr_ioctl(vstream->fd, VIDIOC_G_CTRL, &control);
        if (data_size == 1)
            *(uint8_t *)data = (uint8_t)control.value;
        else if (data_size == 2)
            *(uint16_t *)data = (uint16_t)control.value;
        else if (data_size == 4)
            *(uint32_t *)data = (uint32_t)control.value;
        if(ret < 0)
            V4L2_ERROR("\n\rVIDIOC_G_CTRL failed");
    }

    return ret;
}
//edit by Ian -- feature not needed
#if 0
int mxuvc_video_set_format(video_channel_t ch, video_format_t fmt)
{

    struct video_stream *vstream;
    int ret;
    int restart = 0;

    vstream = &video_stream[ch];
    if(vstream->enabled <= 0)
    {
        V4L2_ERROR("\n\rUnable to get the video format(channel not enabled)");
        return 0;
    }

    if(fmt >= NUM_VID_FORMAT)
    {
        V4L2_ERROR("\n\runknown video format requested");
        return -EINVAL;
    }

    /* Return if the current format correspond to the format requested */
    if (fmt == vstream->cur_vfmt)
        return 0;
    
    if (vstream->cur_vfmt == VID_FORMAT_MUX) {
        V4L2_ERROR("\n\rcannot set format in MUX channel");
        return -EPERM;
    }
    ret = vidformat2fourcc(fmt, &vstream->fmt.fmt.pix.pixelformat);
    if (ret < 0) {
        V4L2_ERROR("\n\rno corresponding format matched!");
        if (restart) mxuvc_video_start(ch);
        return -1;
    }

    /* Stop video streaming if necessary before changing format */
    if (vstream->started) {
        ret = mxuvc_video_stop(ch);
        restart = 1;
    }
    
    ret = v4l_usr_ioctl(vstream->fd, VIDIOC_S_FMT, &vstream->fmt);
    if (ret < 0) {
        V4L2_ERROR("\n\rUnable to set the video format on %s channel", vstream->fd);
        if (restart) mxuvc_video_start(ch);
        return -1;
    }

    vstream->cur_vfmt = fmt;

    /* Restart video streaming if necessary */
    if (restart) {
        ret = mxuvc_video_start(ch);
        V4L2_ERROR("\n\rUnable to set the video format on %s channel", vstream->fd);
    }
    
    return 0;
}
#endif

int mxuvc_video_get_format(video_channel_t ch, video_format_t *fmt)
{
    struct video_stream *vstream;
    uint32_t fourcc = 0;
    vstream = &video_stream[ch];
    if(vstream->enabled <= 0)
    {
        V4L2_ERROR("\n\rUnable to get the video format(channel not enabled)");
        return -1;
    }
    if (vstream->cur_vfmt == VID_FORMAT_MUX) {
        getset_mux_channel_param(ch, (void *)&fourcc, sizeof(uint32_t),
                                 MUX_XU_CHTYPE, 0);
        video_format_t format;
        if(fourcc2vidformat(fourcc, &format) < 0)
        {
            V4L2_ERROR("\n\rUnable to get the video format(Unknown fourcc)");
            return -1;
        }
        *fmt = format;
    } else
        *fmt = vstream->cur_vfmt;

    return 0;
}


int mxuvc_video_set_bitrate (video_channel_t ch, uint32_t value)
{

    struct v4l2_control control;
    struct video_stream *vstream;
    int ret;

    vstream = &video_stream[ch];
    if(vstream->enabled <= 0)
    {
        V4L2_ERROR("\n\rUnable to set bitrate(channel not enabled)");
        return -1;
    }

    if (vstream->cur_vfmt == VID_FORMAT_MUX)
    {
        getset_mux_channel_param(ch, (void *)&value, sizeof(uint32_t),
                                 MUX_XU_BITRATE, 1);
    } else {
        control.id = V4L2_CID_XU_BITRATE;
        control.value = value;
        ret = v4l_usr_ioctl(vstream->fd, VIDIOC_S_CTRL, &control);

        if(ret < 0)
        {
            V4L2_ERROR("\n\rUnable to set the bitrate (VIDIOC_S_CTRL failed )");
            return -1;
        }
    }

    vstream->cur_bitrate = value;

    return 0;
}


int mxuvc_video_get_bitrate(video_channel_t ch, uint32_t *value)
{
    struct v4l2_control control;
    struct video_stream *vstream;
    int ret;

    vstream = &video_stream[ch];
    if(vstream->enabled <= 0)
    {
        V4L2_ERROR("\n\rUnable to get the bitrate(channel not enabled)");
        return -1;
    }

    if (vstream->cur_vfmt == VID_FORMAT_MUX)
    {
        getset_mux_channel_param(ch, (void *)value, sizeof(uint32_t),
                                 MUX_XU_BITRATE, 0);
    } else {
        control.id = V4L2_CID_XU_BITRATE;
        ret = v4l_usr_ioctl(vstream->fd, VIDIOC_G_CTRL, &control);
        if(ret < 0)
        {
            V4L2_ERROR("\n\rUnable to get the bitrate (VIDIOC_G_CTRL failed)");
            return -1;
        }
          
        *value = control.value;
    }

    vstream->cur_bitrate = *value;

    return 0;
}

int mxuvc_video_get_channel_count(uint32_t *count)
{
    int data = 0;
    int ret = 0;
    ret = getset_mux_channel_param(CH1, (void *)&data, sizeof(uint32_t),
                             MUX_XU_CHCOUNT, 0);
    /* Add RAW channel also in channel count */
    *count = data + 1;

    return ret;
}

int mxuvc_video_set_resolution(video_channel_t ch, uint16_t width, uint16_t height)
{
    int ret, restart=0;
    uint32_t data;
    struct v4l2_control control;

    struct video_stream *vstream;
    vstream = &video_stream[ch];
    if(vstream->enabled <= 0)
    {
        V4L2_ERROR("\n\rUnable to set the resolution(channel not enabled)");
        return -1;
    }

    /* Non SECS method */
    switch (vstream->cur_vfmt) {
    case VID_FORMAT_H264_RAW:
    case VID_FORMAT_H264_TS:
    case VID_FORMAT_H264_AAC_TS:
        control.id = V4L2_CID_XU_RESOLUTION2;
        control.value = (width << 16) + height;

        if((ret = v4l_usr_ioctl(vstream->fd, VIDIOC_S_CTRL, &control)) < 0)
        {
            V4L2_ERROR("\n\rUnable to set the resolution: VIDIOC_S_CTRL failed (id = 0x%x, value = %d)", control.id, control.value);
            return -1;
        }
        break;
    case VID_FORMAT_MUX:

            data = (width << 16) + height;
            ret = getset_mux_channel_param(ch, (void *)&data, sizeof(uint32_t),
                                           MUX_XU_RESOLUTION, 1);
        if(ret < 0)
        {
            V4L2_ERROR("\n\rUnable to set the resolution: VIDIOC_S_CTRL failed (id = 0x%x, value = %d)", control.id, control.value);
            return -1;
            }
        break;
    default:
        /* Stop the channel first if necessary */
        if (vstream->started) {
            ret = mxuvc_video_stop(ch);
            if(ret < 0)
            {
                V4L2_ERROR("\n\rFailed to set the video resolution: could not stop stream!");
                return -1;
            }
            restart = 1;
        }
        vstream->fmt.fmt.pix.width = width;
        vstream->fmt.fmt.pix.height = height;

        ret = v4l_usr_ioctl(vstream->fd, VIDIOC_S_FMT, &vstream->fmt);
        /* Restart the channel if necessary */
        if (restart) 
            mxuvc_video_start(ch);

        if(ret < 0)
        {
            V4L2_ERROR("\n\rUnable to set the resolution(VIDIOC_S_FMT failed)");
            return -1;
        }
        break;
    }

end_set_resolution:
    vstream->cur_width = width;
    vstream->cur_height = height;

    return 0;
}

int mxuvc_video_get_resolution(video_channel_t ch, uint16_t *width, uint16_t *height)
{
    int ret = 0;
    struct v4l2_control control;
    uint32_t data = 0;

    struct video_stream *vstream;
    vstream = &video_stream[ch];
    if(vstream->enabled <= 0)
    {
        V4L2_ERROR("\n\rUnable to get resolution(channel not enabled)");
        return -1;
    }

    switch (vstream->cur_vfmt) {
    case VID_FORMAT_MUX:
        getset_mux_channel_param(ch, (void *)&data, sizeof(uint32_t),
                                 MUX_XU_RESOLUTION, 0);
        *width  = (uint16_t) ((data>>16) & 0xffff);
        *height = (uint16_t) (data & 0xffff);
        break;
    case VID_FORMAT_H264_RAW:
    case VID_FORMAT_H264_TS:
    case VID_FORMAT_H264_AAC_TS:
        control.id = V4L2_CID_XU_RESOLUTION2;

        ret = v4l_usr_ioctl(vstream->fd, VIDIOC_G_CTRL, &control);
        if(ret < 0)
        {
            V4L2_ERROR("\n\rUnable to get resolution(VIDIOC_G_CTRL failed (id = 0x%x))", control.id);
            return -1;
        }

        *width  = (uint16_t) ((control.value>>16) & 0xffff);
        *height = (uint16_t) (control.value & 0xffff);
        break;
    default:
        ret = v4l_usr_ioctl(vstream->fd, VIDIOC_G_FMT, &vstream->fmt);
        if(ret < 0)
        {
            V4L2_ERROR("\n\rUnable to get the resolution(VIDIOC_G_FMT failed)");
            return -1;
        }
        *width = vstream->fmt.fmt.pix.width;
        *height = vstream->fmt.fmt.pix.height;
        break;
    }

    vstream->cur_width = *width;
    vstream->cur_height = *height;

    return 0;
}

int mxuvc_video_set_framerate(video_channel_t ch, uint32_t framerate)
{
    int ret;
    struct v4l2_streamparm streamparam;

    struct video_stream *vstream;
    vstream = &video_stream[ch];
    if(vstream->enabled <= 0)
    {
        V4L2_ERROR("\n\rUnable to set the framerate (channel not enabled)");
        return -1;
    }

    if (vstream->cur_vfmt == VID_FORMAT_MUX) {
        uint32_t frminterval = (uint32_t)FRI(framerate);
        getset_mux_channel_param(ch, (void *)&frminterval, sizeof(uint32_t),
                                 MUX_XU_FRAMEINTRVL, 1);
    }

    /* Non SECS method */
    else {
        CLEAR(streamparam);
        streamparam.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        streamparam.parm.capture.timeperframe.numerator = 1;
        streamparam.parm.capture.timeperframe.denominator = framerate;

        ret = v4l_usr_ioctl(vstream->fd, VIDIOC_S_PARM, &streamparam);
        if(ret < 0)
        {
            V4L2_ERROR("Unable to set the framerate");
            return -1;
        }
    }
    vstream->cur_framerate = framerate;
    return 0;
}


int mxuvc_video_get_framerate(video_channel_t ch, uint32_t *framerate)
{
    int ret;
    struct v4l2_streamparm streamparam;
    struct v4l2_fract tpf;

    struct video_stream *vstream;
    vstream = &video_stream[ch];
    if(vstream->enabled <= 0)
    {
        V4L2_ERROR("\n\rUnable to get framerate (channel not enabled)");
        return -1;
    }

    if (vstream->cur_vfmt == VID_FORMAT_MUX) {
        uint32_t data;
        getset_mux_channel_param(ch, (void *)&data, sizeof(uint32_t),
                                 MUX_XU_FRAMEINTRVL, 0);
        *framerate = (uint32_t)FRR(data);
    }
    /* Non SECS method */
    else {
        CLEAR(streamparam);
        streamparam.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        ret = v4l_usr_ioctl(vstream->fd, VIDIOC_G_PARM, &streamparam);
        if(ret < 0)
        {
            V4L2_ERROR("\n\rUnable to get the framerate");
            return -1;
        }
        tpf = streamparam.parm.capture.timeperframe;
        *framerate = (uint32_t)(tpf.denominator/tpf.numerator + 0.5);
    }
    vstream->cur_framerate = *framerate;

    return 0;
}

#if 0
int mxuvc_video_force_iframe(video_channel_t ch)
{
    struct v4l2_control control;
    struct video_stream *vstream;
    int ret;

    vstream = &video_stream[ch];
    if(vstream->enabled <= 0)
    {
        V4L2_ERROR("\n\rUnable to force I frame (channel not enabled)");
        return -1;
    }

    control.value = 1;

    if (vstream->cur_vfmt == VID_FORMAT_MUX) {
        getset_mux_channel_param(ch, (void *)&control.value, sizeof(uint32_t),
                                 MUX_XU_FORCE_I_FRAME, 1);
    } else {
        control.id = V4L2_CID_XU_FORCE_I_FRAME;

        ret = v4l_usr_ioctl(vstream->fd, VIDIOC_S_CTRL, &control);
        if(ret < 0)
        {
            V4L2_ERROR("\n\rUnable to force I frame (VIDIOC_S_CTRL failed)");
            return -1;
        }
    }

    return 0;
}

int mxuvc_video_set_exp(video_channel_t ch, exp_set_t sel, uint16_t value)
{
    struct v4l2_control control;
    struct video_stream *vstream;
    int ret;

    vstream = &video_stream[ch];
    if(vstream->enabled <= 0)
    {
        V4L2_ERROR("\n\rUnable to set exposure (channel not enabled)");
        return -1;
    }

    control.id = V4L2_CID_PU_XU_AUTO_EXPOSURE;
    if (sel == EXP_AUTO)
        control.value = 1;
    else
        control.value = 0;

    ret = v4l_usr_ioctl(vstream->fd, VIDIOC_S_CTRL, &control);
    if(ret < 0)
    {
        V4L2_ERROR("\n\rUnable to set auto exposure (VIDIOC_S_CTRL failed)");
        return -1;
    }

    control.id = V4L2_CID_PU_XU_EXPOSURE_TIME;
    control.value = value;

    ret = v4l_usr_ioctl(vstream->fd, VIDIOC_S_CTRL, &control);
    if(ret < 0)
    {
        V4L2_ERROR("\n\rUnable to set exposure time (VIDIOC_S_CTRL failed)");
        return -1;
    }

    return 0;
}

int mxuvc_video_get_exp(video_channel_t ch, exp_set_t *sel, uint16_t *value)
{
    struct v4l2_control control;
    struct video_stream *vstream;
    int ret;

    vstream = &video_stream[ch];
    if(vstream->enabled <= 0)
    {
        V4L2_ERROR("\n\rUnable to get exposure (channel not enabled)");
        return -1;
    }

    control.id = V4L2_CID_PU_XU_AUTO_EXPOSURE;
    ret = v4l_usr_ioctl(vstream->fd, VIDIOC_G_CTRL, &control);
    if(ret < 0)
    {
        V4L2_ERROR("\n\rUnable to get exposure status (VIDIOC_G_CTRL failed)");
        return -1;
    }

    if (control.value)
        *sel = EXP_AUTO;
    else
        *sel = EXP_MANUAL;

    control.id = V4L2_CID_PU_XU_EXPOSURE_TIME;

    ret = v4l_usr_ioctl(vstream->fd, VIDIOC_G_CTRL, &control);
    if(ret < 0)
    {
        V4L2_ERROR("\n\rUnable to get exposure time (VIDIOC_G_CTRL failed)");
        return -1;
    }

    *value = control.value;

    return 0;
}

int mxuvc_video_set_wb(video_channel_t ch, white_balance_mode_t sel, uint16_t value)
{
    struct v4l2_control control;
    struct video_stream *vstream;
    int ret;

    vstream = &video_stream[ch];
    if(vstream->enabled <= 0)
    {
        V4L2_ERROR("\n\rUnable to set WB (channel not enabled)");
        return -1;
    }

    if ( sel == WB_MODE_AUTO ) {
        control.id = V4L2_CID_PU_XU_AUTO_WHITE_BAL;
        control.value = 1;
    } else if ( sel == WB_MODE_MANUAL ) {
        control.id = V4L2_CID_PU_XU_AUTO_WHITE_BAL;
        control.value = 0;
    }

    if( (sel == WB_MODE_AUTO) || ( sel == WB_MODE_MANUAL ) )
    {
        ret = v4l_usr_ioctl(vstream->fd, VIDIOC_S_CTRL, &control);
        if(ret < 0)
        {
            V4L2_ERROR("\n\rUnable to set Auto WB (VIDIOC_S_CTRL failed)");
            return -1;
        }
    }

    if(value != 0)
    {
        control.id = V4L2_CID_PU_XU_WHITE_BAL_TEMP;
        control.value = value;

        ret = v4l_usr_ioctl(vstream->fd, VIDIOC_S_CTRL, &control);
        if(ret < 0)
        {
            V4L2_ERROR("\n\rUnable to set WBT (VIDIOC_S_CTRL failed)");
            return -1;
        }
    }
    return 0;
}

int mxuvc_video_get_wb(video_channel_t ch, white_balance_mode_t *sel, uint16_t *value)
{
    struct v4l2_control control;
    struct video_stream *vstream;
    int ret;

    vstream = &video_stream[ch];
    if(vstream->enabled <= 0)
    {
        V4L2_ERROR("\n\rUnable to get WB value (channel not enabled)");
        return -1;
    }

    control.id = V4L2_CID_PU_XU_AUTO_WHITE_BAL;
    ret = v4l_usr_ioctl(vstream->fd, VIDIOC_G_CTRL, &control);
    if(ret < 0)
    {
        V4L2_ERROR("\n\rUnable to get ANF value (VIDIOC_G_CTRL failed)");
        return -1;
    }

    if ( control.value )
        *sel = WB_MODE_AUTO;
    else
        *sel = WB_MODE_MANUAL;

    if ( *sel == WB_MODE_MANUAL )
    {
        control.id = V4L2_CID_PU_XU_WHITE_BAL_TEMP;
        ret = v4l_usr_ioctl(vstream->fd, VIDIOC_G_CTRL, &control);
        if(ret < 0)
        {
            V4L2_ERROR("\n\rUnable to get WBT value (VIDIOC_G_CTRL failed)");
            return -1;
        }
        *value = control.value;
    }

    return 0;
}

int mxuvc_video_set_pwr_line_freq(video_channel_t ch, pwr_line_freq_mode_t mode)
{
    struct v4l2_control control;
    struct video_stream *vstream;
    int ret;

    vstream = &video_stream[ch];
    if(vstream->enabled <= 0)
    {
        V4L2_ERROR("\n\rUnable to set pwr line freq (channel not enabled)");
        return -1;
    }

    control.id = V4L2_CID_POWER_LINE_FREQUENCY;
    control.value = mode;
    ret = v4l_usr_ioctl(vstream->fd, VIDIOC_S_CTRL, &control);
    if(ret < 0)
    {
        V4L2_ERROR("\n\rUnable to set pwr line freq (VIDIOC_S_CTRL failed)");
        return -1;
    }

    return 0;
}

int mxuvc_video_get_pwr_line_freq(video_channel_t ch, pwr_line_freq_mode_t *mode)
{
    struct v4l2_control control;
    struct video_stream *vstream;
    int ret;

    vstream = &video_stream[ch];
    if(vstream->enabled <= 0)
    {
        V4L2_ERROR("\n\rUnable to get pwr line freq (channel not enabled)");
        return -1;
    }

    control.id = V4L2_CID_POWER_LINE_FREQUENCY;
    ret = v4l_usr_ioctl(vstream->fd, VIDIOC_G_CTRL, &control);
    if(ret < 0)
    {
        V4L2_ERROR("\n\rUnable to get pwr line freq (VIDIOC_G_CTRL failed)");
        return -1;
    }

    *mode = control.value;

    return 0;
}

int mxuvc_video_set_saturation_mode(video_channel_t ch, saturation_mode_t value)
{
    struct v4l2_control control;
    struct video_stream *vstream;
    int ret;

    vstream = &video_stream[ch];
    if(vstream->enabled <= 0)
    {
        V4L2_ERROR("\n\rUnable to set saturation mode (channel not enabled)");
        return -1;
    }

    control.id = V4L2_CID_PU_XU_SATURATION_MODE;
    control.value = value;

    ret = v4l_usr_ioctl(vstream->fd, VIDIOC_S_CTRL, &control);
    if(ret < 0)
    {
        V4L2_ERROR("\n\rUnable to set saturation mode (VIDIOC_S_CTRL failed)");
        return -1;
    }

    return 0;
}

int mxuvc_video_get_saturation_mode(video_channel_t ch, saturation_mode_t *value)
{
    struct v4l2_control control;
    struct video_stream *vstream;
    int ret;

    vstream = &video_stream[ch];
    if(vstream->enabled <= 0)
    {
        V4L2_ERROR("\n\rUnable to get saturation mode (channel not enabled)");
        return -1;
    }

    control.id = V4L2_CID_PU_XU_SATURATION_MODE;

    ret = v4l_usr_ioctl(vstream->fd, VIDIOC_G_CTRL, &control);
    if(ret < 0)
    {
        V4L2_ERROR("\n\rUnable to get saturation mode (VIDIOC_G_CTRL failed)");
        return -1;
    }

    *value = control.value;

    return 0;
}

int mxuvc_video_set_brightness_mode(video_channel_t ch, brightness_mode_t value)
{
    struct v4l2_control control;
    struct video_stream *vstream;
    int ret;

    vstream = &video_stream[ch];
    if(vstream->enabled <= 0)
    {
        V4L2_ERROR("\n\rUnable to set brightness mode (channel not enabled)");
        return -1;
    }

    control.id = V4L2_CID_PU_XU_BRIGHTNESS_MODE;
    control.value = value;

    ret = v4l_usr_ioctl(vstream->fd, VIDIOC_S_CTRL, &control);
    if(ret < 0)
    {
        V4L2_ERROR("\n\rUnable to set brightness mode (VIDIOC_S_CTRL failed)");
        return -1;
    }

    return 0;
}

int mxuvc_video_get_brightness_mode(video_channel_t ch, brightness_mode_t *value)
{
    struct v4l2_control control;
    struct video_stream *vstream;
    int ret;

    vstream = &video_stream[ch];
    if(vstream->enabled <= 0)
    {
        V4L2_ERROR("\n\rUnable to get brightness mode (channel not enabled)");
        return -1;
    }

    control.id = V4L2_CID_PU_XU_BRIGHTNESS_MODE;

    ret = v4l_usr_ioctl(vstream->fd, VIDIOC_G_CTRL, &control);
    if(ret < 0)
    {
        V4L2_ERROR("\n\rUnable to get brightness mode (VIDIOC_G_CTRL failed)");
        return -1;
    }

    *value = control.value;

    return 0;
}

int mxuvc_video_set_contrast_mode(video_channel_t ch, contrast_mode_t value)
{
    struct v4l2_control control;
    struct video_stream *vstream;
    int ret;

    vstream = &video_stream[ch];
    if(vstream->enabled <= 0)
    {
        V4L2_ERROR("\n\rUnable to set contrast mode (channel not enabled)");
        return -1;
    }

    control.id = V4L2_CID_PU_XU_CONTRAST_MODE;
    control.value = value;

    ret = v4l_usr_ioctl(vstream->fd, VIDIOC_S_CTRL, &control);
    if(ret < 0)
    {
        V4L2_ERROR("\n\rUnable to set contrast mode (VIDIOC_S_CTRL failed)");
        return -1;
    }

    return 0;
}

int mxuvc_video_get_contrast_mode(video_channel_t ch, contrast_mode_t *value)
{
    struct v4l2_control control;
    struct video_stream *vstream;
    int ret;

    vstream = &video_stream[ch];
    if(vstream->enabled <= 0)
    {
        V4L2_ERROR("\n\rUnable to get contrast mode (channel not enabled)");
        return -1;
    }

    control.id = V4L2_CID_PU_XU_CONTRAST_MODE;

    ret = v4l_usr_ioctl(vstream->fd, VIDIOC_G_CTRL, &control);
    if(ret < 0)
    {
        V4L2_ERROR("\n\rUnable to get contrast mode (VIDIOC_G_CTRL failed)");
        return -1;
    }

    *value = control.value;

    return 0;
}

int mxuvc_video_get_channel_info(video_channel_t ch, video_channel_info_t *info)
{
    struct video_stream *vstream;
    uint32_t data = 0;

    vstream = &video_stream[ch];
    if(vstream->enabled <= 0)
    {
        V4L2_ERROR("\n\rUnable to get info (channel not enabled)");
        return -1;
    }

    if ((vstream->cur_vfmt == VID_FORMAT_MUX)) {

        //resolution
        getset_mux_channel_param(ch, (void *)&data, sizeof(uint32_t),
                                 MUX_XU_RESOLUTION, 0);
        info->width  = (uint16_t) ((data>>16) & 0xffff);
        info->height = (uint16_t) (data & 0xffff);

        //format
        uint32_t fourcc;
        getset_mux_channel_param(ch, (void *)&fourcc, sizeof(uint32_t),
                                 MUX_XU_CHTYPE, 0);
        video_format_t format;
        if(fourcc2vidformat(fourcc, &format) < 0)
        {
            V4L2_ERROR("\n\rUnable to get info (Unknown fourcc)");
            return -1;
        }
        info->format = format;

        //frame rate
        getset_mux_channel_param(ch, (void *)&data, sizeof(uint32_t),
                                 MUX_XU_FRAMEINTRVL, 0);
        info->framerate = (uint32_t)FRR(data);

        if (format == VID_FORMAT_H264_RAW ||
                format == VID_FORMAT_H264_TS) {
            //gop
            getset_mux_channel_param(ch, (void *)&data, sizeof(uint32_t),
                                     MUX_XU_GOP_LENGTH, 0);
            info->goplen =  data;

            //h264 profile
            getset_mux_channel_param(ch, (void *)&data, sizeof(uint32_t),
                                     MUX_XU_AVC_PROFILE, 0);
            info->profile = data;

            //bitrate
            getset_mux_channel_param(ch, (void *)&data, sizeof(uint32_t),
                                     MUX_XU_BITRATE, 0);
            info->bitrate = data;
        } else if (format == VID_FORMAT_MJPEG_RAW) {
            //compression_quality
            getset_mux_channel_param(ch, (void *)&data, sizeof(uint32_t),
                                     MUX_XU_COMPRESSION_Q, 0);
            info->compression_quality = data;
        }
    }

    return 0;
}

int mxuvc_audio_set_volume(int vol)
{
    int ret;
    int16_t lvolume;
    AUDIO_CTRL *ctrl;

    if(vol < 0 || vol > 100)
    {
        V4L2_ERROR("\n\rUnable to set MIC volume to %d. Volume must be in the range [0,100]",vol);
        return -1;
    }

    ctrl = get_ctrl_by_id(CTRL_VOLUME);
    lvolume = (ctrl->max-ctrl->min)*vol/100 + ctrl->min;

    if(gMicVolume  != lvolume) {
        gMicVolume = lvolume;
        ret = set_ctrl(CTRL_VOLUME, SET_CUR, &lvolume);
        if(ret < 0)
        {
            V4L2_ERROR("\n\rUnable to set MIC volume to %d",vol);
            return -1;
        }
    }
    else {
        V4L2_PRINTF("\n\rMIC volume is already set to %d\n", vol);
    }

    return 0;
}

int mxuvc_audio_set_mic_mute(int bMute)
{
    int16_t ctrlVal = ((int16_t) bMute) & 0x1;

    int ret = set_ctrl(CTRL_MUTE, SET_CUR, &ctrlVal);
    if(ret < 0)
    {
        V4L2_ERROR("\n\rUnable to set MIC mute to %d",bMute);
        return -1;
    }

    return 0;
}

int mxuvc_audio_set_left_mic_mute(int bMute)
{
    int16_t ctrlVal = ((int16_t) bMute) & 0x1;

    int ret = set_ctrl(CTRL_LEFT_MUTE, SET_CUR, &ctrlVal);
    if(ret < 0)
    {
        V4L2_ERROR("\n\rUnable to set left MIC mute to %d",bMute);
        return -1;
    }

    return 0;
}

int mxuvc_audio_set_right_mic_mute(int bMute)
{
    int16_t ctrlVal = ((int16_t) bMute) & 0x1;

    int ret = set_ctrl(CTRL_RIGHT_MUTE, SET_CUR, &ctrlVal);
    if(ret < 0)
    {
        V4L2_ERROR("\n\rUnable to set right MIC mute to %d",bMute);
        return -1;
    }

    return 0;
}

int mxcam_set_key(const char* keyname, const char* value)
{
    int ret, size;
    unsigned int status;
    char *packet;
    int count = 0;
    uint16_t wValue = 0;

    if(keyname==NULL || value==NULL)
        return MXCAM_ERR_INVALID_PARAM;

    if(mxcam_get_av_stream_status() == STREAM_STATE_RUN){
        V4L2_ERROR("\n\rERR: Camera A/V Streaming is running, Please stop streaming to update flash\n");
        return MXCAM_ERR_FEATURE_NOT_SUPPORTED;
    }

    // form the packet
    size = (int)strlen(keyname) + (int)strlen(value) + 2;
    packet = malloc(size);

    strcpy(packet,keyname);
    strcpy(&packet[strlen(keyname) + 1],value);

    ret = v4l_getset_usb_ctrl(
                    USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_INTERFACE, //bRequestType,
                    SET_EEPROM_KEY_VALUE,
                    wValue,
                    0,
                    (int)strlen(keyname) + (int)strlen(value) + 2,
                    (unsigned char *)packet,
                    EP0_TIMEOUT,
                    1
              );

    free(packet);
    if (ret < 0) {
        V4L2_ERROR("\n\rFailed SET_EEPROM_KEY_VALUE %d\n", ret);
        return ret;
    }

    while(count <= MXCAM_I2C_MAX_RETRIES){
        rtw_usleep_os(1000*500); //typical block erase type will be more then 500ms    
        ret = v4l_getset_usb_ctrl(
                        USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_INTERFACE, //bRequestType,
                        SET_OR_REMOVE_KEY_STATUS,
                        wValue,
                        0,
                        sizeof(unsigned int),
                        (unsigned char *)&status,
                        EP0_TIMEOUT,
                        0
                  );
        if (ret < 0) {
            V4L2_ERROR("\n\rFailed SET_OR_REMOVE_KEY_STATUS %d\n", ret);
            return ret;
        }
        //status = le32toh(status);
        status = rtk_le32_to_cpu(status);
        if((status & MXCAM_STAT_EEPROM_SAVE_IN_PROG) != MXCAM_STAT_EEPROM_SAVE_IN_PROG)
            break;
        count++;
    }

    if(status != MXCAM_OK){
        V4L2_PRINTF("\n\rStatus: %s\n",get_status(status));  
    }

    return MXCAM_OK;
}

int mxcam_get_value(const char* keyname, char** value)
{
    int ret;
    unsigned char cmd_sta;
    unsigned char *lvalue;
    char *data;
    uint16_t wValue = 0;
#define PACKETSIZE 1024

    if(keyname==NULL || value==NULL)
        return -1;

    lvalue = malloc(PACKETSIZE);
    data = malloc(strlen(keyname)+1);
    if (value == NULL || data == NULL) {
        V4L2_ERROR("\n\rUnable to allocate memory");
        free( value ) ;
        free( data  ) ;
        return MXCAM_ERR_NOT_ENOUGH_MEMORY;
    }

    strcpy(data, keyname);
    ret = v4l_getset_usb_ctrl(
                    USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_INTERFACE, //bRequestType,
                    REQ_GET_EEPROM_VALUE,
                    wValue,
                    0,
                    (int)strlen(keyname) + 1,
                    (unsigned char *)data,
                    EP0_TIMEOUT,
                    1
              );
    free(data);
    data = NULL;

    if (ret < 0) {
        free( lvalue ) ;
        V4L2_ERROR("\n\rFailed REQ_GET_EEPROM_VALUE %d\n", ret);
        return ret;
    }

    ret = v4l_getset_usb_ctrl(
                    USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_INTERFACE, //bRequestType,
                    GET_EEPROM_VALUE,
                    wValue,
                    0,
                    PACKETSIZE,
                    lvalue,
                    EP0_TIMEOUT,
                    0
              );
    if (ret < 0) {
        free( lvalue ) ;
        V4L2_ERROR("\n\rFailed GET_EEPROM_VALUE %d\n", ret);
        return ret;
    }
    cmd_sta = *(unsigned char *)value;
    if (cmd_sta == MXCAM_ERR_GETVLAUE_KEY_NOT_FOUND){
        free(lvalue);
        *value = NULL;
        V4L2_ERROR("\n\rKey %s not found\n", keyname);
        return MXCAM_ERR_GETVLAUE_KEY_NOT_FOUND;
    }

    // skip the status byte;
    lvalue += 1;
    *value =(char *)lvalue;

    return MXCAM_OK;
}

int mxcam_free_get_value_mem(char* value_mem)
{
    if(!value_mem )
        return MXCAM_ERR_INVALID_PARAM;

    // add status byte first before the free
    value_mem -= 1 ;
    free(value_mem);

    return MXCAM_OK;
}

int mxcam_reset(void)
{
    unsigned char data[4] = {0};
    uint16_t wLength =4;
    uint16_t wValue = 0;

    int status = v4l_getset_usb_ctrl(
                    USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_INTERFACE, //bRequestType,
                    RESET_BOARD,
                    wValue,
                    0x0,
                    wLength,
                    data,
                    EP0_TIMEOUT,
                    1
              );
    return status;
}

int mxcam_check_ispcfg_syntax(const char *ispcfg)
{
    FIL file;
    int ch0, ch1;
	
    FRESULT res = f_open(&file, ispcfg,  FA_OPEN_EXISTING  | FA_READ);
    if(res){
        return MXCAM_ERR_FILE_NOT_FOUND;
    }

    ch0 = fgetc(file);
    if(ch0 != EOF)
        ch1 = fgetc(file); 

    f_close(&file);
    if(ch0 != 0xfa || ch1 != 0xce){
        return MXCAM_ERR_VEND_ERR_ISPCFG_SYNTAX_ERR;
    }

    return MXCAM_OK;
}

int mxcam_write_ispcfg(const char *ispcfgfile)
{
    int ret = MXCAM_OK, total_size;
    unsigned int status;
    unsigned char *buffer = NULL;
    FIL fd;
    FILINFO stfile;
    int count = 0;
    unsigned char *mbuf = NULL;
    unsigned int bytes_read=0;

    if(mxcam_get_av_stream_status() == STREAM_STATE_RUN){
        V4L2_ERROR("\n\rERR: Camera A/V Streaming is running, Please stop streaming to update flash\n");
        return MXCAM_ERR_FEATURE_NOT_SUPPORTED;
    }

    FRESULT res = f_open(&fd, ispcfgfile,  FA_OPEN_EXISTING  | FA_READ);
    if(res){
        return MXCAM_ERR_FILE_NOT_FOUND;
    }

    res = f_stat (ispcfgfile, &stfile);
    if(res)
        return -1;

    total_size = stfile.fsize;
    if(total_size >= MAX_ISPCFG_FILE_SIZE){ //max supported size is MAX_JSON_FILE_SIZE
        V4L2_ERROR("\n\rERR: Max supported ISP configuration file size %d Bytes\n", MAX_ISPCFG_FILE_SIZE);
        goto out_error;
    }

    buffer = (unsigned char *)malloc(total_size);
    if(buffer == NULL){
        ret = -1;
        goto out_error;
    }
    mbuf = buffer;

    //read the whole file in buffer
    res = f_read(&fd, buffer,total_size, (u32*)&bytes_read);
	
    V4L2_PRINTF("\n\rSending ISP Configuration file %s of size %d Bytes ...\n",ispcfgfile,total_size);

    while(total_size > 0){
        int readl = 0;
        
        if(FWPACKETSIZE > total_size)
            readl = total_size;
        else
            readl = FWPACKETSIZE;
        
        ret = v4l_getset_usb_ctrl(
                        USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_INTERFACE, //bRequestType,
                        WRITE_ISPCFG_FILE,
                        total_size,
                        0x0,
                        readl,
                        buffer,
                        0,
                        1
                  );
        if (ret < 0) {
            V4L2_ERROR("\n\rFailed WRITE_ISPCFG %d\n", ret);
            return ret;
        }
        total_size = total_size - readl;
        buffer = buffer + readl;
    }

    while(count <= MXCAM_I2C_MAX_RETRIES){
        rtw_usleep_os(1000*500); //typical block erase type will be more then 500ms    
        ret = v4l_getset_usb_ctrl(
                        USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_INTERFACE, //bRequestType,
                        SET_OR_REMOVE_KEY_STATUS,
                        0,
                        0,
                        sizeof(unsigned int),
                        (unsigned char *)&status,
                        EP0_TIMEOUT,
                        0
                  );
        if (ret < 0) {
            V4L2_ERROR("\n\rFailed SET_OR_REMOVE_KEY_STATUS %d\n", ret);
            goto out_error;
        }

        if((status & MXCAM_STAT_EEPROM_SAVE_IN_PROG) != MXCAM_STAT_EEPROM_SAVE_IN_PROG)
            break;

        count++;
    }

    if(status != MXCAM_OK){
        V4L2_ERROR("\n\rERROR: %s\n",get_status(status));   
    }

out_error:
    f_close(&fd);
    if(mbuf)free(mbuf);

    return ret;
}

int mxcam_save_eeprom_config(const char *jsonfile)
{
    int ret = MXCAM_OK, total_size, json_size;
    unsigned int status;
    unsigned char *buffer = NULL;
    FILINFO stfile;
    int count = 0;
    unsigned char *mbuf = NULL;
    unsigned int bytes_read = 0;
    FIL fd;

    if(mxcam_get_av_stream_status() == STREAM_STATE_RUN){
        V4L2_ERROR("\n\rERR: Camera A/V Streaming is running, Please stop streaming to update flash\n");
        return MXCAM_ERR_FEATURE_NOT_SUPPORTED;
    }

    FRESULT res = f_open(&fd, jsonfile,  FA_OPEN_EXISTING  | FA_READ);
    if(res){
        return MXCAM_ERR_FILE_NOT_FOUND;
    }

    res = f_stat (jsonfile, &stfile);
    if(res)
        return -1;

    total_size = stfile.fsize;
    if(total_size >= MAX_JSON_FILE_SIZE){ //max supported size is MAX_JSON_FILE_SIZE
        V4L2_ERROR("\n\rERR: Max support json file size %d Bytes\n", MAX_JSON_FILE_SIZE);
        goto out_error;
    }

    buffer = (unsigned char *)malloc(total_size+1);
    if(buffer == NULL){
        ret = -1;
        goto out_error;
    }
    mbuf = buffer;

    //read the whole file in buffer and Minify
    res = f_read(&fd, buffer,total_size, (u32*)&bytes_read);
    buffer[total_size] = 0;
    cJSON_Minify((char *)buffer);
    json_size = total_size = (int)strlen((const char *)buffer);
    json_size += 1;
    total_size += 1;
    V4L2_PRINTF("\n\rSending Minified Configuration file %s of size %d Bytes ...\n",jsonfile,total_size);

    while(total_size > 0){
        int readl = 0;
        
        if(FWPACKETSIZE > total_size)
            readl = total_size;
        else
            readl = FWPACKETSIZE;
        
        ret = v4l_getset_usb_ctrl(
                        USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_INTERFACE, //bRequestType,
                        SEND_JSON_FILE,
                        json_size,
                        0x0,
                        readl,
                        buffer,
                        0,
                        1
                  );
        if (ret < 0) {
            V4L2_ERROR("\n\rFailed SEND_JSON %d\n", ret);
            goto out_error;
        }
        total_size = total_size - readl;
        buffer = buffer + readl;
    }

    while(count <= MXCAM_I2C_MAX_RETRIES){
        rtw_usleep_os(1000*500); //typical block erase type will be more then 500ms    
        ret = v4l_getset_usb_ctrl(
                        USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_INTERFACE, //bRequestType,
                        SET_OR_REMOVE_KEY_STATUS,
                        0,
                        0,
                        sizeof(unsigned int),
                        (unsigned char *)&status,
                        EP0_TIMEOUT,
                        0
                  );
        if (ret < 0) {
            V4L2_ERROR("\n\rFailed SET_OR_REMOVE_KEY_STATUS %d\n", ret);
            goto out_error;
        }
        if((status & MXCAM_STAT_EEPROM_SAVE_IN_PROG) != MXCAM_STAT_EEPROM_SAVE_IN_PROG)
            break;

        count++;
    }
    if(status != MXCAM_OK){
        V4L2_ERROR("\n\rERROR: %s\n",get_status(status));   
    }

out_error:
    f_close(&fd);
    if(mbuf)free(mbuf);

    return ret;
}

int mxcam_qcc_read(uint16_t bid, uint16_t addr, uint16_t length, uint32_t *value)
{
    int ret;
    int mask;

    switch (length) {
    case 1:
        mask = 0xFF;
        break;
    case 2:
        mask = 0xFFFF;
        break;
    case 4:
        mask = 0xFFFFFFFF;
        break;
    default:
        return MXCAM_ERR_INVALID_PARAM;
    }

    ret = v4l_getset_usb_ctrl(
                    USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_INTERFACE, //bRequestType,
                    QCC_READ,
                    bid,
                    addr,
                    length,
                    (unsigned char*)value,
                    EP0_TIMEOUT,
                    0
              );
    if (ret < 0) {
        V4L2_ERROR("\n\rFailed QCC_READ %d\n", ret);
        return ret;
    }

    *value &= mask;

    return MXCAM_OK;
}

int mxcam_read_flash_image_header(image_header_t *header, IMG_HDR_TYPE hdr_type)
{
    int ret;
    uint8_t bRequest;

    if(!header)
        return MXCAM_ERR_INVALID_PARAM;

    if (hdr_type == SNOR_FW_HEADER)
        bRequest = GET_SNOR_IMG_HEADER;
    else if (hdr_type == RUNNING_FW_HEADER)
        bRequest = GET_IMG_HEADER;
    else if (hdr_type == BOOTLOADER_HEADER)
        bRequest= GET_BOOTLOADER_HEADER;
    else 
        return MXCAM_ERR_INVALID_PARAM;

    ret = v4l_getset_usb_ctrl(
                    USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_INTERFACE, //bRequestType,
                    bRequest,
                    0,
                    0,
                    (uint16_t) sizeof(image_header_t),
                    (unsigned char *) header,
                    EP0_TIMEOUT,
                    0
              );

    if (ret < 0) {
        V4L2_ERROR("\n\rFailed GET_SNOR_IMG_HEADER: %d\n", ret);
        return ret;
    }

    return MXCAM_OK;
}

int mxcam_rw_gpio (int gpio_no, int value, int gpio_write, int *status)
{
    int ret;
    uint8_t cmd;
    int count = 0;
    char data[4];

    if(gpio_write)
        cmd = GPIO_WRITE;
    else
        cmd = GPIO_READ;

    ret = v4l_getset_usb_ctrl(
                    USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_INTERFACE, //bRequestType,
                    cmd,
                    (uint16_t)gpio_no,
                    value,
                    4,
                    (unsigned char *)data,
                    1000,
                    1
              );

    if (ret < 0) {
        V4L2_ERROR("\n\rFailed to program GPIO %d\n", ret);
        return ret;
    }

    while(count <= MXCAM_I2C_MAX_RETRIES){
        ret = v4l_getset_usb_ctrl(
                        USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_INTERFACE, //bRequestType,
                        GPIO_RW_STATUS,
                        0,
                        0,
                        4,
                        (unsigned char *) data,
                        1000, //1sec timeout 
                        0
                  );

        if (ret < 0) {
            V4L2_ERROR("\n\rFailed to get GPIO_RW_STATUS %d\n", ret);
            return ret;
        }
        if((data[0] & 0xff) != 0xff)
            break;
        count++;
        rtw_usleep_os(1000*10);
    }       

    *status = data[0];  

    return ret;   
}


static AUDIO_CTRL* get_ctrl_by_id(ctrl_id_t id)
{
    AUDIO_CTRL *control = uac_controls;

    while(control->id != CTRL_NONE) {
        if(control->id == id)
            return control;
        control++;
    }

    V4L2_ERROR( "\n\rUnexpected error: no match for ctrl_id %i in get_ctrl_by_id()\n", (int)id);

    return NULL;        
}

static int set_ctrl(ctrl_id_t id, uint8_t bRequest, void *val)
{
    int ret;
    AUDIO_CTRL *ctrl;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
    unsigned char *data = (unsigned char *) val;

    ctrl = get_ctrl_by_id(id);
    if(ctrl == NULL)
        return -1;

    wValue = (ctrl->cs << 8) + ctrl->cn;
    wLength = ctrl->len;
    wIndex = AUDIO_CTRL_INTERACE + (ctrl->unit << 8);

    ret = v4l_getset_usb_ctrl(
                    USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE, //bRequestType,
                    bRequest,
                    wValue,
                    wIndex,
                    wLength,
                    data,
                    EP0_TIMEOUT,
                    1
              );

    return ret;
}

static int mxcam_get_av_stream_status(void)
{
    int ret = 0;
    uint32_t state;

    ret = v4l_getset_usb_ctrl(
                    USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_INTERFACE, //bRequestType,
                    GET_AV_STREAMING_STATE,
                    0,
                    0,
                    4,
                    (unsigned char *)&state,
                    1000*20,  //20sec timeout 
                    0
              );

    if (ret < 0) {
        V4L2_ERROR("\n\rFailed in GET_AV_STREAMING_STATE %d\n", ret);
        return ret;
    }

    return state;
}

static const char * get_status(const int status)
{
    switch (status) {
        case MXCAM_OK:
            return "No error - operation complete";
        // Staus    
        case MXCAM_STAT_EEPROM_FW_UPGRADE_START:
            return "Started EEPROM FW Upgrade";
        case MXCAM_STAT_EEPROM_FW_UPGRADE_COMPLETE:
            return "Completed  EEPROM FW Upgrade";
        case MXCAM_STAT_SNOR_FW_UPGRADE_START:
            return "Started SNOR FW Upgrade";
        case MXCAM_STAT_SNOR_FW_UPGRADE_COMPLETE:
            return "Completed SNOR FW Upgrade";
        case MXCAM_STAT_FW_UPGRADE_COMPLETE:
            return "Completed FW Upgrade";
        case MXCAM_STAT_EEPROM_ERASE_IN_PROG:
            return "EEPROM Erase in progress";
        case MXCAM_STAT_EEPROM_SAVE_IN_PROG:
            return "EEPROM config save in progress";
        //Errors
        case MXCAM_ERR_FW_IMAGE_CORRUPTED:
            return "FW Image is corrupted";
        case MXCAM_ERR_FW_SNOR_FAILED:
            return "SNOR FW upgrade failed";
        case MXCAM_ERR_FW_UNSUPPORTED_FLASH_MEMORY:
            return "Unsupported Flash memory";
        case MXCAM_ERR_ERASE_SIZE:
            return "Erase size execeds MAX_MXCAM_SIZE"; 
        case MXCAM_ERR_ERASE_UNKNOWN_AREA:
            return "Unknown area to erase";
        case MXCAM_ERR_SAVE_UNKNOWN_AREA:
            return "Unknown area to save";
        case MXCAM_ERR_SETKEY_OVER_FLOW_NO_MEM:
            return "Not enough memory to save new key on memory";
        case MXCAM_ERR_SETKEY_UNKNOWN_AREA:
            return "Unkown area to set key";
        case MXCAM_ERR_REMOVE_KEY_UNKNOWN_AREA:
            return "Unkown area to remove key";
        case MXCAM_ERR_GETVALUE_UNKNOWN_AREA:
            return "Unkown area to get key";
        case MXCAM_ERR_GETVLAUE_KEY_NOT_FOUND:
            return "Key not found";
        case MXCAM_ERR_TCW_FLASH_READ:
            return "Failed to read TCW from camera";
        case MXCAM_ERR_TCW_FLASH_WRITE:
            return "Failed to write TCW to camera";
        case MXCAM_ERR_MEMORY_ALLOC:
            return "Failed to allocate memory on camera";
        //case MXCAM_ERR_MXCAM_AREA_NOT_INIT:
        //  return "Vendor area is not initialized";
        case MXCAM_ERR_VEND_ERR_JSON_SYNTAX_ERR:
            return "Configuration file syntax is wrong";
        case MXCAM_ERR_VEND_ERR_ISPCFG_SYNTAX_ERR:
            return "ISP configuration file syntax is wrong";
        case MXCAM_ERR_SETKEY_UNSUPPORTED:
            return "json is provided while booting so set/remove key is not supported";
        default:
            return "Unknown error";
    }
}
#endif

static void cJSON_Minify(char *json)
{
    char *into=json;
    while (*json)
    {
        if (*json==' ') json++;
        else if (*json=='\t') json++;	// Whitespace characters.
        else if (*json=='\r') json++;
        else if (*json=='\n') json++;
        else if (*json=='/' && json[1]=='/')  while (*json && *json!='\n') json++;	// double-slash comments, to end of line.
        else if (*json=='/' && json[1]=='*') {while (*json && !(*json=='*' && json[1]=='/')) json++;json+=2;}	// multiline comments.
        else if (*json=='\"'){*into++=*json++;while (*json && *json!='\"'){if (*json=='\\') *into++=*json++;*into++=*json++;}*into++=*json++;} // string literals, which are \" sensitive.
        else *into++=*json++;			// All other characters.
    }
    *into=0;	// and null-terminate.
}


static void map_control(int fd, void *mapping)
{
    int ret;
    struct uvc_xu_control_mapping *_mapping = (struct uvc_xu_control_mapping *)mapping;

    ret = v4l_usr_ioctl(fd, UVCIOC_CTRL_MAP, _mapping);
    if(ret < 0)
    {
        V4L2_PRINTF("\n\rUnable to map control id = 0x%x", _mapping->id);
    }    
    return;
}

static void map_xu(int fd, struct uvc_xu_data *data)
{
    struct uvc_xu_control_mapping mapping;

    mapping.id = data->id;
    memcpy(mapping.name, data->name, sizeof(data->name));
    memcpy(mapping.entity, data->entity, sizeof(data->entity));
    mapping.selector = data->selector;
    mapping.size = data->size;
    mapping.offset = data->offset;
    mapping.v4l2_type = V4L2_CTRL_TYPE_INTEGER;
    mapping.data_type = UVC_CTRL_DATA_TYPE_SIGNED;

    map_control(fd, &mapping);
}

static int init_ctrl(video_channel_t ch)
{
    int i;
    struct video_stream *vstream;

    vstream = &video_stream[ch];
    if(vstream->enabled <= 0)
    {
        V4L2_ERROR("\n\rUnable to initialize the controls(channel not enabled)");
        return 0;
    }

    for (i = 0; i < (int)(sizeof(mux_xu_data)/sizeof(mux_xu_data[0])); i++) {
        map_xu(vstream->fd, &mux_xu_data[i]);
    }
 
    return 0;
}

static int geo_init_mmap(video_channel_t ch)
{
    int ret;      
    struct v4l2_requestbuffers req;
    struct video_stream *vstream;
    vstream = &video_stream[ch];
    if(vstream->enabled <= 0)
    {
        V4L2_ERROR("\n\rUnable to initialize the memory buffers (channel not enabled)");
        return -1;
    }

    if (is_mux_stream(vstream) && mux_stream.started) {
        //vstream->buffers = mux_stream.buffers;
        //vstream->n_buffers = mux_stream.n_buffers;
        return 0;
    }

    CLEAR(req);
    req.count = 6;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if((ret = v4l_usr_ioctl(vstream->fd, VIDIOC_REQBUFS, &req)) < 0)
    {
        V4L2_ERROR("\n\rcannot request v4l2 buffers (VIDIOC_REQBUFS)");
        return ret;
    }
    
    mux_stream.n_buffers = req.count;
    vstream->n_buffers = req.count;

    return 0;
}

static int geo_uninit_mmap(video_channel_t ch)
{
    int ret;
    struct video_stream *vstream;
    struct v4l2_requestbuffers req;
    vstream = &video_stream[ch];
    if(vstream->enabled <= 0)
    {
        V4L2_ERROR("\n\rUnable to get resolution(channel not enabled)");
        return -1;
    }

    if (is_mux_stream(vstream) && mux_stream.ref_count)
        return 0;

    CLEAR(req);

    req.count = 0;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if((ret = v4l_usr_ioctl(vstream->fd, VIDIOC_REQBUFS, &req)) < 0)
    {
        V4L2_ERROR("\n\rcannot request v4l2 buffers (VIDIOC_REQBUFS)");
        return ret;
    }
    
    mux_stream.n_buffers = req.count;
    vstream->n_buffers = req.count;
    return 0;
}


int channel_deinit(video_channel_t ch)
{
    struct video_stream *vstream;
    vstream = &video_stream[ch];

    if (vstream->started)
        mxuvc_video_stop(ch);
    vstream->enabled = 0;

    return 0;
}

int channel_init(video_channel_t ch)
{
    int ret;
    struct video_stream *vstream;
    vstream = &video_stream[ch];
#if 0
    if (ipcam_mode && (ch < mux_channel_count))
        vstream->fd = mux_stream.fd;
    else
        vstream->fd = open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);

    CHECK_ERROR(-1 == vstream->fd, -1, "Cannot open '%s': %d, %s.",
                dev_name, errno, strerror(errno));
#endif
    if(ch < mux_channel_count)
        vstream->fd = mux_stream.fd;
    vstream->enabled = 1;
    vstream->started = 0;

    /* Init device */
    struct v4l2_capability cap;
    
    if((ret = v4l_usr_ioctl(vstream->fd, VIDIOC_QUERYCAP, &cap)) < 0)
    {
        if(ret == -EINVAL)
            V4L2_ERROR("\n\rno v4l2 device");
        else
            V4L2_ERROR("\n\rERROR VIDIOC_QUERYCAP");
        return ret;
    }

    /* Get/Use default video settings */
    CLEAR(vstream->fmt);
    vstream->fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if((ret = v4l_usr_ioctl(vstream->fd, VIDIOC_G_FMT, &vstream->fmt)) < 0)
    {
        V4L2_ERROR("\n\rcannot get default video format!");
        return ret;
    }
    ret = fourcc2vidformat(vstream->fmt.fmt.pix.pixelformat, &vstream->cur_vfmt);
    if(ret < 0)
        return ret;
    
    /* Buggy driver paranoia. */
    unsigned int min;
    min = vstream->fmt.fmt.pix.width * 2;
    if (vstream->fmt.fmt.pix.bytesperline < min)
        vstream->fmt.fmt.pix.bytesperline = min;
    min = vstream->fmt.fmt.pix.bytesperline * vstream->fmt.fmt.pix.height;
    if (vstream->fmt.fmt.pix.sizeimage < min)
        vstream->fmt.fmt.pix.sizeimage = min;
#if 1
    /* Map controls */
    ret = init_ctrl(ch);
    if (ret < 0)
        return -1;
#endif
    //vstream->secs_supported = 0;
//edit by Ian -- this is optional so we disable it first
#if 0
    /* Get and cache the initial video settings */
    vstream->cur_width = 0;
    vstream->cur_height = 0;
    vstream->cur_framerate = 0;
    vstream->cur_bitrate = 0;
    mxuvc_video_get_resolution(ch, &(vstream->cur_width), &(vstream->cur_height));
    mxuvc_video_get_framerate(ch, &(vstream->cur_framerate));
    switch (vstream->cur_vfmt) {
    case VID_FORMAT_H264_RAW:
    case VID_FORMAT_H264_TS:
    case VID_FORMAT_H264_AAC_TS:
        mxuvc_video_get_bitrate(ch, &(vstream->cur_bitrate));
        break;
    default:
        break;
    }
#endif
    return 0;
}

int mxuvc_video_start(video_channel_t ch)
{
    int i, ret;
    enum v4l2_buf_type type;
    struct v4l2_buffer buf;

    struct video_stream *vstream;
    vstream = &video_stream[ch];

    if(vstream->enabled <= 0)
    {
        V4L2_ERROR("\n\rUnable to start the video(channel not enabled)");
        return -1;
    }

    if(vstream->started)
    {
        V4L2_ERROR("\n\rthe video is already started");
        return 0;
    }
    Mutex_Lock(&lock);
    //start xu
    if (is_mux_stream(vstream)) {
        uint32_t data = ch;
        getset_mux_channel_param(/*CH1*/ch, (void *)&data, sizeof(uint32_t),
                                 MUX_XU_START_CHANNEL, 1);
    }
    
    if (!is_mux_stream(vstream) ||
            (is_mux_stream(vstream) && !mux_stream.started)) {

        for (i = 0; i < (int)mux_stream.n_buffers; ++i) {
            CLEAR(buf);
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            buf.index = i;
            buf.field = V4L2_FIELD_ANY;
            buf.timestamp = rtw_get_current_time();

            if((ret = v4l_usr_ioctl(vstream->fd, VIDIOC_QBUF, &buf)) < 0)
            {
                V4L2_ERROR("\n\rqueue buffer failed!");
                Mutex_Unlock(&lock);
                return ret;
            }
        }
        
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if((ret = v4l_usr_ioctl(vstream->fd, VIDIOC_STREAMON, &type)) < 0)
        {
                V4L2_ERROR("\n\rStart streaming failed!");
                Mutex_Unlock(&lock);
                return ret;
        }
       
        rtw_mdelay_os(1000);
        vstream->frame_count = 0;
        vstream->started = 1;

        if (is_mux_stream(vstream)) {
            mux_stream.started = 1;
            mux_stream.ref_count++;
        }
    } else if (is_mux_stream(vstream) && mux_stream.started) {
        vstream->frame_count = 0;
        vstream->started = 1;
        mux_stream.ref_count++;
    }
    Mutex_Unlock(&lock);
    return 0;
}

int mxuvc_video_stop(video_channel_t ch)
{
    int is_active = 0;

    struct video_stream *vstream;
    vstream = &video_stream[ch];

    if(vstream->enabled <= 0)
    {
        V4L2_ERROR("\n\rUnable to stop the video(channel not enabled)");
        return 0;
    }
        
    if(vstream->started==0)
    {
        V4L2_ERROR("\n\rthe video has not been started");
        return 0;
    }

    //(void)pthread_mutex_lock(&lock);
    Mutex_Lock(&lock);
    if (is_mux_stream(vstream)) {
        //stop xu -- xu never successfully inited so skip this so far

        uint32_t data = ch;
        getset_mux_channel_param(CH1, (void *)&data, sizeof(uint32_t),
                                 MUX_XU_STOP_CHANNEL, 1);

        mux_stream.ref_count--;
        vstream->started = 0;
        if (mux_stream.ref_count){
            Mutex_Unlock(&lock);
            return 0;
	}
    }

    int ret;
    enum v4l2_buf_type type;

    vstream->started = 0;
    if (is_mux_stream(vstream))
        mux_stream.started = 0;

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v4l_usr_ioctl(vstream->fd, VIDIOC_STREAMOFF, &type);

    //check if mux or raw channel is active
    if((mux_stream.started) ||
       (video_stream[mux_channel_count].started))
            is_active = 1;	

    Mutex_Unlock(&lock);
    return 0;
}

int mxuvc_video_deinit()
{
    uint32_t channel;
    uint32_t ch_count = 0;

    if(videoInit)  {

        int ret;
        
        ret = geo_uninit_mmap(CH1);
        if(ret < 0){
            V4L2_ERROR("\n\rCannot free the mapped memory");
            return -1;	
        }
        
        if((ret = mxuvc_video_get_channel_count(&ch_count)) < 0)
            V4L2_ERROR("\n\r Unable to get channel count");
        else{
            for (channel = CH1 ; channel < ch_count ; channel++) {
                channel_deinit(channel);
            }
        }
        if (video_stream != NULL)
            free(video_stream);

        V4L2_ERROR("\n\rThe video has been successfully uninitialized");
        Mutex_Free(&lock);
        videoInit=0;
    }
    v4l_dev_release();
    mux_stream.fd = -1; //???
    return 0;
}

int mxuvc_video_init()
{
        int ret;
        uint32_t channel;
        uint32_t ch_count = 0;

        /* camera in ipcam mode */
        video_stream = (struct video_stream*)malloc(sizeof(struct video_stream) * NUM_IPCAM_VID_CHANNELS);
        if(video_stream == NULL)
        {
            V4L2_ERROR("\n\rallocate video_stream failed!");
            return -ENOMEM;
        }
        memset((void *)video_stream, 0, sizeof(struct video_stream)* NUM_IPCAM_VID_CHANNELS);

        mux_stream.ref_count = 0;

/*open video device for ioctls to work*/
        mux_stream.fd = v4l_dev_open();
        if(mux_stream.fd < 0)
        {
            free(video_stream);
            V4L2_ERROR("\n\rcannot open video device");
            return -EINVAL;
        }
        ret = channel_init(CH1);
        if(ret < 0) {
            free(video_stream);
            return ret;
        }
        ret = mxuvc_video_get_channel_count(&ch_count);

        mux_channel_count = ch_count - 1; //drop the Raw channel from the mux channel count
        if(mux_channel_count > NUM_MUX_VID_CHANNELS) {
            channel_deinit(CH1);
            free(video_stream);
            V4L2_ERROR("\n\rInvalid Mux channel count received");
            return -EINVAL;
        }

        for (channel = CH2 ; channel < ch_count - 1; channel++)
        {
            ret = channel_init(channel);
            if(ret < 0) {
                video_channel_t ch;
                //Deinitialize channels already initialized
                channel_deinit(CH1);
                for(ch = CH2; ch < channel; ch++)
                    channel_deinit(ch);
                free(video_stream);
                //close(mux_stream.fd);
                V4L2_ERROR("\n\runable to initialize local video channel");
            }
        }
//no need to initialize raw data channel        
#if 0
        ret = channel_init(channel);
        if(ret < 0) {
            //Deinitialize channels already initialized
            channel_deinit(CH1);
            for (channel = CH2 ; channel < ch_count - 1; channel++)
                channel_deinit(channel);
            free(video_stream);
            //close(mux_stream.fd);
            ERROR(-1, "Unable to initialize raw video channel.");
        }
#endif   
        Mutex_Init(&lock);

        /* Allocate/map memory for streaming */
        ret = geo_init_mmap(CH1);
        if (ret < 0){
            V4L2_ERROR("\n\r init mmap failed!");
            return -1;
        }
        
        videoInit=1;
        return 0;
}

static const int SampleFreq[12] = {
    96000, 88200, 64000, 48000, 44100, 32000,
    24000, 22050, 16000, 12000, 11025, 8000
};

void get_adts_header(int size, unsigned char *adtsHeader)
{
	int i;
	int samplingFreqIndex;

	// clear the adts header
	for (i = 0; i < 7; i++)
	{
		adtsHeader[i] = 0;
	}

	// bits 0-11 are sync
	adtsHeader[0] = 0xff;
	adtsHeader[1] = 0xf0;

	// bit 12 is mpeg4 which means clear the bit

	// bits 13-14 is layer, always 00

	// bit 15 is protection absent (no crc)
	adtsHeader[1] |= 0x1;

	// bits 16-17 is profile which is 01 for AAC-LC
	adtsHeader[2] = 0x40;

	// bits 18-21 is sampling frequency index
	for (i=0; i<12; i++)
	{
		if ( SampleFreq[i] == 16000 )
		{
			samplingFreqIndex =  i;
			break;
		}
	}


	adtsHeader[2] |= (samplingFreqIndex << 2);

	// bit 22 is private

	// bit 23-25 is channel config.  However since we are mono or stereo
	// bit 23 is always zero
	adtsHeader[3] =2 << 6;

	// bits 26-27 are original/home and zeroed

	// bits 28-29 are copyright bits and zeroed

	// bits 30-42 is sample length including header len.  First we get qbox length,
	// then sample length and then add header length


	// adjust for headers
	int frameSize = size + 7;

	// get upper 2 bits of 13 bit length and move them to lower 2 bits
	adtsHeader[3] |= (frameSize & 0x1800) >> 11;

	// get middle 8 bits of length
	adtsHeader[4] = (frameSize & 0x7f8) >> 3;

	// get lower 3 bits of length and put as 3 msb
	adtsHeader[5] = (frameSize & 0x7) << 5;

	// bits 43-53 is buffer fulless but we use vbr so 0x7f
	adtsHeader[5] |= 0x1f;
	adtsHeader[6] = 0xfc;

	// bits 54-55 are # of rdb in frame which is always 1 so we write 1 - 1 = 0
	// which means do not write

}

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
#if 1
/* entry for GEO gc6500 *///video & audio multiplex
void spec_v4l2_probe(void *param)
{
	u32 timer1, timer2;
	int ret = 0;
	int ch_count = 0;

	printf("\n\rv4l2_probe -> Available heap 0x%x\n", xPortGetFreeHeapSize());         
	/* Init mxuvc video */
	ret = mxuvc_video_init();
	if(ret < 0)
	{
	V4L2_ERROR("\n\rmux uvc video init fail!");
		return;
	}
	printf("\n\r -> Available heap 0x%x\n", xPortGetFreeHeapSize());  
	//get channel count of MUX channel
	ret = mxuvc_video_get_channel_count(&ch_count);
	if(ret < 0)
	{
		V4L2_ERROR("\n\rget channel count failed");
		goto exit;
	}
	V4L2_ERROR("Total Channel count: %d\n",ch_count);
	//remove raw channel from count
	ch_count = ch_count - 1;
	printf("\n"); 
		
	stream_state = STREAMING_READY;
	while(/*stream_state != STREAMING_OFF*/1)	vTaskDelay(1000);
#if 0	
	if(mxuvc_video_stop(CH1) < 0)
		goto error;  
	if(mxuvc_video_stop(CH2) < 0)
		goto error;
	if(mxuvc_video_stop(CH3) < 0)
		goto error;	
#endif
error:      
exit:
        mxuvc_video_deinit();
        printf("\n\rv4l2_probe <- Available heap 0x%x\n", xPortGetFreeHeapSize());        
}
#endif

