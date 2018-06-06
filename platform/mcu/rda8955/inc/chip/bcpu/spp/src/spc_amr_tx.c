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
#include "spc_profile_codes.h"

#include "spcp_amr.h"
#include "spal_mem.h"



PROTECTED SPC_AMR_STATE_T g_spcAmrState SPAL_BBSRAM_GLOBAL;

UINT32 g_spcCtxAmrCfgTxChg SPAL_BBSRAM_GLOBAL;

// ****************************************************************************
//  spc_UpdateAmrCfg():
//
//      this function updates the AMR configuration parameters
//
//
// ****************************************************************************
VOID spc_UpdateAmrCfg(VOID)
{
    UINT8 i=0;

    SPC_PROFILE_FUNCTION_ENTER(spc_UpdateAmrCfg);
    for(i=0; i<4; i++) g_spcCtx->AMR_Cfg_Rx.activeCodecSet[i]=0;
    g_spcCtx->AMR_Cfg_Rx.version=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.version;
    g_spcCtx->AMR_Cfg_Rx.startModeIdx=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.startModeIdx;
    g_spcCtx->AMR_Cfg_Rx.nbActiveCodecSet=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.nbActiveCodecSet;
    g_spcCtx->AMR_Cfg_Rx.phase=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.CMIPhase; //default phase is odd
    for(i=0; i<g_spcCtx->AMR_Cfg_Rx.nbActiveCodecSet-1; i++)
        g_spcCtx->AMR_Cfg_Rx.hysteresis[i]=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.hysteresis[i];
    for(i=0; i<g_spcCtx->AMR_Cfg_Rx.nbActiveCodecSet-1; i++)
        g_spcCtx->AMR_Cfg_Rx.threshold[i]=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.threshold[i];
    for(i=0; i<g_spcCtx->AMR_Cfg_Rx.nbActiveCodecSet; i++)
        g_spcCtx->AMR_Cfg_Rx.activeCodecSet[i]=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.activeCodecSet[i];

    if (g_spcCtx->AMR_Cfg_Rx.startModeIdx!=0xFF)
    {
        g_spcCtx->RxCodecMode=g_spcCtx->AMR_Cfg_Rx.activeCodecSet[g_spcCtx->AMR_Cfg_Rx.startModeIdx];
        g_spcCtx->TxCodecMode=g_spcCtx->AMR_Cfg_Rx.activeCodecSet[g_spcCtx->AMR_Cfg_Rx.startModeIdx];
        g_spcCtx->CodecModeRequest=g_spcCtx->AMR_Cfg_Rx.startModeIdx;
    }
    else
    {
        if(g_spcCtx->AMR_Cfg_Rx.nbActiveCodecSet==4)
        {
            g_spcCtx->RxCodecMode=g_spcCtx->AMR_Cfg_Rx.activeCodecSet[1];
            g_spcCtx->TxCodecMode=g_spcCtx->AMR_Cfg_Rx.activeCodecSet[1];
            g_spcCtx->CodecModeRequest=1;
        }
        else
        {
            g_spcCtx->RxCodecMode=g_spcCtx->AMR_Cfg_Rx.activeCodecSet[0];
            g_spcCtx->TxCodecMode=g_spcCtx->AMR_Cfg_Rx.activeCodecSet[0];
            g_spcCtx->CodecModeRequest=0;
        }
    }
    // reset internal variables
    g_mailbox.pal2spc.statWin.dedicated.changedAMRCfg=0;
    g_spcCtx->AMR_Cfg_Tx=g_spcCtx->AMR_Cfg_Rx;
    g_spcCtx->ratscch_to_encode=0;
    g_spcCtx->ACK_Activation=0;
    g_spcCtx->REQ_Activation=0;
    g_spcCtx->RATSCCH_REQ=NO_REQ;
    g_spcCtx->ratscch_detected=0;
    SPC_PROFILE_FUNCTION_EXIT(spc_UpdateAmrCfg);
}

