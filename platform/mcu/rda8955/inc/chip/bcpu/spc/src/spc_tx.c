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


#include "cs_types.h"

#include "chip_id.h"

#include "gsm.h"
#include "baseband_defs.h"
// SPC
#include "spc_ctx.h"
#include "spc_private.h"
#include "spc_mailbox.h"
#include "spcp_debug.h"
#include "spc_profile_codes.h"
// SPAL
#include "spal_comregs.h"
#include "spal_irq.h"
#include "spal_rfif.h"
#include "spal_tcu.h"
#include "spal_mem.h"
// CHIP
#include "global_macros.h"
#include "sys_ctrl.h"
#include "sys_irq.h"
#include "bb_irq.h"
#include "bb_ifc.h"
#include "tcu.h"
#include "rf_if.h"

#if (SPC_IF_VER < 5)
// sched_rx_slot_nb in SPC_CONTEXT_T was named as rxslotNb
// when SPC_IF_VER < 5
#define sched_rx_slot_nb rxslotNb
#endif

#if (CHIP_IFC_RF_TX_CHANNEL == 1)
// when IFC CH3 exist, use SW fifo
#define SPC_MAX_TXSWFIFO_SIZE (4*156*3/32 + 1)
UINT32 g_spcSwTxFifo[SPC_MAX_TXSWFIFO_SIZE]  SPAL_BBSRAM_GLOBAL;
#else
#define g_spcSwTxFifo NULL
#endif

//=============================================================================
// spc_ResetCodec()
//-----------------------------------------------------------------------------
/// This function resets the context Tx and Rx codec modes
//=============================================================================
PUBLIC VOID spc_ResetCodec(VOID)
{
    UINT8 i=0;

    SPC_PROFILE_FUNCTION_ENTER(spc_ResetCodec);
    // mute NB_DEC NB_DEC frames to avoid bad noise
    g_spcCtx->DTX_dwnlk_flag=0;
    g_spcCtx->DTX_dwnlk_count=0;
    g_spcCtx->speechEncOut.encMode = MBOX_INVALID_MODE;

    switch (SPEECH(g_spcCtx->ChMode))
    {
        case FR_CODEC:
            if (RATE(g_spcCtx->ChMode))
            {
                g_spcCtx->RxCodecMode = SPP_HR_MODE;
                g_spcCtx->TxCodecMode = SPP_HR_MODE;
                g_spcCtx->SilentFrame = (UINT32*) &(MutedFrame_HR[0]);
            }
            else
            {
                g_spcCtx->RxCodecMode = SPP_FR_MODE;
                g_spcCtx->TxCodecMode = SPP_FR_MODE;
                g_spcCtx->SilentFrame = (UINT32*) &(MutedFrame_FR[0]);
            }
            break;

        case EFR_CODEC:
            // The EFR code is merged in the AMR,
            // so the Mode vars need to be initialised
            g_spcCtx->RxCodecMode = SPP_EFR_MODE;
            g_spcCtx->TxCodecMode = SPP_EFR_MODE;
            g_spcCtx->SilentFrame = (UINT32*) &(MutedFrame_EFR[0]);
            break;

        case AMR_CODEC:
            g_spcCtx->SilentFrame = (UINT32*) &(MutedFrame_AMR[0]);
            // initialize the C/Iest buffer
            for(i=0; i<101; i++)
            {
                g_spcCtx->CIest[i]=0;
            }
            break;

        default:
            break;
    }
    SPC_PROFILE_FUNCTION_EXIT(spc_ResetCodec);
}

