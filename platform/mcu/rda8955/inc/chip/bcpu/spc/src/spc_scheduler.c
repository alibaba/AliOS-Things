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
#include "spcp_spp_cfg.h"
#include "spc_profile_codes.h"
// SPAL
#include "spal_comregs.h"
#include "spal_irq.h"
#include "spal_rfif.h"
#include "spal_xcor.h"
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

// Rx slot number to identify whether multislot window programming is finished.
// Note that it is named as rx_slot_nb in SPC_CONTEXT_T when SPC_IF_VER >= 5.
VOLATILE UINT32 g_spcRxSlotNb;
#endif

extern VOLATILE UINT8 SPAL_BBSRAM_GLOBAL g_spcRxCountTcu;

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------
// interleaver buffer
PROTECTED SPC_ITLV_BUFFERS_T g_spcItlvBufTab __attribute__((section("BB_ITLV_SECTION")));

#if  (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)

PROTECTED UINT32 g_spcItlvBufTabSaic[SPC_ITLV_BUFF_RX_CTRL*5] __attribute__((section("BB_ITLV_SECTION")));

#endif


// Global Context Structure statically placed in BB_SRAM
// Should be referenced by a pointer placed in a reserved register
// to ease address calculation and reduce code size.
PUBLIC SPC_CONTEXT_T g_spcCtxStruct __attribute__((section("BB_SPC_CTX_SECTION")));

#if  (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)

PROTECTED UINT32  g_spcStaticBufTabSaic[BB_BURST_TOTAL_SIZE*2 + 4] __attribute__((section("BB_STATIC_SECTION")));

#endif
// static RF samples reception buffers (uncached data section)
PROTECTED SPC_STATIC_BUFFERS_T g_spcStaticBufTab __attribute__((section("BB_STATIC_SECTION")));

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------
PUBLIC VOID spc_GnrlReset(VOID);

//=============================================================================
// spc_SchedulerMain()
//-----------------------------------------------------------------------------
/// This function performs the scheduling for Signal Processing Control.
//=============================================================================

