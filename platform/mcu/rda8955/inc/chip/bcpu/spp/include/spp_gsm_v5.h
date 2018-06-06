/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/


#ifndef _SPP_GSM_V5_H_
#define _SPP_GSM_V5_H_



// =============================================================================
//  MACROS
// =============================================================================
/// size of the memory used for saving the IR blocks
#define SPP_EGPRS_IR_MEM_LEN                     (52*1024)
/// TBF modulo
#define SPP_EGPRS_IR_SNS                         (2048)
/// Size in bytes of on one Tx burst buffer
#define SPP_EGPRS_TX_BURST_LEN                   (44)
/// size in bytes of one received MCS1 to MCS4 block prior to decoding
#define MCS_1_TO_4_UNCOMPRESSED_BLOCK_LEN        (372)
/// size in bytes of one received MCS5 to MCS6 block prior to decoding
#define MCS_5_TO_6_UNCOMPRESSED_BLOCK_LEN        (1248)
/// size in bytes of one received MCS7 to MCS9 block prior to decoding
#define MCS_7_TO_9_UNCOMPRESSED_BLOCK_LEN        (612)
/// size in bytes of one compressed MCS1 to MCS4 block prior to decoding
#define MCS_1_TO_4_BLOCK_LEN                     (MCS_1_TO_4_UNCOMPRESSED_BLOCK_LEN/2)
/// size in bytes of one compressed MCS5 to MCS6 block prior to decoding
#define MCS_5_TO_6_BLOCK_LEN                     (MCS_5_TO_6_UNCOMPRESSED_BLOCK_LEN/2)
/// size in bytes of one compressed MCS7 to MCS9 block prior to decoding
#define MCS_7_TO_9_BLOCK_LEN                     (MCS_7_TO_9_UNCOMPRESSED_BLOCK_LEN/2)
/// number of compressed MCS1 to MCS4 blocks that can be stored in the IR memory
#define MCS_1_TO_4_BUFFER_LEN                    (SPP_EGPRS_IR_MEM_LEN/MCS_1_TO_4_BLOCK_LEN)
/// number of compressed MCS5 to MCS6 blocks that can be stored in the IR memory
#define MCS_5_TO_6_BUFFER_LEN                    (SPP_EGPRS_IR_MEM_LEN/MCS_5_TO_6_BLOCK_LEN)
/// number of compressed MCS7 to MCS9 blocks that can be stored in the IR memory
#define MCS_7_TO_9_BUFFER_LEN                    (SPP_EGPRS_IR_MEM_LEN/MCS_7_TO_9_BLOCK_LEN)
/// maximum size of one received blocks regardless of the mode
#define MCS_MAX_BLOCK_LEN                        (MCS_5_TO_6_BLOCK_LEN)
/// maximum number of blocks that can contain the IR memory regardless of the mode
#define MCS_MAX_BUFFER_LEN                       (MCS_1_TO_4_BUFFER_LEN)
/// size of table that gives the correct coding scheme between MCS7, MCS8 and MCS9
/// after the header decoding
#define SPP_EGPRS_SIZE_HEAD_DECODE_MCS789        (29)
/// size of table that gives the correct coding scheme between MCS5 and MCS6 after
/// the header decoding
#define SPP_EGPRS_SIZE_HEAD_DECODE_MCS56         (6)
/// size of table that gives the correct coding scheme between MCS1, MCS2, MCS3 and
/// MCS4 after the header decoding
#define SPP_EGPRS_SIZE_HEAD_DECODE_MCS1234       (13)
/// length of the training sequence
#define TRAIN_SEQ_LENGTH                         (26)
/// length of the training sequence
#define HALF_BURST_LEN                           (61)
/// length of the training sequence
#define SPP_LSE_TAP_NB                           (8)
/// quality factor for the prefilter
#define QP                                       (10)
/// number of taps used in the DDFSE treillis
#define MAIN_TAP_LEN                             (2)
/// number of taps used in the DDFSE equalizer
#define ALL_TAP_LEN                              (6)
/// size of the treillis for 8PSK equal to 8**(MAIN_TAP_LEN-1)
#define STATE_LEN                                (8)
/// length of the HASH TABLE (for IR)
#define HASH_LEN                                 (64)
#define SPP_EGPRS_IR_UNIT_SIZE                   (MCS_5_TO_6_BLOCK_LEN/2)