//=============================================================================
// spc_UpdateOnFint()
//-----------------------------------------------------------------------------
/// This function updates all the context variables that change on FINT
//=============================================================================
PROTECTED VOID spc_UpdateOnFint(VOID)
{
    volatile UINT32 critical_sec;
    UINT16 i=0;

    SPC_PROFILE_FUNCTION_ENTER(spc_UpdateOnFint);
    g_spcCtx->currentSnap = spal_ComregsGetSnap();
    g_spcCtx->frame_over=0;
    g_spcCtx->ChMode = g_mailbox.pal2spc.statWin.dedicated.mode;
    g_spcCtx->voc_dtx_en = (g_mailbox.pal2spc.dtxAllowed == TRUE) ? 1 : 0;
    g_spcCtx->DedicatedActive = g_mailbox.pal2spc.statWin.dedicated.active;
    g_spcCtx->txQty = g_mailbox.pal2spc.win[g_spcCtx->currentSnap].tx.qty;
    g_spcCtx->rxQty = g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.qty;

    // Save satic Rx cfg to workaround L1 limitation
    for (i=0; i<MBOX_MAX_BUF_IDX_QTY; i++)
    {
        g_spcCtx->statWinRxCfg[i].itr = g_mailbox.pal2spc.statWin.rx.cfg[i].itr;
        g_spcCtx->statWinRxCfg[i].type= g_mailbox.pal2spc.statWin.rx.cfg[i].type;
    }

    // Reset the TxCtrl & RxCtrl Buffers each time the Tch is started
    if (g_mailbox.pal2spc.statWin.dedicated.initTch==TRUE)
    {
        g_spcCtx->TxBlockOffset=0;
        g_spcCtx->RxBlockOffset=0;
        g_mailbox.pal2spc.statWin.dedicated.initTch=FALSE;
        for (i=0; i<4*32; i++)
        {
            g_spcCtx->ItlvBufRxCtrl[i]=0;
        }
        for (i=0; i<4*4; i++)
        {
            g_spcCtx->ItlvBufTxCtrl[i]=0;
        }
    }

    if (g_spcCtx->DedicatedActive==TRUE)
    {
        // Reset the Tx & Rx dedicated Buffers each time the Tch is modified
        if (g_spcCtx->PrevChMode!=g_spcCtx->ChMode)
        {
            g_spcCtx->TxBlockOffset=0;
            g_spcCtx->RxBlockOffset=0;
            for (i=0; i<8*4; i++)
            {
                g_spcCtx->ItlvBufTxDedicated[i]=0;
            }
            for (i=0; i<8*32; i++)
            {
                g_spcCtx->ItlvBufRxDedicated[i]=0;
            }
            // Reset the TX & Rx codec mode in case of speech
            if (MODE(g_spcCtx->ChMode)==TCH_SPEECH)
            {
                critical_sec = spal_IrqEnterCriticalSection();
                spc_ResetCodec();
                spal_IrqExitCriticalSection(critical_sec);
            }
            g_spcCtx->PrevChMode=g_spcCtx->ChMode;
        }

        // AMR Config update when it has been modified
        if (g_mailbox.pal2spc.statWin.dedicated.changedAMRCfg)
        {
            spc_UpdateAmrCfg();
        }
    }
    else
    {
        // Reset the TxCtrl & RxCtrl Buffers each time the Tch is stopped
        if (g_spcCtx->PrevChMode!=g_spcCtx->ChMode)
        {
            for (i=0; i<4*32; i++)
            {
                g_spcCtx->ItlvBufRxCtrl[i]=0;
            }
            for (i=0; i<4*4; i++)
            {
                g_spcCtx->ItlvBufTxCtrl[i]=0;
            }
            g_spcCtx->PrevChMode=g_spcCtx->ChMode;
        }
    }

    if ((g_spcCtx->rxQty > 0) || (g_spcCtx->txQty > 0))
    {
        g_spcCtx->sched_rx_count=0;
        g_spcCtx->sched_slot_count=0;
        g_spcCtx->sched_rx_slot_nb=0;
        // compute the absolute frame number
        UINT32 AFN=0;
        INT8 nb51=g_mailbox.pal2spc.counters[g_spcCtx->currentSnap].T3 -
                  g_mailbox.pal2spc.counters[g_spcCtx->currentSnap].T2;
        if (nb51<0)
        {
            nb51 += 26;
        }
        else if (nb51>25)
        {
            nb51 -= 26;
        }
        AFN = 26*51*g_mailbox.pal2spc.counters[g_spcCtx->currentSnap].T1 + 51*nb51 +
              g_mailbox.pal2spc.counters[g_spcCtx->currentSnap].T3;
        g_spcCtx->AFNmod104 = (UINT8) (AFN % 104);

        // Re-Enable Tx TCO events eventually disabled by a previous DTX
        spal_TcuEnableEvent(g_mailbox.pal2spc.rf.dtxTcoSettings);

// No TX_SWAP capability in DigRf
#if (CHIP_HAS_ANA_RF==1)
        // Check if I/Q have to be swapped for potential Tx window
        if (g_mailbox.pal2spc.win[g_spcCtx->currentSnap].tx.swapIQ)
        {
            g_spcCtx->RfIfCtrl |= RF_IF_TX_SWAP_I_Q_YES;
        }
        else
        {
            g_spcCtx->RfIfCtrl &= ~RF_IF_TX_SWAP_I_Q_YES;
        }
        // Apply change to RfIf
        spal_RfIfWriteCtrl(g_spcCtx->RfIfCtrl);
#endif

        // start the ciphering sequence generation
        if(g_mailbox.pal2spc.statWin.ciphMode!=NO_CIPHER)
        {
            UINT32 count;
            UINT8 cipher_error;
            count = ((g_mailbox.pal2spc.counters[g_spcCtx->currentSnap].T1)<<11 |
                     (g_mailbox.pal2spc.counters[g_spcCtx->currentSnap].T3)<<5  |
                     (g_mailbox.pal2spc.counters[g_spcCtx->currentSnap].T2) );
            cipher_error = spp_CipherCountInit(count);
            cipher_error = spp_CipherStart (g_mailbox.pal2spc.statWin.ciphMode);
        }
        // dummy PCH detection result reset in mailbox
        g_mailbox.dummyPchResult=SPP_DUMMY_PCH_NO_RESULT;
    }
    SPC_PROFILE_FUNCTION_EXIT(spc_UpdateOnFint);
}