PROTECTED VOID spc_SchedulerMain(VOID)
{
    // Place Global Context address in a reserved register.
    //----------------------------------------------------------
    // CAUTION
    //----------------------------------------------------------
    // This must be done at the entrance (before g_spcCtx is referenced) of
    // each task and each IRQ handler that are using g_spcCtx.
    //----------------------------------------------------------
    g_spcCtx = &g_spcCtxStruct;

    // general reset for context variables & buffers
    spc_GnrlReset();

    // main loop
    while (1)
    {
#if (SPC_IF_VER != 0)
        //--------------------------
        //    Call back hook
        //--------------------------
        // Call a function defined by the XCPU, when the BCPU
        // doesn't have anything to do. This can be useful to add
        // things to do to the BCPU.
        // We do that only when there is no pending IRQ.
        if (g_spcCtx->schedulerHook != NULL &&
                g_spcCtx->bcpuIrqCause == 0)
        {
            ((VOID(*)(VOID))g_spcCtx->schedulerHook)();
        }
#endif // (SPC_IF_VER != 0)

        //--------------------------
        //    check sleep mode
        //--------------------------
        volatile UINT32 critical_sec;
        critical_sec = spal_IrqEnterCriticalSection();
        if (g_spcCtx->bcpuIrqCause == 0)
        {
            // Instruction TCM without clock gating will make
            // CPU read a wrong instruction (next instruction to
            // the correct one) right after CPU sleeps (the lw instruction).
            // Even worse, if the wrong instruction is not aligned in
            // 4 bytes, all the following mips16 32-bit instructions will be
            // parsed in a misaligned manner.
            // Workaround:
            // Do not stop BCPU clock on FPGA.
            // (Note: 8805/8806/8808/8810 do not have a TCM, and they can
            // safely sleep on FPGA)
#if !defined(FPGA) || defined(FPGA_BCPU_CLOCK_GATE)
            // shutdown if nothing to do
            SPC_PROFILE_WINDOW_EXIT(BCPU_RUNNING);
            UINT32 clk_bb_disable;
            spal_IrqWriteCpuSleep((UINT32) BB_IRQ_SLEEP);
            // Flush the write buffer
            clk_bb_disable = spal_IrqReadCpuSleep();
            SPC_PROFILE_WINDOW_ENTER(BCPU_RUNNING);
#endif
        }
        spal_IrqExitCriticalSection(critical_sec);
        //flush write buffer
        critical_sec=spal_ComregsGetSnap();

        //----------------------------------------------------------------
        //                          FINT
        //----------------------------------------------------------------
        // check each irq cause in g_spcCtx->bcpuIrqCause
        // IFC2 service must be prioritary on Fint service for late Rx Window treatment
        if (  (g_spcCtx->bcpuIrqCause & BB_IRQ_BB_IRQ_FRAME) &&
                (!(g_spcCtx->bcpuIrqCause & BB_IRQ_BB_IRQ_IFC2)||(g_spcCtx->frame_over==1)))
        {

            //HAL_W2SET(W2_FINT_SERV);
            SPC_PROFILE_WINDOW_ENTER(FRAME);

            critical_sec = spal_IrqEnterCriticalSection();
            g_spcCtx->bcpuIrqCause &= ~BB_IRQ_BB_IRQ_FRAME;
            spal_IrqExitCriticalSection(critical_sec);

            UINT32 ComRegIrq = 0;
            if (g_spcCtx->sendEqItr == 1)
            {
                g_spcCtx->sendEqItr = 0;
                ComRegIrq = MBOX_DSP_EQUALIZATION_COMPLETE;
#if (SPC_IF_VER <= 5)
                spal_ComregsSetIrq(ComRegIrq);
#endif
                SPC_PROFILE_PULSE(DSP_EQUALIZATION_COMPLETE);
            }

            spc_UpdateOnFint();

            if (g_spcCtx->txQty > 0)
            {
                spc_TxProcess();
            }

            if (g_spcCtx->rxQty > 0)
            {
#if (SPC_IF_VER < 5)
                g_spcCtx->rxslotNb = (UINT8)g_spcRxSlotNb;
#else
                g_spcCtx->sched_rx_slot_nb = g_spcCtx->rx_slot_nb;
#endif
            }
            //HAL_W2CLR(W2_FINT_SERV);
            SPC_PROFILE_WINDOW_EXIT(FRAME);

        }//endif FINT

        //----------------------------------------------------------------
        //        TCU1
        //----------------------------------------------------------------
        // check each irq cause in g_spcCtx->bcpuIrqCause
        if( g_spcCtx->bcpuIrqCause & BB_IRQ_BB_IRQ_TCU1)
        {
            SPC_PROFILE_WINDOW_ENTER(TCU1);
            // if (!(g_spcCtx->bcpuIrqCause & BB_IRQ_BB_IRQ_FRAME))
            g_spcCtx->sched_rx_count = g_spcRxCountTcu;
            g_spcCtx->bcpuIrqCause &= ~BB_IRQ_BB_IRQ_TCU1;
            SPC_PROFILE_WINDOW_EXIT(TCU1);
        }

        //----------------------------------------------------------------
        //        IFC2 RF RX
        //----------------------------------------------------------------
        // check each irq cause in g_spcCtx->bcpuIrqCause
        if (g_spcCtx->bcpuIrqCause & BB_IRQ_BB_IRQ_IFC2)
        {

            //HAL_W2SET(W2_IFC2_SERV);
            SPC_PROFILE_WINDOW_ENTER(RF_IFC);

            critical_sec = spal_IrqEnterCriticalSection();
            // AVOID missing an IFC2 IT if 2 (or more) of them happened
            // during last IFC2 servicing
            if(!(-- g_spcCtx->ifc2_pending))
                g_spcCtx->bcpuIrqCause &= ~BB_IRQ_BB_IRQ_IFC2;
            spal_IrqExitCriticalSection(critical_sec);

            spc_RxProcess();

            SPC_PROFILE_WINDOW_EXIT(RF_IFC);
            //HAL_W2CLR(W2_IFC2_SERV);
        } //end if IFC
    } //end while(1)

} //end main