// ============================================================================
// SPP_EGPRS_HEADER_DATA_DISCRIMINATOR_T
// -----------------------------------------------------------------------------
/// discrimination between header and data for Egprs
// =============================================================================
typedef enum
{
    HEAD_COD                                    = 0x00000000,
    DATA_COD                                    = 0x00000001
} SPP_EGPRS_HEADER_DATA_DISCRIMINATOR_T;

/// crc decoding successful
#define SUCCESS                                  (0)
/// crc decoding failing
#define FAIL                                     (1)

// ============================================================================
// SPP_CODING_SCHEME_T
// -----------------------------------------------------------------------------
/// CS that doesn't need CRC/CONV params are packed toghether at the END to reduce
/// tables
// =============================================================================
typedef enum
{
    CS_RACH                                     = 0x00000000,
    CS_PRACH                                    = 0x00000001,
    CS_SCH                                      = 0x00000002,
    CS_CS1                                      = 0x00000003,
    CS_CS2                                      = 0x00000004,
    CS_CS3                                      = 0x00000005,
    CS_CS4                                      = 0x00000006,
    CS_HEAD_1                                   = 0x00000007,
    CS_HEAD_2                                   = 0x00000008,
    CS_HEAD_3                                   = 0x00000009,
    CS_MCS1                                     = 0x0000000A,
    CS_MCS2                                     = 0x0000000B,
    CS_MCS3                                     = 0x0000000C,
    CS_MCS4                                     = 0x0000000D,
    CS_MCS5                                     = 0x0000000E,
    CS_MCS6                                     = 0x0000000F,
    CS_MCS7                                     = 0x00000010,
    CS_MCS8                                     = 0x00000011,
    CS_MCS9                                     = 0x00000012,
    CS_HR                                       = 0x00000013,
    CS_FR                                       = 0x00000014,
    CS_EFR                                      = 0x00000015,
    CS_H24                                      = 0x00000016,
    CS_H48                                      = 0x00000017,
    CS_F24                                      = 0x00000018,
    CS_F48                                      = 0x00000019,
    CS_F96                                      = 0x0000001A,
    CS_F144                                     = 0x0000001B,
    CS_AFS_SPEECH_122                           = 0x0000001C,
    CS_AFS_SPEECH_102                           = 0x0000001D,
    CS_AFS_SPEECH_795                           = 0x0000001E,
    CS_AFS_SPEECH_74                            = 0x0000001F,
    CS_AFS_SPEECH_67                            = 0x00000020,
    CS_AFS_SPEECH_59                            = 0x00000021,
    CS_AFS_SPEECH_515                           = 0x00000022,
    CS_AFS_SPEECH_475                           = 0x00000023,
    CS_AHS_SPEECH_795                           = 0x00000024,
    CS_AHS_SPEECH_74                            = 0x00000025,
    CS_AHS_SPEECH_67                            = 0x00000026,
    CS_AHS_SPEECH_59                            = 0x00000027,
    CS_AHS_SPEECH_515                           = 0x00000028,
    CS_AHS_SPEECH_475                           = 0x00000029,
    CS_AFS_SID_UPDATE                           = 0x0000002A,
    CS_AFS_RATSCCH                              = 0x0000002B,
    CS_AHS_SID_UPDATE                           = 0x0000002C,
    CS_AHS_RATSCCH_DATA                         = 0x0000002D,
    CS_AFS_SID_FIRST                            = 0x0000002E,
    CS_AFS_ONSET                                = 0x0000002F,
    CS_AHS_SID_UPDATE_INH                       = 0x00000030,
    CS_AHS_SID_FIRST_P1                         = 0x00000031,
    CS_AHS_SID_FIRST_P2                         = 0x00000032,
    CS_AHS_SID_FIRST_INH                        = 0x00000033,
    CS_AHS_ONSET                                = 0x00000034,
    CS_AHS_RATSCCH_MARKER                       = 0x00000035,
    CS_QTY                                      = 0x00000036
} SPP_CODING_SCHEME_T;

