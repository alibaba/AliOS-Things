/*******************************************************************************
**  Name:       ucodec.h
**
**  Description:
**
**  This file contains codec definitions from Widcomm's Universal Embedded
**  Drivers API.
**
*******************************************************************************/

#ifndef UCODEC_H
#define UCODEC_H

#include "bt_target.h"
#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
** Codec APIs
*******************************************************************************/

/**** Codec IDs ****/
#define UCODEC_ID_1             0
#define UCODEC_ID_2             1
#define UCODEC_ID_3             2
#define UCODEC_ID_4             3
#define UCODEC_NUMBER           4

typedef UINT8 tUCODEC_ID;

/**** Status ****/
#define UCODEC_SUCCESS              0x00
#define UCODEC_TX_DONE              0x01
#define UCODEC_RX_READY             0x02
#define UCODEC_FLOW_CTRL_ON         0x03
#define UCODEC_FLOW_CTRL_OFF        0x04
#define UCODEC_OVERFLOW             0x05
#define UCODEC_UNSUPORTED_CNF       0x06
#define UCODEC_WRONG_PARAM          0x07
#define UCODEC_NOT_CONFIGURED       0x08
#define UCODEC_OUT_OF_MEMORY        0x09
#define UCODEC_GENERIC_ERROR        0x0a
#define UCODEC_RECOVERABLE_ERROR    0x0b
#define UCODEC_UNRECOVERABLE_ERROR  0x0c
#define UCODEC_LOW_LEVEL_DRIVER_ERROR   (0x0d)

typedef UINT8 tUCODEC_STATUS;

/**** Media type ****/
#define UCODEC_MEDIA_TYPE_AUDIO 0
#define UCODEC_MEDIA_TYPE_VIDEO 1
#define UCODEC_MEDIA_TYPE_MULTI 2

typedef UINT8 tUCODEC_MEDIA_TYPE;

/**** Audio Codec type ****/
#define UCODEC_AUDIO_SBC        0
#define UCODEC_AUDIO_M12_LAYER1 1   /* layer1 (mp1) */
#define UCODEC_AUDIO_M12_LAYER2 2   /* layer2 (mp2) */
#define UCODEC_AUDIO_M12_LAYER3 3   /* layer3 (mp3) */
#define UCODEC_AUDIO_M24_2LC    4   /* MPEG-2 AAC LC */
#define UCODEC_AUDIO_M24_4LC    5   /* MPEG-4 AAC LC */
#define UCODEC_AUDIO_M24_4LTP   6   /* MPEG-4 AAC LTP */
#define UCODEC_AUDIO_M24_4S     7   /* MPEG-4 AAC scalable */
#define UCODEC_AUDIO_VOLUME     8   /* Volume settings */
#define UCODEC_AUDIO_BALANCE    9   /* Balance settings */

typedef UINT8 tUCODEC_AUDIO_FEAT_TYPE;

/**** Video Codec type -> TODO ****/
/*TBD*/

typedef UINT8 tUCODEC_VIDEO_FEAT_TYPE;

/**** SBC sample frequency ****/
#define UCODEC_SBC_SMP_FREQ_16    0  /* 16 */
#define UCODEC_SBC_SMP_FREQ_32    1  /* 23 */
#define UCODEC_SBC_SMP_FREQ_44    2  /* 44.1 */
#define UCODEC_SBC_SMP_FREQ_48    3  /* 48 */

typedef UINT8 tUCODEC_SBC_SMP_FREQ;

/**** SBC sample frequency ****/
#define UCODEC_SBC_SUBBAND_4    4
#define UCODEC_SBC_SUBBAND_8    8

typedef UINT8 tUCODEC_SBC_SUBBAND;
/**** Allocation method ****/
#define UCODEC_SBC_ALLOC_MD_S   0   /* SNR */
#define UCODEC_SBC_ALLOC_MD_L   1   /* loundess */

typedef UINT8 tUCODEC_SBC_ALLOC_MD;

/**** MPEG sample frequency ****/
#define UCODEC_M12_SMP_FREQ_16    0   /* 16 */
#define UCODEC_M12_SMP_FREQ_22    1   /* 22 */
#define UCODEC_M12_SMP_FREQ_24    2  /* 24 */
#define UCODEC_M12_SMP_FREQ_32    3  /* 32 */
#define UCODEC_M12_SMP_FREQ_44    4  /* 44 */
#define UCODEC_M12_SMP_FREQ_48    5  /* 48 */

typedef UINT8 tUCODEC_M12_SMP_FREQ;

/**** Channel mode ****/
#define UCODEC_CHN_MONO            0
#define UCODEC_CHN_DUAL            1
#define UCODEC_CHN_STEREO          2
#define UCODEC_CHN_JOINT_STEREO    3

