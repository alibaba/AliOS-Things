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


#ifndef _HAL_SPEECH_H_
#define _HAL_SPEECH_H_


#include "cs_types.h"

// =============================================================================
//  MACROS
// =============================================================================
/// FIFO depth.
#define HAL_SPEECH_FIFO_DEPTH                    (4)

// ============================================================================
// HAL_SPEECH_MODE_T
// -----------------------------------------------------------------------------
/// HAL Speech modes
// =============================================================================
typedef enum
{
    HAL_AMR475_DEC                              = 0x000000F0,
    HAL_AMR475_ENC                              = 0x0000000F,
    HAL_AMR515_DEC                              = 0x000000F1,
    HAL_AMR515_ENC                              = 0x0000001F,
    HAL_AMR59_DEC                               = 0x000000F2,
    HAL_AMR59_ENC                               = 0x0000002F,
    HAL_AMR67_DEC                               = 0x000000F3,
    HAL_AMR67_ENC                               = 0x0000003F,
    HAL_AMR74_DEC                               = 0x000000F4,
    HAL_AMR74_ENC                               = 0x0000004F,
    HAL_AMR795_DEC                              = 0x000000F5,
    HAL_AMR795_ENC                              = 0x0000005F,
    HAL_AMR102_DEC                              = 0x000000F6,
    HAL_AMR102_ENC                              = 0x0000006F,
    HAL_AMR122_DEC                              = 0x000000F7,
    HAL_AMR122_ENC                              = 0x0000007F,
    HAL_EFR                                     = 0x00000177,
    HAL_FR                                      = 0x00001FFF,
    HAL_HR                                      = 0x00002FFF,
    HAL_INVALID                                 = 0x0000FFFF
} HAL_SPEECH_MODE_T;


// ============================================================================
// HAL_SPEECH_FRAME_SIZE_T
// -----------------------------------------------------------------------------
/// Size of the frame in bytes, for the various supported codecs.
// =============================================================================
typedef enum
{
    HAL_SPEECH_FRAME_SIZE_AMR475                = 0x0000000C,
    HAL_SPEECH_FRAME_SIZE_AMR515                = 0x00000010,
    HAL_SPEECH_FRAME_SIZE_AMR59                 = 0x00000010,
    HAL_SPEECH_FRAME_SIZE_AMR67                 = 0x00000014,
    HAL_SPEECH_FRAME_SIZE_AMR74                 = 0x00000014,
    HAL_SPEECH_FRAME_SIZE_AMR795                = 0x00000014,
    HAL_SPEECH_FRAME_SIZE_AMR102                = 0x0000001C,
    HAL_SPEECH_FRAME_SIZE_AMR122                = 0x00000020,
    HAL_SPEECH_FRAME_SIZE_AMR_RING              = 0x00000020,
    HAL_SPEECH_FRAME_SIZE_EFR                   = 0x00000020,
    HAL_SPEECH_FRAME_SIZE_FR                    = 0x00000024,
    HAL_SPEECH_FRAME_SIZE_HR                    = 0x00000010,
    HAL_SPEECH_FRAME_SIZE_COD_BUF               = 0x00000024,
    HAL_SPEECH_FRAME_SIZE_PCM_BUF               = 0x00000140
} HAL_SPEECH_FRAME_SIZE_T;


// ============================================================================
// HAL_RX_FRAME_TYPE_T
// -----------------------------------------------------------------------------
/// AMR enum
// =============================================================================
typedef enum
{
    HAL_RX_SPEECH_GOOD                          = 0x00000000,
    HAL_RX_SPEECH_DEGRADED                      = 0x00000001,
    HAL_RX_ONSET                                = 0x00000002,
    HAL_RX_SPEECH_BAD                           = 0x00000003,
    HAL_RX_SID_FIRST                            = 0x00000004,
    HAL_RX_SID_UPDATE                           = 0x00000005,
    HAL_RX_SID_BAD                              = 0x00000006,
    HAL_RX_NO_DATA                              = 0x00000007
} HAL_RX_FRAME_TYPE_T;


// ============================================================================
// HAL_TX_FRAME_TYPE_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
    HAL_TX_SPEECH                               = 0x00000000,
    HAL_TX_SID_FIRST                            = 0x00000001,
    HAL_TX_SID_UPDATE                           = 0x00000002,
    HAL_TX_NO_DATA                              = 0x00000003
} HAL_TX_FRAME_TYPE_T;


// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// HAL_SPEECH_ENC_OUT_T
// -----------------------------------------------------------------------------
/// Structure used to configure VOC frame encode
// =============================================================================
typedef struct
{
    /// Mode of the voice coded output frame
    UINT16                         encMode;                      //0x00000000
    /// Type of the coded output frame (AMR only)
    UINT16                         encFrameType;                 //0x00000002
    /// Flag for skipping the encoded frame, only valid when the ES is activated
    /// (replaces the unused VAD flag)
    UINT16                         echoSkipEncFrame;             //0x00000004
    /// Speech flag (all except AMR)
    UINT16                         sp;                           //0x00000006
    /// Encoder output buffer (coded frame in enc_mode)
    UINT16                         encOutBuf[HAL_SPEECH_FRAME_SIZE_COD_BUF/2]; //0x00000008
} HAL_SPEECH_ENC_OUT_T; //Size : 0x2C