/// Offset for the SRC tables
#define CS_NEED_SRC                              (CS_AFS_SPEECH_122)
/// Offset for Post SRC tables
#define CS_END_NEED_SRC                          (CS_AFS_SID_FIRST)
/// size of the CRC/CONV table
#define CS_PARAM_QTY                             (CS_END_NEED_SRC)
/// size of the SRC table
#define CS_SRC_PARAM_QTY                         (CS_END_NEED_SRC - CS_NEED_SRC)

// ============================================================================
// SPP_SPEECH_MODE_T
// -----------------------------------------------------------------------------
/// vocoder mode
// =============================================================================
typedef enum
{
    SPP_MR475_MODE                              = 0x00000000,
    SPP_MR515_MODE                              = 0x00000001,
    SPP_MR59_MODE                               = 0x00000002,
    SPP_MR67_MODE                               = 0x00000003,
    SPP_MR74_MODE                               = 0x00000004,
    SPP_MR795_MODE                              = 0x00000005,
    SPP_MR102_MODE                              = 0x00000006,
    SPP_MR122_MODE                              = 0x00000007,
    SPP_FR_MODE                                 = 0x00000008,
    SPP_HR_MODE                                 = 0x00000009,
    SPP_EFR_MODE                                = 0x0000000A,
    SPP_INVALID_MODE                            = 0x0000000B
} SPP_SPEECH_MODE_T;


// ============================================================================
// SPP_RX_FRAME_TYPE_T
// -----------------------------------------------------------------------------
/// AMR enum
// =============================================================================
typedef enum
{
    SPP_RX_SPEECH_GOOD                          = 0x00000000,
    SPP_RX_SPEECH_DEGRADED                      = 0x00000001,
    SPP_RX_ONSET                                = 0x00000002,
    SPP_RX_SPEECH_BAD                           = 0x00000003,
    SPP_RX_SID_FIRST                            = 0x00000004,
    SPP_RX_SID_UPDATE                           = 0x00000005,
    SPP_RX_SID_BAD                              = 0x00000006,
    SPP_RX_NO_DATA                              = 0x00000007
} SPP_RX_FRAME_TYPE_T;


// ============================================================================
// SPP_TX_FRAME_TYPE_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
    SPP_TX_SPEECH                               = 0x00000000,
    SPP_TX_SID_FIRST                            = 0x00000001,
    SPP_TX_SID_UPDATE                           = 0x00000002,
    SPP_TX_NO_DATA                              = 0x00000003
} SPP_TX_FRAME_TYPE_T;


// ============================================================================
// SPP_AFS_TYPE_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
    SPP_AFS_SID_UPDATE                          = 0x00000000,
    SPP_AFS_SID_FIRST                           = 0x00000001,
    SPP_AFS_ONSET                               = 0x00000002,
    SPP_AFS_SPEECH                              = 0x00000003,
    SPP_AFS_RATSCCH                             = 0x00000004,
    SPP_AFS_NO_DATA                             = 0x00000005
} SPP_AFS_TYPE_T;


// ============================================================================
// SPP_AHS_TYPE_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
    SPP_AHS_SID_UPDATE                          = 0x00000000,
    SPP_AHS_SID_UPDATE_INH                      = 0x00000001,
    SPP_AHS_SID_FIRST_P1                        = 0x00000002,
    SPP_AHS_SID_FIRST_P2                        = 0x00000003,
    SPP_AHS_SID_FIRST_INH                       = 0x00000004,
    SPP_AHS_ONSET                               = 0x00000005,
    SPP_AHS_SPEECH                              = 0x00000006,
    SPP_AHS_RATSCCH_MARKER                      = 0x00000007,
    SPP_AHS_RATSCCH_DATA                        = 0x00000008,
    SPP_AHS_NO_DATA                             = 0x00000009
} SPP_AHS_TYPE_T;