//=============================================================================
// spc_GnrlReset()
//-----------------------------------------------------------------------------
/// This function executes a general reset of the context
//=============================================================================
PUBLIC VOID spc_GnrlReset(VOID)
{
    UINT8 i,j;

    g_spcCtx->bcpuIrqCause=0;

    g_spcCtx->BlockSnR = 0;
    g_spcCtx->LastBlockSnR = 0;
    g_spcCtx->Rx_Hu_TCH_N_1 = 0;
    g_spcCtx->Rx_Hu_TCH_N = 0;
    g_spcCtx->Rx_Hl_TCH_N = 0;
    g_spcCtx->ChMode = MBOX_NO_TCH;
    g_spcCtx->PrevChMode = MBOX_NO_TCH;
    g_spcCtx->DedicatedActive = FALSE;
    g_spcCtx->Tx_off = FALSE;
    g_spcCtx->Init_FCCH = FALSE;
    for (i=0; i<4; i++) g_spcCtx->Rx_CS[i]=0;
    for (i=0; i<4; i++)
    {
        for (j=0; j<8; j++) g_spcCtx->Rx_q_CCH[i][j] = 0;
    }
    g_spcCtx->currentSnap = 0;
    g_spcCtx->ifc2_pending = 0;
    g_spcCtx->sched_slot_count = 0;
    g_spcCtx->TxBlockOffset = 0;
    g_spcCtx->RxBlockOffset = 0;
    g_spcCtx->Tx_Hu = 0;
    g_spcCtx->Tx_Hl = 0;
    g_spcCtx->FacchEncoded = 0;
    g_spcCtx->DTX_on = 0;

    // RF-related setup
    if (g_mailbox.pal2spc.rf.dcoCalEnable)
        g_spcCtx->RfIfCtrl =    RF_IF_ENABLE_ENABLE | RF_IF_RX_CAL_BYPASS_ENABLED;
    else
        g_spcCtx->RfIfCtrl =    RF_IF_ENABLE_ENABLE | RF_IF_RX_CAL_BYPASS_BYPASSED;

#if (CHIP_HAS_ANA_RF)
    // Add ramping event to DTX TCO settings
    g_mailbox.pal2spc.rf.dtxTcoSettings |= TCU_DISABLE_RAMP_MASK;

    // Setup Rf_if in case of DigRf
    if (g_mailbox.pal2spc.rf.digrfEnable)
    {
        g_spcCtx->RfIfCtrl |=  RF_IF_DIGRF_ENABLE_ENABLE |
                               (g_mailbox.pal2spc.rf.digrfRxRate?RF_IF_DIGRF_RX_RATE_TWO:RF_IF_DIGRF_RX_RATE_ONE) |
                               (g_mailbox.pal2spc.rf.digrfRxClkPol?RF_IF_DIGRF_RX_CLK_POL:0) |
                               (g_mailbox.pal2spc.rf.digrfTxMode?RF_IF_DIGRF_TX_MODE_BLOCK:RF_IF_DIGRF_TX_MODE_STREAM) |
                               (g_mailbox.pal2spc.rf.digrfTxClkPol?RF_IF_DIGRF_TX_CLK_POL:0) |
#if (SPC_IF_VER == 1)
                               RF_IF_DIGRF_SHIFT_SAMPLE(g_mailbox.pal2spc.rf.digrfRxSampleShift);
#else
                               RF_IF_DIGRF_SHIFT_SAMPLE(g_mailbox.pal2spc.rf.digrfRxSampleWidth);
#endif

    }
#else // (!CHIP_HAS_ANA_RF) : DigRf only and modified RF_IF
    UINT8 sample_size = g_mailbox.pal2spc.rf.digrfRxSampleWidth;
    sample_size = (sample_size>16)?0:
                  (sample_size<12)?4:
                  (16-sample_size);
    g_spcCtx->RfIfCtrl |=
        (g_mailbox.pal2spc.rf.digrfRxRate?RF_IF_DIGRF_RX_RATE_TWO:RF_IF_DIGRF_RX_RATE_ONE) |
        (g_mailbox.pal2spc.rf.digrfRxClkPol?RF_IF_DIGRF_RX_CLK_POL:0) |
        (g_mailbox.pal2spc.rf.digrfTxMode?RF_IF_DIGRF_TX_MODE_BLOCK:RF_IF_DIGRF_TX_MODE_STREAM) |
        (g_mailbox.pal2spc.rf.digrfTxClkPol?RF_IF_DIGRF_TX_CLK_POL:0) |
        RF_IF_DIGRF_SAMPLE_SIZE(sample_size) |
        ((g_mailbox.pal2spc.rf.digrfRxSampleAlign==SPC_DIGRF_SAMPLE_ALIGN_MSB)?
         0:RF_IF_DIGRF_ALIGNEMENT_SELECT);

#endif

    spal_RfIfWriteCtrl(g_spcCtx->RfIfCtrl);

    g_spcCtx->FcchFound = 0;
    g_spcCtx->FcchSet = 0;
    g_spcCtx->txQty = 0;
    g_spcCtx->rxQty = 0;
    g_spcCtx->Taf_Flag = 0;
    g_spcCtx->DTX_dwnlk_flag=0;
    g_spcCtx->DTX_dwnlk_count=0;
    g_spcCtx->sendEqItr = 0;
    g_spcCtx->resIdx = 0;
    for (i=0; i<4; i++) g_spcCtx->cs_identifier[i]=CS1_ID;
    g_spcCtx->frame_over=0;
    g_spcCtx->loop_bufIdx=0;
    g_spcCtx->CodecModeRequest=0;
    g_spcCtx->ratscch_to_encode=0;
    g_spcCtx->ACK_Activation=0;
    g_spcCtx->REQ_Activation=0;
    g_spcCtx->RATSCCH_REQ=NO_REQ;
    g_spcCtx->ratscch_detected=0;
    g_spcCtx->sidupdate_ctr=0;
    g_spcCtx->AFNmod104=0;
    g_spcCtx->FCCH_Result.FOf = 0;
    g_spcCtx->FCCH_Result.TOf = 0;
    g_spcCtx->SCH_Result.FOf = 0;
    g_spcCtx->SCH_Result.TOf = 0;
    g_spcCtx->SCH_Result.Snr = 0;
    g_spcCtx->SCH_Result.Data[0] = 0;
    g_spcCtx->SCH_Result.Data[1] = 0;
    g_spcCtx->SCH_Result.Data[2] = 0;
    g_spcCtx->SCH_Result.Data[3] = 0;

    // interleaver input buffer
    g_spcCtx->ItlvBufRxCtrl = ((UINT32 *) KSEG1((UINT32)&(g_spcItlvBufTab.RxCtrl)));
    g_spcCtx->ItlvBufRxDedicated = ((UINT32 *) KSEG1((UINT32)&(g_spcItlvBufTab.RxDedicated)));
#if  (SPC_IF_VER ==5)&&(defined CHIP_DIE_8955)
    if (g_spcCtx->IsSaicEnable)
    {
        g_spcCtx->ItlvBufRxCtrlSaic = ((UINT32 *) KSEG1((UINT32)&(g_spcItlvBufTabSaic[0])));
    }
#endif

    g_spcCtx->ItlvBufTxCtrl = ((UINT32 *) KSEG1((UINT32)&(g_spcItlvBufTab.TxCtrl)));
    g_spcCtx->ItlvBufTxDedicated = ((UINT32 *) KSEG1((UINT32)&(g_spcItlvBufTab.TxDedicated)));
    g_spcCtx->ItlvBufTxFacch = ((UINT32 *) KSEG1((UINT32)&(g_spcItlvBufTab.TxFacch)));

    // interleaver output buffer
    // Starting from the end of g_spcStaticBufTab, and sharing with BCPU software stack.
    // On 8808, without edge supported, the max usage of  g_spcCtx->Malloc is 0x860 bytes;
    // and the max stack usage is about 0x6A0 bytes. This means that the hole between
    // g_spcStaticBufTab end and _bb_sram_romed_globals_start must be larger than
    // (0x860 + 0x6A0) = 0xF00 bytes.
    g_spcCtx->Malloc = (UINT32 *)((UINT32)&g_spcStaticBufTab + sizeof(g_spcStaticBufTab));

    // next address must be aligned on 128 bytes
    g_spcCtx->ItlvBufRxFacch = (UINT32*) KSEG1((UINT32)(&(g_spcStaticBufTab.RxIfcBuffer[2])+0x40)&0xFFFFFFC0);
    g_spcCtx->loopC_buffer = ((UINT8 *) g_spcCtx->ItlvBufRxCtrl);
    // the last 100 estimated C/I in AMR are stored here
    g_spcCtx->CIest=(INT8 *) (g_spcCtx->ItlvBufTxFacch);

    // Tx buffer addresses initialisation in g_mailbox
    for (i=0; i < MBOX_MAX_BUF_IDX_QTY-1; i++)
    {
        g_spcCtx->NBlockDataIn[i]=(UINT32*) KSEG1((UINT32) g_spcCtx->DCH_NBlockDataIn[i]);
    }
    g_spcCtx->NBlockDataIn[MBOX_MAX_BUF_IDX_QTY-1] = (UINT32*) KSEG1((UINT32) g_spcCtx->CCH_NBlockDataIn);
    g_spcCtx->NBlockDataIn[MBOX_MAX_BUF_IDX_QTY] = (UINT32*) KSEG1((UINT32) g_spcCtx->CSD_NBlockDataIn);

    for (i=0; i < MBOX_MAX_BUF_IDX_QTY+1; i++)
    {
        g_mailbox.spc2pal.encBuff[i] = (UINT8 *)g_spcCtx->NBlockDataIn[i];
    }

    // Rx buffer addresses initialisation in g_mailbox
    for (i=0; i < MBOX_MAX_BUF_IDX_QTY-1; i++)   // for PDCH block to encode
    {
        g_spcCtx->NBlockDataOut[i] = (UINT32*) KSEG1((UINT32) g_spcCtx->DCH_NBlockDataOut[i]);
    }
    g_spcCtx->NBlockDataOut[MBOX_MAX_BUF_IDX_QTY-1] = (UINT32*) KSEG1((UINT32) g_spcCtx->CCH_NBlockDataOut);
    g_spcCtx->NBlockDataOut[MBOX_MAX_BUF_IDX_QTY] = (UINT32*) KSEG1((UINT32) g_spcCtx->CSD_NBlockDataOut);

    spal_XcorResetFchFilterParam();
    spp_ResetFullBurstReady();
}