//=============================================================================
// spc_DataEncode()
//-----------------------------------------------------------------------------
/// This function encodes a CSD block
//=============================================================================
PUBLIC VOID spc_DataEncode(UINT32 *InputBuffer)
{
    UINT32 *ptr32;
    UINT8 TxBurstOffset=0;
    UINT8 i,j;

    SPC_PROFILE_FUNCTION_ENTER(spc_DataEncode);
    if(g_spcCtx->FacchEncoded)
    {
        g_spcCtx->FacchEncoded = 0;
        for (i=0; i<4; i++)
        {
            TxBurstOffset = g_spcCtx->TxBlockOffset + i;
            if (TxBurstOffset >= 22)
            {
                TxBurstOffset -= 22;
            }
            ptr32=(UINT32 *)(g_spcCtx->ItlvBufTxDedicated + TxBurstOffset*4);
            for (j=0; j<4; j++)
            {
                g_spcCtx->ItlvBufTxFacch[i * 4 + j] = (*ptr32) & 0xAAAAAAAA;
                ptr32++;
            }
        }
        spp_EncodeBlock(InputBuffer,
                        g_spcCtx->ChMode,
                        0,
                        0,
                        NULL,
                        g_spcCtx->TxBlockOffset,
                        ITLV_TYPE_3,
                        g_spcCtx->ItlvBufTxDedicated);

        for (i=0; i<4; i++)
        {
            TxBurstOffset = g_spcCtx->TxBlockOffset + i;
            if (TxBurstOffset >= 22)
            {
                TxBurstOffset -= 22;
            }
            ptr32=(UINT32 *)(g_spcCtx->ItlvBufTxDedicated + TxBurstOffset*4);
            for (j = 0; j < 4; j++)
            {
                (*ptr32) &= 0x55555555; // reset odd bits
                (*ptr32) |= g_spcCtx->ItlvBufTxFacch[i * 4 + j];
                ptr32++;
            }
        }
    }
    else
    {
        spp_EncodeBlock(InputBuffer,
                        g_spcCtx->ChMode,
                        0,
                        0,
                        NULL,
                        g_spcCtx->TxBlockOffset,
                        ITLV_TYPE_3,
                        g_spcCtx->ItlvBufTxDedicated);
    }
    SPC_PROFILE_FUNCTION_EXIT(spc_DataEncode);
}



//=============================================================================
// spc_DataFacch()
//-----------------------------------------------------------------------------
/// This function prepares a FACCH during a CSD call
//=============================================================================
PUBLIC VOID spc_DataFacch(UINT32 *InputBuffer)
{
    UINT32 *ptr32;
    UINT8 TxBurstOffset=0;
    UINT8 i,j;

    SPC_PROFILE_FUNCTION_ENTER(spc_DataFacch);
    // CSD block is always stored in the last buffer
    spp_EncodeBlock(g_spcCtx->NBlockDataIn[MBOX_MAX_BUF_IDX_QTY],
                    g_spcCtx->ChMode,
                    0,
                    0,
                    NULL,
                    g_spcCtx->TxBlockOffset,
                    ITLV_TYPE_3,
                    g_spcCtx->ItlvBufTxDedicated);

    g_spcCtx->Tx_Hu  = 0x0F;
    g_spcCtx->Tx_Hl |= 0xF0;
    spp_EncodeBlock(InputBuffer,
                    CS1_mode,
                    0,
                    0,
                    NULL,
                    0,
                    ITLV_TYPE_1A,
                    g_spcCtx->ItlvBufTxFacch);

    for (i=0; i<8; i++)
    {
        TxBurstOffset = g_spcCtx->TxBlockOffset + i;
        if (TxBurstOffset >= 22)
        {
            TxBurstOffset -= 22;
        }
        ptr32=(UINT32 *)(g_spcCtx->ItlvBufTxDedicated + TxBurstOffset*4);
        for (j=0; j<4; j++)
        {
            if (i<4)
            {
                (*ptr32) &= 0xAAAAAAAA; // reset even bits
            }
            else
            {
                (*ptr32) &= 0x55555555; // reset odd bits
            }
            (*ptr32) |= g_spcCtx->ItlvBufTxFacch[i*4+j];
            ptr32++;
        }
    }

    SPC_PROFILE_FUNCTION_EXIT(spc_DataFacch);
}