// ============================================================================
// SPP_FCCH_RETURN_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
    FCCH_FOUND                                  = 0x00000000,
    FCCH_NOT_FOUND                              = 0x00000001,
    FCCH_INVALID                                = 0x00000002
} SPP_FCCH_RETURN_T;


// ============================================================================
// SPP_FCCH_MODE_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
    FCCH_SLICED                                 = 0x00000000,
    FCCH_FULL                                   = 0x00000001
} SPP_FCCH_MODE_T;


// ============================================================================
// SPP_ITLV_TYPE_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
    ITLV_TYPE_1A                                = 0x00000000,
    ITLV_TYPE_1B                                = 0x00000001,
    ITLV_TYPE_1C                                = 0x00000002,
    ITLV_TYPE_2A                                = 0x00000003,
    ITLV_TYPE_2B                                = 0x00000004,
    ITLV_TYPE_3                                 = 0x00000005,
    ITLV_TYPE_H1                                = 0x00000006,
    ITLV_TYPE_H2                                = 0x00000007,
    ITLV_TYPE_H3                                = 0x00000008,
    ITLV_TYPE_H4                                = 0x00000009,
    ITLV_TYPE_D1                                = 0x0000000A,
    ITLV_TYPE_D2                                = 0x0000000B,
    ITLV_TYPE_D3                                = 0x0000000C
} SPP_ITLV_TYPE_T;


// ============================================================================
// SPP_MODULATION_TYPE_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
    SPP_UNKNOWN_MOD                             = 0x00000000,
    SPP_GMSK_MOD                                = 0x00000001,
    SPP_8PSK_MOD                                = 0x00000002
} SPP_MODULATION_TYPE_T;


// ============================================================================
// SPP_HEADER_TYPE_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
    SPP_HEADER_TYPE1                            = 0x00000000,
    SPP_HEADER_TYPE2                            = 0x00000001,
    SPP_HEADER_TYPE3                            = 0x00000002,
    SPP_HEADER_UNKNOWN                          = 0x00000003
} SPP_HEADER_TYPE_T;


// ============================================================================
// SPP_DUMMY_PCH_RESULT_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
    SPP_DUMMY_PCH_NOT_DETECTED                  = 0x00000000,
    SPP_DUMMY_PCH_A_DETECTED                    = 0x00000001,
    SPP_DUMMY_PCH_AB_DETECTED                   = 0x00000002,
    SPP_PCH_DECODING_COMPLETE                   = 0x00000003,
    SPP_DUMMY_PCH_NO_RESULT                     = 0x000000FF
} SPP_DUMMY_PCH_RESULT_T;


// ============================================================================
// SPP_FCCH_DELAY_TYPE_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
    FCCH_DELAY2                                 = 0x00000000,
    FCCH_DELAY4                                 = 0x00000001
} SPP_FCCH_DELAY_TYPE_T;


// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// SPP_FCCH_RESULT_T
// -----------------------------------------------------------------------------
/// FCCH Result
// =============================================================================
typedef struct
{
    INT16                          FOf;                          //0x00000000
    INT16                          TOf;                          //0x00000002
} SPP_FCCH_RESULT_T; //Size : 0x4



// ============================================================================
// SPP_FCCH_PARAM_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    INT32                          Acc_Energy;                   //0x00000000
    INT32                          Acc_Corr_I;                   //0x00000004
    INT32                          Acc_Corr_Q;                   //0x00000008
    UINT32                         Max_Abs_Corr;                 //0x0000000C
    INT32                          Corr_order_I;                 //0x00000010
    INT32                          Corr_order_Q;                 //0x00000014
    INT16                          samples_left;                 //0x00000018
    INT16                          max_index;                    //0x0000001A
    UINT8                          FCCH_detected;                //0x0000001C
    UINT8                          counter_high;                 //0x0000001D
    UINT8                          counter_low;                  //0x0000001E
    UINT8                          lastIT;                       //0x0000001F
    UINT8                          debug;                        //0x00000020
} SPP_FCCH_PARAM_T; //Size : 0x24



