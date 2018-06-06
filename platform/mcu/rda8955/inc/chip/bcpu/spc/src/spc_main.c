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

#include "string.h"

#include "spc_mailbox.h"
#include "spc_ctx.h"
#include "spc_map.h"
#include "spcp_spp_cfg.h"
#include "spcp_amr.h"
#include "spcp_debug.h"
#include "spc_profile_codes.h"
#include "spc_private.h"
#include "spp_gsm.h"
#include "spp_map.h"
#include "spal_irq.h"
#include "spal_cfg.h"
#include "spal_comregs.h"
#include "spal_debug.h"
#include "spal_mem.h"
#include "spal_rfif.h" // FCCH RF-IF FIFO FIX
#include "spal_map.h"



#if (SPC_IF_VER >= 4)
#include "hal_mem_map.h"
#endif
#if (CHIP_HAS_BCPU_ROM)
#include "hal_debug.h"
#include "patch_m.h"
#endif
#include "spal_cache.h"



#if (SPC_IF_VER >= 5)
extern SPAL_PROFILE_CONTROL_T g_spalProfileControl;
#if (SPC_IF_VER == 5)
#define SPAL_PROFILE_BUFFER_SIZE 64
#else
//#define SPAL_PROFILE_BUFFER_SIZE 256
#define SPAL_PROFILE_BUFFER_SIZE 512
#endif
UINT32 SPAL_BBSRAM_PATCH_UCBSS g_spalProfileBuffer[SPAL_PROFILE_BUFFER_SIZE];
#endif // SPC_IF_VER >= 5

#if (CHIP_HAS_BCPU_ROM == 0)
#define BCPU_MAIN_LOCATION
#else
#define BCPU_MAIN_LOCATION __attribute__((section(".bcpu_main_entry")))

// BCPU ROM version number (defined in BCPU ROM)
extern CONST UINT32 g_bcpuRomVersionNumber;
#endif

// BCPU GDB stack pointer context
extern UINT32* bcpu_sp_context;

// sp context structure
extern SPC_CONTEXT_T g_spcCtxStruct;

#ifndef CHIP_BYPASS_ROM
extern VOLATILE UINT32 g_bcpuIrqCause;
#endif

#if (SPC_IF_VER < 5)
// Whether SPAL_IRQ_RX_FULL is received for NBurst or SCH.
// Note that it is named as NBurstSchIfc2Done in SPC_CONTEXT_T when SPC_IF_VER >= 5.
extern VOLATILE BOOL g_spcIfc2Done;
#endif


#ifdef CHIP_HAS_EXTRA_WINDOWS
SPC_MBX_EXT_T SPAL_UNCACHED_DATA_INTERNAL g_ext_ref;
extern VOID patch_spc_SchedulerMain(VOID);
extern VOID patch_spc_IrqHandler(SPAL_IRQ_STATUS_T status);
#endif
#ifdef BB_IFC_WORKAROUND
extern VOID patch_spc_IrqHandler(SPAL_IRQ_STATUS_T status);
#endif
#ifdef CHIP_SAIC_CTRL_BY_SOFT
extern VOID patch_spc_IrqHandler(SPAL_IRQ_STATUS_T status);
#endif
extern VOID spc_IrqSetMaskTcu1(VOID);

extern BOOL spp_FcchDetectAsm_v2(UINT32* Ptr_old,
                                 UINT32* Ptr_new,
                                 SPP_FCCH_PARAM_T* Params,
                                 SPP_FCCH_RESULT_T* Res);

#ifdef ENABLE_PATCH_SPC_AMRENCODE
extern SPAL_DATA_INTERNAL UINT32 g_spcCtxAmrCfgTxChg;
#endif


// =============================================================================
// g_spcMapAccess
// -----------------------------------------------------------------------------
/// Contains the addresses of the structures of SPC that will be accessible from
/// a remote PC (e.g. through CoolWatcher).
// =============================================================================
PROTECTED CONST SPC_MAP_ACCESS_T g_spcMapAccess
    = {(SPC_MAILBOX_T*)&g_mailbox, &g_spcCtxStruct};

#if 0
//  HOOK Testing functions
//
#include "global_macros.h"
#include "bb_irq.h"
#include "comregs.h"
#include "spal_debug.h"

PRIVATE UINT32 g_spcHookCtr = 0;