UINT16 spc_RetrieveCodecMode(SPP_AMR_CFG_T *AMR_Cfg)
{
    UINT8 i=0;
    UINT16 CodecModeInd;
#if ((SPC_IF_VER > 7) ||defined CHIP_DIE_8955)
    UINT32 *pAmrCfgTxChg;
#endif
    // get info form the current encoding structure
    CodecModeInd=spc_MboxToSppSpeechMode(g_spcCtx->speechEncOut.encMode);

    if (CodecModeInd>SPP_MR122_MODE)
    {
        // if previous mode was not AMR
        CodecModeInd = SPP_INVALID_MODE;
    }
    // get the relative codec mode indication and request
    for (i=0; i<AMR_Cfg->nbActiveCodecSet; i++)
    {
        if (CodecModeInd==AMR_Cfg->activeCodecSet[i]) break;
    }


#if ((SPC_IF_VER > 7) ||defined CHIP_DIE_8955)

#if (SPC_IF_VER > 7)
    pAmrCfgTxChg = &g_spcCtx->AmrCfgTxChg;
#else
    pAmrCfgTxChg = &g_spcCtxAmrCfgTxChg;
#endif
    if (CodecModeInd!=g_spcCtx->TxCodecMode)
    {
        if((i==AMR_Cfg->nbActiveCodecSet) || (*pAmrCfgTxChg == 1))  //not in codec set, force update , TODO:mute.
        {
            CodecModeInd = g_spcCtx->TxCodecMode;
        }
    }
    else
    {
        *pAmrCfgTxChg = 0;
    }
#else
    if(i==AMR_Cfg->nbActiveCodecSet)
    {
        CodecModeInd = (AMR_Cfg->nbActiveCodecSet==4)?AMR_Cfg->activeCodecSet[1]:AMR_Cfg->activeCodecSet[0];
    }
#endif

    return CodecModeInd;
}

// ****************************************************************************
//  spc_AmrEncode():
//
//      this function encodes one AMR speech frame and applies RATSCCH request
//
//
// ****************************************************************************
VOID spc_AmrEncode(UINT32 *InputBuffer, UINT16 ChMode, UINT16 CodecModeInd, UINT8 TxFrameType)
{

    SPC_PROFILE_FUNCTION_ENTER(spc_AmrEncode);
    if (g_spcCtx->ACK_Activation!=0)
    {
        if(g_spcCtx->ACK_Activation==13)
        {
            // Activation of ratscch request
            g_spcCtx->AMR_Cfg_Tx.version=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.version;
            g_spcCtx->AMR_Cfg_Tx.startModeIdx=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.startModeIdx;
            g_spcCtx->AMR_Cfg_Tx.nbActiveCodecSet=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.nbActiveCodecSet;
            g_spcCtx->AMR_Cfg_Tx.activeCodecSet[0]=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.activeCodecSet[0];
            g_spcCtx->AMR_Cfg_Tx.activeCodecSet[1]=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.activeCodecSet[1];
            g_spcCtx->AMR_Cfg_Tx.activeCodecSet[2]=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.activeCodecSet[2];
            g_spcCtx->AMR_Cfg_Tx.activeCodecSet[3]=g_mailbox.pal2spc.statWin.dedicated.AMRCfg.activeCodecSet[3];
            g_spcCtx->ACK_Activation=0;
#if (SPC_IF_VER >= 7)
            g_spcCtx->AmrCfgTxChg = 1;
#else
            g_spcCtxAmrCfgTxChg = 1;
#endif
            if (g_spcCtx->AMR_Cfg_Tx.startModeIdx!=0xFF)
                g_spcCtx->TxCodecMode=g_spcCtx->AMR_Cfg_Tx.activeCodecSet[g_spcCtx->AMR_Cfg_Tx.startModeIdx];
            else
            {
                if(g_spcCtx->AMR_Cfg_Tx.nbActiveCodecSet==4) g_spcCtx->TxCodecMode=g_spcCtx->AMR_Cfg_Tx.activeCodecSet[1];
                else                                        g_spcCtx->TxCodecMode=g_spcCtx->AMR_Cfg_Tx.activeCodecSet[0];
            }
        }
        else
            g_spcCtx->ACK_Activation++;
    }
    if ((g_mailbox.pal2spc.statWin.loopMode != MBOX_LOOP_CLOSE_A) &&
            (g_mailbox.pal2spc.statWin.loopMode != MBOX_LOOP_CLOSE_B) &&
            (g_mailbox.pal2spc.statWin.loopMode != MBOX_LOOP_CLOSE_I)  )
    {
        CodecModeInd = spc_RetrieveCodecMode(&g_spcCtx->AMR_Cfg_Tx);
    }
    else
    {
        g_spcCtxAmrCfgTxChg = 0;
    }
    if(RATE(g_spcCtx->ChMode) == FR_RATE)
    {
        spc_AFSencoding( g_spcCtx->AMR_Cfg_Tx,
                         CodecModeInd&AMR_MODE_MASK,
                         g_spcCtx->CodecModeRequest,
                         TxFrameType,
                         InputBuffer,
                         g_mailbox.pal2spc.counters[g_spcCtx->currentSnap].T2,
                         g_spcCtx->FacchEncoded,
                         &(g_spcCtx->ratscch_to_encode),
                         g_spcCtx->TxBlockOffset,
                         g_spcCtx->ItlvBufTxDedicated,
                         &(g_spcCtx->Tx_off));
    }
    else
    {
        spc_AHSencoding( g_spcCtx->AMR_Cfg_Tx,
                         CodecModeInd&AMR_MODE_MASK,
                         g_spcCtx->CodecModeRequest,
                         TxFrameType,
                         InputBuffer,
                         g_mailbox.pal2spc.counters[g_spcCtx->currentSnap].T2,
                         g_spcCtx->FacchEncoded,
                         &(g_spcCtx->ratscch_to_encode),
                         g_spcCtx->TxBlockOffset,
                         g_spcCtx->ItlvBufTxDedicated,
                         &(g_spcCtx->Tx_off));
    }
    if (g_spcCtx->ratscch_to_encode==0xFF)
    {
        g_spcCtx->ratscch_to_encode=0;
        if (g_spcCtx->RATSCCH_REQ==AMR_CONFIG_REQ) g_spcCtx->ACK_Activation=1;
    }
    SPC_PROFILE_FUNCTION_EXIT(spc_AmrEncode);
}