typedef UINT8 tUCODEC_CH_MODE;
/**** Audio Codec type ****/
#define UCODEC_M24_SMP_FREQ_8     0   /*  8 */
#define UCODEC_M24_SMP_FREQ_11    1   /* 11 */
#define UCODEC_M24_SMP_FREQ_12    2   /* 12 */
#define UCODEC_M24_SMP_FREQ_16    3   /* 16 */
#define UCODEC_M24_SMP_FREQ_22    4   /* 22.05 */
#define UCODEC_M24_SMP_FREQ_24    5   /* 24 */
#define UCODEC_M24_SMP_FREQ_32    6   /* 32 */
#define UCODEC_M24_SMP_FREQ_44    7   /* 44.1 */
#define UCODEC_M24_SMP_FREQ_48    8   /* 48 */
#define UCODEC_M24_SMP_FREQ_64    9   /* 64 */
#define UCODEC_M24_SMP_FREQ_88    10  /* 88 */
#define UCODEC_M24_SMP_FREQ_96    11  /* 96 */

typedef UINT8 tUCODEC_M24_SMP_FREQ;

/**** Codec configuration structure ****/
typedef struct tUCODEC_CNF_SBC_TAG {
    tUCODEC_SBC_SMP_FREQ    SampleFreq;
    tUCODEC_CH_MODE         ChannelMode;
    UINT16                  Offset;     /* GKI buffer based offset for UCODEC_ReadBuf */
    UINT16                  MtuSize;    /* Max buffer len for UCODEC_ReadBuf*/
    UINT8                   PoolId;     /* GKI pool ID for UCODEC_ReadBuf */
    UINT8
    NumBlock;   /* Number of block in block unit : 4 blocks 8 blocks 12 blocks 16 blocks are the possible value */
    UINT8                   Subband;
    tUCODEC_SBC_ALLOC_MD    AllocMthd;
    UINT8                   MinBitPool;
    UINT8                   MaxBitPool;
} tUCODEC_CNF_SBC;

typedef struct tUCODEC_CNF_M12_TAG {
    tUCODEC_CH_MODE         ChannelMode; /* Mono, Dual, stereo, joint stereo */
    tUCODEC_M12_SMP_FREQ    SampleFreq; /* Sample freq: 16, 22, 24, 32, 44, 48 */
    UINT16                  BitRate;    /* Bit rate in bit per sec */
    UINT16                  Offset;     /* GKI buffer based offset for UCODEC_ReadBuf */
    UINT16                  MtuSize;    /* Max buffer len for UCODEC_ReadBuf*/
    UINT8                   PoolId;     /* GKI pool ID for UCODEC_ReadBuf */
    BOOLEAN                 VBR;        /* Variable Bit Rate */
    BOOLEAN                 CRC_On;     /* CRC error detection */
    BOOLEAN                 MPF;        /* Media payload format */
} tUCODEC_CNF_M12;

typedef struct tUCODEC_CNF_M24_TAG {
    tUCODEC_M24_SMP_FREQ    SampleFreq; /* Sample freq: 8, 11, 12, 16, 22.05, 24, 32, 44.1, 48, 64, 88, 96 */
    UINT32                  BitRate;    /* Bit rate */
    UINT16                  Offset;     /* GKI buffer based offset for UCODEC_ReadBuf */
    UINT16                  MtuSize;    /* Max buffer len for UCODEC_ReadBuf*/
    UINT8                   PoolId;     /* GKI pool ID for UCODEC_ReadBuf */
    UINT8                   Chanels;    /* 1 or 2 chanels */
} tUCODEC_CNF_M24;


typedef union tUCODEC_CODEC_TYPE_TAG {
    tUCODEC_AUDIO_FEAT_TYPE AudioType;
    tUCODEC_VIDEO_FEAT_TYPE VideoType;
} tUCODEC_CODEC_TYPE;

typedef union tUCODEC_FEATURE_TAG {
    /* Add here the audio feature structure */
    tUCODEC_CNF_SBC SBCConfig;
    tUCODEC_CNF_M12 M12Config;
    tUCODEC_CNF_M24 M24Config;
    UINT8           Volume;     /* 0 to mute. 0xFF for the max volume */
    UINT8           Balance;    /* 0->100% right, 255->100% left */
    /* Add here the video feature structure */
    /* TBD */
} tUCODEC_FEATURE;

typedef struct tUCODEC_CNF_TAG {
    tUCODEC_MEDIA_TYPE  MediaType;
    tUCODEC_CODEC_TYPE  Type;
    tUCODEC_FEATURE     Feature;
} tUCODEC_CNF;

typedef struct tUCODEC_BUF_INFO_TAG {
    UINT8       NumOfFrames;
    UINT32      TimesStamp;
} tUCODEC_BUF_INFO;