PROTECTED VOID spc_SchedHook(VOID)
{
    hwp_bbIrq->Mask_Set = BB_IRQ_BB_IRQ_COM0;
    hwp_bbComregs->Mask_Set = 0xff00;
    if(g_spcHookCtr++%100 == 3)
    {
        spal_DbgHstSendEvent(g_spcHookCtr);
    }
}

PROTECTED VOID spc_IrqHook(VOID)
{
    hwp_bbComregs->ItReg_Clr = 0xff00;

    spal_DbgHstSendEvent(0x66600666);
}
#endif


/////////////////////////////////////////////////////////////////
// Add SPC Scheduler hook to set one BfiThresold for each vocoder
//  for Greenstone only
/////////////////////////////////////////////////////////////////
#if (SPC_IF_VER == 1)
PROTECTED VOID SPAL_FUNC_INTERNAL spc_SchedHook(VOID)
{
    SPC_PROFILE_WINDOW_ENTER(SPC_SCHED_HOOK);

    if ((SPEECH(g_spcCtxStruct.ChMode)==EFR_CODEC))
    {
        g_spcCtxStruct.BfiThreshold = 70;//EFR with a strong crc check
    }
    else if (RATE(g_spcCtxStruct.ChMode)==HR_RATE)
    {
        g_spcCtxStruct.BfiThreshold = 40;//HR keep the same threshold as before
    }
    ////////////////////////////////////////////////////////
    // check the dummy PCH burst detection is enabled
    if(g_spcCtxStruct.sched_rx_count==0)
    {
        g_mailbox.dummyPchResult=SPP_DUMMY_PCH_NO_RESULT;
    }
    else if((g_spcCtx->sendEqItr==1)&&(g_mailbox.dummyPchResult==SPP_DUMMY_PCH_NO_RESULT))
    {
        UINT8 buffIdx=g_mailbox.pal2spc.win[g_spcCtxStruct.currentSnap].rx.nBurst.bufIdx;
        UINT8 burstIdx=g_mailbox.pal2spc.win[g_spcCtxStruct.currentSnap].rx.nBurst.bstIdx;
        UINT32 *EqualizerOutput=(UINT32*)(g_spcCtxStruct.ItlvBufRxCtrl+
                                          buffIdx*SPC_ITLV_RX_BUFF_OFFSET+
                                          burstIdx * 32);

        if((g_spcCtxStruct.statWinRxCfg[buffIdx].itr&MBOX_DSP_PCH_READY))
        {
            UINT8 result;
            // check if it is the last burst
            if (burstIdx==3)
            {
                result = SPP_PCH_DECODING_COMPLETE;
            }
            else
            {
                result = spp_DetectDummyPCH(EqualizerOutput,
                                            burstIdx,
                                            ((g_mailbox.dummyPchThresh)&0xFF00)>>8,
                                            (g_mailbox.dummyPchThresh)&0xFF);
            }
            g_mailbox.dummyPchResult = (UINT32) result;
            spal_ComregsSetIrq(MBOX_DSP_PCH_READY);
        }
    }
    ////////////////////////////////////////////////////////
    // send IRQ COM0 to XCPU at the end of the BCPU processing
    if ((g_spcCtxStruct.frame_over==1)&&(g_spcCtxStruct.FcchActive == 0))
    {
        // end of the current frame process
        spal_ComregsSetIrq(MBOX_DSP_TASK_COMPLETE);
    }

    SPC_PROFILE_WINDOW_EXIT(SPC_SCHED_HOOK);
}
#endif

PUBLIC VOID SPAL_INTSRAM_PATCH_TEXT spc_CchDump(UINT16* StartAdrr, UINT16 length, UINT16 Marker)
{
    if(g_spcCtx->statWinRxCfg[0].itr &
            MBOX_DSP_CCH_DECODING_COMPLETE)
    {
        spal_ComregsSetIrq(MBOX_DSP_CCH_DECODING_COMPLETE);
    }
}

