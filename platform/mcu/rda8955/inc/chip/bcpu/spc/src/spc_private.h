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










#ifndef SPC_PRIVATE_H
#define SPC_PRIVATE_H

#include "cs_types.h"
#include "gsm.h"
#include "spp_gsm.h"
#include "spal_irq.h"

#include "sppp_gsm_private.h"


#define CCH           0
#define TCH_SPEECH    1
#define TCH_DATA      2
#define CCH_mode      0
#define CS1_mode      CCH_mode | (GSM_CS1 << 8)

#define CS1_ID      0xFF
#define CS2_ID      0x13
#define CS3_ID      0x84
#define CS4_ID      0x68

#define NO_CIPHER     0xFF  // N.B. must be an u8

#define SPC_A51     0
#define SPC_A52     1
#define SPC_A53     2

#define FR_RATE  0
#define HR_RATE  1
#define TCH_F24  0
#define TCH_F48  1
#define TCH_F96  2
#define TCH_F144 3

#define FR_CODEC  0
#define EFR_CODEC 1
#define AMR_CODEC 2
#define AMR_MODE_MASK 0x7

#define RATE(ChMode) (ChMode&0x1)
#define MODE(ChMode) ((ChMode>>2)&0x3)
#define SPEECH(ChMode) ((ChMode>>4)&0x3)
#define DATA(ChMode) ((ChMode>>6)&0x3)
#define SUBCHANNEL(ChMode) ((ChMode>>1)&0x1)
#define CSCHEME(ChMode) ((ChMode>>8)&0x3)

#define NO_REQ          0xFF
#define CMI_PHASE_REQ   0x00
#define THRESH_REQ      0x01
#define AMR_CONFIG_REQ  0x07
#define ACK_OK          0x12
#define ACK_ERR         0x22
#define ACK_UNKNOWN     0x32
#define ODD_PHASE 1

// ********************************************************
// external declaration of tables used by SPC for AMR
// ********************************************************
// coefficients of C/I measuring filter for TCH AFS in Q16
EXPORT const INT16 AFSfilterCI[101];
// coefficients of C/I measuring filter for TCH AHS in Q16
EXPORT const INT16 AHSfilterCI[51];
//    speech degraded ber threshold

EXPORT const UINT32 MutedFrame_FR[9];
EXPORT const UINT32 MutedFrame_HR[4];
EXPORT const UINT32 MutedFrame_EFR[8];
EXPORT const UINT32 MutedFrame_AMR[2];

// EGPRS MCS data block size in bytes
EXPORT const UINT8 spcSizeFromMCS[13];

/*************************************************
             SPC PRIVATE functions
 *************************************************/

PROTECTED VOID spc_UpdateOnFint(VOID);

PROTECTED VOID spc_AudioProcess(VOID);

PROTECTED VOID spc_TxProcess(VOID);

PROTECTED VOID spc_RxProcess(VOID);

PUBLIC SPP_SPEECH_MODE_T spc_MboxToSppSpeechMode(MBOX_SPEECH_MODE_T encMode);

// AMR related functions

PROTECTED VOID spc_UpdateAmrCfg(VOID);

PROTECTED VOID spc_AmrEncode(UINT32 *InputBuffer,
                             UINT16 ChMode,
                             UINT16 CodecModeInd,
                             UINT8 TxFrameType);

PROTECTED VOID spc_AmrDecode(SPC_RX_LOCALS_T *spRx);

PROTECTED VOID spc_AmrRatscch(VOID);

PROTECTED VOID spc_UpdateCMR(SPP_AMR_CFG_T AMR_Cfg,
                             UINT16 QualityIndicator,
                             UINT8* CodecModeRequest);

PROTECTED VOID spc_AFSdecoding(SPP_AMR_CFG_T AMR_Cfg,
                               UINT32* ItlvBuffInput,
                               UINT32* RxItlvBufferOut,
                               UINT8 RxBuffOffset,
                               UINT32* BlockDecodedAdd,
                               UINT8 T2,
                               SPP_N_BLOCK_RESULT_T* NBlock_result,
                               UINT8* TxCodecMode,
                               UINT8* RxCodecMode,
                               UINT8* RxFrameType,
                               UINT8* ratscch_detected);

PROTECTED VOID spc_AHSdecoding(SPP_AMR_CFG_T AMR_Cfg,
                               UINT32* ItlvBuffInput,
                               UINT32* RxItlvBufferOut,
                               UINT8 RxBuffOffset,
                               UINT32* BlockDecodedAdd,
                               UINT8 T2,
                               SPP_N_BLOCK_RESULT_T* NBlock_result,
                               UINT8* TxCodecMode,
                               UINT8* RxCodecMode,
                               UINT8* RxFrameType,
                               UINT8* ratscch_detected);

PROTECTED VOID spc_AFSencoding(SPP_AMR_CFG_T AMR_Cfg,
                               UINT8 TxCodecMode,
                               UINT8 CodecModeRequest,
                               UINT8 TxFrameType,
                               UINT32* InputBuffer,
                               UINT8 T2,
                               UINT8 facch_to_encode,
                               UINT8* ratscch_to_encode,
                               UINT8 TxBlockOffset,
                               UINT32* ItlvBufTxDedicated,
                               BOOL* Tx_off);

PROTECTED VOID spc_AHSencoding(SPP_AMR_CFG_T AMR_Cfg,
                               UINT8 TxCodecMode,
                               UINT8 CodecModeRequest,
                               UINT8 TxFrameType,
                               UINT32* InputBuffer,
                               UINT8 T2,
                               UINT8 facch_to_encode,
                               UINT8* ratscch_to_encode,
                               UINT8 TxBlockOffset,
                               UINT32* ItlvBufTxDedicated,
                               BOOL* Tx_off);

// ************************************************
//              IRQ Handler
// ************************************************
PROTECTED VOID spc_IrqHandler(SPAL_IRQ_STATUS_T status);
// ************************************************
//              IRQ Handler with TCU1
// ************************************************
PROTECTED VOID spc_IrqHandlerTcu1(SPAL_IRQ_STATUS_T status);
// ************************************************
//              Set mask with TCU1
// ************************************************
PROTECTED VOID spc_IrqSetMaskTcu1(VOID);


// *************************************************
//              Scheduler entry point
// *************************************************
PROTECTED VOID spc_SchedulerMain(VOID);



#endif // SPC_PRIVATE_H

