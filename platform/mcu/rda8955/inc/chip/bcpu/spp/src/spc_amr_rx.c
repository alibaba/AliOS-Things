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










#include "spc_ctx.h"
#include "spc_private.h"
#include "spcp_debug.h"
#include "spcp_amr.h"
#include "spc_profile_codes.h"
#include "spp_gsm.h"


// ****************************************************************************
//  spc_AmrFrameFilter:
//
//      this function used to filter amr degrated frame
//
// ****************************************************************************
VOID spc_AmrFrameFilter(UINT16 BitError, UINT8 Threshold, UINT8*FrameType)
{
    if (BitError > Threshold)
    {
        *FrameType=SPP_RX_SPEECH_DEGRADED;
        if(g_spcCtx->BFI_count>0)
        {
            *FrameType = SPP_RX_SPEECH_BAD;
        }
    }
}

// ****************************************************************************
//  spc_AmrDecode():
//
//      this function decodes one AMR frame, updates Tx & Rx codec mode
//      and analyses the RATSCCH message if one was detected
//
//
// ****************************************************************************

VOID spc_AmrDecode(SPC_RX_LOCALS_T *spRx)
{
    UINT8 cmi=g_spcCtx->RxCodecMode;
    UINT8 cmc=g_spcCtx->TxCodecMode;
    UINT8 T2=g_mailbox.pal2spc.counters[g_spcCtx->currentSnap].T2;
    UINT8 i,j;

    SPC_PROFILE_FUNCTION_ENTER(spc_AmrDecode);
    if (RATE(g_spcCtx->ChMode)==FR_RATE)
        spc_AFSdecoding( g_spcCtx->AMR_Cfg_Rx,
                         spRx->ItlvBuffInput,
                         (UINT32*) g_spcCtx->Malloc,
                         spRx->RxBuffOffset,
                         spRx->BlockDecodedAdd,
                         T2,
                         &g_spcCtx->NBlock_result,
                         &cmc,
                         &cmi,
                         &spRx->RxFrameType,
                         &g_spcCtx->ratscch_detected);

    else
        spc_AHSdecoding( g_spcCtx->AMR_Cfg_Rx,
                         spRx->ItlvBuffInput,
                         (UINT32*) g_spcCtx->Malloc,
                         spRx->RxBuffOffset,
                         spRx->BlockDecodedAdd,
                         T2,
                         &g_spcCtx->NBlock_result,
                         &cmc,
                         &cmi,
                         &spRx->RxFrameType,
                         &g_spcCtx->ratscch_detected);

    // update TxCodecMode and RxCodecMode
    if(cmc!=g_spcCtx->TxCodecMode)
    {
        for (i=0; i<g_spcCtx->AMR_Cfg_Tx.nbActiveCodecSet; i++)
        {
            if (cmc==g_spcCtx->AMR_Cfg_Tx.activeCodecSet[i]) break;
        }
        for (j=0; j<g_spcCtx->AMR_Cfg_Tx.nbActiveCodecSet; j++)
        {
            if (g_spcCtx->TxCodecMode==g_spcCtx->AMR_Cfg_Tx.activeCodecSet[j]) break;
        }
        if (i<g_spcCtx->AMR_Cfg_Tx.nbActiveCodecSet)
        {
            if (i<j) g_spcCtx->TxCodecMode=g_spcCtx->AMR_Cfg_Tx.activeCodecSet[j-1];
            else     g_spcCtx->TxCodecMode=g_spcCtx->AMR_Cfg_Tx.activeCodecSet[j+1];
        }
    }

#if 1
    g_spcCtx->RxCodecMode=cmi;
#else
    if(cmi!=g_spcCtx->RxCodecMode)
    {
        for (i=0; i<g_spcCtx->AMR_Cfg_Rx.nbActiveCodecSet; i++)
        {
            if (cmi==g_spcCtx->AMR_Cfg_Rx.activeCodecSet[i]) break;
        }
        for (j=0; j<g_spcCtx->AMR_Cfg_Rx.nbActiveCodecSet; j++)
        {
            if (g_spcCtx->RxCodecMode==g_spcCtx->AMR_Cfg_Rx.activeCodecSet[j]) break;
        }
        if (i<g_spcCtx->AMR_Cfg_Rx.nbActiveCodecSet)
        {
            if (i<j) g_spcCtx->RxCodecMode=g_spcCtx->AMR_Cfg_Rx.activeCodecSet[j-1];
            else     g_spcCtx->RxCodecMode=g_spcCtx->AMR_Cfg_Rx.activeCodecSet[j+1];
        }
        if(ABS(i-j)>1)
        {
            if(spRx->RxFrameType != SPP_RX_SPEECH_BAD)
            {
                g_spcCtx->RxCodecMode=cmi;
            }
            else
            {
                g_spcCtx->NBlock_result.Bfi=1;
                spRx->RxFrameType=SPP_RX_SPEECH_BAD;
            }
        }
    }
#endif

    if ((g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_A) ||
            (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_B) ||
            (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_I)  )
    {
        for (i=0; i<g_spcCtx->AMR_Cfg_Rx.nbActiveCodecSet; i++)
        {
            if (cmi==g_spcCtx->AMR_Cfg_Rx.activeCodecSet[i]) break;
        }
        g_spcCtx->CodecModeRequest=i;
        if (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_I) g_spcCtx->TxCodecMode=cmc;
        else g_spcCtx->TxCodecMode=cmi;
    }

    // processing of RATSCCH request
    if (g_spcCtx->ratscch_detected==1)
    {
        g_spcCtx->ratscch_detected=0;
        if (g_spcCtx->REQ_Activation==0)   //if no ratscch request is already pending
        {
            if(g_spcCtx->NBlock_result.Bfi)
                g_spcCtx->ratscch_to_encode=ACK_ERR;
            else
            {
                UINT8 thresh[3]= {0x3F,0x3F,0x3F};
                UINT8 hyst[3]= {0xF,0xF,0xF};
                UINT8 acs=((spRx->BlockDecodedAdd[0])>>20)&0xFF;
                UINT8 icm=((spRx->BlockDecodedAdd[0])>>28)&0x3;
                UINT8 nb_actv=0;
                UINT8 actv_codec_set[4]= {0,0,0,0};
                g_spcCtx->ratscch_to_encode=ACK_OK;
                // decoding of the ratscch request
                // switch regarding bits 34 33 32 31 30
                switch((((spRx->BlockDecodedAdd[1])&0x7)<<2)|((spRx->BlockDecodedAdd[0])>>30))
                {
                    case 0:     // CMI_PHASE_REQ
                        if (((spRx->BlockDecodedAdd[0])>>1)==2)
                        {
                            g_mailbox.pal2spc.statWin.dedicated.AMRCfg.CMIPhase=(spRx->BlockDecodedAdd[0])&1;
                            g_spcCtx->REQ_Activation=1;
                            g_spcCtx->RATSCCH_REQ=CMI_PHASE_REQ;
                        }
                        //if BlockDecodedAdd[0]==ACK_OK / ACK_ERR / ACK_UNKNOWN do nothing
                        else if (((spRx->BlockDecodedAdd[0])==1)||
                                 ((spRx->BlockDecodedAdd[0])==2)||
                                 ((spRx->BlockDecodedAdd[0])==3)) g_spcCtx->ratscch_to_encode=0;
                        else g_spcCtx->ratscch_to_encode=ACK_UNKNOWN;
                        break;

                    case 1:     // THRESH_REQ
                        for (i=0; i<g_spcCtx->AMR_Cfg_Rx.nbActiveCodecSet-1; i++)
                        {
                            thresh[i] =((spRx->BlockDecodedAdd[0])>>(i*10))&0x3F;
                            hyst[i]=((spRx->BlockDecodedAdd[0])>>(i*10+6))&0xF;
                        }
                        for (i=0; i<g_spcCtx->AMR_Cfg_Rx.nbActiveCodecSet-1; i++)
                        {
                            g_mailbox.pal2spc.statWin.dedicated.AMRCfg.threshold[i] =thresh[i];
                            g_mailbox.pal2spc.statWin.dedicated.AMRCfg.hysteresis[i]=hyst[i];
                        }
                        g_spcCtx->REQ_Activation=1;
                        g_spcCtx->RATSCCH_REQ=THRESH_REQ;
                        break;

                    case 7:     // AMR_CONFIG_REQ
                        for (i=0; i<8; i++) if ((acs>>i)&1) {actv_codec_set[nb_actv]=i; nb_actv++;}
                        if (nb_actv>4) g_spcCtx->ratscch_to_encode=ACK_UNKNOWN;
                        else
                        {
                            if (((spRx->BlockDecodedAdd[0])&0xFFFFF)!=0xFFFFF)
                            {
                                if (nb_actv==4)
                                {
                                    thresh[0]=(spRx->BlockDecodedAdd[0])&0x3F;
                                    thresh[1]=((spRx->BlockDecodedAdd[0])>>6)&0x3F;
                                    thresh[2]=((spRx->BlockDecodedAdd[0])>>12)&0x3F;
                                    hyst[0]=(((spRx->BlockDecodedAdd[0])>>18)&0x3)<<1;
                                    hyst[1]=hyst[0];
                                    hyst[2]=hyst[0];
                                }
                                else  //normal format
                                {
                                    thresh[0]=(spRx->BlockDecodedAdd[0])&0x3F;
                                    hyst[0]=((spRx->BlockDecodedAdd[0])>>6)&0xF;
                                    thresh[1]=((spRx->BlockDecodedAdd[0])>>10)&0x3F;
                                    hyst[1]=((spRx->BlockDecodedAdd[0])>>16)&0xF;
                                }
                            }
                            // thresh & hyst are updated even if there's no value available
                            for (i=1; i<4; i++)
                            {
                                g_mailbox.pal2spc.statWin.dedicated.AMRCfg.threshold[i-1]=thresh[i-1];
                                g_mailbox.pal2spc.statWin.dedicated.AMRCfg.hysteresis[i-1]=hyst[i-1];
                            }
                            g_mailbox.pal2spc.statWin.dedicated.AMRCfg.startModeIdx=icm;
                            g_mailbox.pal2spc.statWin.dedicated.AMRCfg.nbActiveCodecSet=nb_actv;
                            for (i=0; i<4; i++)
                                g_mailbox.pal2spc.statWin.dedicated.AMRCfg.activeCodecSet[i]=actv_codec_set[i];
                            g_spcCtx->REQ_Activation=1;
                            g_spcCtx->RATSCCH_REQ=AMR_CONFIG_REQ;
                        }
                        break;

                    default:    // UNKNOWN
                        g_spcCtx->ratscch_to_encode=ACK_UNKNOWN;
                        break;
                } //endif switch
            } //endif bfi
        } //endif Req_Act==0
    } //endif ratscch

    // in case of dtx downlink the sidupdate_ctr is incremented from 1 to 4 or 11 to 18
    switch (spRx->RxFrameType)
    {
        case SPP_RX_NO_DATA:
            if (((g_spcCtx->sidupdate_ctr>0) && (g_spcCtx->sidupdate_ctr < 4))
                    ||((g_spcCtx->sidupdate_ctr>10) && (g_spcCtx->sidupdate_ctr < 18)))
            {
                g_spcCtx->sidupdate_ctr++;
            }
            break;
        case SPP_RX_SID_FIRST:
            g_spcCtx->sidupdate_ctr=1;
            break;
        case SPP_RX_SID_UPDATE:
            g_spcCtx->sidupdate_ctr=11;
            break;
        default:
            g_spcCtx->sidupdate_ctr=0;
            break;
    }

    SPC_PROFILE_FUNCTION_EXIT(spc_AmrDecode);
}