// ****************************************************************************
//  spc_AFSencoding:
//
//      this function encodes speech frames and inband data
//      from TxCodecMode and CodecModeRequest
//
//
// ****************************************************************************
VOID spc_AFSencoding(SPP_AMR_CFG_T AMR_Cfg,
                     UINT8 TxCodecMode,
                     UINT8 CodecModeRequest,
                     UINT8 TxFrameType,
                     UINT32* InputBuffer,
                     UINT8 T2,
                     UINT8 facch_to_encode,
                     UINT8 *ratscch_to_encode,
                     UINT8 TxBlockOffset,
                     UINT32* ItlvBufTxDedicated,
                     BOOL *Tx_off)
{

    /*volatile struct mBoxBase *mailbox = (struct mBoxBase *) CT_mailbox_base_ptr;*/

    UINT8 type=0;
    UINT8 cmi=0;
    UINT8 cmr=0;
    UINT8 i=0;
    UINT8 itlv=ITLV_TYPE_1A;
    UINT8 id[2]= {0,0};

    // get the relative codec mode indication and request
    for (i=0; i<AMR_Cfg.nbActiveCodecSet; i++)
    {
        if (TxCodecMode==AMR_Cfg.activeCodecSet[i]) break;
    }
    cmi=i;

    cmr=CodecModeRequest; // CodecModeRequest is an ACS relative Codec Mode
    if ((T2==0)||(T2==8)||(T2==17))
    {
        id[0]=cmi; // CMI sent
    }
    else
    {
        id[0]=cmr; // CMR sent

    }
    *Tx_off=FALSE;

    if ((facch_to_encode)||(*ratscch_to_encode))
    {
        //facch or ratscch to encode
        if (g_spcAmrState.previousFrameType!=SPP_TX_SPEECH)
        {
            type = SPP_AFS_ONSET;
            id[1]=cmi;
            spp_EncodeBlock(InputBuffer,
                            MBOX_TCH_AFS,
                            type,
                            TxCodecMode,
                            id,
                            TxBlockOffset^0x4,
                            itlv,
                            ItlvBufTxDedicated);
            g_spcAmrState.rescheduledSidFirst=1;
        }
        if(facch_to_encode)  //facch to encode
        {
            spp_EncodeBlock(InputBuffer,
                            CS1_mode,
                            0,
                            0,
                            NULL,
                            TxBlockOffset,
                            itlv,
                            ItlvBufTxDedicated);
        }
        else if (*ratscch_to_encode)  //ratscch to encode
        {
            UINT32 ratscch_message[14]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
            ratscch_message[0]=(*ratscch_to_encode)>>4;
            type = SPP_AFS_RATSCCH;
            id[0]=cmr;
            id[1]=cmi;
            spp_EncodeBlock(&ratscch_message[0],
                            MBOX_TCH_AFS,
                            type,
                            TxCodecMode,
                            id,
                            TxBlockOffset,
                            itlv,
                            ItlvBufTxDedicated);
            if (ratscch_message[0]==ACK_OK>>4)*ratscch_to_encode=0xFF;
            else *ratscch_to_encode=0;
        }
        if (TxFrameType==SPP_TX_SID_FIRST) g_spcAmrState.rescheduledSidFirst=1;
        else if (TxFrameType==SPP_TX_SID_UPDATE) {g_spcAmrState.rescheduledSidFirst=1; g_spcAmrState.rescheduledSidUpdate=1;}
        g_spcAmrState.previousFrameType=SPP_TX_SPEECH;
    }
    else // tch speech
    {
        switch(TxFrameType)
        {
            case SPP_TX_SPEECH:
                if (g_spcAmrState.previousFrameType!=SPP_TX_SPEECH)
                {
                    type = SPP_AFS_ONSET;
                    id[1]=cmi;
                    spp_EncodeBlock(InputBuffer,
                                    MBOX_TCH_AFS,
                                    type,
                                    TxCodecMode,
                                    id,
                                    TxBlockOffset^0x4,
                                    itlv,
                                    ItlvBufTxDedicated);
                }
                type = SPP_AFS_SPEECH;
                g_spcAmrState.rescheduledSidFirst=0;
                g_spcAmrState.rescheduledSidUpdate=0;
                g_spcAmrState.previousFrameType=SPP_TX_SPEECH;
                break;
            case SPP_TX_SID_FIRST:
                type = SPP_AFS_SID_FIRST;
                g_spcAmrState.rescheduledSidFirst=0;
                g_spcAmrState.previousFrameType=SPP_TX_SID_FIRST;
                break;
            case SPP_TX_SID_UPDATE:
                if(g_spcAmrState.rescheduledSidFirst)
                {
                    type = SPP_AFS_SID_FIRST;
                    g_spcAmrState.rescheduledSidFirst=0;
                    g_spcAmrState.previousFrameType=SPP_TX_SID_FIRST;
                    g_spcAmrState.rescheduledSidUpdate=1;
                }
                else
                {
                    type = SPP_AFS_SID_UPDATE;
                    id[0]=cmr;
                    id[1]=cmi;
                    itlv=ITLV_TYPE_1B;
                    g_spcAmrState.rescheduledSidUpdate=0;
                    g_spcAmrState.previousFrameType=SPP_TX_SID_UPDATE;
                }
                break;
            case SPP_TX_NO_DATA:
                if (g_spcAmrState.rescheduledSidFirst)
                {
                    type = SPP_AFS_SID_FIRST;
                    g_spcAmrState.rescheduledSidFirst=0;
                    g_spcAmrState.previousFrameType=SPP_TX_SID_FIRST;
                }
                else if(g_spcAmrState.rescheduledSidUpdate)
                {
                    type = SPP_AFS_SID_UPDATE;
                    id[0]=cmr;
                    id[1]=cmi;
                    itlv=ITLV_TYPE_1B;
                    g_spcAmrState.rescheduledSidUpdate=0;
                    g_spcAmrState.previousFrameType=SPP_TX_SID_UPDATE;
                }
                else
                {
                    type = SPP_AFS_NO_DATA;
                    *Tx_off=TRUE;
                    g_spcAmrState.previousFrameType=SPP_TX_NO_DATA;
                }
                break;
        }
        spp_EncodeBlock(InputBuffer,
                        MBOX_TCH_AFS,
                        type,
                        TxCodecMode,
                        id,
                        TxBlockOffset,
                        itlv,
                        ItlvBufTxDedicated);
    }
}