// ============================================================================
// HAL_SPEECH_DEC_IN_T
// -----------------------------------------------------------------------------
/// Structure used to configure VOC frame decode
// =============================================================================
typedef struct
{
    /// DTX allowed by the network, active->G1
    UINT16                         dtxOn;                        //0x00000000
    /// mode of the voice coded input frame and of the next encoder speech frame
    /// output
    UINT16                         codecMode;                    //0x00000002
    /// Type of the coded input frame (AMR only)
    UINT16                         decFrameType;                 //0x00000004
    /// BFI flag (all vocoders except AMR)
    UINT16                         bfi;                          //0x00000006
    /// SID flag (all vocoders except AMR)
    UINT16                         sid;                          //0x00000008
    /// TAF flag (all vocoders except AMR)
    UINT16                         taf;                          //0x0000000A
    /// UFI flag (HR only)
    UINT16                         ufi;                          //0x0000000C
    /// Reserved
    UINT16                         reserved;                     //0x0000000E
    /// Decoder input buffer (coded frame in dec_mode)
    UINT16                         decInBuf[HAL_SPEECH_FRAME_SIZE_COD_BUF/2]; //0x00000010
} HAL_SPEECH_DEC_IN_T; //Size : 0x34



// ============================================================================
// HAL_SPEECH_PCM_HALF_BUF_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef UINT16 HAL_SPEECH_PCM_HALF_BUF_T[HAL_SPEECH_FRAME_SIZE_PCM_BUF/2];


// ============================================================================
// HAL_SPEECH_PCM_BUF_T
// -----------------------------------------------------------------------------
/// Structure defining the Speech PCM swap buffers used in Speech encoding and decoding
// =============================================================================
typedef struct
{
    HAL_SPEECH_PCM_HALF_BUF_T      pcmBuf[2];                    //0x00000000
} HAL_SPEECH_PCM_BUF_T; //Size : 0x280



// ============================================================================
// HAL_SPEECH_FIFO_ELM_T
// -----------------------------------------------------------------------------
/// This types describes a Fifo section. Rx and Tx are joined together because the
/// we always read and write one Rx and a Tx (or inversly) at the same time.
// =============================================================================
typedef struct
{
    /// Received encoded frame
    HAL_SPEECH_DEC_IN_T            rx;                           //0x00000000
    /// Encoded Frame to transmit
    HAL_SPEECH_ENC_OUT_T           tx;                           //0x00000034
} HAL_SPEECH_FIFO_ELM_T; //Size : 0x60



// ============================================================================
// HAL_SPEECH_FIFO_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    /// This types describes a Fifo section. Rx and Tx are joined together because
    /// the we always read and write one Rx and a Tx (or inversly) at the same time.
    HAL_SPEECH_FIFO_ELM_T          fifo[HAL_SPEECH_FIFO_DEPTH];  //0x00000000
    /// That boolean is set to TRUE when the Speech Fifo Rx has overflowed, which
    /// also corresponds to a Tx underflow. The reset is manual through coolwatcher.
    BOOL                           rxOverflowTxUnderflow;        //0x00000180
    /// That boolean is set to TRUE when the Speech Fifo Tx has overflowed, which
    /// also corresponds to a Rx underflow. This is normally expected in case of
    /// handover, for example. The reset is manual through coolwatcher.
    BOOL                           txOverflowRxUnderflow;        //0x00000181
} HAL_SPEECH_FIFO_T; //Size : 0x182





// =============================================================================
// FUNCTIONS
// =============================================================================

// =============================================================================
// hal_SpeechFifoReset
// -----------------------------------------------------------------------------
/// Initializes the speech related mechanisms in HAL (Speech Fifo, ...)
// =============================================================================
PUBLIC VOID hal_SpeechFifoReset(VOID);

// =============================================================================
// hal_SpeechPushRxPopTx
// -----------------------------------------------------------------------------
/// PAL side of the FIFO accessor.
///
/// @param pushedRx Pointer to the received trame to push in the FIFO
/// @param popedTx Pointer to the buffer where the trame to send will
/// be copied.
// =============================================================================
PUBLIC VOID hal_SpeechPushRxPopTx(CONST HAL_SPEECH_DEC_IN_T* pushedRx,
                                  HAL_SPEECH_ENC_OUT_T* popedTx);

// =============================================================================
// hal_SpeechPopRxPushTx
// -----------------------------------------------------------------------------
/// Vois side of the FIFO accessor.
///
/// @param popedRx Pointer where to copy the received trame.
/// @param pushedTx Pointer to the trame to send to push in the FIFO.
// =============================================================================
PUBLIC VOID hal_SpeechPopRxPushTx(HAL_SPEECH_DEC_IN_T* popedRx,
                                  CONST HAL_SPEECH_ENC_OUT_T* pushedTx);

/// @} // --> End of the Speech group.



#endif