//=============================================================================
// spc_MboxToSppSpeechMode function
//-----------------------------------------------------------------------------
/// Convert from VPP encoding speech mode of the encoded Tx buffer to SPP speech mode
/// @return SPP speech encoding mode
//=============================================================================
PUBLIC SPP_SPEECH_MODE_T spc_MboxToSppSpeechMode(MBOX_SPEECH_MODE_T encMode)
{

    if (MBOX_EFR_MODE == encMode)
    {
        return SPP_EFR_MODE;
    }
    else
    {
        switch (encMode | 0xf)
        {
            case MBOX_AMR475_ENC_MODE:     return SPP_MR475_MODE;
            case MBOX_AMR515_ENC_MODE:     return SPP_MR515_MODE;
            case MBOX_AMR59_ENC_MODE:      return SPP_MR59_MODE;
            case MBOX_AMR67_ENC_MODE:      return SPP_MR67_MODE;
            case MBOX_AMR74_ENC_MODE:      return SPP_MR74_MODE;
            case MBOX_AMR795_ENC_MODE:     return SPP_MR795_MODE;
            case MBOX_AMR102_ENC_MODE:     return SPP_MR102_MODE;
            case MBOX_AMR122_ENC_MODE:     return SPP_MR122_MODE;
            case MBOX_FR_MODE:             return SPP_FR_MODE;
            case MBOX_HR_MODE:             return SPP_HR_MODE;
            default:                       return SPP_INVALID_MODE;
        }
    }
}

//=============================================================================
// spc_UpdateSpeechEnc()
//-----------------------------------------------------------------------------
/// This function updates speech variables for Tx
//=============================================================================
PUBLIC VOID spc_UpdateSpeechEnc(UINT32 **InputBuffer, UINT16 *CodecModeInd, UINT8 *TxFrameType)
{

    UINT8 T2=g_mailbox.pal2spc.counters[g_spcCtx->currentSnap].T2;;
    UINT8 voc_sp=1;

    SPC_PROFILE_FUNCTION_ENTER(spc_UpdateSpeechEnc);

    // get info form the current encoding structure
    *CodecModeInd=spc_MboxToSppSpeechMode(g_spcCtx->speechEncOut.encMode);
    *TxFrameType=g_spcCtx->speechEncOut.encFrameType;
    voc_sp=g_spcCtx->speechEncOut.sp;
    *InputBuffer = (UINT32*) &(g_spcCtx->speechEncOut.encOutBuf[0]);

    // FIXME test if it works
    if (SPEECH(g_spcCtx->ChMode)==AMR_CODEC)
    {
        if (*CodecModeInd>SPP_MR122_MODE)
        {
            // if previous mode was not AMR
            *CodecModeInd = SPP_INVALID_MODE;
        }
        else if (*CodecModeInd!=g_spcCtx->TxCodecMode)
        {
            // force the codec mode asuming one step in
            // activ codec set will not be detectable
            // but only on CMI frames
            if((T2==0)||(T2==8)||(T2==17)||(T2==1)||(T2==9)||(T2==18))
            {
                *CodecModeInd = g_spcCtx->TxCodecMode;
            }
        }
    }
    else
    {
        if (*CodecModeInd!=g_spcCtx->TxCodecMode)
        {
            *CodecModeInd = g_spcCtx->TxCodecMode;//SPP_INVALID_MODE;
        }
    }

    if (*CodecModeInd==SPP_INVALID_MODE)
    {
        *CodecModeInd=g_spcCtx->TxCodecMode;
        *TxFrameType=SPP_TX_SID_UPDATE;
        *InputBuffer=(UINT32*) g_spcCtx->SilentFrame;
    }
    //----------------------------- DTX -------------------------------
    if ((voc_sp == 0) && (SPEECH(g_spcCtx->ChMode)!=AMR_CODEC))
    {
        if(RATE(g_spcCtx->ChMode) == FR_RATE)
        {
            if ((g_spcCtx->DTX_on==2)&&(g_spcCtx->voc_dtx_en==1)&&
                    (g_spcCtx->FacchEncoded==0)&&(g_spcCtx->Taf_Flag==0))
            {
                g_spcCtx->Tx_off = TRUE;
            }
            if(g_spcCtx->DTX_on<2)
            {
                g_spcCtx->DTX_on += 1;
            }

            // test if aligned with SACCH
            if (g_spcCtx->AFNmod104==52)
            {
                g_spcCtx->Tx_off = FALSE;
                g_spcCtx->Taf_Flag = 2;
            }
        }
        else
        {
            if ((g_spcCtx->DTX_on)&&(g_spcCtx->voc_dtx_en==1)&&
                    (g_spcCtx->FacchEncoded==0)&&(g_spcCtx->Taf_Flag==0))
            {
                g_spcCtx->Tx_off = TRUE;
            }
            g_spcCtx->DTX_on = 1;
            // test if aligned with SACCH
            if (SUBCHANNEL(g_spcCtx->ChMode) == 0)
            {
                if ((g_spcCtx->AFNmod104==0)||(g_spcCtx->AFNmod104==52))
                {
                    g_spcCtx->Tx_off = FALSE;
                    g_spcCtx->Taf_Flag = 2;
                }
            }
            else
            {
                if ((g_spcCtx->AFNmod104==14)||(g_spcCtx->AFNmod104==66))
                {
                    g_spcCtx->Tx_off = FALSE;
                    g_spcCtx->Taf_Flag = 2;
                }
            }
        }
    }
    else
    {
        g_spcCtx->DTX_on = 0;
        g_spcCtx->Tx_off = FALSE;
    }
    //----------------------------- DTX -------------------------------
    SPC_PROFILE_FUNCTION_EXIT(spc_UpdateSpeechEnc);
}