// ****************************************************************************
//  spc_AmrRatscch():
//
//      this function applies the RATSCCH request
//
//
// ****************************************************************************

VOID spc_AmrRatscch(VOID)
{
    SPC_PROFILE_FUNCTION_ENTER(spc_AmrRatscch);
    // Check Activation of ratscch request
    if (g_spcCtx->REQ_Activation!=0)
    {
        if (g_spcCtx->REQ_Activation==13)
        {
            // Activation of ratscch request
            g_spcCtx->REQ_Activation=0;
            switch(g_spcCtx->RATSCCH_REQ)
            {
                case CMI_PHASE_REQ:
                    g_spcCtx->AMR_Cfg_Rx.phase=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.CMIPhase;
                    break;
                case THRESH_REQ:
                    g_spcCtx->AMR_Cfg_Rx.hysteresis[0]=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.hysteresis[0];
                    g_spcCtx->AMR_Cfg_Rx.hysteresis[1]=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.hysteresis[1];
                    g_spcCtx->AMR_Cfg_Rx.hysteresis[2]=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.hysteresis[2];
                    g_spcCtx->AMR_Cfg_Rx.threshold[0]=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.threshold[0];
                    g_spcCtx->AMR_Cfg_Rx.threshold[1]=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.threshold[1];
                    g_spcCtx->AMR_Cfg_Rx.threshold[2]=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.threshold[2];
                    break;
                case AMR_CONFIG_REQ:
                    g_spcCtx->AMR_Cfg_Rx.version=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.version;
                    g_spcCtx->AMR_Cfg_Rx.startModeIdx=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.startModeIdx;
                    g_spcCtx->AMR_Cfg_Rx.nbActiveCodecSet=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.nbActiveCodecSet;
                    g_spcCtx->AMR_Cfg_Rx.hysteresis[0]=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.hysteresis[0];
                    g_spcCtx->AMR_Cfg_Rx.hysteresis[1]=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.hysteresis[1];
                    g_spcCtx->AMR_Cfg_Rx.hysteresis[2]=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.hysteresis[2];
                    g_spcCtx->AMR_Cfg_Rx.threshold[0]=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.threshold[0];
                    g_spcCtx->AMR_Cfg_Rx.threshold[1]=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.threshold[1];
                    g_spcCtx->AMR_Cfg_Rx.threshold[2]=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.threshold[2];
                    g_spcCtx->AMR_Cfg_Rx.activeCodecSet[0]=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.activeCodecSet[0];
                    g_spcCtx->AMR_Cfg_Rx.activeCodecSet[1]=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.activeCodecSet[1];
                    g_spcCtx->AMR_Cfg_Rx.activeCodecSet[2]=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.activeCodecSet[2];
                    g_spcCtx->AMR_Cfg_Rx.activeCodecSet[3]=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.activeCodecSet[3];
                    g_spcCtx->CodecModeRequest=g_spcCtx->AMR_Cfg_Rx.startModeIdx;
                    break;
            }
            g_spcCtx->RATSCCH_REQ=NO_REQ;
        }
        else
            g_spcCtx->REQ_Activation++;
    }
    SPC_PROFILE_FUNCTION_EXIT(spc_AmrRatscch);
}

