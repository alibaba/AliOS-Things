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
#include "gsm.h"
#include "baseband_defs.h"
#include "chip_id.h"
// SPC
#include "spc_ctx.h"
#include "spc_private.h"
#include "spc_mailbox.h"
#include "spcp_debug.h"
#include "spc_profile_codes.h"
// SPAL
#include "spal_comregs.h"
#include "spal_tcu.h"
#include "spal_ifc.h"
// CHIP
#include "global_macros.h"
#include "tcu.h"


#if (SPC_IF_VER < 5)
// sched_rx_slot_nb in SPC_CONTEXT_T was named as rxslotNb
// when SPC_IF_VER < 5
#define sched_rx_slot_nb rxslotNb
#endif



//=============================================================================
// spc_FcchProcess()
//-----------------------------------------------------------------------------
/// This function processes the FCCH samples for each interrupt
//=============================================================================
PUBLIC VOID spc_FcchProcess(VOID)
{
    SPC_PROFILE_FUNCTION_ENTER(spc_FcchProcess);

    UINT32 *SearchFcchInput;
    if (g_spcCtx->FcchInit == 1)
    {
        g_spcCtx->Init_FCCH = TRUE;
        g_spcCtx->FcchFound = 0;
        g_spcCtx->FcchInit = 0;
    }

    SearchFcchInput = (UINT32*) &(g_spcStaticBufTab.RxIfcBuffer[0]);

    if (g_spcCtx->FcchFound == 0)
    {
        SPP_FCCH_RETURN_T Res;
        // Search for FCCH
#if (SPC_IF_VER >= 4)
        if (g_spcCtx->FchSearchMode == FCCH_SEARCH_ALGO_FILTER)
        {
            Res = spp_SearchFcchFilter( SearchFcchInput,
                                        g_spcCtx->Init_FCCH,
                                        (g_spcCtx->DedicatedActive == TRUE) ? FCCH_SLICED: FCCH_FULL,
                                        &g_spcCtx->FCCH_Result);
        }
        else
        {
            Res = spp_SearchFcch( SearchFcchInput,
                                  g_spcCtx->Init_FCCH,
                                  (g_spcCtx->DedicatedActive == TRUE) ? FCCH_SLICED
                                  : FCCH_FULL,
                                  (g_spcCtx->FchSearchMode == FCCH_SEARCH_ALGO_DELAY2) ? FCCH_DELAY2
                                  : FCCH_DELAY4,
                                  &g_spcCtx->FCCH_Result);

        }
#else
        Res = spp_SearchFcch( (UINT32*) &(g_spcStaticBufTab.RxIfcBuffer[0]),
                              g_spcCtx->Init_FCCH,
                              (g_spcCtx->DedicatedActive == TRUE) ? FCCH_SLICED
                              : FCCH_FULL,
                              FCCH_DELAY2,
                              &g_spcCtx->FCCH_Result);
#endif
        // An FCCH has been found, we stop the process
        if ( Res != FCCH_NOT_FOUND)
        {
            g_spcCtx->FcchFound = 1;
            // This could be risky: we might disable the IFC after it was started under interrupt
            // Disable IFC as we are done
            UINT32 critical_sec = spal_IrqEnterCriticalSection();
            // Disable IFC2 only if there is no other window programmed
            if ( (g_spcCtx->bcpuIrqCause&BB_IRQ_BB_IRQ_FRAME) == 0 ||
                    g_mailbox.pal2spc.win[spal_ComregsGetSnap()].rx.qty == 0)
            {
                spal_IfcDisableRf();
#if (SPC_IF_VER >= 5)
                if(g_spcCtx->xcvPdnEnable == 1)
                {
                    spal_TcuClrTco(g_mailbox.pal2spc.rf.rxOnTcoSettings); //xcv.PDN
                }
#endif
            }
            spal_IrqExitCriticalSection(critical_sec);

            // Send results only if FCCH detection is valid
            if (Res != FCCH_INVALID)
            {
                g_mailbox.spc2pal.statWin.rx.fcch.valid = TRUE;
                g_mailbox.spc2pal.statWin.rx.fcch.fof =
                    g_spcCtx->FCCH_Result.FOf;
                // We remove FcchPreOpening as this offset is hidden
                // from L1
                // TODO : more logical to do it in PAL ?
                g_mailbox.spc2pal.statWin.rx.fcch.tof =
                    (INT16) g_spcCtx->FCCH_Result.TOf // Found offset
                    - g_spcCtx->FcchPreOpening; // Window "pre-opening"
            }
        }
        g_spcCtx->Init_FCCH = FALSE;
    }
    SPC_PROFILE_FUNCTION_EXIT(spc_FcchProcess);
}

//=============================================================================
// spc_MonitoringProcess()
//-----------------------------------------------------------------------------
/// This function processes the monitoring results
//=============================================================================
PUBLIC VOID spc_MonitoringProcess(UINT8 rxCount, UINT8 winIdx)
{
    UINT32 Power;

    spp_GetMonPower(rxCount,(UINT32 *) &(g_spcStaticBufTab.MonIfcBuffer[rxCount*(g_spcCtx->MonWinSize)]),&Power);



    g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.monRes[winIdx].power = Power;
}