// ****************************************************************************
//  spc_AHSencoding:
//
//      this function encodes speech frames and inband data
//      from TxCodecMode and CodecModeRequest
//
//
// ****************************************************************************
VOID spc_AHSencoding(SPP_AMR_CFG_T AMR_Cfg,
                     UINT8 TxCodecMode,
                     UINT8 CodecModeRequest,
                     UINT8 TxFrameType,
                     UINT32* InputBuffer,
                     UINT8 T2,
                     UINT8 facch_to_encode,
                     UINT8 *ratscch_to_encode,
                     UINT8 TxBlockOffset,
                     UINT32* ItlvBufTxDedicated,
                     BOOL *Tx_off)
{

    /*volatile struct mBoxBase *mailbox = (struct mBoxBase *) CT_mailbox_base_ptr;*/

    UINT8 type=0;
    UINT8 cmi=0;
    UINT8 cmr=0;
    UINT8 i=0;
    UINT8 id[2]= {0,0};
    UINT8 itlv=ITLV_TYPE_2A;
    UINT8 TxBlockOffsetInh=(TxBlockOffset<2)? 4 : TxBlockOffset-2;

    // get the relative codec mode indication and request
    for (i=0; i<AMR_Cfg.nbActiveCodecSet; i++)
    {
        if (TxCodecMode==AMR_Cfg.activeCodecSet[i]) break;
    }
    cmi=i;
    cmr=CodecModeRequest; // CodecModeRequest is an ACS relative Codec Mode
    if ((T2==0)||(T2==8)||(T2==17)||(T2==1)||(T2==9)||(T2==18))
    {
        id[0]=cmi; // CMI sent
    }
    else
    {
        id[0]=cmr; // CMR sent

    }
    *Tx_off=FALSE;

    if ((facch_to_encode)||(*ratscch_to_encode))
    {
        //facch or ratscch to encode
        switch(g_spcAmrState.previousFrameType)
        {
            case SPP_TX_SID_FIRST:
                type = SPP_AHS_SID_FIRST_INH;
                spp_EncodeBlock(InputBuffer,
                                MBOX_TCH_AHS,
                                type,
                                TxCodecMode,
                                &g_spcAmrState.prevId,
                                TxBlockOffsetInh,
                                itlv,
                                ItlvBufTxDedicated);
                g_spcAmrState.rescheduledSidFirst=1;
                break;
            case SPP_TX_SID_UPDATE:
                type = SPP_AHS_SID_UPDATE_INH;
                id[1]=cmi;
                spp_EncodeBlock(InputBuffer,
                                MBOX_TCH_AHS,
                                type,
                                TxCodecMode,
                                id,
                                TxBlockOffsetInh,
                                itlv,
                                ItlvBufTxDedicated);
                g_spcAmrState.rescheduledSidFirst=1;
                g_spcAmrState.rescheduledSidUpdate=1;
                break;
            case SPP_TX_NO_DATA:
                type = SPP_AHS_ONSET;
                id[1]=cmi;
                spp_EncodeBlock(InputBuffer,
                                MBOX_TCH_AHS,
                                type,
                                TxCodecMode,
                                id,
                                TxBlockOffsetInh,
                                itlv,
                                ItlvBufTxDedicated);
                g_spcAmrState.rescheduledSidFirst=1;
                break;
        }

        if(facch_to_encode)  //facch to encode
        {
            if(((*ratscch_to_encode)&0xF)==1) (*ratscch_to_encode)^=3;
            if (facch_to_encode==2)
                spp_EncodeBlock(InputBuffer,
                                CS1_mode,
                                0,
                                0,
                                0,
                                TxBlockOffset,
                                ITLV_TYPE_1C,
                                ItlvBufTxDedicated);
        }
        else if (*ratscch_to_encode)  //ratscch to encode
        {
            UINT32 ratscch_message[14]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
            ratscch_message[0]=(*ratscch_to_encode)>>4;
            if (((*ratscch_to_encode)&0xF)==2)
            {
                type = SPP_AHS_RATSCCH_MARKER;
                (*ratscch_to_encode)^=3;
            }
            else
            {
                type = SPP_AHS_RATSCCH_DATA;
                if (ratscch_message[0]==ACK_OK>>4) *ratscch_to_encode=0xFF;
                else *ratscch_to_encode=0;
            }
            spp_EncodeBlock(&ratscch_message[0],
                            MBOX_TCH_AHS,
                            type,
                            TxCodecMode,
                            id,
                            TxBlockOffset,
                            itlv,
                            ItlvBufTxDedicated);
        }
        g_spcAmrState.previousFrameType=SPP_TX_SPEECH;
        if (TxFrameType==SPP_TX_SID_FIRST) g_spcAmrState.rescheduledSidFirst=1;
        else if (TxFrameType==SPP_TX_SID_UPDATE) {g_spcAmrState.rescheduledSidFirst=1; g_spcAmrState.rescheduledSidUpdate=1;}
    }
    else // tch speech
    {
        switch(TxFrameType)
        {
            case SPP_TX_SPEECH:
                switch(g_spcAmrState.previousFrameType)
                {
                    case SPP_TX_SID_FIRST:
                        type = SPP_AHS_SID_FIRST_INH;
                        spp_EncodeBlock(InputBuffer,
                                        MBOX_TCH_AHS,
                                        type,
                                        TxCodecMode,
                                        &g_spcAmrState.prevId,
                                        TxBlockOffsetInh,
                                        itlv,
                                        ItlvBufTxDedicated);
                        break;
                    case SPP_TX_SID_UPDATE:
                        type = SPP_AHS_SID_UPDATE_INH;
                        id[1]=cmi;
                        spp_EncodeBlock(InputBuffer,
                                        MBOX_TCH_AHS,
                                        type,
                                        TxCodecMode,
                                        id,
                                        TxBlockOffsetInh,
                                        itlv,
                                        ItlvBufTxDedicated);
                        break;
                    case SPP_TX_NO_DATA:
                        type = SPP_AHS_ONSET;
                        id[1]=cmi;
                        spp_EncodeBlock(InputBuffer,
                                        MBOX_TCH_AHS,
                                        type,
                                        TxCodecMode,
                                        id,
                                        TxBlockOffsetInh,
                                        itlv,
                                        ItlvBufTxDedicated);
                        break;
                }
                type = SPP_AHS_SPEECH;
                g_spcAmrState.rescheduledSidFirst=0;
                g_spcAmrState.rescheduledSidUpdate=0;
                g_spcAmrState.previousFrameType=SPP_TX_SPEECH;
                break;
            case SPP_TX_SID_FIRST:
                type = SPP_AHS_SID_FIRST_P1;
                g_spcAmrState.rescheduledSidFirst=0;
                g_spcAmrState.previousFrameType=SPP_TX_SID_FIRST;
                break;
            case SPP_TX_SID_UPDATE:
                if (g_spcAmrState.previousFrameType==SPP_TX_SID_FIRST)
                {
                    type = SPP_AHS_SID_FIRST_P2;
                    g_spcAmrState.previousFrameType=SPP_TX_NO_DATA;
                    g_spcAmrState.rescheduledSidUpdate=1;
                }
                else if(g_spcAmrState.rescheduledSidFirst)
                {
                    type = SPP_AHS_SID_FIRST_P1;
                    g_spcAmrState.rescheduledSidFirst=0;
                    g_spcAmrState.rescheduledSidUpdate=1;
                    g_spcAmrState.previousFrameType=SPP_TX_SID_FIRST;
                }
                else
                {
                    type = SPP_AHS_SID_UPDATE;
                    id[0]=cmr;
                    id[1]=cmi;
                    g_spcAmrState.rescheduledSidUpdate=0;
                    g_spcAmrState.previousFrameType=SPP_TX_SID_UPDATE;
                }
                break;
            case SPP_TX_NO_DATA:
                if (g_spcAmrState.rescheduledSidFirst)
                {
                    type = SPP_AHS_SID_FIRST_P1;
                    g_spcAmrState.rescheduledSidFirst=0;
                    g_spcAmrState.previousFrameType=SPP_TX_SID_FIRST;
                }
                else if(g_spcAmrState.rescheduledSidUpdate)
                {
                    if(g_spcAmrState.previousFrameType==SPP_TX_SID_FIRST)
                    {
                        type = SPP_AHS_SID_FIRST_P2;
                        g_spcAmrState.previousFrameType=SPP_TX_NO_DATA;
                    }
                    else
                    {
                        type = SPP_AHS_SID_UPDATE;
                        id[0]=cmr;
                        id[1]=cmi;
                        g_spcAmrState.rescheduledSidUpdate=0;
                        g_spcAmrState.previousFrameType=SPP_TX_SID_UPDATE;
                    }
                }
                else
                {
                    if(g_spcAmrState.previousFrameType==SPP_TX_SID_FIRST)
                    {
                        type = SPP_AHS_SID_FIRST_P2;
                    }
                    else if(g_spcAmrState.previousFrameType==SPP_TX_NO_DATA)
                    {
                        *Tx_off=TRUE;
                        type=SPP_AHS_NO_DATA;
                    }

                    g_spcAmrState.previousFrameType=SPP_TX_NO_DATA;
                }
                break;
        }
        spp_EncodeBlock(InputBuffer,
                        MBOX_TCH_AHS,
                        type,
                        TxCodecMode,
                        id,
                        TxBlockOffset,
                        itlv,
                        ItlvBufTxDedicated);
    }
}