// ****************************************************************************
//  spc_UpdateCMR:
//
//      this function updates the Codec Mode Request and the snr estimation
//
//
// ****************************************************************************

VOID spc_UpdateCMR(SPP_AMR_CFG_T AMR_Cfg,
                   UINT16 QualityIndicator,
                   UINT8 *CodecModeRequest)
{

    // Quality Indicator is a filtered estimation of the Snr
    UINT8 cmr=*CodecModeRequest;
    QualityIndicator = (QualityIndicator*3)>>1;
    if (cmr>AMR_Cfg.nbActiveCodecSet-1) cmr=AMR_Cfg.nbActiveCodecSet-1;
    if (AMR_Cfg.nbActiveCodecSet==1) cmr=0;
    else
    {
        // if there's no thresh value available cmr=icm
        if (AMR_Cfg.threshold[0]==0x3F)
        {
            if (AMR_Cfg.startModeIdx!=0xFF) cmr=AMR_Cfg.startModeIdx;
            else
            {
                if(AMR_Cfg.nbActiveCodecSet==4) cmr=1;
                else cmr=0;
            }
        }
        else
        {
            if(cmr==AMR_Cfg.nbActiveCodecSet-1)
            {
                if(QualityIndicator<AMR_Cfg.threshold[cmr-1]) cmr--;
            }
            else if(cmr==0)
            {
                if(QualityIndicator>AMR_Cfg.threshold[0]+AMR_Cfg.hysteresis[0]) cmr++;
            }
            else
            {
                if(QualityIndicator<AMR_Cfg.threshold[cmr-1]) cmr--;
                else if(QualityIndicator>AMR_Cfg.threshold[cmr]+AMR_Cfg.hysteresis[cmr]) cmr++;
            }
        }
    }
    *CodecModeRequest=cmr;
}