//=============================================================================
// spc_SchProcess()
//-----------------------------------------------------------------------------
/// This function processes the Synchro Burst reception
//=============================================================================
PUBLIC VOID spc_SchProcess()
{
    BOOL  EqualizeDecodeSchFlag = FALSE;


    EqualizeDecodeSchFlag = spp_EqualizeDecodeSch((UINT32 *)&(g_spcStaticBufTab.RxIfcBuffer[(BB_BURST_ACTIVE_SIZE+2*BB_RX_NBURST_MARGIN)]),
                            (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                            &g_spcCtx->SCH_Result);


    if (EqualizeDecodeSchFlag)
    {
        g_mailbox.spc2pal.statWin.rx.sch[0].data[0] = g_spcCtx->SCH_Result.Data[0];
        g_mailbox.spc2pal.statWin.rx.sch[0].data[1] = g_spcCtx->SCH_Result.Data[1];
        g_mailbox.spc2pal.statWin.rx.sch[0].data[2] = g_spcCtx->SCH_Result.Data[2];
        g_mailbox.spc2pal.statWin.rx.sch[0].data[3] = g_spcCtx->SCH_Result.Data[3];
        g_mailbox.spc2pal.statWin.rx.sch[0].valid = TRUE;
    }
    else
    {
        g_mailbox.spc2pal.statWin.rx.sch[0].valid = FALSE;
    }
    g_mailbox.spc2pal.statWin.rx.sch[0].tof  =
        (INT8) ((int) g_spcCtx->SCH_Result.TOf - 4*BB_RX_SCH_MARGIN);
    g_mailbox.spc2pal.statWin.rx.sch[0].fof  =        g_spcCtx->SCH_Result.FOf;
    g_mailbox.spc2pal.statWin.rx.sch[0].snr  =
        (UINT8) ((g_spcCtx->SCH_Result.Snr>=0)?g_spcCtx->SCH_Result.Snr:0);
    g_mailbox.spc2pal.statWin.rx.sch[0].power=(UINT8) g_spcCtx->SCH_Result.Pwr;
    spal_ComregsSetIrq(MBOX_DSP_SCH_DECODING_COMPLETE);
}

//=============================================================================
// spc_SetFacch()
//-----------------------------------------------------------------------------
/// This function is used to deinterleave FACCH block in CSD buffer
//=============================================================================
PUBLIC VOID spc_SetFacch(SPC_RX_LOCALS_T *spRx)
{
    UINT8 i,j,k,l,itlv;
    itlv=22;

    SPC_PROFILE_FUNCTION_ENTER(spc_SetFacch);

    if (g_spcCtx->RxBlockOffset >= 8)
    {
        k = g_spcCtx->RxBlockOffset - 8;
        l = 8;
    }
    else
    {
        k = g_spcCtx->RxBlockOffset + itlv - 8;
        l = itlv - k;
    }
    for (i=0; i<l; i++)
    {
        for (j=0; j<32; j++)
        {
            g_spcCtx->ItlvBufRxFacch[32*i+j] =
                g_spcCtx->ItlvBufRxDedicated[32*(k+i)+j];
        }
    }
    k = l;
    l = (l==8)? 0 : 8-l;
    for (i=0; i<l; i++)
    {
        for (j=0; j<32; j++)
        {
            g_spcCtx->ItlvBufRxFacch[32*(k+i)+j] =
                g_spcCtx->ItlvBufRxDedicated[32*i+j];
        }
    }
    spRx->ItlvBuffInput = g_spcCtx->ItlvBufRxFacch;

    spRx->RxBuffOffset = 0;

    SPC_PROFILE_FUNCTION_EXIT(spc_SetFacch);
}

//=============================================================================
// spc_BfiFilter()
//-----------------------------------------------------------------------------
/// This function adds a BFI to the speech frame when needed
//=============================================================================
PUBLIC VOID spc_BfiFilter(SPC_RX_LOCALS_T *spRx)
{
#define LOOPBACK_FSBFIBER2THRESH  0
#define LOOPBACK_FSBFIBERTHRESH 40
#define LOOPBACK_FSBFISNRTHRESH -8

#define LOOPBACK_HSBFIBER2THRESH  0
#define LOOPBACK_HSBFIBERTHRESH 30
#define LOOPBACK_HSBFISNRTHRESH 1

    if (!((spRx->StealingResult)&&(spRx->burstIdx == 3)))
    {
        UINT8 LoopBackMode = 0;

        SPC_PROFILE_FUNCTION_ENTER(spc_BfiFilter);
        if ((g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_A) ||
                (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_B) ||
                (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_D) ||
                (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_E) ||
                (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_F) ||
                (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_I) )
        {
            goto LABEL_BLOCKSNR;
            LoopBackMode = 1;
        }
        // BFi Filter relevant for FS, HS and AMR
        // FS and HS
        if ((MODE(g_spcCtx->ChMode)==TCH_SPEECH)&&
                (SPEECH(g_spcCtx->ChMode)==FR_CODEC))
        {
            INT16 TotalBlockSnr = g_spcCtx->BlockSnR;
            INT8 SnrThreshold  = (LoopBackMode==0)?g_spcCtx->FsBfiSnrThresh:LOOPBACK_FSBFISNRTHRESH;
            UINT8 BerThreshold  = (LoopBackMode==0)?g_spcCtx->FsBfiBerThresh:LOOPBACK_FSBFIBERTHRESH;
            UINT8 Ber2Threshold  = (LoopBackMode==0)?g_spcCtx->FsBfiBer2Thresh:LOOPBACK_FSBFIBER2THRESH;

            if (RATE(g_spcCtx->ChMode)==FR_RATE)
            {
                TotalBlockSnr += g_spcCtx->LastBlockSnR;
            }
            else if (RATE(g_spcCtx->ChMode)==HR_RATE)
            {
                SnrThreshold  = (LoopBackMode==0)?g_spcCtx->HsBfiSnrThresh:LOOPBACK_HSBFISNRTHRESH;
                BerThreshold  = (LoopBackMode==0)?g_spcCtx->HsBfiBerThresh:LOOPBACK_HSBFIBERTHRESH;
                Ber2Threshold  = (LoopBackMode==0)?g_spcCtx->HsBfiBer2Thresh:LOOPBACK_HSBFIBER2THRESH;
            }

            if (((TotalBlockSnr < SnrThreshold)&&
                    (g_spcCtx->NBlock_result.BitError > Ber2Threshold)) ||
                    (g_spcCtx->NBlock_result.BitError > BerThreshold))
            {
                g_spcCtx->NBlock_result.Bfi = 1;
            }
        }
        // AMR
        else if ((MODE(g_spcCtx->ChMode)==TCH_SPEECH)&&
                 (SPEECH(g_spcCtx->ChMode)==AMR_CODEC))
        {
            INT16 TotalBlockSnr = g_spcCtx->BlockSnR;
            const UINT8 AFSBerThresh[8] = {61,60,60,60,59,59,59,50};
            const UINT8 AHSBerThresh[6] = {28,27,22,16,13,9};
            const UINT8 AFSBerThreshLoopback[8] = {77,76,76,76,75,75,75,70};
            const UINT8 AHSBerThreshLoopback[6] = {30,29,24,18,15,11};
            UINT8  *AMRBerThresh = (LoopBackMode==0)?(UINT8 *)AFSBerThresh:(UINT8 *)AFSBerThreshLoopback;

            INT8 SnrThreshold  = g_spcCtx->FsBfiSnrThresh;

            if (RATE(g_spcCtx->ChMode)==FR_RATE)
            {
                TotalBlockSnr += g_spcCtx->LastBlockSnR;
            }
            else if (RATE(g_spcCtx->ChMode)==HR_RATE)
            {
                AMRBerThresh = (LoopBackMode==0)?(UINT8 *)AHSBerThresh:(UINT8 *)AHSBerThreshLoopback;
                SnrThreshold  = g_spcCtx->HsBfiSnrThresh;
            }

            if ((TotalBlockSnr < SnrThreshold) &&
                    (g_spcCtx->NBlock_result.BitError>AMRBerThresh[g_spcCtx->RxCodecMode]))
            {
                g_spcCtx->NBlock_result.Bfi = 1;
            }

        }
        SPC_PROFILE_FUNCTION_EXIT(spc_BfiFilter);
    }
LABEL_BLOCKSNR:
    g_spcCtx->LastBlockSnR = g_spcCtx->BlockSnR;


}

//=============================================================================
// spc_VocBfiFilter()
//-----------------------------------------------------------------------------
/// This function implements a state machine for DTX downlink
//=============================================================================
PUBLIC VOID spc_VocBfiFilter(SPC_RX_LOCALS_T *spRx)
{
    UINT32 StealingSoftBits[2]= {0,0};
    UINT8 Facch_detected=0;
    UINT8 bfi_threshold;

    // set the bfi threshold corresponding to the codec
    // don't care about AMR since the bfi is not managed
    // the same way (done in AMR decode)
    if (RATE(g_spcCtx->ChMode) == FR_RATE)
    {
        if (SPEECH(g_spcCtx->ChMode)==FR_CODEC)
            bfi_threshold = g_spcCtx->FR_BfiThreshold;
        else
            bfi_threshold = g_spcCtx->EFR_BfiThreshold;
    }
    else
    {
        bfi_threshold = g_spcCtx->HR_BfiThreshold;
    }

    // derive the new bfi for voc from NBlock_result.Bfi
    spRx->voc_bfi |= g_spcCtx->NBlock_result.Bfi |
                     spRx->StealingResult     |
                     (g_spcCtx->NBlock_result.BitError > bfi_threshold);

    // discard non aligned sidupdate frame in DTX downlink
    if ((spRx->voc_taf ==0)&&
            (spRx->voc_sid)&&(g_spcCtx->DTX_dwnlk_flag))
    {
        spRx->voc_bfi=1;
    }

    // update the BFI counter
    if ((g_spcCtx->NBlock_result.Bfi==1)&&
            (g_spcCtx->BFI_count<2))
    {
        g_spcCtx->BFI_count++;
    }
    else if ((g_spcCtx->NBlock_result.Bfi==0)&&
             (g_spcCtx->BFI_count>0))
    {
        g_spcCtx->BFI_count--;
    }

    //update the current state: speech or DTX downlink
    // if SIDUPDATE: =>DTX_dwnlk_flag DTX_dwnlk_count=2
    if((spRx->voc_sid)||(g_spcCtx->BFI_count==2))
    {
        g_spcCtx->DTX_dwnlk_flag=1;
        g_spcCtx->DTX_dwnlk_count=2;
    }
    // if no SIDUPDATE && no BFI && DTX downlink: DTX_dwnlk_count--
    else if(g_spcCtx->NBlock_result.Bfi==0)
    {
        if (g_spcCtx->DTX_dwnlk_count)
        {
            g_spcCtx->DTX_dwnlk_count--;
        }
    }
    // if BFI && DTX downlink: DTX_dwnlk_count=2;
    else
    {
        if (g_spcCtx->DTX_dwnlk_flag)
        {
            g_spcCtx->DTX_dwnlk_count=2;
        }
    }
    // if DTX_dwnlk_count==0 => speech
    if (g_spcCtx->DTX_dwnlk_count==0)
    {
        g_spcCtx->DTX_dwnlk_flag=0;
    }

    if (g_spcCtx->DTX_dwnlk_flag)
    {
        if ((spRx->voc_sid==0))
        {
            spRx->voc_bfi = 1;
        }
    }
    // detection of FACCH peempted frame for HR
    if (RATE(g_spcCtx->ChMode) == HR_RATE)
    {
        StealingSoftBits[0] = g_spcCtx->Rx_Hu_TCH_N;
        StealingSoftBits[1] = g_spcCtx->Rx_Hl_TCH_N;
        spp_DecodeStealingBits(StealingSoftBits,
                               0,
                               &Facch_detected,
                               g_spcCtx->FacchThreshold);
        if (Facch_detected)
        {
            spRx->voc_bfi=1;
        }
    }
}

//=============================================================================
// spc_SppToMboxSpeechMode function
//-----------------------------------------------------------------------------
/// Convert from SPP speech mode to VPP speech mode
/// @param enc : SPP speech mode for encoding
/// @param dec : SPP speech mode for decoding
/// @return UINT16 codecMode formed by logical and of two MBOX_SPEECH_MODE_T
/// encMode & decMode
//=============================================================================
PUBLIC UINT16 spc_SppToMboxSpeechMode(SPP_SPEECH_MODE_T enc, SPP_SPEECH_MODE_T dec)
{

    MBOX_SPEECH_MODE_T encMode;
    MBOX_SPEECH_MODE_T decMode;

    switch (dec)
    {
        case SPP_MR475_MODE:        decMode = MBOX_AMR475_DEC_MODE; break;
        case SPP_MR515_MODE:        decMode = MBOX_AMR515_DEC_MODE; break;
        case SPP_MR59_MODE:         decMode = MBOX_AMR59_DEC_MODE;  break;
        case SPP_MR67_MODE:         decMode = MBOX_AMR67_DEC_MODE;  break;
        case SPP_MR74_MODE:         decMode = MBOX_AMR74_DEC_MODE;  break;
        case SPP_MR795_MODE:        decMode = MBOX_AMR795_DEC_MODE; break;
        case SPP_MR102_MODE:        decMode = MBOX_AMR102_DEC_MODE; break;
        case SPP_MR122_MODE:        decMode = MBOX_AMR122_DEC_MODE; break;
        case SPP_FR_MODE:           decMode = MBOX_FR_MODE;         break;
        case SPP_HR_MODE:           decMode = MBOX_HR_MODE;         break;
        case SPP_EFR_MODE:          decMode = MBOX_EFR_MODE;        break;
        default:                    decMode = MBOX_INVALID_MODE;    break;
    }

    if ((decMode | MBOX_AMR122_DEC_MODE) == MBOX_AMR122_DEC_MODE)
    {
        // if the decoder mode is AMR
        switch (enc)
        {
            case SPP_MR475_MODE:        encMode = MBOX_AMR475_ENC_MODE; break;
            case SPP_MR515_MODE:        encMode = MBOX_AMR515_ENC_MODE; break;
            case SPP_MR59_MODE:         encMode = MBOX_AMR59_ENC_MODE;  break;
            case SPP_MR67_MODE:         encMode = MBOX_AMR67_ENC_MODE;  break;
            case SPP_MR74_MODE:         encMode = MBOX_AMR74_ENC_MODE;  break;
            case SPP_MR795_MODE:        encMode = MBOX_AMR795_ENC_MODE; break;
            case SPP_MR102_MODE:        encMode = MBOX_AMR102_ENC_MODE; break;
            case SPP_MR122_MODE:
            default:                    encMode = MBOX_AMR122_ENC_MODE; break;
        }
    }
    else
    {
        // else the encoder and decoder modes are the same
        encMode = MBOX_INVALID_MODE;
    }

    return (encMode & decMode);

}

//=============================================================================
// spc_UpdateSpeechDec()
//-----------------------------------------------------------------------------
/// This function updates the context speech variables for VOC
//=============================================================================
PUBLIC VOID spc_UpdateSpeechDec(SPC_RX_LOCALS_T *spRx)
{
    UINT16 *Addr16;
    UINT8 voc_bfi;
    UINT8 i;

    SPC_PROFILE_FUNCTION_ENTER(spc_UpdateSpeechDec);
    // update parameters for VOC : voc_sid
    spRx->voc_sid = g_spcCtx->NBlock_result.Voc_Sid;
    spRx->voc_taf=0;
    spRx->voc_bfi=0;

    if (RATE(g_spcCtx->ChMode) == FR_RATE)
    {
        if (g_spcCtx->AFNmod104==59)
        {
            spRx->voc_taf = 1;
        }
    }
    else
    {
        if (SUBCHANNEL(g_spcCtx->ChMode) == 0)
        {
            if ((g_spcCtx->AFNmod104==6)||(g_spcCtx->AFNmod104==58))
            {
                spRx->voc_taf = 1;
            }
        }
        else
        {
            if ((g_spcCtx->AFNmod104==20)||(g_spcCtx->AFNmod104==72))
            {
                spRx->voc_taf = 1;
            }
        }
        if( g_spcCtx->NBlock_result.BitError > g_spcCtx->UfiThreshold)
        {
            spRx->voc_ufi=0x1;
            if(g_spcCtx->NBlock_result.Voc_Sid==1)
            {
                spRx->voc_bfi=1;
            }
        }
    }
    // update parameters for VOC : voc_bfi
    spc_VocBfiFilter(spRx);

    if(SPEECH(g_spcCtx->ChMode)==AMR_CODEC)
    {
        voc_bfi=g_spcCtx->NBlock_result.Bfi;
    }
    else
    {
        voc_bfi=spRx->voc_bfi;
    }

    // copy the results in speech structure
    g_mailbox.spc2pal.speechDecIn.dtxOn = g_spcCtx->voc_dtx_en;
    g_mailbox.spc2pal.speechDecIn.codecMode =
        spc_SppToMboxSpeechMode(g_spcCtx->TxCodecMode,g_spcCtx->RxCodecMode);
    g_mailbox.spc2pal.speechDecIn.decFrameType = spRx->RxFrameType;
    g_mailbox.spc2pal.speechDecIn.bfi = voc_bfi;
    g_mailbox.spc2pal.speechDecIn.sid = spRx->voc_sid;
    g_mailbox.spc2pal.speechDecIn.taf = spRx->voc_taf;
    g_mailbox.spc2pal.speechDecIn.ufi = spRx->voc_ufi;
    Addr16 = (UINT16*) spRx->BlockDecodedAdd;
    for (i=0; i<MBOX_MAX_SPEECH_FRAME_SIZE; i++)
    {
        g_mailbox.spc2pal.speechDecIn.decInBuf[i] = Addr16[i];
    }
    // copy the Tx struct in BB context for the next speech encoding
    g_spcCtx->speechEncOut = g_mailbox.pal2spc.speechEncOut;

    SPC_PROFILE_FUNCTION_EXIT(spc_UpdateSpeechDec);
}

//=============================================================================
// spc_CheckLoopMode()
//-----------------------------------------------------------------------------
/// This function close and open the TCH test loop mode
//=============================================================================
PUBLIC VOID spc_CheckLoopMode(SPC_RX_LOCALS_T *spRx)
{
    UINT8 i;
    UINT32* BlockDecodedAdd=g_spcCtx->NBlockDataOut[0];
    SPC_PROFILE_FUNCTION_ENTER(spc_CheckLoopMode);
    // test for loop A, D, E and F
    if ((g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_A) ||
            (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_B) ||
            (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_D) ||
            (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_E) ||
            (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_F) ||
            (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_I) )
    {
        UINT8 zeros = 0;
        switch (g_mailbox.pal2spc.statWin.loopMode)
        {
            case MBOX_LOOP_CLOSE_A:
                if (g_spcCtx->NBlock_result.Bfi)
                {
                    zeros = 1;
                }
                break;
            case MBOX_LOOP_CLOSE_D:
                if ((spRx->voc_bfi) || (spRx->voc_ufi) || (spRx->StealingResult))
                {
                    zeros = 1;
                }
                break;
            case MBOX_LOOP_CLOSE_E:
                if (spRx->voc_sid == 0)
                {
                    zeros = 1;
                }
                break;
            case MBOX_LOOP_CLOSE_F:
                if ((spRx->voc_bfi) || (spRx->voc_ufi) || (spRx->StealingResult) || (spRx->voc_sid != 2))
                {
                    zeros = 1;
                }
                break;
            case MBOX_LOOP_CLOSE_I:
                if (spRx->StealingResult)
                {
                    zeros = 1;
                }
                else
                {
                    for (i = 0; i < 14; i++)
                    {
                        BlockDecodedAdd[i] = 0x55555555;
                    }

                }
                break;
            default:
                break;
        }
        if (zeros)
        {
            for (i = 0; i < 14; i++)
            {
                BlockDecodedAdd[i] = 0;
            }
        }
        else
        {
            if(g_mailbox.pal2spc.statWin.loopMode != MBOX_LOOP_CLOSE_I)
            {
                for (i = 0; i < 14; i++)
                {
                    BlockDecodedAdd[i] = spRx->BlockDecodedAdd[i];
                }
            }
        }
        g_spcCtx->loop_bufIdx = 0 ;
    }
    SPC_PROFILE_FUNCTION_EXIT(spc_CheckLoopMode);
}

//=============================================================================
// spc_CchDecDoneIrq()
//-----------------------------------------------------------------------------
/// This function generate CCH dec done irq
//=============================================================================
PRIVATE VOID spc_CchDecDoneIrq()
{
    if(g_spcCtx->statWinRxCfg[0].itr &
            MBOX_DSP_CCH_DECODING_COMPLETE)
    {
        spal_ComregsSetIrq(MBOX_DSP_CCH_DECODING_COMPLETE);
    }
}


//=============================================================================
// spc_CCHRxProcess(SPC_RX_LOCALS_T *spRx)
//-----------------------------------------------------------------------------
/// This function processes the Normal Burst Equalization for Control Channel
//=============================================================================
PUBLIC VOID spc_CCHRxProcess(SPC_RX_LOCALS_T *spRx)
{
    // default value set for SACCH
    UINT32 *ItlvOutput=(UINT32*)(g_spcCtx->Malloc) ;
    // default value set for SACCH
    UINT32* SoftBitAddress=g_spcCtx->ItlvBufRxCtrl;
    UINT32 *EqualizerInput;
    UINT32 *EqualizerOutput;
    UINT8 BuffIdx = spRx->buffIdx+spRx->slotIdx;
    UINT8 i=0;

#if (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
    UINT8 decodeFlagTmp = 0;
    UINT32 *ItlvOutputSaic = ((UINT32 *) &g_spcStaticBufTabSaic[0]);
    UINT32* SoftBitAddressSaic = g_spcCtx->ItlvBufRxCtrlSaic;
    UINT32* EqualizerOutputSaic  = g_spcCtx->ItlvBufRxCtrlSaic;
#endif


    SPC_PROFILE_FUNCTION_ENTER(spc_CCHRxProcess);

    if (g_spcCtx->DedicatedActive==FALSE)
    {
        SoftBitAddress = g_spcCtx->ItlvBufRxCtrl+BuffIdx*SPC_ITLV_RX_BUFF_OFFSET;

#if  (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
        if((BuffIdx < 4)&&(g_spcCtx->IsSaicEnable))
        {
            SoftBitAddressSaic = g_spcCtx->ItlvBufRxCtrlSaic+BuffIdx*SPC_ITLV_RX_BUFF_OFFSET;
        }
#endif
    }

    g_spcCtx->NBurst_result.Dco.i = 0;
    g_spcCtx->NBurst_result.Dco.q = 0;



    //Adjacent Channel Interference detection

    //----------------------------- EQUALIZE  ---------------------------------


#if (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)

    if(g_spcCtx->IsSaicEnable)
    {
        EqualizerInput = (UINT32 *) &(g_spcStaticBufTab.RxIfcBuffer[spRx->slotIdx*
                                      2*(BB_BURST_ACTIVE_SIZE+2*BB_RX_NBURST_MARGIN)]);

        for(i = 0; i < BB_BURST_TOTAL_SIZE; i += 4)
        {
            g_spcStaticBufTabSaic[i+1] = EqualizerInput[2*i];
            g_spcStaticBufTabSaic[i + 159] = EqualizerInput[2*i + 1];
            g_spcStaticBufTabSaic[i+2] = EqualizerInput[2*i+2];
            g_spcStaticBufTabSaic[i + 160] = EqualizerInput[2*i + 3];
            g_spcStaticBufTabSaic[i+3] = EqualizerInput[2*i + 4];
            g_spcStaticBufTabSaic[i + 161] = EqualizerInput[2*i + 5];
            g_spcStaticBufTabSaic[i+4] = EqualizerInput[2*i + 6];
            g_spcStaticBufTabSaic[i + 162] = EqualizerInput[2*i + 7];
        }
        EqualizerInput = (UINT32 *) &g_spcStaticBufTabSaic[0];
    }
    else
    {

        EqualizerInput=(UINT32 *) &(g_spcStaticBufTab.RxIfcBuffer[spRx->slotIdx*1*
                                    (BB_BURST_ACTIVE_SIZE+2*BB_RX_NBURST_MARGIN)]);
    }

#else

    EqualizerInput=(UINT32 *) &(g_spcStaticBufTab.RxIfcBuffer[spRx->slotIdx*
                                (BB_BURST_ACTIVE_SIZE+2*BB_RX_NBURST_MARGIN)]);


#endif

    EqualizerOutput = (UINT32 *) (SoftBitAddress + spRx->burstIdx * 32);

#if  (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
    if((BuffIdx < 4)&&(g_spcCtx->IsSaicEnable))
    {
        EqualizerOutputSaic = (UINT32 *) (SoftBitAddressSaic + spRx->burstIdx * 32);
    }
#endif


#if (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
    if (g_spcCtx->IsSaicEnable)
    {
        spp_EqualizeNBurstSaic(EqualizerInput,
                               g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.nBurst.tsc,
                               EqualizerOutput,
                               EqualizerOutputSaic,
                               (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                               &g_spcCtx->NBurst_result);
    }
    else
    {
        spp_EqualizeNBurst(EqualizerInput,
                           g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.nBurst.tsc,
                           EqualizerOutput,
                           (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                           &g_spcCtx->NBurst_result);
    }


#else
    spp_EqualizeNBurst(EqualizerInput,
                       g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.nBurst.tsc,
                       EqualizerOutput,
                       (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                       &g_spcCtx->NBurst_result);
#endif

    g_spcCtx->Rx_q_CCH[spRx->slotIdx][ spRx->burstIdx << 1     ] =
        ((EqualizerOutput[28]) >> 16) & 0x0FF;
    g_spcCtx->Rx_q_CCH[spRx->slotIdx][(spRx->burstIdx << 1) + 1] =
        ( EqualizerOutput[28]) >> 24;

    ////////////////////////////////////////////////////////
    // check the dummy PCH burst detection is enabled
    if (g_spcCtx->statWinRxCfg[BuffIdx].itr&MBOX_DSP_PCH_READY)
    {
        UINT8 result=0;
        result = spp_DetectDummyPCH(EqualizerOutput,
                                    spRx->burstIdx,
                                    ((g_mailbox.dummyPchThresh)&0xFF00)>>8,
                                    (g_mailbox.dummyPchThresh)&0xFF);

        if (g_spcCtx->NBurst_result.Snr <= (g_spcCtx->DummySnrThreshold))
            result = SPP_DUMMY_PCH_NOT_DETECTED;
        g_mailbox.dummyPchResult = (UINT32) result;
        spal_ComregsSetIrq(MBOX_DSP_PCH_READY);
    }
    ////////////////////////////////////////////////////////

    //----------------------------- EQUALIZE  ---------------------------------
    //----------------------------  RESULTS  ----------------------------------

    UINT8 bstSnr = (UINT8)(g_spcCtx->NBurst_result.Snr>=0)?g_spcCtx->NBurst_result.Snr:0;
    if (spRx->slotIdx==0)
    {
        g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.tof=
            (INT8)((int) g_spcCtx->NBurst_result.TOf - 4*BB_RX_NBURST_MARGIN);
        g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.fof=
            g_spcCtx->NBurst_result.FOf;
        g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.snr=bstSnr;
        g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.pwr=
            g_spcCtx->NBurst_result.Pwr;
#ifdef CHIP_DIE_8955
        g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.dco_I = g_spcCtx->NBurst_result.Dco.i;
        g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.dco_Q = g_spcCtx->NBurst_result.Dco.q;
#endif
    }
    else// save multislot pwr and snr
    {
#ifdef CHIP_DIE_8955
        g_mailbox.spc2pal.bstSnr[4*g_spcCtx->currentSnap + spRx->slotIdx] = bstSnr;
        g_mailbox.spc2pal.pwrs[4*g_spcCtx->currentSnap + spRx->slotIdx] =
            g_spcCtx->NBurst_result.Pwr;
        // Only report the first three dc value.
        if (spRx->slotIdx <= 2)
        {
            g_mailbox.spc2pal.multiDcI[2*g_spcCtx->currentSnap + spRx->slotIdx-1] =
                g_spcCtx->NBurst_result.Dco.i;
            g_mailbox.spc2pal.multiDcQ[2*g_spcCtx->currentSnap + spRx->slotIdx-1] =
                g_spcCtx->NBurst_result.Dco.q;
        }
#endif

    }
    //----------------------------  RESULTS  ----------------------------------
    //---------------------------- CIPHERING ----------------------------------
    if (g_mailbox.pal2spc.statWin.ciphMode!=NO_CIPHER)
    {
        spp_CipherDecipherNb(EqualizerOutput,EqualizerOutput);

#if  (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
        if((BuffIdx < 4)&&(g_spcCtx->IsSaicEnable))
        {
            spp_CipherDecipherNb(EqualizerOutputSaic,EqualizerOutputSaic);
        }
#endif

    }
    //---------------------------- CIPHERING ----------------------------------
    //----------------------------  DECODE  -----------------------------------
    if (spRx->burstIdx == 3)
    {
        g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].data[0] =
            (UINT8 *) g_spcCtx->NBlockDataOut[BuffIdx];
        g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].data[1] = NULL;

        // initialize the output buffer to avoid bit error due to tail bits
        for(i=0; i<14; i++)
        {
            g_spcCtx->NBlockDataOut[BuffIdx][i]=0;
        }
        // initialize the block decoding output address
        spRx->BlockDecodedAdd = g_spcCtx->NBlockDataOut[BuffIdx];

        spRx->ItlvBuffInput = g_spcCtx->ItlvBufRxCtrl;
        // initialize the usfs
        if (spRx->slotIdx == 0)
        {
            g_mailbox.spc2pal.statWin.rx.decodedUsf[0]=0xFF;
            g_mailbox.spc2pal.statWin.rx.decodedUsf[1]=0xFF;
            g_mailbox.spc2pal.statWin.rx.decodedUsf[2]=0xFF;
            g_mailbox.spc2pal.statWin.rx.decodedUsf[3]=0xFF;
        }
        // decode Coding Scheme for the current slot
        spp_DecodeStealingBits((UINT32 *) (g_spcCtx->Rx_q_CCH[spRx->slotIdx]),
                               1,
                               &(g_spcCtx->Rx_CS[spRx->slotIdx]),
                               0);
        // default value for SACCH
        spRx->Copy_ChMode = CCH_mode + (GSM_CS1<<8);

        //--------------------------  DEINTERLEAVE  ---------------------------
        if (g_spcCtx->DedicatedActive==FALSE)
        {
            if (spRx->slotIdx == 0)
            {
                ItlvOutput=(UINT32*)(g_spcCtx->ItlvBufRxCtrl +
                                     SPC_TMP_ITLV_RX_BUFF_OFFSET);
            }
            else if(spRx->slotIdx == g_spcCtx->sched_rx_slot_nb-1)
            {
                ItlvOutput=(UINT32*)(g_spcCtx->Malloc);
            }
            else
            {
                ItlvOutput=(UINT32*)(g_spcCtx->ItlvBufRxCtrl +
                                     (spRx->slotIdx-1)*SPC_ITLV_RX_BUFF_OFFSET);
            }
        }
        spp_DeinterleaveBlock(SoftBitAddress,
                              ITLV_TYPE_1B,
                              0,
                              (UINT32*)ItlvOutput);


#if  (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
        if((BuffIdx < 4)&&(g_spcCtx->IsSaicEnable))
        {
            if (g_spcCtx->DedicatedActive==FALSE)
            {
                if (spRx->slotIdx == 0)
                {
                    ItlvOutputSaic = (UINT32*)(g_spcCtx->ItlvBufRxCtrlSaic + 4*SPC_ITLV_RX_BUFF_OFFSET);
                }
                else
                {
                    ItlvOutputSaic = (UINT32*)(g_spcCtx->ItlvBufRxCtrlSaic +
                                               (spRx->slotIdx-1)*SPC_ITLV_RX_BUFF_OFFSET);
                }
            }
            spp_DeinterleaveBlock(SoftBitAddressSaic,
                                  ITLV_TYPE_1B,
                                  0,
                                  (UINT32*)ItlvOutputSaic);
        }
#endif

        //--------------------------  DEINTERLEAVE  ---------------------------

        g_mailbox.spc2pal.statWin.rx.decodedUsf[spRx->slotIdx] =
            spp_GetUSF((UINT32*)ItlvOutput,
                       g_spcCtx->Rx_CS[spRx->slotIdx]);

        if(g_spcCtx->sched_slot_count==g_spcCtx->sched_rx_slot_nb)
        {
            UINT8 k=0;

            // send an it to the XCPU if the last slot has been received
            if (g_spcCtx->statWinRxCfg[BuffIdx].itr&MBOX_DSP_USF_READY)
            {
                spal_ComregsSetIrq(MBOX_DSP_USF_READY);
            }
            for (k=0; k<g_spcCtx->sched_rx_slot_nb; k++)
            {
                BuffIdx = spRx->buffIdx+k;
                if (g_spcCtx->DedicatedActive==FALSE)
                {
                    if (k == 0)
                    {
                        ItlvOutput=(UINT32*)(g_spcCtx->ItlvBufRxCtrl +
                                             SPC_TMP_ITLV_RX_BUFF_OFFSET);
                    }
                    else if(k == g_spcCtx->sched_rx_slot_nb-1)
                    {
                        ItlvOutput=(UINT32*)(g_spcCtx->Malloc);
                    }
                    else
                    {
                        ItlvOutput=(UINT32*)(g_spcCtx->ItlvBufRxCtrl +
                                             (k-1)*SPC_ITLV_RX_BUFF_OFFSET);
                    }
                    spRx->Copy_ChMode=CCH_mode +
                                      ((g_spcCtx->Rx_CS[k])<<8);
                    g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].data[0] =
                        (UINT8 *) g_spcCtx->NBlockDataOut[BuffIdx];
                    spRx->BlockDecodedAdd=g_spcCtx->NBlockDataOut[BuffIdx];
                }


                spp_DecodeBlock((UINT32*)ItlvOutput,
                                spRx->Copy_ChMode,
                                0,
                                0,
                                spRx->BlockDecodedAdd,
                                (g_mailbox.spc2pal.statWin.rx.decodedUsf[k])&0x7,
                                (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                                &g_spcCtx->NBlock_result);


#if  (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
                if( (g_spcCtx->NBlock_result.Bfi&0x1) != 0)
                {
                    if((BuffIdx < 4)&&(g_spcCtx->IsSaicEnable))
                    {
                        if (g_spcCtx->DedicatedActive==FALSE)
                        {
                            if (k == 0)
                            {
                                ItlvOutputSaic = (UINT32*)(g_spcCtx->ItlvBufRxCtrlSaic + 4*SPC_ITLV_RX_BUFF_OFFSET);
                            }
                            else
                            {
                                ItlvOutputSaic = (UINT32*)(g_spcCtx->ItlvBufRxCtrlSaic +
                                                           (k-1)*SPC_ITLV_RX_BUFF_OFFSET);
                            }
                            spRx->Copy_ChMode=CCH_mode +
                                              ((g_spcCtx->Rx_CS[k])<<8);
                            g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].data[0] =
                                (UINT8 *) g_spcCtx->NBlockDataOut[BuffIdx];
                            spRx->BlockDecodedAdd=g_spcCtx->NBlockDataOut[BuffIdx];
                        }

                        spp_DecodeBlock((UINT32*)ItlvOutputSaic,
                                        spRx->Copy_ChMode,
                                        0,
                                        0,
                                        spRx->BlockDecodedAdd,
                                        (g_mailbox.spc2pal.statWin.rx.decodedUsf[k])&0x7,
                                        (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                                        &g_spcCtx->NBlock_result);
                        if ((g_spcCtx->NBlock_result.Bfi&0x1) == 0)
                        {
                            decodeFlagTmp = 1;
                        }
                    }
                    else
                    {
                        decodeFlagTmp = 0;
                    }
                }
#endif
                // dump the decoder output bits if asked
                if ((g_spcCtx->dumpMask==SPC_DBG_DUMP_GSM_CCH_DEC_OUT)&&
                        (g_spcCtx->dumpPtr!=NULL))
                {
                    ((VOID(*)(UINT16*,UINT16,UINT16))g_spcCtx->dumpPtr)(
                        (UINT16*)spRx->BlockDecodedAdd,
                        28,
                        SPC_DBG_DUMP_DEC_OUT_MARKER);
                }

                spc_CchDecDoneIrq();

#if (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)

                if(g_spcCtx->IsSaicEnable)
                {
                    if (decodeFlagTmp)
                    {
                        g_mailbox.spc2pal.decodeFlag |= (1 << BuffIdx);
                    }
                    else
                    {
                        g_mailbox.spc2pal.decodeFlag &= (~(1 << BuffIdx));
                    }
                }
                else
                {
                    g_mailbox.spc2pal.decodeFlag = 0;
                }
#endif
                g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].meanBep =
                    g_spcCtx->NBlock_result.MeanBEP;
                g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].cvBep =
                    g_spcCtx->NBlock_result.CvBEP;
                g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].bitError =
                    g_spcCtx->NBlock_result.BitError;
                g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].bitTotal =
                    g_spcCtx->NBlock_result.BitTotal;
                g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].bfi =
                    g_spcCtx->NBlock_result.Bfi & 0x01;
                g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].cs =
                    g_spcCtx->Rx_CS[k];
                g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].sid =
                    g_spcCtx->NBlock_result.Voc_Sid |
                    (spRx->RxFrameType==SPP_RX_SID_UPDATE);

            }
            // reset the Rx Ctrl buffer to allow decoding with only two bursts
            for (i=0; i<4*32; i++) g_spcCtx->ItlvBufRxCtrl[i]=0;