// ============================================================================
// SPP_COMPLEX_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    INT16                          i;                            //0x00000000
    INT16                          q;                            //0x00000002
} SPP_COMPLEX_T; //Size : 0x4



// ============================================================================
// SPP_SCH_RESULT_T
// -----------------------------------------------------------------------------
/// SCH Result
// =============================================================================
typedef struct
{
    UINT16                         Pwr;                          //0x00000000
    INT16                          Snr;                          //0x00000002
    INT16                          FOf;                          //0x00000004
    UINT8                          TOf;                          //0x00000006
    UINT8                          Data[4];                      //0x00000007
} SPP_SCH_RESULT_T; //Size : 0xC



// ============================================================================
// SPP_N_BURST_RESULT_T
// -----------------------------------------------------------------------------
/// Normal Burst Result
// =============================================================================
typedef struct
{
    UINT16                         Pwr;                          //0x00000000
    INT16                          Snr;                          //0x00000002
    INT16                          FOf;                          //0x00000004
    UINT8                          TOf;                          //0x00000006
    SPP_COMPLEX_T                  Dco;                          //0x00000008
} SPP_N_BURST_RESULT_T; //Size : 0xC



// ============================================================================
// SPP_N_BLOCK_RESULT_T
// -----------------------------------------------------------------------------
/// Normal block Result
// =============================================================================
typedef struct
{
    BOOL                           Bfi;                          //0x00000000
    UINT8                          Cs;                           //0x00000001
    UINT8                          Voc_Sid;                      //0x00000002
    UINT8                          MeanBEP;                      //0x00000003
    UINT8                          CvBEP;                        //0x00000004
    UINT16                         BitError;                     //0x00000006
    UINT16                         BitTotal;                     //0x00000008
} SPP_N_BLOCK_RESULT_T; //Size : 0xA



// ============================================================================
// SPP_HEADER_RESULT_T
// -----------------------------------------------------------------------------
/// Header structure for Egprs
// =============================================================================
typedef struct
{
    UINT16                         bsn[2];                       //0x00000000
    UINT8                          mcs;                          //0x00000004
    UINT8                          cps[2];                       //0x00000005
    UINT8                          spb;                          //0x00000007
    UINT8                          bfi;                          //0x00000008
} SPP_HEADER_RESULT_T; //Size : 0xA



// ============================================================================
// SPP_NB_VITAC_T
// -----------------------------------------------------------------------------
/// Viterbi NB Equalisation Buffers Structure SPAL_ISI is fixed, equal to 5
// =============================================================================
typedef struct
{
    /// 2068 bytes
    VOLATILE UINT32                PathMetric[2*16/2];           //0x00000000
    VOLATILE UINT32                path_diff_bkw[61*16/4];       //0x00000040
    VOLATILE UINT32                path_diff_fwd[61*16/4];       //0x00000410
    VOLATILE INT16                 Xcorr[2*(8+5+1)];             //0x000007E0
} SPP_NB_VITAC_T; //Size : 0x818



// ============================================================================
// SPP_SCH_VITAC_T
// -----------------------------------------------------------------------------
/// Viterbi SCH Equalisation Buffers Structure
// =============================================================================
typedef struct
{
    /// 1643 bytes
    VOLATILE UINT32                PathMetric[2*16/2];           //0x00000000
    VOLATILE UINT32                path_diff_bkw[42*16/4];       //0x00000040
    VOLATILE UINT32                path_diff_fwd[42*16/4];       //0x000002E0
    VOLATILE INT32                 Xcorr_I[24+5];                //0x00000580
    VOLATILE INT32                 Xcorr_Q[24+5];                //0x000005F4
} SPP_SCH_VITAC_T; //Size : 0x668