// ****************************************************************************
//  spc_AFSdecoding:
//
//      this function decodes speech or DTX control AFS frames and inband data
//      TxCodecMode or/and RxCodecMode and RxFrameType are updated
//
//
// ****************************************************************************
VOID spc_AFSdecoding(SPP_AMR_CFG_T AMR_Cfg,
                     UINT32 *ItlvBuffInput,
                     UINT32 *RxItlvBufferOut,
                     UINT8 RxBuffOffset,
                     UINT32 *BlockDecodedAdd,
                     UINT8 T2,
                     SPP_N_BLOCK_RESULT_T *NBlock_result,
                     UINT8* TxCodecMode,
                     UINT8* RxCodecMode,
                     UINT8 *RxFrameType,
                     UINT8 *ratscch_detected)
{

    UINT8 FrameType=0;
    UINT8 mode=0;
    UINT8 acs=0;
    UINT8 id1=0;
    UINT8 id2=0;
    UINT8 cm1=*RxCodecMode;
    UINT8 cm2=*TxCodecMode;
    UINT8 nb_diff=0xff;
    UINT8 icBitErr_threshold = 1;
    UINT8 metric=0xff;
    UINT8 metric1_threshold=30;
    UINT8 metric2_threshold=48;
    if (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_I)
    {
        metric1_threshold = 20;
        metric2_threshold = 32;
        icBitErr_threshold = 2;
    }
    spp_DeinterleaveBlock(ItlvBuffInput, ITLV_TYPE_1A, RxBuffOffset, RxItlvBufferOut);

    FrameType=spp_SearchAmrMarker(RxItlvBufferOut, &id1, &id2, 0);
    // if id1 is not enough reliable we take the previous value
    if (id1==0xFF)
    {
        if ((T2==3)||(T2==11)||(T2==20))
        {
            if (AMR_Cfg.phase==ODD_PHASE) cm1=*RxCodecMode; // odd phase CMI received
            else                          cm1=*TxCodecMode; //even phase CMC received
        }
        else
        {
            if (AMR_Cfg.phase==ODD_PHASE) cm1=*TxCodecMode; // odd phase CMC received
            else                          cm1=*RxCodecMode; //even phase CMI received
        }
    }
    else
    {
        if (id1>=AMR_Cfg.nbActiveCodecSet)
        {
            id1=AMR_Cfg.nbActiveCodecSet-1;
        }
        cm1=AMR_Cfg.activeCodecSet[id1];
    }

    switch (FrameType)
    {

        case SPP_AFS_SPEECH:
            if ((T2==3)||(T2==11)||(T2==20))
            {
                mode=(AMR_Cfg.phase==ODD_PHASE)?cm1:*RxCodecMode; //odd phase CMI received even phase CMC received
            }
            else
            {
                mode=(AMR_Cfg.phase==ODD_PHASE)?*RxCodecMode:cm1; //odd phase CMC received even phase CMI received
            }

            if (g_spcAmrState.dtxUsed)
            {
                if((g_spcCtx->sidupdate_ctr!=4)&&(g_spcCtx->sidupdate_ctr!=18)&&(g_spcCtx->sidupdate_ctr!=0))
                {
                    NBlock_result->Bfi = 1;
                    *RxFrameType=SPP_RX_NO_DATA;
                }
                else
                {
                    g_spcAmrState.dtxUsed = 0;
                }
            }

            if(g_spcAmrState.dtxUsed == 0)
            {
                spp_DecodeBlock(RxItlvBufferOut,
                                MBOX_TCH_AFS,
                                FrameType,
                                mode,
                                BlockDecodedAdd,
                                0,
                                (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                                NBlock_result);

                if(NBlock_result->Bfi==0)
                {
                    *RxFrameType=SPP_RX_SPEECH_GOOD;

                    spc_AmrFrameFilter(NBlock_result->BitError, g_spcCtx->AFS_ber1_threshold[*RxCodecMode], RxFrameType);
                    metric = spp_GetAmrMetric((UINT8 *)RxItlvBufferOut,FR_RATE,id1, &nb_diff);
                    if((id1 != 0xff) && (nb_diff <= icBitErr_threshold))
                    {
                        if((*RxFrameType != SPP_RX_SPEECH_BAD) )
                        {
                            if ((T2==3)||(T2==11)||(T2==20))
                            {
                                if (AMR_Cfg.phase==ODD_PHASE) *RxCodecMode=cm1; // odd phase CMI received
                                else                          *TxCodecMode=cm1; //even phase CMC received
                            }
                            else
                            {
                                if (AMR_Cfg.phase==ODD_PHASE) *TxCodecMode=cm1; // odd phase CMC received
                                else                          *RxCodecMode=cm1; //even phase CMI received
                            }
                        }
                        else
                        {
                            if(metric>metric1_threshold)
                            {
                                if ((T2==3)||(T2==11)||(T2==20))
                                {
                                    if (AMR_Cfg.phase==ODD_PHASE) *RxCodecMode=cm1; // odd phase CMI received
                                    else                          *TxCodecMode=cm1; //even phase CMC received
                                }
                                else
                                {
                                    if (AMR_Cfg.phase==ODD_PHASE) *TxCodecMode=cm1; // odd phase CMC received
                                    else                          *RxCodecMode=cm1; //even phase CMI received
                                }
                            }
                        }
                    }
                }
                else
                {

                    if (NBlock_result->BitError > g_spcCtx->AFS_ber2_threshold[*RxCodecMode])
                    {
                        *RxFrameType=SPP_RX_NO_DATA;
                    }
                    else
                    {
                        *RxFrameType=SPP_RX_SPEECH_BAD;
                    }

                    if(id1 != 0xff)
                    {

                        metric = spp_GetAmrMetric((UINT8 *)RxItlvBufferOut,FR_RATE,id1, &nb_diff);
                        if((nb_diff <= icBitErr_threshold) && (metric>metric2_threshold))
                        {
                            if ((T2==3)||(T2==11)||(T2==20))
                            {
                                if (AMR_Cfg.phase==ODD_PHASE) *RxCodecMode=cm1; // odd phase CMI received
                                else                          *TxCodecMode=cm1; //even phase CMC received
                            }
                            else
                            {
                                if (AMR_Cfg.phase==ODD_PHASE) *TxCodecMode=cm1; // odd phase CMC received
                                else                          *RxCodecMode=cm1; //even phase CMI received
                            }
                        }
                    }
                }
            }


            break;

        case SPP_AFS_SID_UPDATE:
        {
            UINT32 tmp_word[4];
            UINT8 i;
            g_spcAmrState.dtxUsed=1;
            spp_DeinterleaveBlock(ItlvBuffInput+((RxBuffOffset&0x4)^4)*32, ITLV_TYPE_1B, 0, RxItlvBufferOut);
            for (i=0; i<4; i++) tmp_word[i]=RxItlvBufferOut[2*i];
            id2=spp_GetAmrIc((UINT8*)tmp_word, 2);
            if (id2<AMR_Cfg.nbActiveCodecSet) cm2=AMR_Cfg.activeCodecSet[id2];
            spp_DecodeBlock(RxItlvBufferOut,
                            MBOX_TCH_AFS,
                            FrameType,
                            cm1,
                            BlockDecodedAdd,
                            0,
                            (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                            NBlock_result);
            if(NBlock_result->Bfi==0)
            {
                *RxFrameType=SPP_RX_SID_UPDATE;
                *TxCodecMode=cm2;
                *RxCodecMode=cm1;
            }
            else
            {
                if((BlockDecodedAdd[0]==0xFFFFFFFF)&&((BlockDecodedAdd[1]&0x7)==0x7))
                {
                    // SID FILLER in TFO
                    *RxFrameType=SPP_RX_NO_DATA;
                }
                else
                {
                    *RxFrameType=SPP_RX_SID_BAD;
                }
            }
        }

        break;

        case SPP_AFS_SID_FIRST:
            g_spcAmrState.dtxUsed=1;
            *RxFrameType=SPP_RX_SID_FIRST;
            if ((T2==3)||(T2==11)||(T2==20))
            {
                if (AMR_Cfg.phase==ODD_PHASE) *RxCodecMode=cm1; // odd phase CMI received
                else                          *TxCodecMode=cm1; //even phase CMC received
            }
            else
            {
                if (AMR_Cfg.phase==ODD_PHASE) *TxCodecMode=cm1; // odd phase CMC received
                else                          *RxCodecMode=cm1; //even phase CMI received
            }
            break;

        case SPP_AFS_ONSET:
            g_spcAmrState.dtxUsed=0;
            *RxFrameType=SPP_RX_ONSET;
            *RxCodecMode=cm1; // CMI received
            break;

        case SPP_AFS_RATSCCH:
            g_spcAmrState.dtxUsed=0;
            *RxFrameType=SPP_RX_NO_DATA;
            if (id2<AMR_Cfg.nbActiveCodecSet) cm2=AMR_Cfg.activeCodecSet[id2];
            spp_DecodeBlock(RxItlvBufferOut,
                            MBOX_TCH_AFS,
                            FrameType,
                            cm1,
                            BlockDecodedAdd,
                            0,
                            (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                            NBlock_result);
            if(NBlock_result->Bfi==0)
            {
                *TxCodecMode=cm2;
                *RxCodecMode=cm1;
            }
            *ratscch_detected=1;
            break;
    }

    id1=0;
    id2=0;
    for(id1=0; id1<8; id1++)
    {
        if (id1==AMR_Cfg.activeCodecSet[id2]) {id2++; acs|=1<<id1;}
    }

}

// ****************************************************************************
//  spc_AHSdecoding:
//
//      this function decodes speech or DTX control AHS frames and inband data
//      TxCodecMode or/and RxCodecMode and RxFrameType are updated
//
//
// ****************************************************************************
VOID spc_AHSdecoding(SPP_AMR_CFG_T AMR_Cfg,
                     UINT32 *ItlvBuffInput,
                     UINT32 *RxItlvBufferOut,
                     UINT8 RxBuffOffset,
                     UINT32 *BlockDecodedAdd,
                     UINT8 T2,SPP_N_BLOCK_RESULT_T *NBlock_result,
                     UINT8* TxCodecMode,
                     UINT8* RxCodecMode,
                     UINT8 *RxFrameType,
                     UINT8 *ratscch_detected)
{

    /*volatile struct mBoxBase *mailbox = (struct mBoxBase *) CT_mailbox_base_ptr;*/

    UINT8 FrameType=0;
    UINT8 mode=0;
    UINT8 acs=0;
    UINT8 i=0;
    UINT8 id1=0;
    UINT8 id2=0;
    UINT8 cm1=*RxCodecMode;
    UINT8 cm2=*TxCodecMode;
    UINT8 *ptr_byte;
    UINT8 nb_diff = 0xff;
    UINT8 icBitErr_threshold = 1;
    UINT8 metric=0xff;
    UINT8 metric1_threshold=30;
    UINT8 metric2_threshold=48;
    if (g_mailbox.pal2spc.statWin.loopMode==MBOX_LOOP_CLOSE_I)
    {
        metric1_threshold = 20;
        metric2_threshold = 32;
    }
    spp_DeinterleaveBlock(ItlvBuffInput, ITLV_TYPE_2A, RxBuffOffset, RxItlvBufferOut);


    FrameType=spp_SearchAmrMarker(RxItlvBufferOut, &id1, &id2, 1);
    // if id1 is not enough reliable we take the previous value
    if (id1==0xFF)
    {
        if ((T2==2)||(T2==10)||(T2==19)||(T2==3)||(T2==11)||(T2==20))
        {
            if (AMR_Cfg.phase==ODD_PHASE) cm1=*RxCodecMode; // odd phase CMI received
            else                          cm1=*TxCodecMode; //even phase CMC received
        }
        else
        {
            if (AMR_Cfg.phase==ODD_PHASE) cm1=*TxCodecMode; // odd phase CMC received
            else                          cm1=*RxCodecMode; //even phase CMI received
        }
    }
    else
    {
        if (id1>=AMR_Cfg.nbActiveCodecSet)
        {
            id1=AMR_Cfg.nbActiveCodecSet-1;
        }
        cm1=AMR_Cfg.activeCodecSet[id1];
    }


    switch (FrameType)
    {

        case SPP_AHS_SPEECH:
            if (*ratscch_detected==2)
            {
                FrameType=SPP_AHS_RATSCCH_DATA;
                id1=spp_GetAmrIc((UINT8*)RxItlvBufferOut, 2);
                if (id1<AMR_Cfg.nbActiveCodecSet) cm1=AMR_Cfg.activeCodecSet[id1];
            }
            if ((T2==2)||(T2==10)||(T2==19)||(T2==3)||(T2==11)||(T2==20))
            {
                mode=(AMR_Cfg.phase==ODD_PHASE)?cm1:*RxCodecMode; //odd phase CMI received even phase CMC received
            }
            else
            {
                mode=(AMR_Cfg.phase==ODD_PHASE)?*RxCodecMode:cm1; //odd phase CMC received even phase CMI received
            }

            if (g_spcAmrState.dtxUsed)
            {
                mode=*RxCodecMode; //to avoid decoding with a wrong mode during dtx downlink
                if((g_spcCtx->sidupdate_ctr!=4)&&(g_spcCtx->sidupdate_ctr!=18)&&(g_spcCtx->sidupdate_ctr!=0))
                {
                    NBlock_result->Bfi = 1;
                    *RxFrameType=SPP_RX_NO_DATA;
                }
                else
                {
                    g_spcAmrState.dtxUsed = 0;
                }

            }

            if(g_spcAmrState.dtxUsed == 0)
            {
                spp_DecodeBlock(RxItlvBufferOut,
                                MBOX_TCH_AHS,
                                FrameType,
                                mode,
                                BlockDecodedAdd,
                                0,
                                (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                                NBlock_result);

                if(NBlock_result->Bfi==0)
                {
                    *RxFrameType=SPP_RX_SPEECH_GOOD;
                    spc_AmrFrameFilter(NBlock_result->BitError, g_spcCtx->AHS_ber1_threshold[*RxCodecMode], RxFrameType);
                    metric = spp_GetAmrMetric((UINT8 *)RxItlvBufferOut,HR_RATE,id1, &nb_diff);
                    if((id1 != 0xff)&& (nb_diff <= icBitErr_threshold))
                    {
                        if(*RxFrameType!=SPP_RX_SPEECH_BAD)
                        {
                            if ((T2==2)||(T2==10)||(T2==19)||(T2==3)||(T2==11)||(T2==20))
                            {
                                if (AMR_Cfg.phase==ODD_PHASE) *RxCodecMode=cm1; // odd phase CMI received
                                else                          *TxCodecMode=cm1; //even phase CMC received
                            }
                            else
                            {
                                if (AMR_Cfg.phase==ODD_PHASE) *TxCodecMode=cm1; // odd phase CMC received
                                else                          *RxCodecMode=cm1; //even phase CMI received
                            }

                        }
                        else
                        {

                            if(metric>metric1_threshold)
                            {
                                if ((T2==2)||(T2==10)||(T2==19)||(T2==3)||(T2==11)||(T2==20))
                                {
                                    if (AMR_Cfg.phase==ODD_PHASE) *RxCodecMode=cm1; // odd phase CMI received
                                    else                          *TxCodecMode=cm1; //even phase CMC received
                                }
                                else
                                {
                                    if (AMR_Cfg.phase==ODD_PHASE) *TxCodecMode=cm1; // odd phase CMC received
                                    else                          *RxCodecMode=cm1; //even phase CMI received
                                }
                            }
                        }
                    }

                }
                else
                {
                    if (NBlock_result->BitError > g_spcCtx->AHS_ber2_threshold[*RxCodecMode])
                    {
                        *RxFrameType=SPP_RX_NO_DATA;
                    }
                    else
                    {
                        *RxFrameType=SPP_RX_SPEECH_BAD;
                    }
                    if(id1 != 0xff)
                    {

                        metric = spp_GetAmrMetric((UINT8 *)RxItlvBufferOut,HR_RATE,id1, &nb_diff);
                        if((nb_diff <= icBitErr_threshold) && (metric>metric2_threshold))
                        {
                            if ((T2==2)||(T2==10)||(T2==19)||(T2==3)||(T2==11)||(T2==20))
                            {
                                if (AMR_Cfg.phase==ODD_PHASE) *RxCodecMode=cm1; // odd phase CMI received
                                else                          *TxCodecMode=cm1; //even phase CMC received
                            }
                            else
                            {
                                if (AMR_Cfg.phase==ODD_PHASE) *TxCodecMode=cm1; // odd phase CMC received
                                else                          *RxCodecMode=cm1; //even phase CMI received
                            }
                        }
                    }

                }
            }

            if (*ratscch_detected==2) { *RxFrameType=SPP_RX_NO_DATA; *ratscch_detected=1;}

            break;

        case SPP_AHS_SID_UPDATE:
            g_spcAmrState.dtxUsed=1;
            spp_DeinterleaveBlock(ItlvBuffInput, ITLV_TYPE_2B, RxBuffOffset, RxItlvBufferOut);
            // decode the in band data id0
            id2=spp_GetAmrIc((UINT8*)RxItlvBufferOut, 2);
            if (id2<AMR_Cfg.nbActiveCodecSet) cm2=AMR_Cfg.activeCodecSet[id2];
            spp_DecodeBlock(RxItlvBufferOut,
                            MBOX_TCH_AHS,
                            FrameType,
                            cm1,
                            BlockDecodedAdd,
                            0,
                            (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                            NBlock_result);
            if(NBlock_result->Bfi==0)
            {
                *RxFrameType=SPP_RX_SID_UPDATE;
                *TxCodecMode=cm2;
                *RxCodecMode=cm1;
            }
            else
            {
                if((BlockDecodedAdd[0]==0xFFFFFFFF)&&((BlockDecodedAdd[1]&0x7)==0x7))
                {
                    *RxFrameType=SPP_RX_NO_DATA; // SID FILLER in TFO
                }
                else
                {
                    *RxFrameType=SPP_RX_SID_BAD;
                }
            }
            break;

        case SPP_AHS_SID_UPDATE_INH:
            g_spcAmrState.dtxUsed=0;
            *RxCodecMode=cm1;
            *RxFrameType=SPP_RX_NO_DATA;
            break;

        case SPP_AHS_SID_FIRST_P1:
            g_spcAmrState.dtxUsed=1;
            RxBuffOffset+=2;
            if (RxBuffOffset>4) RxBuffOffset=0;
            spp_DeinterleaveBlock(ItlvBuffInput, ITLV_TYPE_2A, RxBuffOffset, RxItlvBufferOut);
            // decode the in band data in SID_FIRST_P2 (even bits only)
            ptr_byte=(UINT8*)RxItlvBufferOut;
            for (i=0; i<16; i++)
            {
                ptr_byte[32+i]=ptr_byte[2*i];
            }
            id2=spp_GetAmrIc(ptr_byte+32, 2);
            if (id2<AMR_Cfg.nbActiveCodecSet) cm2=AMR_Cfg.activeCodecSet[id2];
            *RxFrameType=SPP_RX_SID_FIRST;
            if ((T2==2)||(T2==10)||(T2==19)||(T2==3)||(T2==11)||(T2==20))
            {
                if (AMR_Cfg.phase==ODD_PHASE) {*RxCodecMode=cm1; *TxCodecMode=cm2;} // odd phase CMI received
                else                          {*TxCodecMode=cm1; *RxCodecMode=cm2;} //even phase CMC received
            }
            else
            {
                if (AMR_Cfg.phase==ODD_PHASE) {*TxCodecMode=cm1; *RxCodecMode=cm2;} // odd phase CMC received
                else                          {*RxCodecMode=cm1; *TxCodecMode=cm2;} //even phase CMI received
            }
            break;

        case SPP_AHS_SID_FIRST_INH:
            g_spcAmrState.dtxUsed=0;
            if ((T2==2)||(T2==10)||(T2==19)||(T2==3)||
                    (T2==11)||(T2==20))
            {
                if (AMR_Cfg.phase==ODD_PHASE) *RxCodecMode=cm1; // odd phase CMI received
                else                          *TxCodecMode=cm1; //even phase CMC received
            }
            else
            {
                if (AMR_Cfg.phase==ODD_PHASE) *TxCodecMode=cm1; // odd phase CMC received
                else                          *RxCodecMode=cm1; //even phase CMI received
            }
            *RxFrameType=SPP_RX_NO_DATA;
            break;

        case SPP_AHS_ONSET:
            g_spcAmrState.dtxUsed=0;
            *RxFrameType=SPP_RX_ONSET;
            *RxCodecMode=cm1; // CMI received
            break;

        case SPP_AHS_RATSCCH_MARKER:
            g_spcAmrState.dtxUsed=0;
            *RxFrameType=SPP_RX_NO_DATA;
            if ((T2==2)||(T2==10)||(T2==19)||(T2==3)||
                    (T2==11)||(T2==20))
            {
                if (AMR_Cfg.phase==ODD_PHASE) *RxCodecMode=cm1; // odd phase CMI received
                else                          *TxCodecMode=cm1; //even phase CMC received
            }
            else
            {
                if (AMR_Cfg.phase==ODD_PHASE) *TxCodecMode=cm1; // odd phase CMC received
                else                          *RxCodecMode=cm1; //even phase CMI received
            }
            *ratscch_detected=2;
            break;
    }


    id1=0;
    id2=0;
    for(id1=0; id1<8; id1++)
    {
        if (id1==AMR_Cfg.activeCodecSet[id2]) {id2++; acs|=1<<id1;}
    }
}