//=============================================================================
// spc_RACHTxProcess()
//-----------------------------------------------------------------------------
/// This function executes the RACH Tx processing for the current frame
//=============================================================================
PROTECTED VOID spc_RACHTxProcess(UINT8 buffIdx)
{
    UINT32 BurstToSend[5]= {0,0,0,0,0};
    UINT32 RachData=0;
    UINT8 i;

    SPC_PROFILE_FUNCTION_ENTER(spc_RACHTxProcess);

    g_spcCtx->Tx_off = FALSE;
    if ((g_mailbox.pal2spc.win[g_spcCtx->currentSnap].tx.bstIdx == 0)&&
            (g_mailbox.pal2spc.statWin.tx.ctx[buffIdx].encDone))
    {
        RachData = (UINT32)g_mailbox.pal2spc.statWin.tx.enc.data;
#ifdef CHIP_DIE_8955
        spp_EncodeABurst(&RachData,
                         g_mailbox.pal2spc.statWin.tx.enc.bsic,
                         g_mailbox.pal2spc.statWin.tx.enc.size,
                         0,
                         g_spcCtx->ABurstBuffer);
#else
        spp_EncodeABurst(&RachData,
                         g_mailbox.pal2spc.statWin.tx.enc.bsic,
                         g_mailbox.pal2spc.statWin.tx.enc.size,
                         g_spcCtx->ABurstBuffer);
#endif
        g_mailbox.pal2spc.statWin.tx.ctx[buffIdx].encDone=0;
    }
    for (i=0; i<5; i++)
    {
        BurstToSend[i] = g_spcCtx->ABurstBuffer[i];
    }

    spp_Modulation( BurstToSend,
                    g_mailbox.pal2spc.win[g_spcCtx->currentSnap].tx.bitMap,
                    g_mailbox.pal2spc.rf.txBstMap.preamble,
                    g_mailbox.pal2spc.rf.txBstMap.postamble,
                    g_mailbox.pal2spc.rf.txBstMap.guardStart,
                    g_spcSwTxFifo,
                    SPP_GMSK_MOD,
                    FALSE);

    SPC_PROFILE_FUNCTION_EXIT(spc_RACHTxProcess);

}