// ============================================================================
// SPP_DECODE_VITAC_T
// -----------------------------------------------------------------------------
/// Viterbi Decoding Buffers Structure
// =============================================================================
typedef struct
{
    /// 2144 bytes
    VOLATILE UINT32                RxItlvBufferOut[114];         //0x00000000
    VOLATILE UINT32                ViterbiOut[16];               //0x000001C8
    VOLATILE UINT32                SurvivorMEM[(64*171)/(16*2)]; //0x00000208
    VOLATILE UINT32                PathMetric[64];               //0x00000760
} SPP_DECODE_VITAC_T; //Size : 0x860



// ============================================================================
// SPP_EGPRS_PREFILTER_BUFS_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    SPP_COMPLEX_T                  InArray[157];                 //0x00000000
    SPP_COMPLEX_T                  RLS_W[8];                     //0x00000274
    SPP_COMPLEX_T                  PrefiltMin[10+8];             //0x00000294
    SPP_COMPLEX_T                  PrefiltMax[10+8];             //0x000002DC
    SPP_COMPLEX_T                  LpChan[8];                    //0x00000324
    SPP_COMPLEX_T                  A1[8];                        //0x00000344
    SPP_COMPLEX_T                  A2[10];                       //0x00000364
    SPP_COMPLEX_T                  P_Array[10+1];                //0x0000038C
    SPP_COMPLEX_T                  P_Array_New[10];              //0x000003B8
    SPP_COMPLEX_T                  H_CorArray[2*8-1];            //0x000003E0
    SPP_COMPLEX_T                  LocalTsc[16];                 //0x0000041C
    SPP_COMPLEX_T                  TscSym[26];                   //0x0000045C
    SPP_COMPLEX_T                  DcIqBufs[11];                 //0x000004C4
} SPP_EGPRS_PREFILTER_BUFS_T; //Size : 0x4F0



// ============================================================================
// SPP_EGPRS_DDFSE_BUFS_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    SPP_COMPLEX_T                  InArray[157];                 //0x00000000
    SPP_COMPLEX_T                  RLS_W[8];                     //0x00000274
    SPP_COMPLEX_T                  PrefiltMin[10+8];             //0x00000294
    SPP_COMPLEX_T                  PrefiltMax[10+8];             //0x000002DC
    SPP_COMPLEX_T                  LpChan[8];                    //0x00000324
    SPP_COMPLEX_T                  BmmlZfMetric[6/2 * 8 * 8 + 1]; //0x00000344
    UINT8                          SoftOutputFwd[8*61*4];        //0x00000648
    UINT8                          SoftOutputBck[8*61*4];        //0x00000DE8
    UINT8                          TscSym[26];                   //0x00001588
} SPP_EGPRS_DDFSE_BUFS_T; //Size : 0x15A2



// ============================================================================
// SPP_EGPRS_DECODE_VITAC_T
// -----------------------------------------------------------------------------
/// Viterbi Decoding Buffers Structure
// =============================================================================
typedef struct
{
    VOLATILE UINT32                CombinedData[459];            //0x00000000
    VOLATILE UINT32                SurvivorData[(64*612)/(16*2)]; //0x0000072C
    VOLATILE UINT32                PunctTable[58];               //0x00001A4C
    VOLATILE UINT32                PathMetric[64];               //0x00001B34
    VOLATILE UINT32                DecodedData[20];              //0x00001C34
} SPP_EGPRS_DECODE_VITAC_T; //Size : 0x1C84



// ============================================================================
// SPP_AMR_CFG_T
// -----------------------------------------------------------------------------
/// AMR Config
// =============================================================================
typedef struct
{
    UINT8                          activeCodecSet[4];            //0x00000000
    UINT8                          hysteresis[3];                //0x00000004
    UINT8                          threshold[3];                 //0x00000007
    UINT8                          version;                      //0x0000000A
    UINT8                          noiseSuppression;             //0x0000000B
    UINT8                          startModeIdx;                 //0x0000000C
    UINT8                          nbActiveCodecSet;             //0x0000000D
    UINT8                          phase;                        //0x0000000E
} SPP_AMR_CFG_T; //Size : 0xF





typedef struct
{
    UINT8 (*Data)[SPP_EGPRS_IR_UNIT_SIZE];
} SPP_EGPRS_IR_BUF_T;


#endif