#if  (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
            if(g_spcCtx->IsSaicEnable)
            {
                for (i=0; i<4*32; i++) g_spcCtx->ItlvBufRxCtrlSaic[i]=0;
            }

#endif

        }
    }

    //----------------------------  DECODE  -----------------------------------
    SPC_PROFILE_FUNCTION_EXIT(spc_CCHRxProcess);
}


//=============================================================================
// spc_TCHRxProcess(SPC_RX_LOCALS_T *spRx)
//-----------------------------------------------------------------------------
/// This function processes the Normal Burst Equalization for Control Channel
//=============================================================================
PUBLIC VOID spc_TCHRxProcess(SPC_RX_LOCALS_T *spRx)
{
#define DECODE_SPEECH   1
#define DECODE_DATA     2
#define WRITE_DATA      3

    UINT32 StealingSoftBits[2]= {0,0};
    UINT32 *EqualizerOutput;
    UINT8 RxBurstOffset;
    UINT8 i;
    UINT8 ItlvBufLength=8;
    UINT8 Mask=0x3;

#if (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
    UINT8 EqSaicSoft[116];
    UINT8 decodeFlagTmp = 0;
    UINT32* EqualizerOutputSaic = (UINT32*) (g_spcCtx->ItlvBufRxCtrlSaic + SPC_ITLV_BUFF_RX_CTRL);
    UINT32* ItlvBuffInputSaic  = (UINT32*)(g_spcCtx->ItlvBufRxCtrlSaic + SPC_ITLV_BUFF_RX_CTRL);
    UINT32 *EqualizerInput = NULL;
    UINT8* EqSaicPtr = (UINT8 *)EqualizerOutputSaic;
#endif


    SPC_PROFILE_FUNCTION_ENTER(spc_TCHRxProcess);

    // DTX is actually done here, except for HBURST equalization
    // because this would clear the PDN and the burst would not
    // be entirely received.
    if ((g_spcCtx->Tx_off==TRUE) && (!g_spcCtx->equ_hburst_mode))
    {
        spal_TcuDisableEvent(g_mailbox.pal2spc.rf.dtxTcoSettings);
    }

    if (RATE(g_spcCtx->ChMode) == HR_RATE)
    {
        ItlvBufLength=6;
        Mask=0x1;

    }
    else if(MODE(g_spcCtx->ChMode) == TCH_DATA)
    {
        ItlvBufLength=22;
    }

    //------------------------------ EQUALIZE ----------------------------------
    RxBurstOffset = g_spcCtx->RxBlockOffset + (spRx->burstIdx & Mask);
    if (RxBurstOffset >= ItlvBufLength)
    {
        RxBurstOffset -= ItlvBufLength;    //test for data interleaving
    }
    EqualizerOutput = (UINT32*) (g_spcCtx->ItlvBufRxDedicated +
                                 RxBurstOffset * 32);

#if (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
    if(g_spcCtx->IsSaicEnable)
    {
        EqualizerOutputSaic = (UINT32*) (g_spcCtx->ItlvBufRxCtrlSaic + SPC_ITLV_BUFF_RX_CTRL +
                                         RxBurstOffset * 32);
    }
#endif

    g_spcCtx->NBurst_result.Dco.i = 0;
    g_spcCtx->NBurst_result.Dco.q = 0;

#if (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
    if(g_spcCtx->IsSaicEnable)
    {
        EqualizerInput = (UINT32 *) &(g_spcStaticBufTab.RxIfcBuffer[spRx->slotIdx]);

        for(i = 0; i < BB_BURST_TOTAL_SIZE; i += 4)
        {
            g_spcStaticBufTabSaic[i+1] = EqualizerInput[2*i];
            g_spcStaticBufTabSaic[i + 159] = EqualizerInput[2*i + 1];
            g_spcStaticBufTabSaic[i+2] = EqualizerInput[2*i+2];
            g_spcStaticBufTabSaic[i + 160] = EqualizerInput[2*i + 3];
            g_spcStaticBufTabSaic[i+3] = EqualizerInput[2*i + 4];
            g_spcStaticBufTabSaic[i + 161] = EqualizerInput[2*i + 5];
            g_spcStaticBufTabSaic[i+4] = EqualizerInput[2*i + 6];
            g_spcStaticBufTabSaic[i + 162] = EqualizerInput[2*i + 7];
        }
        EqualizerInput = (UINT32 *) &g_spcStaticBufTabSaic[0];

        spp_EqualizeNBurstSaic(EqualizerInput,
                               g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.nBurst.tsc,
                               EqualizerOutput,
                               EqualizerOutputSaic,
                               (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                               &g_spcCtx->NBurst_result);

        EqSaicPtr = (UINT8 *)EqualizerOutputSaic;
        for(i = 0; i < 116; i++)
        {
            EqSaicSoft[i] = EqSaicPtr[i];
        }

    }
    else
    {
        spp_EqualizeNBurst((UINT32 *) &(g_spcStaticBufTab.RxIfcBuffer[spRx->slotIdx]),
                           g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.nBurst.tsc,
                           EqualizerOutput,
                           (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                           &g_spcCtx->NBurst_result);
    }

#else
    spp_EqualizeNBurst((UINT32 *) &(g_spcStaticBufTab.RxIfcBuffer[spRx->slotIdx]),
                       g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.nBurst.tsc,
                       EqualizerOutput,
                       (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                       &g_spcCtx->NBurst_result);
#endif


    // Store Stealing Flag
    g_spcCtx->Rx_Hu_TCH_N <<= 8;
    g_spcCtx->Rx_Hl_TCH_N <<= 8;
    g_spcCtx->Rx_Hu_TCH_N |= ( EqualizerOutput[28]) >> 24;
    g_spcCtx->Rx_Hl_TCH_N |= ((EqualizerOutput[28]) >> 16) & 0x0FF;


    if (spRx->burstIdx == 0)
    {
        g_spcCtx->BlockSnR  = g_spcCtx->NBurst_result.Snr;
    }
    else
    {
        g_spcCtx->BlockSnR += g_spcCtx->NBurst_result.Snr;
    }

    if (SPEECH(g_spcCtx->ChMode)==AMR_CODEC)
    {
        INT32 QI=0;
        INT16 *filter=(RATE(g_spcCtx->ChMode)==FR_RATE)?
                      (INT16*)AFSfilterCI:(INT16*)AHSfilterCI;
        UINT8 length=(RATE(g_spcCtx->ChMode)==FR_RATE)?101:51;
        for(i=1; i<=length-1; i++)
        {
            g_spcCtx->CIest[length-i]=g_spcCtx->CIest[length-i-1];
        }
        g_spcCtx->CIest[0]=g_spcCtx->NBurst_result.Snr;
        for(i=0; i<length; i++)
        {
            QI+=filter[i]*g_spcCtx->CIest[i];
        }
        if (QI<0)
        {
            QI=0;
        }
        spc_UpdateCMR(g_spcCtx->AMR_Cfg_Rx,
                      (UINT16)(QI>>15),
                      &g_spcCtx->CodecModeRequest);
    }

    g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.sid =
        (g_spcCtx->sidupdate_ctr==2)?1:0;

    //------------------------------ EQUALIZE ----------------------------------
    //----------------------------- LOOPBACK C ---------------------------------
    g_spcCtx->loopC_buffer = (UINT8 *) EqualizerOutput;
    //----------------------------- LOOPBACK C ---------------------------------
    //-----------------------------  RESULTS  ----------------------------------
    g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.tof=
        (INT8)((int) g_spcCtx->NBurst_result.TOf - 4*BB_RX_NBURST_MARGIN);
    g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.fof=
        g_spcCtx->NBurst_result.FOf;
    g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.snr=
        (UINT8)(g_spcCtx->NBurst_result.Snr>=0)?g_spcCtx->NBurst_result.Snr:0;
    g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.pwr=
        g_spcCtx->NBurst_result.Pwr;
#ifdef CHIP_DIE_8955
    g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.dco_I = g_spcCtx->NBurst_result.Dco.i;
    g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.dco_Q = g_spcCtx->NBurst_result.Dco.q;
#endif

    //-----------------------------  RESULTS  ----------------------------------
    //----------------------------- CIPHERING ----------------------------------
    if(g_mailbox.pal2spc.statWin.ciphMode!=NO_CIPHER)
    {
        spp_CipherDecipherNb(EqualizerOutput,EqualizerOutput);

#if (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
        if(g_spcCtx->IsSaicEnable)
        {
            spp_CipherDecipherNb(EqualizerOutputSaic,EqualizerOutputSaic);
        }
#endif

    }
    //----------------------------- CIPHERING ----------------------------------
    //------------------------------ DECODE ------------------------------------
    if (RATE(g_spcCtx->ChMode) == FR_RATE)
    {
        if (spRx->burstIdx == 3)
        {
            spRx->RX_decode=DECODE_SPEECH;
            spRx->itlv_mode=ITLV_TYPE_1A;
            g_spcCtx->RxBlockOffset += 4;
            spRx->RxBuffOffset = g_spcCtx->RxBlockOffset;
            if (g_spcCtx->RxBlockOffset >= ItlvBufLength)
            {
                g_spcCtx->RxBlockOffset -= ItlvBufLength;
            }
        }
        if(MODE(g_spcCtx->ChMode) == TCH_DATA)
        {
            if(spRx->burstIdx == 1)
            {
                spRx->RX_decode=DECODE_DATA;
                spRx->itlv_mode=ITLV_TYPE_3;
                spRx->ItlvBuffInput = g_spcCtx->ItlvBufRxDedicated;

#if (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
                if(g_spcCtx->IsSaicEnable)
                {
                    ItlvBuffInputSaic = g_spcCtx->ItlvBufRxCtrlSaic + SPC_ITLV_BUFF_RX_CTRL;
                }
#endif
                spRx->RxBuffOffset = g_spcCtx->RxBlockOffset + 2;
                if (spRx->RxBuffOffset == ItlvBufLength)
                {
                    spRx->RxBuffOffset = 0;
                }
                spRx->BlockDecodedAdd = g_spcCtx->CSD_NBlockDataOut;
                for(i=0; i<14; i++)
                {
                    g_spcCtx->CSD_NBlockDataOut[i]=0;
                }
            }
        }
    }
    else
    {
        if ((spRx->burstIdx & Mask) == 1)
        {
            spRx->RX_decode=DECODE_SPEECH;
            g_spcCtx->RxBlockOffset += 2;
            if (g_spcCtx->RxBlockOffset == ItlvBufLength)
            {
                g_spcCtx->RxBlockOffset = 0;
            }
            spRx->RxBuffOffset = g_spcCtx->RxBlockOffset + 2;
            if (spRx->RxBuffOffset == ItlvBufLength)
            {
                spRx->RxBuffOffset = 0;
            }
            spRx->itlv_mode=ITLV_TYPE_2A;
        }
    }

    if (spRx->RX_decode==DECODE_SPEECH)
    {
        g_mailbox.spc2pal.statWin.rx.nBlock[spRx->buffIdx].data[0] = NULL;
        g_mailbox.spc2pal.statWin.rx.nBlock[spRx->buffIdx].data[1] = NULL;

        // initialize the output buffer to avoid bit error due to tail bits
        for(i=0; i<14; i++)
        {
            g_spcCtx->NBlockDataOut[spRx->buffIdx][i]=0;
        }
        // initialize the block decoding output address
        spRx->BlockDecodedAdd = g_spcCtx->NBlockDataOut[spRx->buffIdx];

        if(MODE(g_spcCtx->ChMode) == TCH_DATA)
        {
            spRx->RX_decode=WRITE_DATA;
            g_mailbox.spc2pal.statWin.rx.nBlock[spRx->buffIdx].data[1] =
                (UINT8 *) g_spcCtx->CSD_NBlockDataOut;
        }
        spRx->ItlvBuffInput = g_spcCtx->ItlvBufRxDedicated;

#if (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
        if(g_spcCtx->IsSaicEnable)
        {
            ItlvBuffInputSaic = g_spcCtx->ItlvBufRxCtrlSaic + SPC_ITLV_BUFF_RX_CTRL;
        }
#endif

        StealingSoftBits[0] = g_spcCtx->Rx_Hu_TCH_N_1;
        StealingSoftBits[1] = g_spcCtx->Rx_Hl_TCH_N;
        g_spcCtx->Rx_Hu_TCH_N_1 = g_spcCtx->Rx_Hu_TCH_N;
        spp_DecodeStealingBits(StealingSoftBits,
                               0,
                               &(spRx->StealingResult),
                               g_spcCtx->FacchThreshold);
        if ((spRx->StealingResult)&&(spRx->burstIdx == 3))
        {
            if(RATE(g_spcCtx->ChMode) == HR_RATE)
            {
                spRx->RxBuffOffset = g_spcCtx->RxBlockOffset;
                spRx->itlv_mode=ITLV_TYPE_1C;
            }
            else if(MODE(g_spcCtx->ChMode) == TCH_DATA)
            {
                spRx->RxBuffOffset = g_spcCtx->RxBlockOffset;
                spc_SetFacch(spRx);
            }

            spRx->Copy_ChMode = CCH_mode + (GSM_CS1<<8);
            // force CS to GSM_CS1 in case of FACCH
            g_spcCtx->Rx_CS[0]=GSM_CS1;
            g_mailbox.spc2pal.statWin.rx.nBlock[spRx->buffIdx].data[0] =
                (UINT8 *) g_spcCtx->NBlockDataOut[spRx->buffIdx];
            spRx->RX_decode=DECODE_SPEECH;
        }
    }
    if(spRx->RX_decode)
    {

        if (SPEECH(g_spcCtx->ChMode)==AMR_CODEC)
        {
            spc_AmrRatscch();
            if ((SPEECH(g_spcCtx->ChMode)==AMR_CODEC)&&
                    (spRx->StealingResult==0))
            {
                spc_AmrDecode(spRx);
                spRx->RX_decode=0;
            }
        }

        if((spRx->RX_decode==DECODE_SPEECH) || (spRx->RX_decode==DECODE_DATA))
        {
            // Deinterleave the current block
            spp_DeinterleaveBlock(spRx->ItlvBuffInput,
                                  spRx->itlv_mode,
                                  spRx->RxBuffOffset,
                                  (UINT32*)g_spcCtx->Malloc);

            spp_DecodeBlock((UINT32*)g_spcCtx->Malloc,
                            spRx->Copy_ChMode,
                            0,
                            0,
                            spRx->BlockDecodedAdd,
                            (g_mailbox.spc2pal.statWin.rx.decodedUsf[0])&0x7,
                            (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                            &g_spcCtx->NBlock_result);
#ifdef CHIP_DIE_8955
            if ((g_spcCtx->NBlock_result.Bfi&0x1) == 0)
            {
                decodeFlagTmp = 0;
            }
#endif
        }
#ifdef CHIP_DIE_8955
        else
        {
            decodeFlagTmp = 0;
        }
#endif

#if (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
        if((spRx->RX_decode==DECODE_SPEECH) || (spRx->RX_decode==DECODE_DATA))
        {
            //g_spcCtx->NBlock_result.Bfi = 1;
            if( (g_spcCtx->NBlock_result.Bfi&0x1) != 0)
            {
                if(g_spcCtx->IsSaicEnable)
                {
                    g_spcCtx->loopC_buffer = (UINT8 *) EqSaicSoft;
                    // initialize the output buffer to avoid bit error due to tail bits
                    for(i=0; i<14; i++)
                    {
                        g_spcCtx->NBlockDataOut[spRx->buffIdx][i]=0;
                    }
// initialize the block decoding output address
                    spRx->BlockDecodedAdd = g_spcCtx->NBlockDataOut[spRx->buffIdx];
                    // Deinterleave the current block
                    spp_DeinterleaveBlock(ItlvBuffInputSaic,
                                          spRx->itlv_mode,
                                          spRx->RxBuffOffset,
                                          (UINT32*)g_spcCtx->Malloc);

                    spp_DecodeBlock((UINT32*)g_spcCtx->Malloc,
                                    spRx->Copy_ChMode,
                                    0,
                                    0,
                                    spRx->BlockDecodedAdd,
                                    (g_mailbox.spc2pal.statWin.rx.decodedUsf[0])&0x7,
                                    (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                                    &g_spcCtx->NBlock_result);

                    if ((g_spcCtx->NBlock_result.Bfi&0x1) == 0)
                    {
                        decodeFlagTmp = 1;
                    }
                }

            }

        }
#endif

#if (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
        if(g_spcCtx->IsSaicEnable)
        {
            if (decodeFlagTmp)
            {
                g_mailbox.spc2pal.decodeFlag |= (1 << spRx->buffIdx);
            }
            else
            {
                g_mailbox.spc2pal.decodeFlag &= (~(1 << spRx->buffIdx));
            }
        }
        else
        {
            g_mailbox.spc2pal.decodeFlag = 0;
        }
#endif

        g_mailbox.spc2pal.statWin.rx.nBlock[spRx->buffIdx].meanBep =
            g_spcCtx->NBlock_result.MeanBEP;
        g_mailbox.spc2pal.statWin.rx.nBlock[spRx->buffIdx].cvBep =
            g_spcCtx->NBlock_result.CvBEP;
        g_mailbox.spc2pal.statWin.rx.nBlock[spRx->buffIdx].bitError =
            g_spcCtx->NBlock_result.BitError;
        g_mailbox.spc2pal.statWin.rx.nBlock[spRx->buffIdx].bitTotal =
            g_spcCtx->NBlock_result.BitTotal;
        g_mailbox.spc2pal.statWin.rx.nBlock[spRx->buffIdx].bfi =
            g_spcCtx->NBlock_result.Bfi;
        g_mailbox.spc2pal.statWin.rx.nBlock[spRx->buffIdx].cs =
            g_spcCtx->Rx_CS[0];
        g_mailbox.spc2pal.statWin.rx.nBlock[spRx->buffIdx].sid =
            g_spcCtx->NBlock_result.Voc_Sid|(spRx->RxFrameType==SPP_RX_SID_UPDATE);



        if(MODE(g_spcCtx->ChMode) == TCH_SPEECH)
        {
            spc_BfiFilter(spRx);
            // Update Mbox Bfi result as it may be modified by the filter
            g_mailbox.spc2pal.statWin.rx.nBlock[spRx->buffIdx].bfi =
                g_spcCtx->NBlock_result.Bfi;
            spc_UpdateSpeechDec(spRx);
        }

        spc_CheckLoopMode(spRx);
    }
    //------------------------------ DECODE ------------------------------------
    SPC_PROFILE_FUNCTION_EXIT(spc_TCHRxProcess);
}

//=============================================================================
// spc_NBurstProcess(SPC_RX_LOCALS_T *spRx)
//-----------------------------------------------------------------------------
/// This function processes the Normal Burst reception
//=============================================================================
PUBLIC VOID spc_NBurstProcess(SPC_RX_LOCALS_T *spRx)
{

    g_spcCtx->NBlock_result.Bfi = 0;
    g_spcCtx->NBlock_result.BitError = 0;
    g_spcCtx->NBlock_result.Voc_Sid = 0;
    g_spcCtx->NBlock_result.Cs = GSM_CS1;    // to avoid pal assert


    switch(g_spcCtx->statWinRxCfg[spRx->buffIdx].type)
    {
        case MBOX_CCH_CHN :
        case MBOX_PDCH_CHN:

            spc_CCHRxProcess(spRx);

            break;
        case MBOX_TCHF_CHN :
        case MBOX_TCHH_CHN :
        case MBOX_FACCH_CHN:
        case MBOX_CDATA_CHN:
            spc_TCHRxProcess(spRx);
            break;
        default:
            break;
    }
    //send an interrupt to XCPu at next FINT
    g_spcCtx->sendEqItr = 1;

}


//=============================================================================
// spc_RxProcess()
//-----------------------------------------------------------------------------
/// This function executes the whole Rx for the current frame
//=============================================================================
PROTECTED VOID spc_RxProcess(VOID)
{
    UINT8 current_type;
    UINT8 winIdx;

    SPC_RX_LOCALS_T spRx =
    {
        .ItlvBuffInput=NULL,
        .BlockDecodedAdd=g_spcCtx->NBlockDataOut[0],
        .burstIdx=g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.nBurst.bstIdx,
        .buffIdx=g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.nBurst.bufIdx,
        // duplicate the sched_slot_count for timing reason
        .slotIdx=g_spcCtx->sched_slot_count,
        .RxBuffOffset=0,
        .itlv_mode=ITLV_TYPE_1B,
        .StealingResult=0,
        .Copy_ChMode=g_spcCtx->ChMode,
        .voc_bfi=0,
        .voc_sid=0,
        .voc_taf=0,
        .voc_ufi=0,
        .RxFrameType=SPP_RX_NO_DATA,
        .RX_decode=0
    };

    SPC_PROFILE_FUNCTION_ENTER(spc_RxProcess);
    //read the rxtype
    if (g_spcCtx->FcchActive == 0)
    {
        current_type = g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.
                       rxTypeWin[g_spcCtx->sched_rx_count];
        if (g_spcCtx->FcchSet==1)
        {
            g_spcCtx->FcchActive=1;
        }
        g_spcCtx->FcchSet=0;
    }
    else
    {
        current_type = GSM_WIN_TYPE_FCCH;
    }
    switch (current_type)
    {
        //--------------------------------------------------------------------
        //           FREQUENCY BURST
        //--------------------------------------------------------------------
        case GSM_WIN_TYPE_FCCH:
            //HAL_W2SET(W2_FCCH);
            SPC_PROFILE_WINDOW_ENTER(FCCH);
            spc_FcchProcess();
            //HAL_W2CLR(W2_FCCH);
            SPC_PROFILE_WINDOW_EXIT(FCCH);
            break;
        //--------------------------------------------------------------------
        //           MONITORING
        //--------------------------------------------------------------------
        case GSM_WIN_TYPE_MONIT:
            //HAL_W2SET(W2_MONITORING);
            SPC_PROFILE_WINDOW_ENTER(MONITORING);
            winIdx = g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.
                     monWinIdx[g_spcCtx->sched_rx_count];
            spc_MonitoringProcess(g_spcCtx->sched_rx_count,winIdx);
            g_spcCtx->sched_rx_count++;
            //HAL_W2CLR(W2_MONITORING);
            SPC_PROFILE_WINDOW_EXIT(MONITORING);
            break;
        //--------------------------------------------------------------------
        //           MONITORING
        //--------------------------------------------------------------------
        case GSM_WIN_TYPE_INTRF:
            // XXX: This will work only if INTRF and MONIT window are not programmed
            // XXX: within the same frame
            //HAL_W2SET(W2_INTRF);
            SPC_PROFILE_WINDOW_ENTER(INTERF);
            // TODO: to save space we only have 3 MonIfcBuffer.
            //       this implies that INTRF process for slot 0 MUST
            //       be finished before slot 3 reception
            spc_MonitoringProcess((g_spcCtx->sched_slot_count % 3),
                                  g_spcCtx->sched_slot_count);
            g_spcCtx->sched_slot_count++;
            if (g_spcCtx->sched_slot_count==g_spcCtx->sched_rx_slot_nb)
            {
                g_spcCtx->sched_rx_count++;
            }
            //HAL_W2CLR(W2_INTRF);
            SPC_PROFILE_WINDOW_EXIT(INTERF);
            break;

        //--------------------------------------------------------------------
        //           SYNCHRO BURST
        //--------------------------------------------------------------------
        case GSM_WIN_TYPE_SCH:
            //HAL_W2SET(W2_SCH);
            SPC_PROFILE_WINDOW_ENTER(SCH);
            spc_SchProcess();
            g_spcCtx->sched_rx_count++;
            //HAL_W2CLR(W2_SCH);
            SPC_PROFILE_WINDOW_EXIT(SCH);
            break;

        //--------------------------------------------------------------------
        //             NORMAL BURST
        //--------------------------------------------------------------------
        case GSM_WIN_TYPE_NBURST:
            //HAL_W2SET(W2_NORMAL_BURST);
            SPC_PROFILE_WINDOW_ENTER(NBURST);
            // increment it rigth now for timming reasons
            g_spcCtx->sched_slot_count++;

            if(g_spcCtx->sched_slot_count==g_spcCtx->sched_rx_slot_nb)
            {
                g_spcCtx->sched_rx_count++;
            }
            spc_NBurstProcess(&spRx);
            //HAL_W2CLR(W2_NORMAL_BURST);
            SPC_PROFILE_WINDOW_EXIT(NBURST);
            break;

        default:
            break;
    }                         //end swtich type

    if (g_spcCtx->sched_rx_count==g_spcCtx->rxQty)
    {
        g_spcCtx->frame_over=1; // end of the current frame process
        if (g_spcCtx->FcchActive == 0)
            spal_ComregsSetIrq(MBOX_DSP_TASK_COMPLETE);
    }

    SPC_PROFILE_FUNCTION_EXIT(spc_RxProcess);
}