//=============================================================================
// spc_CCHTxProcess()
//-----------------------------------------------------------------------------
/// This function executes the CCH Tx processing for the current frame
//=============================================================================
PROTECTED VOID spc_CCHTxProcess(UINT8 buffIdx)
{
    volatile SPC_MBOX_STATWIN_TX_CTX_T *BuffCtx;
    UINT32 *ItlvBuffAdd;
    UINT32 BurstToSend[4*5]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    UINT8 bitMap=g_mailbox.pal2spc.win[g_spcCtx->currentSnap].tx.bitMap;
    UINT8 Tx_HuHl;
    UINT8 slotIdx;
    UINT8 slotNb=0;

    SPC_PROFILE_FUNCTION_ENTER(spc_CCHTxProcess);

    while(bitMap)
    {
        if ((bitMap&0x1)==1)
        {
            slotNb++;
        }
        bitMap>>=1;
    }

    for (slotIdx=0; slotIdx<slotNb; slotIdx++)
    {

        buffIdx=g_mailbox.pal2spc.win[g_spcCtx->currentSnap].
                tx.routeMap[slotIdx];
        BuffCtx=&(g_mailbox.pal2spc.statWin.tx.ctx[buffIdx]);

        //------------------------------ ENCODE -------------------------------
        if ((g_mailbox.pal2spc.win[g_spcCtx->currentSnap].tx.bstIdx==0)&&
                (BuffCtx->encDone))
        {
            BuffCtx->encDone=0;
            switch (BuffCtx->cs)
            {
                case GSM_CS1:
                    g_spcCtx->cs_identifier[slotIdx]=CS1_ID;
                    break;
                case GSM_CS2:
                    g_spcCtx->cs_identifier[slotIdx]=CS2_ID;
                    break;
                case GSM_CS3:
                    g_spcCtx->cs_identifier[slotIdx]=CS3_ID;
                    break;
                case GSM_CS4:
                    g_spcCtx->cs_identifier[slotIdx]=CS4_ID;
                    break;
                default:
                    g_spcCtx->cs_identifier[slotIdx]=CS1_ID;
                    break;
            }

            spp_EncodeBlock(g_spcCtx->NBlockDataIn[buffIdx],         // input
                            CCH_mode | (BuffCtx->cs << 8),         // mode
                            0,                                     // AMR
                            0,                                     // AMR
                            NULL,                                  // AMR
                            0,                                     // offset
                            ITLV_TYPE_1B,                          // itlv mode
                            g_spcCtx->ItlvBufTxCtrl+slotIdx*16);   // output

            BuffCtx->encDone=0;
        }
        //------------------------------ ENCODE -------------------------------

        Tx_HuHl = g_spcCtx->cs_identifier[slotIdx]&0x3;
        g_spcCtx->cs_identifier[slotIdx]>>=2;
        g_spcCtx->Tx_off = FALSE;
        ItlvBuffAdd=(UINT32 *)(g_spcCtx->ItlvBufTxCtrl +
                               (g_mailbox.pal2spc.win[g_spcCtx->currentSnap].tx.bstIdx)*4);
        //----------------------------- CIPHERING -----------------------------
        if(g_mailbox.pal2spc.statWin.ciphMode!=NO_CIPHER)
        {
            spp_CipherCipherNb(ItlvBuffAdd, ItlvBuffAdd);
        }
        //----------------------------- CIPHERING -----------------------------

        spp_BuildNBurst(ItlvBuffAdd + 16*slotIdx,       // In
                        g_mailbox.pal2spc.win[g_spcCtx->currentSnap].tx.tsc,     // Tsc
                        Tx_HuHl,                                    // Steal flags
                        &(BurstToSend[slotIdx*5]));     // Out


    } // end loop on slot idx

    //------------------------------ MODULATION ---------------------------------
    spp_Modulation( BurstToSend,
                    g_mailbox.pal2spc.win[g_spcCtx->currentSnap].tx.bitMap,
                    g_mailbox.pal2spc.rf.txBstMap.preamble,
                    g_mailbox.pal2spc.rf.txBstMap.postamble,
                    g_mailbox.pal2spc.rf.txBstMap.guardStart,
                    g_spcSwTxFifo,
                    SPP_GMSK_MOD,
                    TRUE);

    //------------------------------ MODULATION ---------------------------------

    SPC_PROFILE_FUNCTION_EXIT(spc_CCHTxProcess);
}