/// **********************************************
/// BCPU entry point
/// This function should NOT be ROM'ed
/// **********************************************/
VOID BCPU_MAIN_LOCATION bcpu_main(VOID)
{

    SPP_GLOBAL_PARAMS_T spcSppCfg;
    SPAL_GLOBAL_PARAMS_T spcSpalCfg;
    
#if ((defined CHIP_HAS_MMU) && (CHIP_HAS_MMU == 1))
    spal_tlb_reset();
#endif

#if (SPC_IF_VER >= 5)
#ifdef ENABLE_PATCH_SPC_AMRENCODE
    g_spcCtxAmrCfgTxChg = 0;
#endif
    g_spalProfileControl.config =
        SPAL_PROFILE_CONTROL_SPC
        //|SPAL_PROFILE_CONTROL_SPP
        //|SPAL_PROFILE_CONTROL_SPAL
        |SPAL_PROFILE_CONTROL_BB_IRQ
        |SPAL_PROFILE_CONTROL_BB_RX_WIN
        |SPAL_PROFILE_CONTROL_BB_IFC2_SYM_CNT;
    g_spalProfileControl.buffer_start = g_spalProfileBuffer;
    g_spalProfileControl.buffer_pos = 0;
    g_spalProfileControl.buffer_size = sizeof(g_spalProfileBuffer)/sizeof(UINT32);
#endif

#if (SPC_IF_VER >= 4)
    UINT32 *src, *dst;
    // Copy patch code & data from ROM space to BB SRAM.
    for (src = (UINT32*) &_bb_sram_patch_flash_start,
            dst = (UINT32*) &_bb_sram_patch_start;
            src < (UINT32*) &_bb_sram_patch_flash_end;
            src++, dst++)
    {
        *dst = *src;
    }

    // Clear patch BSS in BSS SRAM.
    for (dst = (UINT32*) &_bb_sram_patch_bss_start;
            dst < (UINT32*) &_bb_sram_patch_bss_end;
            dst++)
    {
        *dst = 0;
    }

#if (CHIP_HAS_BCPU_ROM)
    // Copy code & data from ROM space to Dualport SRAM.
    for (src = (UINT32*) &_bcpu_dualport_sram_flash_start_location,
            dst = (UINT32*) &_bcpu_dualport_sram_start;
            src < (UINT32*) &_bcpu_dualport_sram_flash_end_location;
            src++, dst++)
    {
        *dst = *src;
    }

    // Clear BSS in Dualport SRAM.
    for (dst = (UINT32*) &_bcpu_dualport_bss_start;
            dst < (UINT32*) &_bcpu_dualport_bss_end;
            dst++)
    {
        *dst = 0;
    }

    // Copy code & data from ROM space to Internal SRAM.
    for (src = (UINT32*) &_bcpu_sys_sram_flash_start_location,
            dst = (UINT32*) &_bcpu_sys_sram_start;
            src < (UINT32*) &_bcpu_sys_sram_flash_end_location;
            src++, dst++)
    {
        *dst = *src;
    }

    // Clear BSS in Internal SRAM.
    for (dst = (UINT32*) &_bcpu_sys_sram_bss_start;
            dst < (UINT32*) &_bcpu_sys_sram_bss_end;
            dst++)
    {
        *dst = 0;
    }

#ifdef CHIP_HAS_AP
    // Copy ram data from load space to execution space.
    for (src = (UINT32*) &_bcpu_ram_data_load_start,
            dst = (UINT32*) &_bcpu_ram_data_start;
            src < (UINT32*) &_bcpu_ram_data_load_end;
            src++, dst++)
    {
        *dst = *src;
    }
#else
    // Copy code & data from ROM space to External RAM.
    for (src = (UINT32*) &_bcpu_ram_flash_start_location,
            dst = (UINT32*) &_bcpu_ram_start;
            src < (UINT32*) &_bcpu_ram_flash_end_location;
            src++, dst++)
    {
        *dst = *src;
    }
#endif

    // Clear BSS in External RAM.
    for (dst = (UINT32*) &_bcpu_ram_bss_start;
            dst < (UINT32*) &_bcpu_ram_bss_end;
            dst++)
    {
        *dst = 0;
    }

#ifndef REWRITE_ROM_AT_BOOT
    // Check that the BCPU ROM version in the chip is the same as the BCPU ROM version
    // of the BCPU ROM Library against which the code has been compiled.
    if (ROM_LINKED_AGAINST_VERSION != g_bcpuRomVersionNumber)
    {
        // Mismatch!
        g_mailbox.spc2pal.debug[0] = MBOX_BCPU_ROM_ERR_WORD;
        while(1);
    }
#endif // !REWRITE_ROM_AT_BOOT

    // Install all patches
    patch_Open();
#if ((defined BCPU_CACHE_MODE_WRITE_BACK) && (BCPU_CACHE_MODE_WRITE_BACK == 1))
    spal_probe_cache();
    spal_FlushDCache(TRUE);
#endif
#endif // CHIP_HAS_BCPU_ROM

#endif // (SPC_IF_VER >= 4)

    // Check that Xcpu did initiate the handshake
    if (g_mailbox.spc2pal.debug[0] == MBOX_XCPU_HELLO_WORD)
    {
        // Init AMR state
        g_spcAmrState.dtxUsed = 0;
        g_spcAmrState.prevId = 0;
        g_spcAmrState.rescheduledSidFirst = 0;
        g_spcAmrState.rescheduledSidUpdate = 0;
        g_spcAmrState.previousFrameType = SPP_TX_SPEECH;

        // Acknowledge the handshake
        g_mailbox.spc2pal.debug[0] = MBOX_BCPU_ACKNOWLEDGE_WORD;

        ///////////////////////////////////////////////////////////////////////
        //                  Setup SPC switches and parameters                //
        ///////////////////////////////////////////////////////////////////////
#if (SPC_IF_VER != 0)
#if (SPC_IF_VER == 1) // keep scheduler hook for greenstone
        g_spcCtxStruct.schedulerHook = (VOID*)spc_SchedHook;// NULL;
#else
        g_spcCtxStruct.schedulerHook = NULL;
#endif // (SPC_IF_VER == 1)
        g_spcCtxStruct.irqHook = /*(VOID*)spc_IrqHook;*/ NULL;
#endif // (SPC_IF_VER != 0)

        g_spcCtxStruct.FacchThreshold = 0xFFF8;// -8;

#if (SPC_IF_VER < 2)
        g_spcCtxStruct.loop_bufIdx = 0;
        g_spcCtxStruct.BfiThreshold = 80;//50;
#else // (SPC_IF_VER >= 3)
        g_spcCtxStruct.dumpPtr = NULL;//(VOID*)spal_DbgPxtsDump;
        g_spcCtxStruct.dumpMask = 0;
        g_spcCtxStruct.ifc2_burst_mode_fcch = FALSE;
        g_spcCtxStruct.ifc2_burst_mode_sbnb = TRUE;
        g_spcCtxStruct.FR_BfiThreshold = 80;//50;
        g_spcCtxStruct.EFR_BfiThreshold = 100;//70;
        g_spcCtxStruct.HR_BfiThreshold = 70;//40;
        g_spcCtxStruct.FcchSet =  0;
        g_spcCtxStruct.FcchWinSize =  BB_FCCH_SIZE;
        g_spcCtxStruct.FcchPreOpening = BB_FCCH_PRE_OPENING;
        g_spcCtxStruct.MonWinSize =  MBOX_MON_WIN_SIZE;
#endif // (SPC_IF_VER >= 3)


        g_spcCtxStruct.dumpPtr = spc_CchDump;//(VOID*)spal_DbgPxtsDump;
        g_spcCtxStruct.dumpMask = SPC_DBG_DUMP_GSM_CCH_DEC_OUT;


#if (SPC_IF_VER >= 4)
        g_spcCtxStruct.FchSearchMode = FCCH_SEARCH_ALGO_DELAY2;
#endif
#if (SPC_IF_VER >= 5)
        g_spcCtxStruct.NBurstSchIfc2Done = FALSE;
        g_spcCtxStruct.xcvPdnEnable = 1; //enable/disable xcv pdn after fch searching
        g_spcCtxStruct.amrFrmFltEnable = 1; //enable/disable amr frame filter
#endif


#if (SPC_IF_VER < 5)
        g_spcIfc2Done = FALSE;
#endif

        g_spcCtxStruct.UfiThreshold = 30;
        g_spcCtxStruct.FsBfiBerThresh = 80;//60;
        g_spcCtxStruct.FsBfiBer2Thresh = 0;//50;
        g_spcCtxStruct.FsBfiSnrThresh = -12;
        g_spcCtxStruct.HsBfiBerThresh = 60;//45;
        g_spcCtxStruct.HsBfiBer2Thresh = 0;//20;
        g_spcCtxStruct.HsBfiSnrThresh = 0;

        g_spcCtxStruct.AFS_ber1_threshold[0]=75;
        g_spcCtxStruct.AFS_ber1_threshold[1]=75;
        g_spcCtxStruct.AFS_ber1_threshold[2]=75;
        g_spcCtxStruct.AFS_ber1_threshold[3]=75;
        g_spcCtxStruct.AFS_ber1_threshold[4]=75;
        g_spcCtxStruct.AFS_ber1_threshold[5]=75;
        g_spcCtxStruct.AFS_ber1_threshold[6]=75;
        g_spcCtxStruct.AFS_ber1_threshold[7]=55;
        g_spcCtxStruct.AHS_ber1_threshold[0]=35;
        g_spcCtxStruct.AHS_ber1_threshold[1]=35;
        g_spcCtxStruct.AHS_ber1_threshold[2]=35;
        g_spcCtxStruct.AHS_ber1_threshold[3]=34;
        g_spcCtxStruct.AHS_ber1_threshold[4]=33;
        g_spcCtxStruct.AHS_ber1_threshold[5]=32;
        g_spcCtxStruct.AFS_ber2_threshold[0]=100;
        g_spcCtxStruct.AFS_ber2_threshold[1]=95;
        g_spcCtxStruct.AFS_ber2_threshold[2]=95;
        g_spcCtxStruct.AFS_ber2_threshold[3]=95;
        g_spcCtxStruct.AFS_ber2_threshold[4]=95;
        g_spcCtxStruct.AFS_ber2_threshold[5]=85;
        g_spcCtxStruct.AFS_ber2_threshold[6]=70;
        g_spcCtxStruct.AFS_ber2_threshold[7]=50;
        g_spcCtxStruct.AHS_ber2_threshold[0]=30;
        g_spcCtxStruct.AHS_ber2_threshold[1]=30;
        g_spcCtxStruct.AHS_ber2_threshold[2]=25;
        g_spcCtxStruct.AHS_ber2_threshold[3]=20;
        g_spcCtxStruct.AHS_ber2_threshold[4]=15;
        g_spcCtxStruct.AHS_ber2_threshold[5]=15;


#ifdef CHIP_DIE_8955
#ifdef  CHIP_SAIC_SUPPORTED
        g_spcCtxStruct.IsSaicEnable = TRUE;
#ifdef CHIP_SAIC_ENABLE_STEP2
        spcSppCfg.IsSaicStep2Enable = TRUE;
#else
        spcSppCfg.IsSaicStep2Enable = FALSE;
#endif
#else
        g_spcCtxStruct.IsSaicEnable = FALSE;
        spcSppCfg.IsSaicStep2Enable = FALSE;
#endif
        g_spcCtxStruct.DummySnrThreshold = 10;
        spcSppCfg.GcfMode = FALSE;
        spcSppCfg.SaicDiagDelta1 = 64;
        spcSppCfg.SaicDiagDelta2 = 100;
        spcSppCfg.CgIternum = 17;
        spcSppCfg.TofFixedEnable = TRUE;
        spcSppCfg.DigitalAgc = FALSE;
        spcSppCfg.DigitalAgcTarget= 23;
        spcSppCfg.EnhanceMode = TRUE;
        spcSppCfg.ChannelTracking = TRUE;
        spcSppCfg.TapsShorten = FALSE;
        spcSppCfg.InterfernceDetect = FALSE;
        spcSppCfg.Ht100QrPrefilter= FALSE;
        spcSppCfg.Ht100Mode = TRUE;
        spcSppCfg.Ht100QrNoise = 25600;
        spcSppCfg.FadingChannelCnt = 0;
        spcSppCfg.FadingChannel8TapNum = 0;
        spcSppCfg.FadingChannel8TapState = 0;
        spcSppCfg.NoiseRatio = 0;
        spcSppCfg.RxBufferIndex = 0;
        spcSppCfg.TapsShortFac = 5;
#endif


        ///////////////////////////////////////////////////////////////////////
        //                  Setup SPP switches and parameters                //
        ///////////////////////////////////////////////////////////////////////
#if 1
        spcSppCfg.bypassDcoc = (g_mailbox.pal2spc.rf.dcoCalEnable & SPC_SW_DCOC_ENABLED)?FALSE:
                               TRUE;
        // Cleanup the field to match rom'ed implementation
        g_mailbox.pal2spc.rf.dcoCalEnable &= SPC_HW_DCOC_ENABLED;
#else
        // TODO update interface
        spcSppCfg.bypassDcoc = TRUE;
#endif
        spcSppCfg.correctCS1 = TRUE;
#ifdef GCF_TEST
        spcSppCfg.correctCS4 = TRUE;
#else
        spcSppCfg.correctCS4 = FALSE;
#endif
        spcSppCfg.equ_hburst_mode =
            g_spcCtxStruct.equ_hburst_mode = FALSE;
        spcSppCfg.symb_max = _SYMB_MAX;
        spcSppCfg.symb_min = _SYMB_MIN;
        spcSppCfg.fof_nb_symb = _FOF_NB_SYMB;
        spcSppCfg.fof_start_1 = _FOF_START_1;
        spcSppCfg.fof_start_2 = _FOF_START_2;
        spcSppCfg.fof_corr_gap = _FOF_CORR_GAP;
        spcSppCfg.agc_threshold = _AGC_THRESHOLD;
        spcSppCfg.mon_win_size = MBOX_MON_WIN_SIZE;
        spcSppCfg.flag_enableDC = FALSE;
        spcSppCfg.flag_enableIQ = FALSE;
        // TODO: The shifts might be different depending on the xcv and the interface RF.
        // We might need in the future to pass these parameters from the xcv driver.
        spcSppCfg.shift_bm_offset= 0;
        spcSppCfg.shift_sb_offset= 1;

#if (SPC_IF_VER >= 2)
        spcSppCfg.fcch_size =  g_spcCtxStruct.FcchWinSize;
#if (SPC_IF_VER < 5)
        spcSppCfg.fcch_detect_func =  spp_FcchDetectAsm_v2;
#else
        spcSppCfg.fcch_detect_func =  spp_FcchDetectAsm;
#endif
        spcSppCfg.fcch_sliding_energy = TRUE;
        spcSppCfg.fcch_hw_sliding_window = TRUE;
#endif

#if (SPC_IF_VER >= 5 && CHIP_ASIC_ID != CHIP_ASIC_ID_8809)
        CONST UINT8 eqShiftTable[EQ_SHIFT_MAX_NUM] =
        {
            // shift bits         snr
            5,              // 0
            3,              // 1
            3,              // 2
            2,              // 3
            1,              // 4
            1,              // 5
            0,              // 6
            0,              // 7
        };
        memcpy(spcSppCfg.eqShiftTable, eqShiftTable, sizeof(spcSppCfg.eqShiftTable));
#endif
#if ((defined BCPU_CACHE_MODE_WRITE_BACK) && (BCPU_CACHE_MODE_WRITE_BACK == 1))
        spal_FlushDCache(TRUE);
#endif

        // Open SPP (configure SPP)
        spp_Open(&spcSppCfg);
        // Init BCPU stack pointer context to 0 for CoolGDB
        // (it was initialized to 0x0db90db9 in spp_Open->spal_GdbOpen
        //  before and including 8810)
        bcpu_sp_context = (UINT32*)0;

#if ((defined BCPU_CACHE_MODE_WRITE_BACK) && (BCPU_CACHE_MODE_WRITE_BACK == 1))
    spal_memInit(0); //malloc sie by defalt value
#endif
        ///////////////////////////////////////////////////////////////////////
        //                  Setup SPAL switches and parameters               //
        ///////////////////////////////////////////////////////////////////////
#ifdef CHIP_BYPASS_ROM

        spcSpalCfg.irqHandler = spc_IrqHandler;
        spcSpalCfg.irqInRom = FALSE;
        spal_Open(&spcSpalCfg);
#if (SPC_IF_VER < 5)
        spc_IrqSetMaskTcu1();
#endif

#else // !CHIP_BYPASS_ROM

        spcSpalCfg.irqHandler = spc_IrqHandler;
#ifdef CHIP_HAS_EXTRA_WINDOWS
        spcSpalCfg.irqHandler = patch_spc_IrqHandler;
#endif
#ifdef BB_IFC_WORKAROUND
        spcSpalCfg.irqHandler = patch_spc_IrqHandler;
#endif
#ifdef CHIP_SAIC_CTRL_BY_SOFT
        spcSpalCfg.irqHandler = patch_spc_IrqHandler;
#endif
        spcSpalCfg.irqInRom = TRUE;
        spal_Open(&spcSpalCfg);
#if (SPC_IF_VER < 5)
        spc_IrqSetMaskTcu1();
#endif

#endif // !CHIP_BYPASS_ROM

#ifdef CHIP_HAS_EXTRA_WINDOWS
        g_mailbox.extRef = &g_ext_ref;
#endif

        ///////////////////////////////////////////////////////////////////////
        //                 Start the main while loop                         //
        ///////////////////////////////////////////////////////////////////////
#if defined(CHIP_HAS_EXTRA_WINDOWS) || (CHIP_ASIC_ID == CHIP_ASIC_ID_8810) || (CHIP_ASIC_ID == CHIP_ASIC_ID_8850E)
        patch_spc_SchedulerMain();
#else
        spc_SchedulerMain();
#endif
    }
    else
    {
        g_mailbox.spc2pal.debug[0] = MBOX_BCPU_ERROR_WORD;
    }

    while (1);
}