/******************************************************************************
**
** Function         tUCODEC_CBACK_PTR
**
** Description      This call back report CODEC indication.
**                  It report codec error as well as flow onfrol indication.
**
**                  Input : CodecId: Id of the codec that calls this call back.
**                          Status: ->UCODEC_FLOW_CTRL_OFF if the Tx Q just
**                                  went below the low watermark
**                                  ->UCODEC_RX_READY if data are ready to be
**                                  read. This olny hapens when the Rx Q was
**                                  empty before receiving data.
**                                  ->UCODEC_INTERNAL_ERROR if something went
**                                  wrong with the driver
**
**                  Output Parameters : None
**
** Returns          None.
**
******************************************************************************/
typedef void (* tUCODEC_CBACK_PTR)(tUCODEC_ID, tUCODEC_STATUS);

/*******************************************************************************
** Function Prototypes
*******************************************************************************/

/******************************************************************************
**
** Function         UCODEC_Init
**
** Description      Startup initialisation function. This function is called
**                  before any orther function of UCODEC it initialize UCODEC
**                  internal structure an the external codec.
**
**                  Input : CodecId: Id of the codec to perform the operation on.
**
**                  Output Parameters : None
**
** Returns          UCODEC_SUCCESS if The action was performed with sucess.
**                  Error code else.
**
******************************************************************************/
BT_API extern tUCODEC_STATUS    UCODEC_Init       (void *);

/******************************************************************************
**
** Function         UCODEC_Configure
**
** Description      Initialise the CODEC for a particular stream.
**
**
**                  Input : CodecId: Id of the codec to perform the operation on.
**                          CbackPrt: Call back pointer for codec feedback.
**                          pConfig: Pointer on a codec configuration structure.
**
**                  Output Parameters : None
**
** Returns          UCODEC_SUCCESS if The action was performed with sucess.
**
******************************************************************************/
BT_API extern tUCODEC_STATUS    UCODEC_Configure  (tUCODEC_ID, tUCODEC_CBACK_PTR, tUCODEC_CNF *);

/******************************************************************************
**
** Function         UCODEC_FlushTx
**
** Description      Fluch Tx buffer Q.
**
**                  Input : CodecId: Id of the codec to perform the operation on.
**
**                  Output Parameters : None
**
** Returns          UCODEC_SUCCESS if The action was performed with sucess.
**                  Error code else.
**
******************************************************************************/
BT_API extern tUCODEC_STATUS    UCODEC_FlushTx      (tUCODEC_ID);

/******************************************************************************
**
** Function         UCODEC_FlushRx
**
** Description      Fluch Rx buffer Q.
**
**                  Input : CodecId: Id of the codec to perform the operation on.
**
**                  Output Parameters : None
**
** Returns          UCODEC_SUCCESS if The action was performed with sucess.
**                  Error code else.
**
******************************************************************************/
BT_API extern tUCODEC_STATUS    UCODEC_FlushRx      (tUCODEC_ID);

/******************************************************************************
**
** Function         UCODEC_WriteBuf
**
** Description      Send a buffer to the codec.
**
**                  Input : CodecId: Id of the codec to perform the operation on.
**                          pBuf: Pointer onto the GKI buffer to be send to the CODEC.
**
**                  Output Parameters : None
**
** Returns          UCODEC_SUCCESS if The action was performed with sucess.
**                  UCODEC_FLOW_CTRL_ON if The codec buffer Q had reach a UCODEC_HIGH_WM
**                                      watermark. The buffer is queued
**                  UCODEC_OVERFLOW if The codec buffer Q had reach a critical
**                                     watermark. The buffer is dropped.
**
******************************************************************************/
BT_API extern tUCODEC_STATUS    UCODEC_WriteBuf   (tUCODEC_ID, BT_HDR *);

/******************************************************************************
**
** Function         UCODEC_ReadBuf
**
** Description      Get a buffer from the codec.
**
**                  Input : CodecId: Id of the codec to perform the operation on.
**
**                  Output Parameters : None
**
** Returns          Pointer on the GKI buffer. NULL if the Rx Q is empty
**
******************************************************************************/
BT_API extern tUCODEC_STATUS  UCODEC_ReadBuf    (tUCODEC_ID, BT_HDR **, tUCODEC_BUF_INFO *);

/******************************************************************************
**
** Function         UCODEC_Close
**
** Description      This function is called to put the codec in low power mode
**
**
**                  Input : CodecId: Id of the codec to perform the operation on.
**
**                  Output Parameters : None
**
** Returns          UCODEC_SUCCESS : The action was performed with sucess.
**                  Error code else.
**
******************************************************************************/
BT_API extern tUCODEC_STATUS   UCODEC_Close   (tUCODEC_ID);

/******************************************************************************
**
** Function         UCODEC_Open
**
** Description      This function is called to resume the codec from low power
**                  mode after UCODEC_Close had been called. It will put the
**                  codec in the state it was before UCODEC_Close being called.
**
**                  Input : CodecId: Id of the codec to perform the operation on.
**
**                  Output Parameters : None
**
** Returns          UCODEC_SUCCESS : The action was performed with sucess.
**                  Error code else.
**
******************************************************************************/
BT_API extern tUCODEC_STATUS   UCODEC_Open     (tUCODEC_ID);

#ifdef __cplusplus
};
#endif


#endif /* UCODEC_H */