//=============================================================================
// spc_TCHTxProcess()
//-----------------------------------------------------------------------------
/// This function executes the TCH Tx processing for the current frame
//=============================================================================
PROTECTED VOID spc_TCHTxProcess(UINT8 buffIdx)
{

    volatile SPC_MBOX_STATWIN_TX_CTX_T *BuffCtx;
    UINT32 loopCtmp[4] = {0,0,0,0};
    UINT32 BurstToSend[5]= {0,0,0,0,0};
    UINT32 *InputBuffer=(UINT32*) g_spcCtx->SilentFrame;
    UINT32 *ItlvBuffAdd=(UINT32 *)g_spcCtx->ItlvBufTxCtrl;
    UINT16 CodecModeInd=g_spcCtx->TxCodecMode;
    UINT16 Copy_ChMode=g_spcCtx->ChMode;
    UINT8 TxBurstOffset=0;
    UINT8 TxFrameType=SPP_TX_SPEECH;
    UINT8 TX_encode=0;
    UINT8 itlv_mode=ITLV_TYPE_1A;
    UINT8 Tx_HuHl=0;
    UINT8 i,j;
    UINT8 ItlvBufLength=8;
    UINT8 Mask=0x3;
    UINT8 ItlvOffset=4;

    SPC_PROFILE_FUNCTION_ENTER(spc_TCHTxProcess);

    BuffCtx=&(g_mailbox.pal2spc.statWin.tx.ctx[buffIdx]);

    //-------------------------- SETTINGS -------------------------------------
    if (RATE(g_spcCtx->ChMode) == HR_RATE)
    {
        itlv_mode=ITLV_TYPE_2A;
        ItlvBufLength=6;
        ItlvOffset=2;
        Mask=0x1;

    }
    else if(MODE(g_spcCtx->ChMode) == TCH_DATA)
    {
        itlv_mode=ITLV_TYPE_3;
        ItlvBufLength=22;
    }
    //-------------------------- SETTINGS -------------------------------------

    //-------------------------- CHECK ENCODE ---------------------------------
    if (((g_mailbox.pal2spc.win[g_spcCtx->currentSnap].tx.bstIdx)&Mask)==0)
    {
        TX_encode=1;
        g_spcCtx->TxBlockOffset += ItlvOffset;
        if (g_spcCtx->TxBlockOffset >= ItlvBufLength)
        {
            g_spcCtx->TxBlockOffset -= ItlvBufLength;
        }

        if(MODE(g_spcCtx->ChMode) == TCH_SPEECH)
        {
            spc_UpdateSpeechEnc(&InputBuffer,
                                &CodecModeInd,
                                &TxFrameType);
        }

        //-------------------------- LOOPBACK -----------------------------
        if ((g_mailbox.pal2spc.statWin.loopMode == MBOX_LOOP_CLOSE_A) ||
                (g_mailbox.pal2spc.statWin.loopMode == MBOX_LOOP_CLOSE_B) ||
                (g_mailbox.pal2spc.statWin.loopMode == MBOX_LOOP_CLOSE_D) ||
                (g_mailbox.pal2spc.statWin.loopMode == MBOX_LOOP_CLOSE_E) ||
                (g_mailbox.pal2spc.statWin.loopMode == MBOX_LOOP_CLOSE_F) ||
                (g_mailbox.pal2spc.statWin.loopMode == MBOX_LOOP_CLOSE_I)  )
        {
            InputBuffer = g_spcCtx->NBlockDataOut[g_spcCtx->loop_bufIdx];
            TxFrameType=SPP_TX_SPEECH;
            CodecModeInd=g_spcCtx->TxCodecMode;
        }
        //-------------------------- LOOPBACK -----------------------------
        //---------------------------- FACCH ------------------------------
        if((BuffCtx->type==MBOX_FACCH_CHN)&&(BuffCtx->encDone)&&
                (g_mailbox.pal2spc.win[g_spcCtx->currentSnap].tx.bstIdx==0))
        {
            g_spcCtx->FacchEncoded = 1;
            g_spcCtx->Tx_off = FALSE;
            Copy_ChMode = CS1_mode;   // force mode to control
            InputBuffer = g_spcCtx->NBlockDataIn[buffIdx];

            if (RATE(g_spcCtx->ChMode) == FR_RATE)
            {
                if(MODE(g_spcCtx->ChMode) == TCH_DATA)
                {
                    spc_DataFacch(InputBuffer);
                    TX_encode=0;
                    BuffCtx->encDone=0;
                }
                else
                {
                    g_spcCtx->Tx_Hu  = 0x0F;
                    g_spcCtx->Tx_Hl |= 0xF0;
                }
            }
            else
            {
                itlv_mode = ITLV_TYPE_1C;
                g_spcCtx->FacchEncoded = 2;
                g_spcCtx->Tx_Hu  = 0x0F;
                g_spcCtx->Tx_Hl |= 0x3C;
            }
        }
        else if((RATE(g_spcCtx->ChMode) == HR_RATE)&&(g_spcCtx->FacchEncoded==2))
        {
            g_spcCtx->FacchEncoded--;
            Copy_ChMode = CS1_mode;
            g_spcCtx->Tx_off = FALSE;
            TX_encode=0;
        }
        else
        {
            if (g_spcCtx->Taf_Flag > 0)
            {
                g_spcCtx->Taf_Flag--;
            }
            if(MODE(g_spcCtx->ChMode) != TCH_DATA)
            {
                g_spcCtx->FacchEncoded=0;
            }
        }
        //---------------------------- FACCH ------------------------------
    }
    //-------------------------- CHECK ENCODE ---------------------------------

    //------------------------------- ENCODE ----------------------------------
    if(TX_encode)
    {
        if (SPEECH(g_spcCtx->ChMode)==AMR_CODEC)
        {
            spc_AmrEncode(InputBuffer,
                          Copy_ChMode,
                          CodecModeInd,
                          TxFrameType);
        }
        else
        {
            if(MODE(g_spcCtx->ChMode) != TCH_DATA)
            {
                spp_EncodeBlock(  InputBuffer,
                                  Copy_ChMode,
                                  0,
                                  0,
                                  NULL,
                                  g_spcCtx->TxBlockOffset,
                                  itlv_mode,
                                  g_spcCtx->ItlvBufTxDedicated);
            }
            else
            {
                // CSD block is always stored in the last buffer
                InputBuffer=g_spcCtx->NBlockDataIn[MBOX_MAX_BUF_IDX_QTY];
                spc_DataEncode(InputBuffer);
            }
        }
        BuffCtx->encDone=0;
    }
    //------------------------------- ENCODE ----------------------------------

    TxBurstOffset=g_spcCtx->TxBlockOffset +
                  ((g_mailbox.pal2spc.win[g_spcCtx->currentSnap].tx.bstIdx)&Mask);
    if (TxBurstOffset >= ItlvBufLength)
    {
        TxBurstOffset -= ItlvBufLength;
    }

    Tx_HuHl = ((g_spcCtx->Tx_Hu)&0x1)<<1 | ((g_spcCtx->Tx_Hl)&0x1);
    g_spcCtx->Tx_Hu>>=1;
    g_spcCtx->Tx_Hl>>=1;
    //------------------------------ LOOPBACK C --------------------------------
    if ((g_mailbox.pal2spc.statWin.loopMode == MBOX_LOOP_CLOSE_C)&&
            (Tx_HuHl==0))
    {
        for (i=0; i<4; i++)
        {
            for (j = 0; j < 32; j++)
            {
                loopCtmp[i]|=((g_spcCtx->loopC_buffer[32*i+j]>>7)&0x01)<<j;
            }
        }
        ItlvBuffAdd = (UINT32 *) loopCtmp;
    }
    else
    {
        ItlvBuffAdd = (UINT32 *) (g_spcCtx->ItlvBufTxDedicated +
                                  TxBurstOffset * 4);
    }
    //------------------------------ LOOPBACK C --------------------------------

    //------------------------------ CIPHERING ---------------------------------
    if(g_mailbox.pal2spc.statWin.ciphMode!=NO_CIPHER)
    {
        spp_CipherCipherNb(ItlvBuffAdd, ItlvBuffAdd);
    }
    //------------------------------ CIPHERING ---------------------------------

    //------------------------------ MODULATION ---------------------------------
    spp_BuildNBurst(ItlvBuffAdd,                                           // In
                    g_mailbox.pal2spc.win[g_spcCtx->currentSnap].tx.tsc,   // Tsc
                    Tx_HuHl,                                       // Steal flags
                    BurstToSend);                                          // Out
    // Handle DTX
    if (g_spcCtx->Tx_off==TRUE)
    {
        if (g_mailbox.pal2spc.win[g_spcCtx->currentSnap].tx.bstIdx==3)
        {
            g_mailbox.spc2pal.dtxUsed=1;
        }
    }
    else
    {
        spp_Modulation( BurstToSend,
                        g_mailbox.pal2spc.win[g_spcCtx->currentSnap].tx.bitMap,
                        g_mailbox.pal2spc.rf.txBstMap.preamble,
                        g_mailbox.pal2spc.rf.txBstMap.postamble,
                        g_mailbox.pal2spc.rf.txBstMap.guardStart,
                        g_spcSwTxFifo,
                        SPP_GMSK_MOD,
                        TRUE);
    }
    //------------------------------ MODULATION ---------------------------------

    SPC_PROFILE_FUNCTION_EXIT(spc_TCHTxProcess);

}


//=============================================================================
// spc_TxProcess()
//-----------------------------------------------------------------------------
/// This function executes all the Tx processing for the current frame
//=============================================================================
PROTECTED VOID spc_TxProcess(VOID)
{
    UINT8 buffIdx=g_mailbox.pal2spc.win[g_spcCtx->currentSnap].tx.routeMap[0];
    //UINT8 cs=g_mailbox.pal2spc.statWin.tx.ctx[buffIdx].cs;

    SPC_PROFILE_FUNCTION_ENTER(spc_TxProcess);

    switch(g_mailbox.pal2spc.statWin.tx.ctx[buffIdx].type)
    {
        case MBOX_CCH_CHN :
        case MBOX_PDCH_CHN:
            // CS1-CS4
            spc_CCHTxProcess(buffIdx);
            break;

        case MBOX_TCHF_CHN :
        case MBOX_TCHH_CHN :
        case MBOX_FACCH_CHN :
        case MBOX_CDATA_CHN:
            spc_TCHTxProcess(buffIdx);
            break;
        case MBOX_CHAN_TYPE_RACH:
            spc_RACHTxProcess(buffIdx);
        default:
            break;
    }

    SPC_PROFILE_FUNCTION_EXIT(spc_TxProcess);

}// end spc_tx_process

