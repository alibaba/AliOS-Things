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


#include "chip_id.h"

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8809 || CHIP_ASIC_ID == CHIP_ASIC_ID_8809P)

// TOP
#include "cs_types.h"
#include "gsm.h"
#include "baseband_defs.h"

// SPC
#include "spc_ctx.h"
#include "spc_private.h"
#include "spc_mailbox.h"
//#include "spcp_debug.h"
#include "spc_profile_codes.h"
// SPP
#include "spp_gsm.h"
#include "sppp_gsm_private.h"
#include "sppp_cfg.h"
// SPAL
#include "spal_comregs.h"
#include "spal_mem.h"
#include "spal_tcu.h"
#include "spal_ifc.h"
#include "xcor.h"
// CHIP
#include "global_macros.h"
#include "tcu.h"


#include "spal_mem.h"
#include "spal_irq.h"
#include "spal_rfif.h"

#include "string.h"

//#include "excor.h"


#define GPRS_CCHRX_DEBUG        0
#define GPRS_EQ8TAP_DEBUG       0
#define GPRS_EQPREFLT_DEBUG     0
#define EGPRS_PREFLT_DEBUG      0
#define EGPRS_LPEST_DEBUG       0
#define GPRS_3TAP_REB_DEBUG     0


// copy declarations from "spcp_debug.h"

// separation marker for dump
#define SPC_DBG_DUMP_EQU_IN_MARKER 0xAAAA
#define SPC_DBG_DUMP_EQU_OUT_MARKER 0xBBBB
#define SPC_DBG_DUMP_DEC_IN_MARKER 0xCCCC
#define SPC_DBG_DUMP_DEC_OUT_MARKER 0xDDDD
// GSM CCH (GPRS)
// dump equalization input: (I,Q) Marker = 0xAAAA
#define SPC_DBG_DUMP_GSM_CCH_EQU_IN 0x1
// dump equalization output: SoftBits Marker = xBBBB
#define SPC_DBG_DUMP_GSM_CCH_EQU_OUT 0x2
// dump decoder input: decoded bit Marker =  0xCCCC
#define SPC_DBG_DUMP_GSM_CCH_DEC_IN 0x4
// dump decoder output: decoded bit Marker =  0xDDDD
#define SPC_DBG_DUMP_GSM_CCH_DEC_OUT 0x8

// GSM TCH
// dump equalization input: (I,Q) Marker = 0xAAAA
#define SPC_DBG_DUMP_GSM_TCH_EQU_IN 0x10
// dump equalization output: SoftBits Marker = xBBBB
#define SPC_DBG_DUMP_GSM_TCH_EQU_OUT 0x20
// dump decoder input: decoded bit Marker =  0xCCCC
#define SPC_DBG_DUMP_GSM_TCH_DEC_IN 0x40
// dump decoder output: decoded bit Marker =  0xDDDD
#define SPC_DBG_DUMP_GSM_TCH_DEC_OUT 0x80
// EGPRS
// dump equalization input: (I,Q) Marker = 0xAAAA
#define SPC_DBG_DUMP_EGPRS_EQU_IN 0x100
// dump equalization output: SoftBits Marker = xBBBB
#define SPC_DBG_DUMP_EGPRS_EQU_OUT 0x200
// dump decoder input: decoded bit Marker =  0xCCCC
#define SPC_DBG_DUMP_EGPRS_DEC_IN 0x400
// dump decoder output: decoded bit Marker =  0xDDDD
#define SPC_DBG_DUMP_EGPRS_DEC_OUT 0x800


#ifdef SPC_PATCH_PROFILING
#define SPC_PROFILE_FUNC_ENTER      SPC_BUFFER_PROFILE_FUNCTION_ENTER
#define SPC_PROFILE_FUNC_EXIT       SPC_BUFFER_PROFILE_FUNCTION_EXIT
#define SPC_PROFILE_WIN_ENTER       SPC_BUFFER_PROFILE_WINDOW_ENTER
#define SPC_PROFILE_WIN_EXIT        SPC_BUFFER_PROFILE_WINDOW_EXIT
#else
#define SPC_PROFILE_FUNC_ENTER(eventName)
#define SPC_PROFILE_FUNC_EXIT(eventName)
#define SPC_PROFILE_WIN_ENTER(eventName)
#define SPC_PROFILE_WIN_EXIT(eventName)
#endif

// Reuse the padding space after the g_mailbox variable in dualport sram
//0: traditional 5tap eq
//1: new algo: old ce&eq combined with 8tap ce and 5tap eq
#define g_mailbox_2ndEq_Flag (*(UINT8 *)0xa1b007fc)

#ifdef BUILD_ROM_EXT_8809
#define SPAL_BBSRAM_GLOBAL_EXT_8809 __attribute__((section(".bbsram_globals_ext_8809")))
#else
#define SPAL_BBSRAM_GLOBAL_EXT_8809 SPAL_BBSRAM_GLOBAL
#endif

#define EQ_SHIFT_MAX_NUM 8

UINT8 SPAL_BBSRAM_GLOBAL_EXT_8809 g_EqShiftTable_Ext_8809[EQ_SHIFT_MAX_NUM];

UINT8 SPAL_BBSRAM_GLOBAL_EXT_8809 g_fadingchannel_cnt;
UINT8 SPAL_BBSRAM_GLOBAL_EXT_8809 g_fadingchannel_8tap_num;
UINT8 SPAL_BBSRAM_GLOBAL_EXT_8809 g_fadingchannel_8tap_state;
UINT8 SPAL_BBSRAM_GLOBAL_EXT_8809 g_dgc_pwr_target_shift;


CONST UINT8 gc_EqShiftTableDefault_Ext_8809[EQ_SHIFT_MAX_NUM] =
{
//   shift bits         snr
    5,              // 0
    3,              // 1
    3,              // 2
    2,              // 3
    1,              // 4
    1,              // 5
    0,              // 6
    0               // 7
};

// Egprs Chest Table
CONST INT16 gc_sppChestTabEgprs_Ext_8809[8][8][19] =
{
    {
        {-1878,-1976,-1278,1745,1275,1681,1103,-1595,2347,2470,1597,-2175,1745,1745,-1423,1595,-1878,-1976,-1278},
        {1253,-1382,-1505,-2355,2054,1665,1536,1665,-1567,1728,1882,2084,-2355,1740,2183,-1665,1253,-1382,-1505},
        {-1265,1155,-1290,-2018,-2335,2012,1901,2012,1582,-1444,1613,2372,2077,-2018,1871,2083,-1265,1155,-1290},
        {1265,-1155,1290,-2077,-1760,-2012,2193,2083,2513,1444,-1613,1723,2018,2018,-1871,2012,1265,-1155,1290},
        {1398,1300,-1278,1745,-2000,-1595,-2172,1681,2347,2470,1597,-2175,1745,1745,1853,-1681,1398,1300,-1278},
        {-1253,1382,1505,-1740,2041,-1665,-1536,-1665,1567,2367,2213,2011,-1740,2355,1912,1665,-1253,1382,1505},
        {1265,-1155,1290,2018,-1760,2083,-1901,-2012,-1582,1444,2482,1723,2018,-2077,2224,2012,1265,-1155,1290},
        {1868,1794,-958,1308,1776,-2015,1646,-2015,-2335,-2243,1198,2464,1308,1308,-1886,2015,1868,1794,-958}
    },
    {
        {-1878,-1976,-1278,1745,-1423,-1103,-2003,-1275,2347,2470,1597,-1745,1853,-1917,-1423,1595,-1878,-1976,-1278},
        {1253,-1382,-1505,-2355,2183,-1536,-1493,-2054,-1567,1728,1882,2355,-1912,1837,-1912,-1665,1253,-1382,-1505},
        {-1265,1155,-1290,-2018,-2224,2193,-1865,-1760,-2513,-1444,1613,2018,1871,-1935,1871,-2012,-1265,1155,-1290},
        {-1398,-1300,1278,-1745,-1853,-2172,2003,-2000,-2347,-2470,-1597,1745,1423,1917,-1853,1681,-1398,-1300,1278},
        {1253,-1382,-1505,1740,-1912,-1536,-1493,2041,-1567,-2367,-2213,-1740,2183,1837,2183,-1665,1253,-1382,-1505},
        {-1265,1155,-1290,-2018,1871,-1901,-1865,-1760,1582,-1444,-2482,-2077,-2224,2160,1871,2083,-1265,1155,-1290},
        {1265,-1155,1290,-2077,-1871,1901,-2230,-2335,-1582,1444,-1613,-2018,-1871,-2160,2224,2012,1265,-1155,1290},
        {1868,1794,-958,1308,-1886,-1646,2593,-1776,-2335,-2243,1198,-1308,-1886,-1438,-1886,2015,1868,1794,-958}
    },
    {
        {-958,-1794,1868,-2015,-1886,-1308,1308,-2464,1198,2243,-2335,2015,1646,2015,1776,-1308,-958,-1794,1868},
        {-1290,-1155,-1265,2012,-2224,-2077,-2018,1723,-2482,1444,1582,-2012,1901,2083,1760,2018,-1290,-1155,-1265},
        {1505,-1382,-1253,-1665,1912,-2355,-1740,-2011,2213,-2367,1567,1665,-1536,1665,2041,1740,1505,-1382,-1253},
        {1278,1300,-1398,-1681,-1853,1745,-1745,-2175,-1597,2470,-2347,1681,2172,-1595,2000,1745,1278,1300,-1398},
        {1290,1155,1265,-2012,-1871,-2018,2018,-1723,-1613,-1444,2513,-2083,2193,2012,-1760,2077,1290,1155,1265},
        {1290,1155,1265,2083,-1871,-2018,-2077,2372,-1613,-1444,-1582,2012,-1901,2012,2335,-2018,1290,1155,1265},
        {-1505,1382,1253,1665,2183,-1740,-2355,-2084,1882,-1728,-1567,-1665,1536,-1665,2054,2355,-1505,1382,1253},
        {1278,-1976,1878,1595,1423,1745,-1745,-2175,-1597,2470,-2347,-1595,-1103,1681,-1275,1745,1278,-1976,1878}
    },
    {
        {-958,-1794,1868,-2015,-1886,1438,-1886,1308,1198,2243,-2335,1776,2593,1646,-1886,-1308,-958,-1794,1868},
        {-1290,-1155,-1265,2012,-2224,-2160,1871,-2018,1613,1444,1582,-2335,2230,1901,1871,-2077,-1290,-1155,-1265},
        {-1290,-1155,-1265,-2083,1871,-2160,-2224,2077,-2482,1444,1582,1760,-1865,1901,1871,2018,-1290,-1155,-1265},
        {1505,-1382,-1253,-1665,-2183,1837,-2183,-1740,2213,-2367,1567,2041,1493,-1536,1912,1740,1505,-1382,-1253},
        {1278,1300,-1398,-1681,-1853,-1917,1423,-1745,-1597,2470,-2347,2000,2003,2172,-1853,1745,1278,1300,-1398},
        {1290,1155,1265,-2012,-1871,-1935,-1871,2018,-1613,-1444,2513,-1760,1865,2193,2224,-2018,1290,1155,1265},
        {-1505,1382,1253,1665,-1912,-1837,-1912,-2355,1882,-1728,-1567,2054,-1493,1536,2183,2355,-1505,1382,1253},
        {1278,-1976,1878,1595,1423,-1917,-1853,-1745,-1597,2470,-2347,-1275,2003,-1103,1423,1745,1278,-1976,1878}
    },
    {
        {1249,-1384,-1480,-2076,1874,1706,-1874,2076,2331,1528,1336,2221,-1706,-2221,1706,-1874,1249,-1384,-1480},
        {-1269,1153,-1269,-2365,-1903,2047,1903,-1730,1413,2480,1730,1903,2047,-1903,-2047,1903,-1269,1153,-1269},
        {1269,-1153,1269,-1730,-2192,-2047,2192,1730,-1413,1615,2365,2192,2047,1903,-2047,-1903,1269,-1153,1269},
        {-1269,1153,-1269,1730,-1903,-2047,-2192,2365,1413,-1615,1730,1903,2047,2192,2047,-2192,-1269,1153,-1269},
        {-1480,-1384,1249,-2076,1874,-1706,-1874,-2019,1648,1528,-1394,2221,1706,1874,2389,2221,-1480,-1384,1249},
        {1249,-1384,-1480,2019,-2221,1706,-1874,-2019,-1764,1528,1336,-1874,2389,1874,1706,2221,1249,-1384,-1480},
        {1480,1384,-1249,-2019,2221,-2389,1874,-2076,-1648,-1528,1394,1874,-1706,2221,1706,1874,1480,1384,-1249},
        {1499,1615,1499,-1672,-1846,2047,-2249,1672,-1297,-1442,-1672,1846,2047,-1846,2047,1846,1499,1615,1499}
    },
    {
        {1269,-1153,1269,-1730,-2192,2047,2192,1730,2365,1615,-1413,1903,2047,-1903,-2047,-2192,1269,-1153,1269},
        {-1269,1153,-1269,1730,-1903,-2047,1903,2365,1730,2480,1413,-1903,2047,1903,-2047,-1903,-1269,1153,-1269},
        {-1480,-1384,1249,-2076,1874,-1706,-1874,2076,1336,1528,2331,1874,-1706,2221,1706,-2221,-1480,-1384,1249},
        {-1499,-1615,-1499,1672,-2249,2047,-1846,-1672,1672,1442,1297,1846,2047,-1846,2047,1846,-1499,-1615,-1499},
        {1249,-1384,-1480,-2076,1874,-2389,2221,-2019,-1394,1528,1648,1874,1706,2221,-1706,1874,1249,-1384,-1480},
        {1480,1384,-1249,-2019,-1874,1706,-2221,2019,-1336,-1528,1764,2221,1706,1874,2389,-1874,1480,1384,-1249},
        {-1249,1384,1480,-2019,-1874,-1706,1874,-2076,1394,-1528,-1648,2221,2389,1874,1706,2221,-1249,1384,1480},
        {1269,-1153,1269,2365,-2192,-2047,-1903,1730,-1730,1615,-1413,-2192,2047,2192,2047,1903,1269,-1153,1269}
    },
    {
        {-1269,-1269,-1153,2192,-2047,-1903,2047,1903,1413,-2365,1615,-1730,2192,1730,-2192,-2047,-1269,-1269,-1153},
        {-1269,-1269,-1153,-1903,2047,-1903,-2047,1903,1413,1730,-2480,2365,-1903,1730,1903,-2047,-1269,-1269,-1153},
        {-1269,-1269,-1153,-1903,-2047,2192,-2047,-2192,1413,1730,1615,-1730,2192,-2365,1903,2047,-1269,-1269,-1153},
        {1480,-1249,-1384,-1874,-1706,-1874,1706,-2221,-1648,1394,1528,2019,-1874,2076,-2221,2389,1480,-1249,-1384},
        {1499,1499,-1615,-1846,-2047,-1846,-2047,1846,-1297,-1672,1442,1672,2249,-1672,1846,-2047,1499,1499,-1615},
        {-1480,1249,1384,-2221,-2389,-2221,-1706,-1874,1648,-1394,-1528,2076,1874,2019,-1874,1706,-1480,1249,1384},
        {1249,-1480,1384,1874,-1706,-2221,-2389,-1874,-1764,1336,-1528,-2019,1874,2019,2221,-1706,1249,-1480,1384},
        {-1249,1480,-1384,2221,1706,-1874,-1706,-2221,-2331,-1336,1528,-2076,-1874,2076,1874,1706,-1249,1480,-1384}
    },
    {
        {-1480,1384,1249,1706,-1874,2389,1874,-1874,1648,-1528,-1394,-2221,2076,-2221,-2019,-1706,-1480,1384,1249},
        {-1249,-1384,1480,1706,1874,-1706,2221,1874,-2331,1528,-1336,-1874,-2076,2221,-2076,-1706,-1249,-1384,1480},
        {-1269,-1153,-1269,2047,1903,2047,-1903,1903,1413,-2480,1730,-1903,-1730,-1903,2365,-2047,-1269,-1153,-1269},
        {-1269,-1153,-1269,-2047,1903,2047,2192,-2192,1413,1615,-2365,2192,-1730,-1903,-1730,2047,-1269,-1153,-1269},
        {1480,-1384,-1249,-1706,-2221,1706,2221,1874,-1648,1528,1394,-1874,2019,-1874,-2076,-2389,1480,-1384,-1249},
        {-1499,1615,-1499,-2047,-1846,-2047,1846,2249,1297,-1442,1672,1846,-1672,1846,-1672,-2047,-1499,1615,-1499},
        {-1249,-1384,1480,-2389,-2221,-1706,-1874,1874,1764,1528,-1336,2221,2019,-1874,2019,-1706,-1249,-1384,1480},
        {-1269,-1153,-1269,2047,-2192,-2047,-1903,-2192,1413,1615,1730,-1903,2365,2192,-1730,2047,-1269,-1153,-1269}
    }
};

// decaration
extern SPP_FCCH_RETURN_T  spp_SearchFcch_Filter(UINT32* RxBuffer, BOOL Init, SPP_FCCH_MODE_T Mode,
        SPP_FCCH_RESULT_T* Res);

extern CONST VOID spp_ChannelEstimation(VOID);
//extern CONST VOID spp_Equalize(VOID);
//extern CONST VOID spp_FofEstimation(VOID);
#ifdef CHIP_DIE_8955
extern PUBLIC VOID spp_Equalize(UINT8 WinType,
                                UINT8 offset,
                                SPP_EQU_BURST_T* burst,
                                SPP_UNCACHED_BUFFERS_T* UncachedBuffers);

extern PUBLIC VOID spp_FofEstimation(UINT8 WinType, SPP_EQU_BURST_T* burst);
#else
extern PUBLIC VOID spp_Equalize(UINT8 WinType,
                                SPP_EQU_BURST_T* burst,
                                SPP_UNCACHED_BUFFERS_T* UncachedBuffers);

extern PUBLIC VOID spp_FofEstimation(UINT8 WinType, SPP_EQU_BURST_T* burst);
#endif

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

PUBLIC VOID spc_InitRx_Ext_8809(VOID)
{
    g_mailbox_2ndEq_Flag = 0;

    memcpy(g_EqShiftTable_Ext_8809, gc_EqShiftTableDefault_Ext_8809, sizeof(g_EqShiftTable_Ext_8809));
    g_fadingchannel_cnt = 0;
    g_fadingchannel_8tap_num = 0;
    g_fadingchannel_8tap_state = 0;
    g_dgc_pwr_target_shift = 14;
}

PUBLIC VOID spal_XcorSrec_8tap_Ext_8809(UINT32* tap8, UINT8 size, UINT32* HardBits,UINT32* refBurst, UINT32* tscRef)
{
#define chTabNb_Local        8
    UINT32 tmpTsc[3];
    UINT32 tmpTap[5];
    UINT8 i;
    INT16* refBurstLocal = (INT16*)(MEM_ACCESS_UNCACHED(refBurst));
    INT16* tscRefLocal = (INT16*)(MEM_ACCESS_UNCACHED(tscRef));

    for(i=0; i<size+ chTabNb_Local -2; i+=32)
    {
        tmpTsc[i/32] = HardBits[i/32];
    }
    spal_XcorSrec( (INT16*)(tap8+8-5), // Channel Taps
                   size, // Reconstruction length
                   (UINT32*) &(tmpTsc[0]), // ref. hardbit sequence
                   (UINT32*)refBurstLocal, // reconstructed output
                   SPAL_ISI); // channel tap number

    for(i=0; i<(size+chTabNb_Local-2-1)/32; i++)
    {
        tmpTsc[i] = (tmpTsc[i+1]<<29) | (tmpTsc[i]>>3);
    }
    tmpTsc[i] >>= 3;


    tmpTap[0] = tap8[0];
    tmpTap[1] = tap8[1];
    tmpTap[2] = tap8[2];
    tmpTap[3] =0;
    tmpTap[4] =0;
    spal_XcorSrec( (INT16*)(tmpTap), // Channel Taps
                   size ,// Reconstruction length
                   (UINT32*) &(tmpTsc[0]), // ref. hardbit sequence
                   (UINT32*)(tscRefLocal), // reconstructed output, uncached
                   5); // channel tap number
    for (i=0; i<2*size; i ++)
        * refBurstLocal++ += *tscRefLocal++;
}



//=============================================================================
// spal_XcorChest_8taps_Ext_8809
//-----------------------------------------------------------------------------
// Performs LSE channel estimation. CHIP_CHEST_LEN gives the maximum number of taps that
// can be estimated.
//
// @param RxBuffer INT16*. INPUT. Pointer to the buffer of received samples.
// The samples are complex with RxBuffer[2*t] the inphase part of sample t and
// RxBuffer[2*t+1] the quadrature part of sample t.
// @param ChestTab CONST INT16*. INPUT. channel estimation table
// @param taps INT16*. OUTPUT. Estimated taps of the channel. The taps are complex.
// @param chTapNb UINT8. INPUT. number of channel taps to estimate. Should be smaller or equal to CHIP_CHEST_LEN
// taps[2*t] is the inphase part of taps t,
// taps[2*t+1] is the quadrature part of taps t.
//=============================================================================
PUBLIC VOID spal_XcorChest_8taps_Ext_8809(INT16* RxBuffer, INT16 *ChestTab, INT16* taps, UINT8 chTapNb, INT8  fracBits)
{
    int i;
    // 11 when CHIP_CHEST_LEN = 5
    // 10 when CHIP_CHEST_LEN = 8
    UINT32 local_table[(27+1-8)/2];

    //SPAL_PROFILE_FUNC_ENTER(spal_XcorChest);
    for(i=0; i<chTapNb; i++)
    {
        register UINT8 j;
        register UINT16 *tmp_src = (UINT16*)&(ChestTab[i*(27-8)]);
        register UINT16 *tmp_dst = (UINT16*)local_table;
        // Copy table locally
        //for (j=0;j<(27+1-CHIP_CHEST_LEN);j++)
        // the following line changed by tcj bug removed @20110519.
        for (j=0; j<(27-8); j++)
        {
            *tmp_dst++ = *tmp_src++;
        }

        hwp_xcor->addr0    = (UINT32)RxBuffer;
        hwp_xcor->data[2]  =   (UINT32)local_table;
#if (CHIP_XCOR_8PSK_SUPPORTED)
        hwp_xcor->convolution_0 = XCOR_NB_SYMB((27-CHIP_CHEST_LEN))|
                                  XCOR_NB_ILOOP(3);
        hwp_xcor->command  =   XCOR_CHEST_EN | XCOR_DEROTATION_EN;
#else
        hwp_xcor->command  =   XCOR_CHEST_EN |
                               XCOR_DEROTATION_EN | // needed for datapath setup
                               XCOR_NB_SYMB((27-8))|
                               XCOR_NB_ILOOP(3);
#endif
        // Poll until the end of processing
        while (((hwp_xcor->status) & XCOR_OP_PENDING));

        // Get results
        taps[2*i] = (INT16)((INT32)hwp_xcor->hv0 >> fracBits);
        taps[2*i+1] = (INT16)((INT32)hwp_xcor->hv1 >> fracBits);
    }

    //SPAL_PROFILE_FUNC_EXIT(spal_XcorChest);
}


#define NBURST_EQ_SCHEME 2

#if (NBURST_EQ_SCHEME == 1)

#if 0

PUBLIC VOID spp_BuildRefNBurst_8taps_Ext_8809(SPP_EQU_BURST_T* burst, UINT32* tap8)
{
    UINT32 hard_refBurst_tab[4];
    UINT32* hard_refBurst;
    UINT32 tmp_hardRef[3];
    UINT32 tscTmp[NB_HPAYLD_SIZE+1+2+3];
    hard_refBurst = MEM_ACCESS_UNCACHED(hard_refBurst_tab);

    // First extract hard values from equalized output
    // 116 SoftValues => 29 words
    spal_XcorHvextract( burst->sv,
                        (UINT8) 29,
                        hard_refBurst);

    // Build the first part of the burst
    // add the 3 needed tail bits ...(2 extra tail is imagined)
    tmp_hardRef[0] = hard_refBurst[0] << 5;
    // ...to the first 57bits of payload...
    tmp_hardRef[1] = (hard_refBurst[1] << 5)|(hard_refBurst[0] >> 27);
    // ...mask...
    tmp_hardRef[1] &= 0xFFFFFFFFU >> (32-(57+5-32));
    // ...and add the first stealing flag...
    tmp_hardRef[1] |= (hard8(burst->sv[114]) >> 7 ) << (57+5-32);
    // ...and the beginning of the TS
    tmp_hardRef[1] |= (g_sppGsmTscNburst[burst->tsc]) << (57+5-32+1);
    tmp_hardRef[2] = (g_sppGsmTscNburst[burst->tsc]) >> (32-(57+5-32+1));

    // Reconstruct reference symbols of first part of the burst:
    // 57bits payload + 1 stealing flag + 5 TS bits not done during snr calc.
    // use 2 part 5+3 to replace the above, functional the same
    spal_XcorSrec_8tap_Ext_8809(tap8, // channel taps
                                (NB_HPAYLD_SIZE+1+2+3), // number of symbols
                                tmp_hardRef, // hardbit reference
                                &(burst->ref[0]),// output
                                tscTmp);



    // Build the second part of the burst
    // add the 7 last TS bits
    tmp_hardRef[0] = (g_sppGsmTscNburst[burst->tsc]) >> (NB_TS_SIZE-4-3);
    // add the second stealing flag
    tmp_hardRef[0] |= (hard8(burst->sv[115])>>7) << (4+3);
    // ...to the second 57bits of payload...
    tmp_hardRef[0] |= (hard_refBurst[1] >> (NB_HPAYLD_SIZE-32-5-3)) & 0xFFFFFF00;
    tmp_hardRef[0] |= hard_refBurst[2] << (64-NB_HPAYLD_SIZE+5+3);
    tmp_hardRef[1] = (hard_refBurst[2] >> (NB_HPAYLD_SIZE-32-5-3));
    tmp_hardRef[1] |= (hard_refBurst[3] << (64-NB_HPAYLD_SIZE+5+3));
    tmp_hardRef[2] |= (hard_refBurst[3] >>(32-(64-NB_HPAYLD_SIZE+5+3)));


    // add mask for the 2 needed tail bits ...
    tmp_hardRef[2] &= 0xFFFFFFFFU >> (32+(32-(4+1+3+57-32)));

    // Reconstruct reference symbols of second half part of the burst:
    // use 2 part 5+3 to replace the above, functional the same
    spal_XcorSrec_8tap_Ext_8809(tap8, // channel taps
                                (NB_HPAYLD_SIZE+1+2), // number of symbols
                                tmp_hardRef, // hardbit reference
                                &(burst->ref[142-(NB_HPAYLD_SIZE+1+2)]),// output
                                tscTmp);
}

//=============================================================================
// spp_FofEstimation_8taps_Ext_8809
//-----------------------------------------------------------------------------
/// Reconstruct the reference burst (softvalues), part of it
/// (training sequence) has already been done during SNR estimation.
///
/// Once the reference burst has been rebuilt, determines the Fof by
/// comparing the first FOF_NB_SYMB symbols of reference burst and
/// received burst, as well as the last FOF_NB_SYMB symbols of reference
/// burst and received burst.
///
/// The Fof is returned in Hertz.
///
/// @param WinType UINT8. Toggles channel estimation function to either
///                       normal burst or SCH.
/// @param burst SPP_EQU_BURST_T*. Pointer to the burst structure.
///
//=============================================================================
PUBLIC VOID spp_FofEstimation_8taps_Ext_8809(SPP_EQU_BURST_T* burst, UINT32* tap8)
{
    INT32 Icorr,divider;

    //SPP_PROFILE_FUNCTION_ENTRY(spp_FofEstimation);
    // Reconstruct the reference burst (softvalues), part of it
    // (training sequence) has already been done during SNR estimation.
    // NOTE: It is not necessary to rebuild the full burst as we only
    // need FOF_NB_SYMB on each side to estimate FOF, but this is more
    // flexible and doesn't cost much thanks to the accelerators.
    spp_BuildRefNBurst_8taps(burst,tap8);



#if 1
    burst->noise = spal_XcorErrCalc(
                       (UINT32*)&(burst->samples[3]), // original
                       (UINT32*)&(burst->ref[0]), // reference
                       142); // Number of samples

    // Normalise NoisePower and calculate Snr
    burst->noise /= 142;

#endif


    // Calculate Imaginary part of correlation on start of burst
    // '+3' because tail bits are not present in refBurst
    Icorr = - spal_XcorIcorCalc(&(burst->samples[FOF_START_1+3]),
                                &(burst->ref[FOF_START_1]),
                                FOF_NB_SYMB);

    // Calculate Imaginary part of correlation on end of burst
    // '+3' because tail bits are not present in refBurst
    Icorr += spal_XcorIcorCalc( &(burst->samples[FOF_START_2+3]),
                                &(burst->ref[FOF_START_2]),
                                FOF_NB_SYMB);

    // FOf is given by:
    //
    //                           CorrI x Fs
    // FOF = _______________________________________________________
    //
    //       corr_gap x 2 PI x Norm(Power-NoisePower) x nb_symb_corr
    divider= 43104 / (FOF_CORR_GAP*FOF_NB_SYMB);// Fs/(2*PI)=43104
    divider=(INT32)((burst->power-burst->noise)/divider);
    if(divider)
        burst->fof = (INT16)(Icorr / divider);
    else
        burst->fof = 0;
    //SPP_PROFILE_FUNCTION_EXIT(spp_FofEstimation);
}

//=============================================================================
// spp_ChannelEstimation_8taps_Ext_8809
//-----------------------------------------------------------------------------
/// This function performs Least Square Error (LSE) channel estimation.
///
/// @param WinType UINT8. Toggles channel estimation function to either
///                       normal burst or SCH.
/// @param burst SPP_EQU_BURST_T*. Pointer to the burst structure.
///
//=============================================================================
PUBLIC VOID spp_ChannelEstimation_8taps_Ext_8809(SPP_EQU_BURST_T* burst,
        UINT32 *tap8,
        SPP_UNCACHED_BUFFERS_T* UncachedBuffers)
{
    UINT32 Xcorr_Energy[SCH_TSC_XCOR_NBR]; // Sized for SCH
    UINT8 search_size;
    int i;
    UINT8 TOf = 0;
    UINT16 tapWindow [3] = {6,7,8};

    //SPP_PROFILE_FUNCTION_ENTRY(spp_ChannelEstimation);
    // ******************************
    // Search Time Synchronization
    // ******************************
    // Use XCOR to compute Xcorrelation with Training Sequence
    volatile INT16 *Xcorr= UncachedBuffers->NbVitac.Xcorr;
    // The first five bits of the training sequence are not
    // used to perform the following correlation, as they are
    // only a repetition of the last 5 bits of the training
    // sequence.
    // Therefore, the received pilots of interest start at earliest
    // on bit 65.
    // The correlation is performed using 16 consecutive bits of the
    // training sequence and it is performed on 8+SPAL_ISI=13
    // consecutive time slot.
    // We can detect a ToF of +/- 4 samples.
    spal_XcorTscxcNb(
        g_sppGsmTscNburst[burst->tsc],
        (UINT32*)(burst->samples + 60 + 4), // 4 samples margin
        (INT16*)Xcorr);

    // Calculate I^2 + Q^2 for every coeffs
    for(i=0; i<NB_TSC_XCOR_NBR; i++)
    {
        Xcorr_Energy[i] =   Xcorr[2*i]     * Xcorr[2*i] +
                            Xcorr[2*i + 1] * Xcorr[2*i + 1];
    }
    search_size = NB_TSC_XCOR_NBR - SPAL_ISI + 1;

    // Now that we have the correlation on 13 consecutive time
    // slots, we find the maximum power profile considering that
    // the channel is 5 taps long with the main tap in the
    // middle (this assumptions corresponds to a GMSK on an AWGN
    // channel).

    // the correlation window could be flatter at the expense
    // of a little snr decrease (~1dB) in static conditions
    spp_TofSearch( Xcorr_Energy,
                   tapWindow,
                   search_size,
                   &(burst->tof),
                   &TOf);

    // TODO : see if this saturation is usefull
    // TODO : if yes -> do it for SCH also
    // if(burst->tof>32) burst->tof=32;


    // Shift Symbol Buffer Origin for remaining of equalization process
    // After origin shift we have the following bits:
    //
    // NORMAL BURST
    // 0:2     -> guard bits
    // 3:59    -> data bits
    // 60      -> steal bit
    // 61:86   -> pilot bits
    // 87      -> steal bit
    // 88:144  -> data bits
    // 145:147 -> guard bits
    burst->samples += (TOf);
    burst->samples_offset = TOf; // Keep track of symbol offset for HBURST mode

    // ******************************
    // Channel taps Estimation
    // ******************************

    // CE Output shape:
    // tap idx: 0,1,2...7
    // peak value at idx==2
    //
    spal_XcorChest_8taps_Ext_8809(
        (INT16 *) (&burst->samples[63 + 8 - SPAL_ISI]), // format 3.9
        (INT16 *) (&gc_sppChestTabEgprs_Ext_8809_8809[burst->tsc][0][0]),//format: Q(15)
        (INT16 *) tap8,// format: Q(LPCH)(i.e. 0.LPCH)
        8,
        15);
    // copy 5 taps for Equalizer
    for (i=0; i<5; i++)
        burst->taps[i] = tap8[i];

#if 0
    // approximative weightening
    taps[0]=(taps[0]*25231)>>15;
    taps[1]=(taps[1]*25231)>>15;
    taps[2]=(taps[2]*30749)>>15;
    taps[3]=(taps[3]*30749)>>15;
    taps[4]= taps[4];
    taps[5]= taps[5];
    taps[6]=(taps[6]*30749)>>15;
    taps[7]=(taps[7]*30749)>>15;
    taps[8]=(taps[8]*25231)>>15;
    taps[9]=(taps[9]*25231)>>15;
#endif
    //SPP_PROFILE_FUNCTION_EXIT(spp_ChannelEstimation);
}


//=============================================================================
// spp_Equalize_5taps_Ext_8809
//-----------------------------------------------------------------------------
/// This function performs Viterbi equalization of a burst.
///
/// @param WinType UINT8. INPUT. Toggles channel estimation function to either
///                       normal burst or SCH.
/// @param burst SPP_EQU_BURST_T*. INPUT/OUTPUT Pointer to the burst structure.
/// @param UncachedBuffers SPP_UNCACHED_BUFFERS_T*. INPUT/OUTPUT. Pointer to
/// the work buffers designated by SPC layer.
//=============================================================================
PUBLIC VOID spp_Equalize_5taps_Ext_8809(UINT8 WinType,
                                        UINT8 offset,
                                        SPP_EQU_BURST_T* burst,
                                        SPP_UNCACHED_BUFFERS_T* UncachedBuffers)
{
    INT16 H[4];
    int i,survivor;
    SPAL_VITAC_EQU_PARAM_T equ_param;
    UINT32 SumTable[2*2*8/2];
    volatile UINT8  *path_diff_bkw;
    volatile UINT8  *path_diff_fwd;
    volatile UINT16 *PathMetric;
    UINT8* tb_wr_ptr = &(burst->sv[1]);
    UINT8   tb_start,tb_end,tb_idx,tb_size;
    INT16* taps = (INT16*) burst->taps;
    SPAL_DC_OFFSET_T Dco;
    BOOL fwd_vitac_done = FALSE;

#define BM_SHIFT_MAX    17
#define MAXU_NBIT(n) ((1<<n)-1)
#define SV_MAX MAXU_NBIT(7)

    // SPP_PROFILE_FUNCTION_ENTRY(spp_Equalize);
    // ***********************************
    // Derive the VITAC shifters settings
    // ***********************************
    equ_param.shift_bm = spp_Log2(burst->noise);
    if (equ_param.shift_bm>SHIFT_BM_OFFSET) equ_param.shift_bm -= SHIFT_BM_OFFSET;

    equ_param.shift_sb = SHIFT_SB_OFFSET;

    if (equ_param.shift_bm>BM_SHIFT_MAX)
    {
        equ_param.shift_sb += (equ_param.shift_bm-BM_SHIFT_MAX);
        equ_param.shift_bm=BM_SHIFT_MAX;
    }
    else if (equ_param.shift_bm<4) equ_param.shift_bm = 4;
    if (equ_param.shift_sb>9) equ_param.shift_sb = 9;


    // ***********************************
    // Precalculation of Sum terms for BMs
    // ***********************************
    // The branch metric for a given node can be precalculated
    // since the register state is directly linked to the node.
    spal_XcorBmsum(taps, (INT16*)SumTable);

    // Compute H0 and HL for Viterbi Accelerator
    H[0] = taps[0];
    H[1] = taps[1];
    H[2] = taps[SPAL_DELAY*2];
    H[3] = taps[SPAL_DELAY*2+1];

    // Saturation on SYMB_SIZE bits signed
    for (i=0; i<4; i++)
    {
        H[i] = (H[i] > SYMB_MAX) ? SYMB_MAX : H[i];
        H[i] = (H[i] < SYMB_MIN) ? SYMB_MIN : H[i];
    }

    equ_param.H0 = (UINT32)((H[1]<<16)|((0xFFFF)&H[0]));
    equ_param.HL = (UINT32)((H[3]<<16)|((0xFFFF)&H[2]));
    equ_param.bmSum     = (INT16*)SumTable;

    // Setup for first half-burst equalization (backward)
    if (WinType == GSM_WIN_TYPE_NBURST)
    {
        equ_param.length = 3 + NB_HPAYLD_SIZE + 1;
        equ_param.in     = (burst->samples +3 +NB_TS_START +SPAL_DELAY -1 -2 + offset);
        equ_param.out    =
            (UINT8*)UncachedBuffers->NbVitac.path_diff_bkw;
        path_diff_bkw    =
            (volatile UINT8*)UncachedBuffers->NbVitac.path_diff_bkw;
        path_diff_fwd    =
            (volatile UINT8*)UncachedBuffers->NbVitac.path_diff_fwd;
        equ_param.pm     =
            (UINT16*)UncachedBuffers->NbVitac.PathMetric;
        PathMetric       =
            (volatile UINT16*)UncachedBuffers->NbVitac.PathMetric;
        equ_param.first_state = g_sppNbBwequStartSt[burst->tsc];
    }
    else // SCH
    {
        equ_param.length = 3 + SCH_HPAYLD_SIZE;
        equ_param.in     = (burst->samples +3 +SCH_TS_START +SPAL_DELAY -1 -2);
        equ_param.out    =
            (UINT8*)UncachedBuffers->SchVitac.path_diff_bkw;
        path_diff_bkw    =
            (volatile UINT8*)UncachedBuffers->SchVitac.path_diff_bkw;
        path_diff_fwd    =
            (volatile UINT8*)UncachedBuffers->SchVitac.path_diff_fwd;
        equ_param.pm     =
            (UINT16*)UncachedBuffers->SchVitac.PathMetric;
        PathMetric       =
            (volatile UINT16*)UncachedBuffers->SchVitac.PathMetric;
        equ_param.first_state = SCH_BWEQU_START_ST;
    }

    // Run the viterbi for the backward part of the burst (1st half)
    spal_VitacEqualize(BACKWARD_TRELLIS, &equ_param);

    // Poll till end of processing
    while(spal_VitacBusy());
    //-------------------------------------------------------------------

    // ****************************************
    // Traceback Processing of backward Viterbi
    // ****************************************
    // Update node 0 for transition 62 with node 0 and 8
    // And get the first payload bit
    // Due to Vitac architecture (PM double buffering), the actual
    // PM position depends on nb_symbol parity.
    PathMetric = (equ_param.length & 0x1) ? PathMetric + VIT_NSTATES:
                 PathMetric;
#if 1
    // another way is length++, use hardware computer extra path,
    // which will occupy path_diff_fwd's first few, which are not
    // usefull before forward computation begins.
    // here we implement soft path calc:
    if (offset==2)
    {
        INT16 dx = (INT16)(*(burst->samples+3));
        INT16 dy = (INT16)(*(burst->samples+3)>>16);
        /*
        INT16 diffx,diffy;
        INT32 ds1,ds2;
        diffx = dx - (taps[0] + taps[2] + taps[4] + taps[6]);
        diffy = dy - (taps[1] + taps[3] + taps[5] + taps[7]);
        ds1 = diffx*diffx + diffy*diffy;

        diffx = dx - (-taps[0] + taps[2] + taps[4] + taps[6]);
        diffy = dy - (-taps[1] + taps[3] + taps[5] + taps[7]);
        ds2 = diffx*diffx + diffy*diffy;

        ds1 = ds1-ds2;

        */
        dx = taps[2] + taps[4] + taps[6] - dx;
        dy = taps[3] + taps[5] + taps[7] - dy;
        PathMetric[0] += ((dx*taps[0] + dy*taps[1])>>(equ_param.shift_bm-2));
    }
#endif

    if (PathMetric[0]<PathMetric[8])
    {
        UINT16 diff;

        diff = (PathMetric[8]-PathMetric[0]) >>  equ_param.shift_sb;
        burst->sv[0]=((diff>SV_MAX)? SV_MAX: diff);
        survivor=0;
    }
    else
    {
        UINT16 diff;

        diff = (PathMetric[0]-PathMetric[8]) >>  equ_param.shift_sb;
        burst->sv[0]=0x80|((diff>SV_MAX)? SV_MAX: diff);
        survivor=8;
    }

    // ***************************************************
    // Launching HW Viterbi Trellis processing of the 2nd
    // burst during the Trace-back of the first half-burst
    // if we are not in EQU_HBURST_MODE
    // ***************************************************
    if (WinType == GSM_WIN_TYPE_NBURST)
    {
        // Setup for second half-burst equalization (forward)
        // No change needed for the other parameters
        equ_param.in     = (burst->samples +3 +NB_TS_START +NB_TS_SIZE -2 + offset);
        equ_param.out    = (UINT8 *)path_diff_fwd;
        equ_param.first_state = g_sppNbFwequStartSt[burst->tsc];

        if (!EQU_HBURST_MODE)
        {
            // Run the viterbi now only in non-HalfBurst mode
            spal_VitacEqualize(FORWARD_TRELLIS, &equ_param);
        }
        // Start now even in EQU_BURST_MODE if the full Nburst
        // has been received
        else if (spp_FullBurstReady() && (WinType == GSM_WIN_TYPE_NBURST))
        {
            // Run DCOC and Derotation on remaining samples
            // (take into account offset induced by TOf)
            Dco.I=burst->dco.i;
            Dco.Q=burst->dco.q;
            spal_XcorDcocHburst((UINT32*)&(burst->samples[BB_BURST_HALF_SIZE-
                                           burst->samples_offset]),
                                BB_BURST_TOTAL_SIZE-BB_BURST_HALF_SIZE,
                                TRUE, // Derotation
                                BYPASS_DCOC,
                                Dco);
            // Run the viterbi (Setup for second half-burst
            // equalization has already been done)
            spal_VitacEqualize(FORWARD_TRELLIS, &equ_param);
            // Update "servicing" flags
            spp_OneFullBurstDone();
            fwd_vitac_done = TRUE;
        }
    }
    else // SCH
    {
        // SCH doesn't support HalfBurst mode
        equ_param.in     = (burst->samples +3 +SCH_TS_START +SCH_TS_SIZE -2);
        equ_param.out    = (UINT8 *)path_diff_fwd;
        equ_param.first_state = SCH_FWEQU_START_ST;
        // Run the viterbi
        spal_VitacEqualize(FORWARD_TRELLIS, &equ_param);
    }


    // ****************************************
    // Back to the trace-back of 1st half-burst
    // ****************************************
    //  Setup the traceback loop bounds
    //  Note that for NB stealing bits are handled separately
    if (WinType == GSM_WIN_TYPE_NBURST)
    {
        tb_start = (56 + 1) + SPAL_DELAY - 1; // +1 for steal bit shift
        tb_end   = ( 0 + 1) + SPAL_DELAY - 1;
        tb_size  = 2*57; // steal bits not accounted for
    }
    else //SCH
    {
        tb_start = 38 + SPAL_DELAY - 1;
        tb_end   =  0 + SPAL_DELAY - 1;
        tb_size  = 2*39;
    }

    // Move traceback pointer to beginning of first part of payload
    tb_wr_ptr = &(burst->sv[1]);

    // Get the remaining payload bits
    for (tb_idx=tb_start ; tb_idx>tb_end ; tb_idx--)
    {
        *tb_wr_ptr = (path_diff_bkw[VIT_NSTATES*(tb_idx) + survivor]);

        if (hard8(*tb_wr_ptr++))    survivor = survivor/2+(VIT_NSTATES/2);
        else                        survivor = survivor/2;
    }
    // Store the first stealing bit (hl) at the end of the burst
    if (WinType == GSM_WIN_TYPE_NBURST)
        burst->sv[114] = (path_diff_bkw[VIT_NSTATES*SPAL_DELAY+survivor]);
    //-------------------------------------------------------------------

    // In case of EQU_HBURST_MODE we need to wait for the samples of
    // the second half burst to be ready
    if (EQU_HBURST_MODE && (!fwd_vitac_done))
    {
        if (WinType == GSM_WIN_TYPE_NBURST)
        {
            // Wait for EndOfBurst Interrupt
            while(!spp_FullBurstReady());
            // Run DCOC and Derotation on remaining samples
            // (take into account offset induced by TOf)
            Dco.I=burst->dco.i;
            Dco.Q=burst->dco.q;
            spal_XcorDcocHburst((UINT32*)&(burst->samples[BB_BURST_HALF_SIZE-
                                           burst->samples_offset]),
                                BB_BURST_TOTAL_SIZE-BB_BURST_HALF_SIZE,
                                TRUE, // Derotation
                                BYPASS_DCOC,
                                Dco);

            // Run the viterbi (Setup for second half-burst
            // equalization has already been done)
            spal_VitacEqualize(FORWARD_TRELLIS, &equ_param);
            // Update "servicing" flag
            spp_OneFullBurstDone();
        }
    }

    // Poll till end of processing
    while(spal_VitacBusy());
    //-------------------------------------------------------------------

    // ****************************************
    // Traceback Processing of forward Viterbi
    // ****************************************
    // Update node 0 for transition 62 with node 0 and 8
    // And get first bit of payload
    if (PathMetric[0]<PathMetric[8])
    {
        UINT16 diff;

        diff = (PathMetric[8]-PathMetric[0]) >>  equ_param.shift_sb;
        burst->sv[tb_size-1]=((diff>SV_MAX)? SV_MAX: diff);
        survivor=0;
    }
    else
    {
        UINT16 diff;

        diff = (PathMetric[0]-PathMetric[8]) >>  equ_param.shift_sb;
        burst->sv[tb_size-1]=0x80|((diff>SV_MAX)? SV_MAX: diff);
        survivor=8;
    }

    // Move traceback pointer to the end of second half of payload
    tb_wr_ptr = &(burst->sv[tb_size - 2]);

    for (tb_idx=tb_start ; tb_idx>tb_end ; tb_idx--)
    {
        *tb_wr_ptr = (path_diff_fwd[VIT_NSTATES*tb_idx + survivor]);

        if (hard8(*tb_wr_ptr--))    survivor = survivor/2+(VIT_NSTATES/2);
        else                        survivor = survivor/2;
    }
    // Store the second stealing bit (hu) at the end of the burst
    if (WinType == GSM_WIN_TYPE_NBURST)
        burst->sv[115] = (path_diff_fwd[VIT_NSTATES*SPAL_DELAY+survivor]);

    //SPP_PROFILE_FUNCTION_EXIT(spp_Equalize);
}


//=============================================================================
// spp_EqualizeNBurst_8taps_Ext_8809
//-----------------------------------------------------------------------------
// Performs normal burst equalization including:
// - DC offset estimation and correction,
// - channel estimation,
// - SNR estimation,
// - Viterbi equalization and traceback,
// - Frequency Offset estimation.
// .
//
// @param RxBuffer UINT32*. Address of the received samples.
// @param Tsc UINT8. Training sequence number.
// @param EqBufferOut UINT32*.
// @param Res SPP_N_BURST_RESULT_T*. Pointer to the burst equalization structure.
//
//=============================================================================
PUBLIC VOID spp_EqualizeNBurst_8taps_Ext_8809(UINT32* RxBuffer,
        UINT8 Tsc,
        UINT32* EqBufferOut,
        UINT32* EqOut2nd,
        SPP_UNCACHED_BUFFERS_T* UncachedBuffers,
        SPP_N_BURST_RESULT_T* Res)
{
    UINT32 refBurst[142]; // stores reconstructed burst
    UINT32 tap8[8];
    UINT32 tscRef[19];
    UINT16 i;
    UINT8 Shft;
    UINT32 SignBits;
    UINT32 Msk;
    INT16 tap8Snr;

#if 1
    UINT8 k;
    UINT32 maxPwr, sumPwr;
    UINT32 chPwr[8];
    UINT8 chPos=0;
#endif

    // TODO : see if this structure can be used one level higher
    // to avoid passing too much parameters and keep structured datas.
    SPP_EQU_BURST_T burst; // equalisation structure
    SPAL_DC_OFFSET_T Dco;

    //SPP_PROFILE_FUNCTION_ENTRY(spp_EqualizeNBurst);
    burst.tsc       = Tsc;
    burst.samples   = RxBuffer;
    burst.ref       = refBurst;
    burst.sv        = (UINT8*)EqBufferOut;


#if (GPRS_EQ8TAP_DEBUG)
    if  ( g_tcj_EqINIT== 0x00)
    {
        UINT32 *tmppt = MEM_ACCESS_UNCACHED(RxBuffer);
        UINT32 *tmpdes = MEM_ACCESS_UNCACHED(&g_tcj_EqBuf[0]);
        UINT16 kk;
        g_tcj_EqCnt = Tsc;
        for(kk=0; kk<156; kk++)
        {
            *tmpdes++ = *tmppt++;
        }
    }

#endif
    // DC Offset Correction
    // --------------------
    if (EQU_HBURST_MODE)
    {
        burst.power = spal_XcorDcoc((UINT32*)RxBuffer,
                                    BB_BURST_HALF_SIZE,
                                    TRUE, // Derotation
                                    BYPASS_DCOC,
                                    &Dco);
        burst.dco.i=Dco.I;
        burst.dco.q=Dco.Q;
        burst.power /= BB_BURST_HALF_SIZE; // Normalised power
    }
    else
    {
        burst.power = spal_XcorDcoc((UINT32*)RxBuffer,
                                    BB_BURST_TOTAL_SIZE,
                                    TRUE, // Derotation
                                    BYPASS_DCOC,
                                    &Dco);
        burst.dco.i=Dco.I;
        burst.dco.q=Dco.Q;
        burst.power /= BB_BURST_TOTAL_SIZE; // Normalised power
    }

    // 5 tap old ce and eq
    spp_ChannelEstimation(GSM_WIN_TYPE_NBURST, &burst, UncachedBuffers);

    spal_XcorSrec( (INT16*)burst.taps, // Channel Taps
                   (NB_TS_SIZE-4), // Reconstruction length
                   (UINT32*) &(g_sppGsmTscNburst[burst.tsc]), // ref. hardbit sequence
                   &(burst.ref[NB_TS_START+2]), // reconstructed output
                   SPAL_ISI); // channel tap number
    burst.noise = spal_XcorErrCalc(
                      (UINT32*)&(burst.samples[NB_TS_START+3+2]),// original
                      (UINT32*)&(burst.ref[NB_TS_START  +2]), // reference
                      (NB_TS_SIZE-4)); // Number of samples
    burst.noise /= (NB_TS_SIZE-4);

    spp_Equalize(GSM_WIN_TYPE_NBURST,&burst, UncachedBuffers);
    spp_FofEstimation(GSM_WIN_TYPE_NBURST,&burst);
    burst.noise = spal_XcorErrCalc(
                      (UINT32*)&(burst.samples[3]), // original
                      (UINT32*)&(burst.ref[0]), // reference
                      142); // Number of samples
    burst.noise /= 142;
    if (burst.power > burst.noise)
        Res->Snr =  spp_10log10(burst.power-burst.noise)-spp_10log10(burst.noise);
    else
        Res->Snr =- spp_10log10(burst.noise);

    // Fill result structure
    Res->Pwr = spp_10log10(burst.power);
    Res->TOf = burst.tof;
    Res->FOf = burst.fof;
    Res->Dco.i = burst.dco.i;
    Res->Dco.q = burst.dco.q;


    if (Res->Snr < EQ_SHIFT_MAX_NUM)
    {
        Shft = g_EqShiftTable_Ext_8809[Res->Snr <0 ? 0: Res->Snr];
        switch(Shft)
        {
            case 1:
                Msk = 0x7E7E7E7E;
                break;
            case 2:
                Msk = 0x7C7C7C7C;
                break;
            case 3:
                Msk = 0x78787878;
                break;
            case 4:
                Msk = 0x70707070;
                break;
            case 5:
                Msk = 0x60606060;
                break;
            case 6:
                Msk = 0x40404040;
                break;
            default:
                Msk = 0x0;
                break;
        }
        if (Shft)
        {
            for (i=0; i<29; i++)
            {
                SignBits = (EqBufferOut[i] & 0x80808080);
                EqBufferOut[i] &= Msk;
                EqBufferOut[i] >>= Shft;
                EqBufferOut[i] |= SignBits;
            }
        }
    }

    if (EqOut2nd)
    {
#if (GPRS_EQ8TAP_DEBUG)
        if  ( g_tcj_EqINIT== 0x00)
        {
            UINT32 *tmppt = MEM_ACCESS_UNCACHED(RxBuffer);
            UINT32 *tmpdes = MEM_ACCESS_UNCACHED(&g_tcj_EqBuf[156]);
            UINT16 kk;
            for(kk=0; kk<156; kk++)
            {
                *tmpdes++ = *tmppt++;
            }
        }
#endif

        // Channel Estimation Process, 8taps, only for normal burst
        // Output format:
        // 8taps, 0,1,2,...7
        // peak value @ idx 2
        // --------------------------
        spp_ChannelEstimation_8taps(&burst, tap8, UncachedBuffers);
#if (GPRS_EQ8TAP_DEBUG)
        if  ( g_tcj_EqINIT== 0x00)
        {
            UINT32 *tmppt = MEM_ACCESS_UNCACHED(tap8);
            UINT32 *tmpdes = MEM_ACCESS_UNCACHED(&g_tcj_EqBuf[156*2]);
            UINT16 kk;
            for(kk=0; kk<8; kk++)
            {
                *tmpdes++ = *tmppt++;
            }
            *tmpdes = burst.tof;
        }
#endif
        // SNR Estimation
        // --------------
        // First reconstruct TrainingSequence
        // tsc idx length 26: idx: 0,1,2...25
        // output recovered tsc idx length 19 = 26 - (8-1)
        // 5,6,...23
        // explanation:
        // tap length is 8, 2 anti-causal and 5 causal taps
        // tsc symbols of position 0,1,2,3,4 will be contaminated by former unknown symbols
        // tsc symbols of postion 24,25 will be contaminated by later unkown symbols.
        // therefore, 5,6,...23 will be calculated based on known 26 tsc symbols.
        spal_XcorSrec_8tap_Ext_8809(tap8, // Channel Taps
                                    (NB_TS_SIZE-4-3), // Reconstruction length
                                    (UINT32*) &(g_sppGsmTscNburst[burst.tsc]), // ref. hardbit sequence
                                    &(burst.ref[NB_TS_START+2+3]), // reconstructed output
                                    tscRef); // temp buffer

        // Run power of the error calculation upon Training Sequence
        // The three guard bits are not included in the reference (reconstructed) burst
        burst.noise = spal_XcorErrCalc(
                          (UINT32*)&(burst.samples[NB_TS_START+3+2+3]), // original
                          (UINT32*)&(burst.ref[NB_TS_START  +2+3]), // reference
                          (NB_TS_SIZE-4-3)); // Number of samples

        // Normalise NoisePower and calculate Snr
        burst.noise /= (NB_TS_SIZE-4-3);

#if (GPRS_EQ8TAP_DEBUG)
        if  ( g_tcj_EqINIT== 0x00)
        {
            UINT32 *tmppt = MEM_ACCESS_UNCACHED(&(burst.ref[NB_TS_START+2+3]));
            UINT32 *tmpdes = MEM_ACCESS_UNCACHED(&g_tcj_EqBuf[156*2+8+1]);
            UINT16 kk;
            for(kk=0; kk<19; kk++)
            {
                *tmpdes++ = *tmppt++;
            }
            tmppt = MEM_ACCESS_UNCACHED(&tscRef[0]);
            for(kk=0; kk<19; kk++)
            {
                *tmpdes++ = *tmppt++;
            }

            *tmpdes = burst.noise;
        }
#endif



        // Equalize : Viterbi + traceback
        // ------------------------------
        // select 5 maxim channel tap
#if 1

        for (i=0; i<8; i++)
        {
            INT16 x,y;
            x = (INT16)(tap8[i]&0xFFFF);
            y = (INT16)(tap8[i]>>16);
            chPwr[i] = ((x*x)>>2) + ((y*y)>>2);
        }

        maxPwr = 0;
        for (i=0; i<3; i++)
        {
            sumPwr = 0;
            for(k=i; k<i+5; k++)
                sumPwr += chPwr[k];
            if (maxPwr < sumPwr)
                chPos = i;
        }
        if (chPos)
        {
            for (i=0; i<5; i++)
                burst.taps[i] = tap8[i+chPos];
        }
#endif
        // Equalize only supports 5 taps
        burst.sv = (UINT8*)EqOut2nd;
        spp_Equalize_5taps(GSM_WIN_TYPE_NBURST,chPos,&burst, UncachedBuffers);
        // Frequency error estimation, includes srec and noise calculation
        // --------------------------
        spp_FofEstimation_8taps(&burst, tap8);

#if (GPRS_EQ8TAP_DEBUG)
        if  ( g_tcj_EqINIT== 0x00)
        {
            UINT32 *tmppt = MEM_ACCESS_UNCACHED(EqBufferOut);
            UINT32 *tmpdes = MEM_ACCESS_UNCACHED(&g_tcj_EqBuf[156*2+8+1+19+19+1]);
            UINT16 kk;
            for(kk=0; kk<116/4; kk++)
            {
                *tmpdes++ = *tmppt++;
            }
            tmppt = MEM_ACCESS_UNCACHED(refBurst);
            for(kk=0; kk<142; kk++)
            {
                *tmpdes++ = *tmppt++;
            }
            *tmpdes++ = burst.power;
            *tmpdes++ = burst.noise;
            *tmpdes++ = chPos;
            *tmpdes++ = burst.fof;
            *tmpdes++ = &burst;
            *(UINT32*)0x0 = 0x0;
        }
#endif
        if (burst.power > burst.noise)
            tap8Snr =  spp_10log10(burst.power-burst.noise)-spp_10log10(burst.noise);
        else
            tap8Snr =- spp_10log10(burst.noise);
        // Eq output shift
        if (tap8Snr < EQ_SHIFT_MAX_NUM)
        {
            Shft = g_EqShiftTable_Ext_8809[tap8Snr <0 ? 0: tap8Snr];
            switch(Shft)
            {
                case 1:
                    Msk = 0x7E7E7E7E;
                    break;
                case 2:
                    Msk = 0x7C7C7C7C;
                    break;
                case 3:
                    Msk = 0x78787878;
                    break;
                case 4:
                    Msk = 0x70707070;
                    break;
                case 5:
                    Msk = 0x60606060;
                    break;
                case 6:
                    Msk = 0x40404040;
                    break;
                default:
                    Msk = 0x0;
                    break;
            }
            if (Shft)
            {
                for (i=0; i<29; i++)
                {
                    SignBits = (EqOut2nd[i] & 0x80808080);
                    EqOut2nd[i] &= Msk;
                    EqOut2nd[i] >>= Shft;
                    EqOut2nd[i] |= SignBits;
                }
            }
        }
    }


    if ((Res->Snr <= 0) && (g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.nBurst.bufIdx == 4))
    {
        for (i=0; i<29; i++)
        {
            EqBufferOut[i] &= 0x80808080;
        }
    }


    //SPP_PROFILE_FUNCTION_EXIT(spp_EqualizeNBurst);
}

#endif

#elif (NBURST_EQ_SCHEME == 2)

// the LPQP is the scale for LP prefilter.
// the LPSFL is the sub of LPQP and
// the LPCH  is the scale of the channel taps
#define LPQP             11
#define LPSFL             1
#define LPCH             11
/// Chip ID for 8808 or 8808s
#define    SPAL_CHIP_ID_8808           0
#define    SPAL_CHIP_ID_8808s         1
#define    SPAL_CHIP_ID_8809         2
#define    SPAL_CHIP_ID_OTHER        0xFF

// NOTE: to save code size, use Macro instead global variable to discrete 8808 and 8808s
#define     SPAL_CHIP_ID            (SPAL_CHIP_ID_8809)

// Xcor shifter range
#define SPAL_XCOR_SHIFT_RANGE_MIN        0
#define SPAL_XCOR_SHIFT_RANGE_MAX       15

#define SPAL_XCOR_SHIFT_CHECK_RANGE(x,id)

#define SPAL_XCOR_MAC_32(in1,in2,res)   res+=(in1)*(in2);
#define SPAL_XCOR_MSU_32(in1,in2,res)   res-=(in1)*(in2);
#define OPL_SHR_32(in,nbbits_shift,result)  result=((in)>>(nbbits_shift));
#define OPL_SAT_16(in,out)      \
        out = ((in) > MAX_INT16) ? MAX_INT16 : \
          ((in) < MIN_INT16) ? MIN_INT16 : \
           (INT16)(in);


#define OPL_MOV_32(in, result)      result=in;

PUBLIC VOID spal_XcorComplexConvolution_Ext_8809 (    INT16 *Xseq,
        UINT16 Xlen,
        INT16 *Yseq,
        UINT16 Ylen,
        INT16 *Zseq,
        UINT16 Zoff,
        UINT16 Zlen,
        INT8 fracBits  )
{

    SPAL_COMPLEX_T *XseqCmp =  (SPAL_COMPLEX_T*) Xseq;
    SPAL_COMPLEX_T *YseqCmp =  (SPAL_COMPLEX_T*) Yseq;
    SPAL_COMPLEX_T *ZseqCmp =  (SPAL_COMPLEX_T*) Zseq;

    UINT16 OutLoopLen = Xlen + Ylen - 1 - Zoff;

    //               "config registers"
    UINT16            regLoopExt = ((OutLoopLen > Zlen) && (Zlen > 0)) ? Zlen : OutLoopLen;
    UINT16            regPtrIdx  = Zoff + 1;
    UINT16            regMinLen  = (Xlen < Ylen) ? Xlen : Ylen;
    UINT16            regAllLen  = Xlen + Ylen;
    UINT16            regYLen    = Ylen;
    INT8              regShift   = fracBits;
    SPAL_COMPLEX_T * regPtrX    = XseqCmp - Ylen;
    SPAL_COMPLEX_T * regPtrY    = YseqCmp - 1;
    SPAL_COMPLEX_T * regPtrZ    = ZseqCmp;


    SPAL_XCOR_SHIFT_CHECK_RANGE(fracBits,4);


    // executed by the HW accelerator
    //--------------------------------

    for (; regLoopExt > 0; regLoopExt--)
    {
        //               "internal registers"
        UINT16            regLoopInt = MIN(regMinLen, MIN(regPtrIdx, regAllLen - regPtrIdx));
        SPAL_COMPLEX_T * regRdPtrX  = regPtrX + MAX(regYLen,regPtrIdx);
        SPAL_COMPLEX_T * regRdPtrY  = regPtrY + MIN(regYLen,regPtrIdx);
        SPAL_CMPLX32_T   regAccu    = {0,0};

        for (; regLoopInt > 0; regLoopInt--)
        {
            SPAL_XCOR_MAC_32(regRdPtrX->i, regRdPtrY->i, regAccu.i);
            SPAL_XCOR_MSU_32(regRdPtrX->q, regRdPtrY->q, regAccu.i);
            SPAL_XCOR_MAC_32(regRdPtrX->i, regRdPtrY->q, regAccu.q);
            SPAL_XCOR_MAC_32(regRdPtrX->q, regRdPtrY->i, regAccu.q);

            regRdPtrX++;
            regRdPtrY--;
        }

        OPL_SHR_32(regAccu.i, regShift, regAccu.i);
        OPL_SHR_32(regAccu.q, regShift, regAccu.q);

        OPL_SAT_16(regAccu.i, regPtrZ->i);
        OPL_SAT_16(regAccu.q, regPtrZ->q);

        regPtrZ++;
        regPtrIdx++;
    }


    //--------------------------------

}

PUBLIC SPAL_CMPLX32_T spal_XcorComplexMatrixMultiply_Ext_8809(   INT16* inVector,  // N*1
        INT16* inMatrix, // M*N
        INT16* outMatrix, // M*1
        UINT8   columnLen, // M
        UINT8   rowLen, // N
        INT8    fracBits)
{
    UINT8 i,j;
    SPAL_CMPLX32_T Accu = { 0, 0 };

    SPAL_COMPLEX_T* inVectCmp  =  (SPAL_COMPLEX_T*) inVector;
    SPAL_COMPLEX_T* inMatrCmp  =  (SPAL_COMPLEX_T*) inMatrix;
    SPAL_COMPLEX_T* outMatrCmp =  (SPAL_COMPLEX_T*) outMatrix;

    if (outMatrix)
    {
        SPAL_XCOR_SHIFT_CHECK_RANGE(fracBits,3);
    }

    for (i = 0; i < columnLen; i++)
    {

        OPL_MOV_32((INT32)0, Accu.i);
        OPL_MOV_32((INT32)0, Accu.q);

        for (j = 0; j < rowLen; j++)
        {

            SPAL_XCOR_MAC_32(inMatrCmp[rowLen*i+j].i, inVectCmp[j].i, Accu.i);
            SPAL_XCOR_MSU_32(inMatrCmp[rowLen*i+j].q, inVectCmp[j].q, Accu.i);
            SPAL_XCOR_MAC_32(inMatrCmp[rowLen*i+j].i, inVectCmp[j].q, Accu.q);
            SPAL_XCOR_MAC_32(inMatrCmp[rowLen*i+j].q, inVectCmp[j].i, Accu.q);
        }

        if (outMatrix)
        {
            OPL_SHR_32(Accu.i,fracBits,Accu.i);
            OPL_SAT_16(Accu.i,outMatrCmp[i].i);
            OPL_SHR_32(Accu.q,fracBits,Accu.q);
            OPL_SAT_16(Accu.q,outMatrCmp[i].q);
        }
    }

    return (Accu);

}

//=============================================================================
// spp_BuildRefNBurst_Prefilter_Ext_8809
//-----------------------------------------------------------------------------
/// Rebuilds the theoretical received normal burst
/// from the hard bits decisions and the estimated channel taps.
///
/// @param burst SPP_EQU_BURST_T*. Pointer to the burst structure.
/// @param tap8 UINT32*. Pointer to the tap.
///
//=============================================================================
PUBLIC VOID spp_BuildRefNBurst_Prefilter_Ext_8809(SPP_EQU_BURST_T* burst, UINT32* tap8)
{
    UINT32 hard_refBurst_tab[4];
    UINT32* hard_refBurst;
    UINT32 tmp_hardRef[3];
    UINT32 tscTmp[NB_HPAYLD_SIZE+1+3+3];
    hard_refBurst = MEM_ACCESS_UNCACHED(hard_refBurst_tab);

    // First extract hard values from equalized output
    // 116 SoftValues => 29 words
    spal_XcorHvextract( burst->sv,
                        (UINT8) 29,
                        hard_refBurst);

    // Build the first part of the burst
    // add the 3 needed tail bits ...(3 extra tail is imagined)
    tmp_hardRef[0] = hard_refBurst[0] << 6;
    // ...to the first 57bits of payload...
    tmp_hardRef[1] = (hard_refBurst[1] << 6)|(hard_refBurst[0] >> 26);
    // ...mask...
    tmp_hardRef[1] &= 0xFFFFFFFFU >> (32-(57+6-32));
    // ...and add the first stealing flag...
    tmp_hardRef[1] |= (hard8(burst->sv[114]) >> 7 ) << (57+6-32);
    // ...and the beginning of the TS
    tmp_hardRef[2] = (g_sppGsmTscNburst[burst->tsc]);

    // Reconstruct reference symbols of first part of the burst:
    // 57bits payload + 1 stealing flag + 5 TS bits not done during snr calc.
    // use 2 part 5+3 to replace the above, functional the same
    spal_XcorSrec_8tap_Ext_8809(tap8, // channel taps
                                (NB_HPAYLD_SIZE+1+3+3), // number of symbols
                                tmp_hardRef, // hardbit reference
                                &(burst->ref[0]),// output
                                tscTmp);



    // Build the second part of the burst
    // add the 10 last TS bits
    tmp_hardRef[0] = (g_sppGsmTscNburst[burst->tsc]) >> (NB_TS_SIZE-10);
    // add the second stealing flag
    tmp_hardRef[0] |= (hard8(burst->sv[115])>>7) << (10);
    // ...to the second 57bits of payload...
    tmp_hardRef[0] |= ((hard_refBurst[1] >> (NB_HPAYLD_SIZE-32-10-1)) & 0xFFFFF800);
    tmp_hardRef[0] |= hard_refBurst[2] << (64-NB_HPAYLD_SIZE+10+1);
    tmp_hardRef[1] = (hard_refBurst[2] >> (NB_HPAYLD_SIZE-32-10-1));
    tmp_hardRef[1] |= (hard_refBurst[3] << (64-NB_HPAYLD_SIZE+10+1));
    tmp_hardRef[2] = (hard_refBurst[3] >>(32-(64-NB_HPAYLD_SIZE+10+1)));


    // add mask for the 2 needed tail bits ...
    tmp_hardRef[2] &= 0xFFFFFFFFU >> (32+(32-(7+1+3+57-32)));

    // Reconstruct reference symbols of second half part of the burst:
    // use 2 part 5+3 to replace the above, functional the same
    spal_XcorSrec_8tap_Ext_8809(tap8, // channel taps
                                (NB_HPAYLD_SIZE+1+4), // number of symbols
                                tmp_hardRef, // hardbit reference
                                &(burst->ref[142-(NB_HPAYLD_SIZE+1+4)]),// output
                                tscTmp);

}

//=============================================================================
// spp_FofEstimation_Prefilter_Ext_8809
//-----------------------------------------------------------------------------
/// Reconstruct the reference burst (softvalues), part of it
/// (training sequence) has already been done during SNR estimation.
///
/// Once the reference burst has been rebuilt, determines the Fof by
/// comparing the first FOF_NB_SYMB symbols of reference burst and
/// received burst, as well as the last FOF_NB_SYMB symbols of reference
/// burst and received burst.
///
/// The Fof is returned in Hertz.
///
/// @param burst SPP_EQU_BURST_T*. Pointer to the burst structure.
/// @param tap8 UINT32*. Pointer to the tap
///
//=============================================================================
PUBLIC VOID spp_FofEstimation_Prefilter_Ext_8809(SPP_EQU_BURST_T* burst, UINT32* tap8)
{
    INT32 Icorr,divider;

    //SPP_PROFILE_FUNCTION_ENTRY(spp_FofEstimation);
    // Reconstruct the reference burst (softvalues), part of it
    // (training sequence) has already been done during SNR estimation.
    // NOTE: It is not necessary to rebuild the full burst as we only
    // need FOF_NB_SYMB on each side to estimate FOF, but this is more
    // flexible and doesn't cost much thanks to the accelerators.
    spp_BuildRefNBurst_Prefilter_Ext_8809(burst,tap8);

    burst->noise = spal_XcorErrCalc(
                       (UINT32*)&(burst->samples[3]), // original
                       (UINT32*)&(burst->ref[0]), // reference
                       142); // Number of samples

    // Normalise NoisePower and calculate Snr
    burst->noise /= 142;

    // Calculate Imaginary part of correlation on start of burst
    // '+3' because tail bits are not present in refBurst
    Icorr = - spal_XcorIcorCalc(&(burst->samples[FOF_START_1+3]),
                                &(burst->ref[FOF_START_1]),
                                FOF_NB_SYMB);

    // Calculate Imaginary part of correlation on end of burst
    // '+3' because tail bits are not present in refBurst
    Icorr += spal_XcorIcorCalc( &(burst->samples[FOF_START_2+3]),
                                &(burst->ref[FOF_START_2]),
                                FOF_NB_SYMB);

    // FOf is given by:
    //
    //                           CorrI x Fs
    // FOF = _______________________________________________________
    //
    //       corr_gap x 2 PI x Norm(Power-NoisePower) x nb_symb_corr
    divider= 43104 / (FOF_CORR_GAP*FOF_NB_SYMB);// Fs/(2*PI)=43104
    divider=(INT32)((burst->power-burst->noise)/divider);
    if(divider)
        burst->fof = (INT16)(Icorr / divider);
    else
        burst->fof = 0;
    //SPP_PROFILE_FUNCTION_EXIT(spp_FofEstimation);
}

//=============================================================================
// spp_LpChannelEstimation_Ext_8809
//-----------------------------------------------------------------------------
// This function perform Linear-prediction for minimization of the outout power of the error filter
//
// @param RLS_W SPAL_COMPLEX_T*.INPUT.Address of the channel coefficients
// @param P_Array SPAL_COMPLEX_T*.OUTPUT.Address of the P_Array
// @param H_CorArray SPAL_COMPLEX_T*.OUTPUT.Address of the H_CorArray
// @param A1 SPAL_COMPLEX_T*.OUTPUT.Address of the conjugated channel coefficients
// @param LogChanPwr INT16*.INPUT.Channel Pwr Log
//
// @author Chang Yuguang, modified by Yordan Tabakov and Chunjiang TU
// @date 13/09/2012
//=============================================================================

PUBLIC VOID spp_LpChannelEstimation_Ext_8809(INT16 *RLS_W,
        INT16 *P_Array,
        INT16 *H_CorArray,
        INT16 *A1,
        INT8 LogChanPwr)
{

    // Here we are using a conjugated P_Array compared to the initial algo
    SPAL_COMPLEX_T LD_E, LD_K;
    SPAL_COMPLEX_T P_Array_New[QP];

    SPAL_COMPLEX_T* RLS_W_Cmp      = (SPAL_COMPLEX_T*) RLS_W;
    SPAL_COMPLEX_T* P_Array_Cmp    = (SPAL_COMPLEX_T*) P_Array ;
    SPAL_COMPLEX_T* H_CorArray_Cmp = (SPAL_COMPLEX_T*) (MEM_ACCESS_UNCACHED(H_CorArray));
    SPAL_COMPLEX_T* A1_Cmp         = (SPAL_COMPLEX_T*) A1;

    UINT8 i, j;

    SPAL_COMPLEX_T* p_LD_K2, * p_P_Array_New;
    SPAL_COMPLEX_T LD_K2;

    //SPP_PROFILE_FUNC_ENTER(spp_LpChannelEstimation_Ext_8809);

    p_LD_K2 = (SPAL_COMPLEX_T*)(MEM_ACCESS_UNCACHED(&LD_K2));
    p_P_Array_New = (SPAL_COMPLEX_T *)(MEM_ACCESS_UNCACHED(&P_Array_New[0]));

#if (EGPRS_LPEST_DEBUG)
    if (g_tcj_lpestINIT == 0x00)
    {
        UINT32 * tmppt = MEM_ACCESS_UNCACHED(RLS_W);
        for (UINT8 kk = 0; kk<8; kk++)
            g_tcj_lpestBuffer[kk] = *tmppt++;
        g_tcj_lpestBuffer[8] = (UINT32)LogChanPwr;
    }
#endif

    //calculate the A1(n)=conjugate(RLS(-n))
    //conjugate the RLS
    for (i = 0; i < SPP_LSE_TAP_NB; i++)
    {
        A1_Cmp[SPP_LSE_TAP_NB - 1 - i].i =  RLS_W_Cmp[i].i;
        A1_Cmp[SPP_LSE_TAP_NB - 1 - i].q = -RLS_W_Cmp[i].q;
    }

//convolution(h(k),h*(-k))
    spal_XcorComplexConvolution_Ext_8809(    (INT16*)RLS_W, // format Q(LPCH)
            SPP_LSE_TAP_NB,
            (INT16*)A1_Cmp,                    // format Q(LPCH)
            SPP_LSE_TAP_NB,
            (INT16*)H_CorArray_Cmp,            // format Q(LPQP+LPSFL+LPCH-LogChanPwr)
            0,
            0,
            (INT8)(LPCH + LogChanPwr - LPQP - LPSFL));


#if (EGPRS_LPEST_DEBUG)
    if (g_tcj_lpestINIT == 0x00)
    {
        UINT32 * tmppt = (UINT32*)H_CorArray_Cmp;
        for (UINT8 kk = 0; kk<8+8-1; kk++)
            g_tcj_lpestBuffer[9 + kk] = *tmppt++;


        // *(UINT32*)0x0 = 0;
    }

#endif


    // the recursive equations of LD algorithm are as follows:
    //
    // P_Array<m+1,k> = P<m,k> + LD_K<m+1>*P_Array<m,m+1-k>
    // P_Array<m+1,m+1> = LD_K<m+1>
    // LD_E<m+1> = (1-|LD_K<m+1>|^2)*LD_E<m>
    // LD_K(m+1) = (H_CorArray<m+1> + sum(P<m,k>*H<m+1-k>))/LD_E<m>
    // LD_E<0> = H_CorArray<0>
    // format:
    // H_CorArray: Q(LPQP+LPSFL+LPCH-LogChanPwr)
    // P_Array: Q(LPQP)
    // LD_E: Q(LPQP+LPSFL+LPCH-LogChanPwr)
    // LD_K: Q(LPQP)
    LD_E = H_CorArray_Cmp[SPP_LSE_TAP_NB - 1];


    for (i = 0; i < QP; i++)
    {
        INT32 num_i,num_q, denom;
        INT16 delta;

        UINT8 k = MIN(i,SPP_LSE_TAP_NB - 1);

        //********************************************************
        // k = i, if i==0,1,2,...7
        // k = 7, if i==8,9
        // -----------------
        // H_CorArray_Cmp[7+j]: R(j).  R(-7), R(-6),...R(0),R(1),...R(7). [ R(j) = conj(R(-j))]. Fracbits: aa = LPCH - LogChanPwr + LPQP + LPSFL
        // LD_K : K. Fracbits: LPQP
        // LD_E : E. Fracbits: aa
        // -----------------
        // K = conj(R(-k)*P[i-k+1]+R(-k+1)*P[i-k+2]+R(-k+2)*P[i-k+3]+...+R(-1)*P[i]])
        // K = K+conj(R(-i-1))
        // -----------------
        // i.e.
        // K = R(i+1) + R(k)*conj(P(i-k+1))+R(k-1)*conj(P(i-k+2))+...+R(1)*conj(P(i))
        // if we suppose R(9) = R(8) ==0
        // i.e.
        // K = R(i+1) + R(i)*conj(P(1))+R(i-1)*conj(P(2))+...+R(1)*conj(P(i))
        spal_XcorComplexMatrixMultiply_Ext_8809 ((INT16*)&H_CorArray_Cmp[SPP_LSE_TAP_NB - 1 - k],    // format Q(LPQP+LPSFL)
                (INT16*)&P_Array_Cmp[i - k + 1],                    // format Q(LPQP)
                (INT16*)p_LD_K2,                                    // format Q(LPQP+LPSFL)
                1,
                k,
                LPQP);

        LD_K.i = p_LD_K2->i;
        LD_K.q = -p_LD_K2->q;

        if (i < SPP_LSE_TAP_NB - 1)
        {
            LD_K.i += H_CorArray_Cmp[SPP_LSE_TAP_NB - 2 - i].i;
            LD_K.q -= H_CorArray_Cmp[SPP_LSE_TAP_NB - 2 - i].q;
        }

        //********************************************************
        // K = -K/E
        // delta = 1-|K|^2
        // E = E*(1-delta)
        num_i   = - LD_K.i*LD_E.i - LD_K.q*LD_E.q;
        num_q   = - LD_K.i*LD_E.q - LD_K.q*LD_E.i;
        denom =   LD_E.i*LD_E.i + LD_E.q*LD_E.q;

        LD_K.i = spp_DivL(num_i,denom,LPQP);
        LD_K.q = spp_DivL(num_q,denom,LPQP);

        delta = SPP_SAT_INT16((((1<<(2*LPQP)) - 1) - LD_K.i*LD_K.i - LD_K.q*LD_K.q) >> LPQP);

        // avoid overflows
        if (delta < 0)
        {
            LD_K.i = 0;
            LD_K.q = 0;
            delta = ((1 << LPQP) - 1);
        }

        LD_E.i = SPP_SAT_INT16((delta*LD_E.i) >> LPQP);
        LD_E.q = SPP_SAT_INT16((delta*LD_E.q) >> LPQP);

#if (EGPRS_LPEST_DEBUG)
        if (g_tcj_lpestINIT == 0x00)
        {
            g_tcj_lpestBuffer[9 + 15 + 12*i] = *((UINT32*)(&LD_K));
            g_tcj_lpestBuffer[9 + 15 + 12*i+1] = *((UINT32*)(&LD_E));
        }
#endif

        //********************************************************
        // p_P_Array_New: Pn. Fracbits: LPQP
        // p_Array_Cmp: P. Fracbits: LPQP
        // ----------------------------------
        // Pn(j-1) = K*P(j), j = 1,2,..i
        // P(j) = P(j) + conj(Pn(i-j)) = P(j) + conj(K*P(i-j+1))
        // -----------------------
        // i.e. P(1) = P(1)+conj(K*P(i)), P(2) = P(2)+conj(K*P(i-1)),...P(i) = P(i)+conj(K*P(1))
        // P(i+1) = conj(K)
        spal_XcorComplexMatrixMultiply_Ext_8809 ((INT16*)&LD_K,   // format Q(LPQP)
                (INT16*)&P_Array_Cmp[1],        // format Q(LPQP)
                (INT16*)(&p_P_Array_New[1]),    // format Q(LPQP)
                i,
                1,
                LPQP);
        for(j = 0; j < i; j++)
        {
            P_Array_Cmp[j+1].i += p_P_Array_New[i-j].i;
            P_Array_Cmp[j+1].q -= p_P_Array_New[i-j].q;
        }

        P_Array_Cmp[i+1].i =  LD_K.i;
        P_Array_Cmp[i+1].q = -LD_K.q;


#if (EGPRS_LPEST_DEBUG)
        if (g_tcj_lpestINIT == 0x00)
        {
            UINT32 * tmppt = (UINT32*)(&P_Array_Cmp[1]);
            for (UINT8 kk = 0; kk < 10; kk++)
            {
                g_tcj_lpestBuffer[9 + 15 + 12*i + 2 + kk] = *tmppt++;
            }
        }
#endif

    }


    P_Array_Cmp[0].i = ((1 << LPQP) - 1);
    P_Array_Cmp[0].q = 0;


#if (EGPRS_LPEST_DEBUG)
    if (g_tcj_lpestINIT == 0x00)
    {
        UINT32 * tmppt = MEM_ACCESS_UNCACHED(P_Array);
        for (UINT8 kk = 0; kk<11; kk++)
            g_tcj_lpestBuffer[9 + 15 + 120 + kk] = *tmppt++;
        tmppt = MEM_ACCESS_UNCACHED(H_CorArray);
        for (UINT8 kk = 0; kk<15; kk++)
            g_tcj_lpestBuffer[9 + 15 + 120 + 11 + kk] = *tmppt++;
        tmppt = MEM_ACCESS_UNCACHED(A1);
        for (UINT8 kk = 0; kk<8; kk++)
            g_tcj_lpestBuffer[9 + 15 + 120 + 11 + 15 + kk] = *tmppt++;

        g_tcj_lpestINIT = 0x02;
        *(UINT32*)0x0 = 0;
    }
#endif
    //SPP_PROFILE_FUNC_EXIT(spp_LpChannelEstimation_Ext_8809);


}



//=============================================================================
// spp_Max_Ext_8809()
//-----------------------------------------------------------------------------
/// This function is used to search max abs value
//=============================================================================

PUBLIC UINT16 spp_Max_Ext_8809(INT16* in, UINT16 n)
{
    UINT16 i,v;
    UINT32 max = 0;
    for (i=0; i<n; i++)
    {
        v = ABS(in[i]);
        if (max<v)
            max = v;
    }
    return max;
}

//=============================================================================
// spp_8tapDetect_Ext_8809()
//-----------------------------------------------------------------------------
/// This function is used to detect where last 3 tap has enough power
/// output global:g_fadingchannel_8tap_state(long term)
/// 0: 5tap channel
/// 1: 8tap channel
/// return value: current channel :
/// 0: 5tap channel
/// 1: 8tap channel
/// TapPwr: 8 tap pwr sum
//=============================================================================
PUBLIC UINT8 spp_8tapDetect_Ext_8809(SPP_COMPLEX_T* RLS_W, UINT32*TapPwr)
{
#define SPP_8TAP_DETECT_PWR_THD             3
#define SPP_8TAP_DETECT_CNT_LIMIT           127
#define SPP_8TAP_DETECT_8TAP_NUM            32
#define SPP_8TAP_DETECT_5TAP_NUM            16



    UINT8 Valid = 0;
    UINT32 prepwr=0, postpwr=0;
#if 0
    prepwr = spal_XcorComplexPower16((INT16*) RLS_W, NULL, 5, 0);
    postpwr = spal_XcorComplexPower16((INT16*) (&RLS_W[5]), NULL, 3, 0);
#else
    for(UINT8 i=0; i<8; i++)
    {
        UINT32 Pwr = ((RLS_W[i].i*RLS_W[i].i + RLS_W[i].q*RLS_W[i].q) >> 4);
        if (i<5)
            prepwr += Pwr;
        else
            postpwr += Pwr;
    }
#endif
    *TapPwr = ((prepwr + postpwr)>>(LPCH-4));// RLS_W.i and RLS_W.q : Q(LPCH)
    if ( (prepwr>>SPP_8TAP_DETECT_PWR_THD) < postpwr )
    {
        g_fadingchannel_8tap_num ++;
    }

    g_fadingchannel_cnt++;

    if (g_fadingchannel_cnt > SPP_8TAP_DETECT_CNT_LIMIT)
    {
        if (g_fadingchannel_8tap_num > SPP_8TAP_DETECT_8TAP_NUM)
            g_fadingchannel_8tap_state = TRUE;
        else if (g_fadingchannel_8tap_num < SPP_8TAP_DETECT_5TAP_NUM)
            g_fadingchannel_8tap_state = FALSE;

        g_fadingchannel_cnt = 0;
        g_fadingchannel_8tap_num  = 0;
    }

    if (g_fadingchannel_8tap_state == FALSE)
        Valid = 0;
    else if ((prepwr >> 5) < postpwr)
        Valid = 1;

    return Valid;
}



//=============================================================================
// spp_PrefltCheckValid_Ext_8809()
//-----------------------------------------------------------------------------
// This function generates LpChan based on H_CorArray and A2, detect where prefilters are valid
// returned value:
// 0:   Prefilter is not valid due to LpChan is not min-phase, use original CE5 algo
// 1:   Prefilter is valid.
// >1:  Prefilter is valid, but LpChan still has large 3taps at the end, needs 3tap removal algo.
//=============================================================================
PUBLIC UINT8 spp_PrefltCheckValid_Ext_8809(SPP_EGPRS_PREFILTER_BUFS_T *PrefilterBufs,
        INT8  LogChanPwr,
        UINT8 LPQP_Shift)
{
#define LPCHAN_MIN_PHASE_SHIFT_THD      4
    SPAL_COMPLEX_T LpChan_Tmp[((QP+1)+(2*SPP_LSE_TAP_NB-1)-1)];
    UINT32 LpChanPwr_Pre=0,LpChanPwr_Post=0;
    SPAL_COMPLEX_T* LpChan_Tmp_cmp = (SPAL_COMPLEX_T*)(MEM_ACCESS_UNCACHED(&LpChan_Tmp[0]));
    UINT8 Valid;
    UINT16 i;

    spal_XcorComplexConvolution_Ext_8809((INT16*)PrefilterBufs->H_CorArray,    // format Q(LPQP+LPSFL)+LPSFL2
                                         2*SPP_LSE_TAP_NB-1,
                                         (INT16*)PrefilterBufs->A2,            // format Q(LPQP2)13
                                         QP + 1,
                                         (INT16*)LpChan_Tmp_cmp,        // format Q(LPCH)
                                         0,
                                         0,
                                         (INT8)(2*LPQP + LPSFL - LogChanPwr - LPQP_Shift));

#if (GPRS_CCHRX_DEBUG)
    if  ( g_tcj_CCHINIT== 0x00)
    {
        UINT32 *tmpsrc = (UINT32*)(LpChan_Tmp_cmp);
        UINT16 kk;
        UINT32 *tmpdes = MEM_ACCESS_UNCACHED(&g_tcj_CCHBuf[4*156+4*32+4*3+4*25]);
        for(kk=0; kk<25; kk++)
        {
            *tmpdes++ = *tmpsrc++;
        }
    }
#endif

    for (i=0; i<(QP+SPP_LSE_TAP_NB-1); i++)
    {
        LpChanPwr_Pre += (LpChan_Tmp_cmp[i].i*LpChan_Tmp_cmp[i].i +LpChan_Tmp_cmp[i].q*LpChan_Tmp_cmp[i].q)>>3;
    }
    for (; i<((QP+1)+(2*SPP_LSE_TAP_NB-1)-1); i++)
    {
        LpChanPwr_Post += (LpChan_Tmp_cmp[i].i*LpChan_Tmp_cmp[i].i +LpChan_Tmp_cmp[i].q*LpChan_Tmp_cmp[i].q)>>3;
        *(UINT32*)(&PrefilterBufs->LpChan[i-(QP+SPP_LSE_TAP_NB-1)]) = *((UINT32*)(&LpChan_Tmp_cmp[i]));
    }
    if ((LpChanPwr_Post>>LPCHAN_MIN_PHASE_SHIFT_THD) < LpChanPwr_Pre)
    {
        // LpChan Min Phase Fail. Improper P_Array
        Valid = 0;
    }
    else
    {
        Valid = 2;//>1, means use 3tap removal equalization algo later
    }
    return Valid;
}


//=============================================================================
// spp_LsePrefilter_Ext_8809
//-----------------------------------------------------------------------------
// Pre-filter function for each burst.
//
// @param PrefilterBufs  SPP_EGPRS_PREFILTER_BUFS_T  *. INPUT.  Pointer to prefilter coefficients' structure.
//
// @author Chang Yuguang, modified by Yordan Tabakov and Chunjiang TU
// @date 13/09/2012
//=============================================================================

PUBLIC UINT8 spp_LsePrefilter_Ext_8809(
    UINT8                       tsc,
    SPP_EGPRS_PREFILTER_BUFS_T* PrefilterBufs,
    UINT32*                     NoisePower)

{
    INT32 i;
    UINT32 NoisePwr;
    UINT32 TapsPwr;
    INT8  LogChanPwr;
    // tmpTsc must be uncached
    // put tmpTsc to end of PrefilterBufs, because UncachedBuffers->EgprsPrefilterBufs 's end
    // is currently no use. and UncachedBuffers (g_spcCtx->Malloc) is quite large.
    // tmpTsc only use (26-7)*sizeof(SPP_COMPLEX_T).
    SPP_COMPLEX_T *tmpTsc = ((SPP_COMPLEX_T*)PrefilterBufs) + sizeof(SPP_EGPRS_PREFILTER_BUFS_T)/sizeof(SPP_COMPLEX_T);
    INT16 *LocalTscUC ;
    UINT16 PMax;
    UINT8 LPQP_Shift;
    UINT8 Valid;

    //    the overall transfer function of the FIR prefilter is below in Z domain.
    //             F(Z) = z-(qh+qp)A1(z)A2(z)
    //             A1(n) = conjugate(h(-n))
    //               A2(n) = P(n)
    //        so,
    //               Hmin(n) = A1(n)*A2(n)
    //               Hmax(n) = A1(n)*conjugate(A2(n))
    //
    //        so the prefilter result is,
    //               (the back half of input)*Hmin(n)
    //               (the front half of input)*Hmax(n)
#if (EGPRS_PREFLT_DEBUG)
    if (g_tcj_prefltINIT == 0x0)
    {
        UINT32 *tmp = (UINT32*)(&(PrefilterBufs->InArray[0]));
        for (UINT8 kk=0; kk<157; kk++)
        {
            g_tcj_prefltBuffer[kk] = *tmp++;
        }
        g_tcj_prefltBuffer[157] = (UINT32)tsc;
    }
#endif
    // 8tap Channel Estimation
    spal_XcorChest_8taps_Ext_8809((INT16 *) &PrefilterBufs->InArray[59 + SPP_LSE_TAP_NB], // format 3.9
                                  (INT16 *) (&gc_sppChestTabEgprs_Ext_8809[tsc][0][0]),//format: Q(15)
                                  (INT16 *) PrefilterBufs->RLS_W,// format: Q(LPCH)
                                  SPP_LSE_TAP_NB,
                                  9+15 - LPCH);

#if (EGPRS_PREFLT_DEBUG)
    if (g_tcj_prefltINIT == 0x0)
    {
        UINT32 *tmp = (UINT32*)(&(PrefilterBufs->RLS_W[0]));
        for (UINT8 kk=0; kk<8; kk++)
        {
            g_tcj_prefltBuffer[158+kk] = *tmp++;
        }
    }
#endif

    // detect where fading channel has 8tap
    Valid = spp_8tapDetect_Ext_8809(PrefilterBufs->RLS_W, &TapsPwr);
    if (!Valid)
        return 0;

#if (EGPRS_PREFLT_DEBUG)
    if (g_tcj_prefltINIT == 0x0)
    {
        //UINT32 *tmp = (UINT32*)(&(g_sppEgprsTapPwrMax[0][0]));
        UINT32 *tmp = NULL;// note debug load tap pwr.
        for (UINT8 kk=0; kk<8; kk++)
        {
            g_tcj_prefltBuffer[158+8+kk] = *tmp++;
        }
    }
#endif

#if 0
    // the same as #else ... #end part.
    spal_XcorSrec( (INT16*)(PrefilterBufs->RLS_W+(SPP_LSE_TAP_NB-5)), // Channel Taps
                   16,// Reconstruction length
                   (UINT32*) &(g_sppGsmTscNburst[tsc]),     // ref. hardbit sequence
                   (UINT32*) &(PrefilterBufs->LocalTsc[0]), // output
                   5);                         // reconstructed output

#if (EGPRS_PREFLT_DEBUG)
    if (g_tcj_prefltINIT == 0x0)
    {
        UINT32 *tmp = (UINT32*)(MEM_ACCESS_UNCACHED(&(PrefilterBufs->LocalTsc[0])));
        for (UINT8 kk=0; kk<16; kk++)
        {
            g_tcj_prefltBuffer[158+8+8+kk] = *tmp++;
        }
    }
#endif

    spal_XcorSrec_Ext( (INT16*)PrefilterBufs->RLS_W, // Channel Taps
                       16,// Reconstruction length
                       (UINT32*) &(g_sppGsmTscNburst[tsc]),     // ref. hardbit sequence
                       (UINT32*) &(tmpTsc[0]), // output
                       3);             // reconstructed output

#if (EGPRS_PREFLT_DEBUG)
    if (g_tcj_prefltINIT == 0x0)
    {
        UINT32 *tmp = (UINT32*)(MEM_ACCESS_UNCACHED(&(tmpTsc[0])));
        for (UINT8 kk=0; kk<16; kk++)
        {
            g_tcj_prefltBuffer[158+8+8+16+kk] = *tmp++;
        }
    }
#endif

    // step 3. get the training sequence error
    LocalTscUC = (INT16*)(MEM_ACCESS_UNCACHED(&(PrefilterBufs->LocalTsc[0])));
    for (i=0; i< 16 ; i++)
    {
        /*PrefilterBufs->LocalTsc[i].i += tmpTsc[i].i;
            PrefilterBufs->LocalTsc[i].q += tmpTsc[i].q;
            PrefilterBufs->LocalTsc[i].i >>= 2;
            PrefilterBufs->LocalTsc[i].q >>= 2;        */

        *LocalTscUC += tmpTsc[i].i;
        (*LocalTscUC) >>=(LPCH-9);
        LocalTscUC++;
        *LocalTscUC += tmpTsc[i].q;
        (*LocalTscUC) >>=(LPCH-9);
        LocalTscUC++;
    }
#else
    spal_XcorSrec_8tap_Ext_8809((UINT32*)(&PrefilterBufs->RLS_W[0]),
                                16,
                                (UINT32*) &(g_sppGsmTscNburst[tsc]),
                                (UINT32*) &(PrefilterBufs->LocalTsc[0]), // output
                                (UINT32*)tmpTsc);
    LocalTscUC = (INT16*)(MEM_ACCESS_UNCACHED(&(PrefilterBufs->LocalTsc[0])));
    for (i=0; i< 16 ; i++)
    {
        (*LocalTscUC) >>=(LPCH-9);
        LocalTscUC++;
        (*LocalTscUC) >>=(LPCH-9);
        LocalTscUC++;
    }
#endif

#if (EGPRS_PREFLT_DEBUG)
    if (g_tcj_prefltINIT == 0x00)
    {
        UINT32 *tmp = (UINT32*)(MEM_ACCESS_UNCACHED(&(PrefilterBufs->LocalTsc[0])));
        for (UINT8 kk=0; kk<16; kk++)
        {
            g_tcj_prefltBuffer[158+8+8+16+16+kk] = *tmp++;
        }
    }
#endif


    NoisePwr = spal_XcorErrCalc((UINT32*) PrefilterBufs->LocalTsc, (UINT32*) &(PrefilterBufs->InArray[67]), (UINT8) 16);
    NoisePwr >>= 4;
    // output noise power per sample
    *NoisePower=NoisePwr;

    //TapsPwr   = spal_XcorComplexPower16((INT16*)PrefilterBufs->RLS_W,NULL,SPP_LSE_TAP_NB,LPCH);
    LogChanPwr = spp_Log2(TapsPwr);


    // the shifter is limited to [0, 15]
#define LP_CHAN_PWR_MIN (2*LPQP + LPSFL - 15)
#define LP_CHAN_PWR_MAX (LPQP + LPSFL + 15 - LPCH)

    LogChanPwr = (LogChanPwr > LP_CHAN_PWR_MAX) ? LP_CHAN_PWR_MAX :
                 (LogChanPwr < LP_CHAN_PWR_MIN) ? LP_CHAN_PWR_MIN :
                 LogChanPwr;

#if (EGPRS_PREFLT_DEBUG)
    if (g_tcj_prefltINIT == 0x00)
    {
        g_tcj_prefltBuffer[158+8+8+16+16+16+0] = NoisePwr;
        g_tcj_prefltBuffer[158+8+8+16+16+16+1] = (LogChanPwr<<8) | 0;
        g_tcj_prefltBuffer[158+8+8+16+16+16+2] = TapsPwr;
    }
#endif

    // LP algorithm to compute the P(n)
    spp_LpChannelEstimation_Ext_8809((INT16*)PrefilterBufs->RLS_W,  // format Q(LPCH)
                                     (INT16*)PrefilterBufs->P_Array,        // format Q(LPQP)
                                     (INT16*)PrefilterBufs->H_CorArray,    // format Q(LPQP+LPSFL+LPCH-LogChanPwr)
                                     (INT16*)PrefilterBufs->A1,            // format Q(LPCH)
                                     LogChanPwr);
#if (EGPRS_PREFLT_DEBUG)
    if (g_tcj_prefltINIT == 0x00)
    {
        UINT32 *tmp = (UINT32*)(MEM_ACCESS_UNCACHED(PrefilterBufs->P_Array));
        for (UINT8 kk=0; kk<11; kk++)
        {
            g_tcj_prefltBuffer[158+8+8+16+16+16+3+kk] = *tmp++;
        }
        tmp = (UINT32*)(MEM_ACCESS_UNCACHED(PrefilterBufs->H_CorArray));
        for (UINT8 kk=0; kk<15; kk++)
        {
            g_tcj_prefltBuffer[158+8+8+16+16+16+3+11+kk] = *tmp++;
        }
        tmp = (UINT32*)(MEM_ACCESS_UNCACHED(PrefilterBufs->A1));
        for (UINT8 kk=0; kk<8; kk++)
        {
            g_tcj_prefltBuffer[158+8+8+16+16+16+3+11+15+kk] = *tmp++;
        }
    }
#endif
    // ----------------------------------
    // Search max tap in P_Array. Avoid spal_Excor hardware input saturation
    // ----------------------------------
    PMax = spp_Max_Ext_8809((INT16*)PrefilterBufs->P_Array, QP+1);
    PMax >>= 14;// 14 is spal_Excor input max range
    LPQP_Shift = 0;
    while(PMax)
    {
        PMax >>= 1;
        LPQP_Shift ++;
    }
    LPQP_Shift = MIN(LPQP_Shift, 4);

    // ----------------------------------
    // compute the prefilter coefficients
    // ----------------------------------

    for (i = QP; i >= 0; i--)
    {
        PrefilterBufs->A2[QP-i].i   =  (PrefilterBufs->P_Array[i].i)>>LPQP_Shift;
        PrefilterBufs->A2[QP-i].q   =  (PrefilterBufs->P_Array[i].q)>>LPQP_Shift;
        PrefilterBufs->P_Array[i].q = (-PrefilterBufs->P_Array[i].q)>>LPQP_Shift;
        PrefilterBufs->P_Array[i].i = (PrefilterBufs->P_Array[i].i)>>LPQP_Shift;
    }


    // compute the Hmin using the A1(z),A2(z),and H(z)
    Valid = spp_PrefltCheckValid_Ext_8809(PrefilterBufs, LogChanPwr, LPQP_Shift);
    if (!Valid) // prefiltering is not necessary
        return 0;


    spal_XcorComplexConvolution_Ext_8809((INT16*)PrefilterBufs->A1,            // format Q(LPCH)11
                                         SPP_LSE_TAP_NB,
                                         (INT16*)PrefilterBufs->A2,            // format Q(LPQP2)13
                                         QP + 1,
                                         (INT16*)PrefilterBufs->PrefiltMin,    // format Q(LPCH)
                                         0,
                                         0,
                                         LPQP);

    spal_XcorComplexConvolution_Ext_8809((INT16*)PrefilterBufs->A1,            // format Q(LPCH)
                                         SPP_LSE_TAP_NB,
                                         (INT16*)PrefilterBufs->P_Array,        // format Q(LPQP)
                                         QP + 1,
                                         (INT16*)PrefilterBufs->PrefiltMax,    // format Q(LPCH)
                                         0,
                                         0,
                                         LPQP);


#if (EGPRS_PREFLT_DEBUG)
    if (g_tcj_prefltINIT == 0x00)
    {
        UINT32 *tmp = (UINT32*)(MEM_ACCESS_UNCACHED(&(PrefilterBufs->LpChan)));
        UINT8 kk;
        for (kk=0; kk<8; kk++)
            g_tcj_prefltBuffer[158+8+8+16+16+16+3+11+15+8+kk] = *tmp++;
        tmp = (UINT32*)(MEM_ACCESS_UNCACHED(&(PrefilterBufs->PrefiltMin)));
        for (kk=0; kk<18; kk++)
            g_tcj_prefltBuffer[158+8+8+16+16+16+3+11+15+8+8+kk] = *tmp++;
        tmp = (UINT32*)(MEM_ACCESS_UNCACHED(&(PrefilterBufs->PrefiltMax)));
        for (kk=0; kk<18; kk++)
            g_tcj_prefltBuffer[158+8+8+16+16+16+3+11+15+8+8+18+kk] = *tmp++;

    }
#endif

    //SPP_PROFILE_FUNC_EXIT(spp_LsePrefilter_Ext_8809);
    return Valid;
}

//=============================================================================
// spp_GprsLsePrefilter_Ext_8809
//-----------------------------------------------------------------------------
/// step 1. 8 tap ce
/// step 2. prefilter input data if necessary
/// step 3. if not, perform original 5 tap ce
///
/// @param burst SPP_EQU_BURST_T*. Pointer to the burst structure.
/// @param UncachedBuffers SPP_UNCACHED_BUFFERS_T*. Pointer to the prefilters structure
/// @param prefilterOut UINT32*. Pointer to the prefiltered data output
/// @param tap8 UINT32*. Pointer to the estimated 8 taps
/// @param LpChanExt UINT32*. Pointer to the last 3 tap of LpChan
/// @param LPValid UINT8*. Pointer to the output valid flag
//=============================================================================

PUBLIC VOID spp_GprsLsePrefilter_Ext_8809(
    SPP_EQU_BURST_T* burst,
    SPP_UNCACHED_BUFFERS_T* UncachedBuffers,
    UINT32 *prefilterOut,
    UINT32 *tap8,
    UINT32 *LpChanExt,
    UINT8 * LPValid)
{
    UINT32 Xcorr_Energy[SCH_TSC_XCOR_NBR]; // Sized for SCH
    UINT8 search_size;
    UINT8 i;
    UINT8 TOf = 0;
    UINT32 *tmppt;
    UINT16 tapWindow [3] = {6,7,8};
    SPC_PROFILE_FUNC_ENTER(spp_LsePrefilter_Ext_8809);

    // ******************************
    // Search Time Synchronization
    // ******************************
    // Use XCOR to compute Xcorrelation with Training Sequence
    if (1)
    {
        volatile INT16 *Xcorr= UncachedBuffers->NbVitac.Xcorr;// not enough size for Xcorr???
        // The first five bits of the training sequence are not
        // used to perform the following correlation, as they are
        // only a repetition of the last 5 bits of the training
        // sequence.
        // Therefore, the received pilots of interest start at earliest
        // on bit 65.
        // The correlation is performed using 16 consecutive bits of the
        // training sequence and it is performed on 8+SPAL_ISI=13
        // consecutive time slot.
        // We can detect a ToF of +/- 4 samples.
        spal_XcorTscxcNb(
            g_sppGsmTscNburst[burst->tsc],
            (UINT32*)(burst->samples + 60 + 4), // 4 samples margin
            (INT16*)Xcorr);

        // Calculate I^2 + Q^2 for every coeffs
        for(i=0; i<NB_TSC_XCOR_NBR; i++)
        {
            Xcorr_Energy[i] =   Xcorr[2*i]     * Xcorr[2*i] +
                                Xcorr[2*i + 1] * Xcorr[2*i + 1];
        }
        search_size = NB_TSC_XCOR_NBR - SPAL_ISI + 1;
    }

    // Now that we have the correlation on 13 consecutive time
    // slots, we find the maximum power profile considering that
    // the channel is 5 taps long with the main tap in the
    // middle (this assumptions corresponds to a GMSK on an AWGN
    // channel).

    // the correlation window could be flatter at the expense
    // of a little snr decrease (~1dB) in static conditions
    spp_TofSearch( Xcorr_Energy,
                   tapWindow,
                   search_size,
                   &(burst->tof),
                   &TOf);


    burst->samples_offset = TOf; // Keep track of symbol offset for HBURST mode


#if 1
    // It's easy to get a false tof, under HT100 environment.
    // To avoid false TOf calc, larger TOf may damage data, tof is limited.
    // if (TOf<=3 || TOf >=5)     TOf = 4;
    TOf = 4;
#endif
    burst->samples += (TOf);


    // Shift Symbol Buffer Origin for remaining of equalization process
    // After origin shift we have the following bits:
    //
    // NORMAL BURST
    // 0:2     -> guard bits
    // 3:59    -> data bits
    // 60      -> steal bit
    // 61:86   -> pilot bits
    // 87      -> steal bit
    // 88:144  -> data bits
    // 145:147 -> guard bits

    // copy data from RxBuffer to InArray
    tmppt = (UINT32*)(&UncachedBuffers->EgprsPrefilterBufs.InArray[0]);
    for (i = 0; i<157; i++)
        *tmppt++ =  burst->samples[i];

    // prefilter generation
    *LPValid = spp_LsePrefilter_Ext_8809(    burst->tsc,
               &UncachedBuffers->EgprsPrefilterBufs,
               &burst->noise);
    // prefilter
    if (*LPValid)
    {
        // avoid RxBuffer overwriting
        spal_XcorComplexConvolution_Ext_8809(  (INT16*)(UncachedBuffers->EgprsPrefilterBufs.InArray),     // format 3.9
                                               74,
                                               (INT16*)(UncachedBuffers->EgprsPrefilterBufs.PrefiltMax),  // format Q(LPCH)
                                               QP + SPP_LSE_TAP_NB,
                                               (INT16*)prefilterOut,                 // format 3.9
                                               SPP_LSE_TAP_NB - 2,
                                               74,
                                               LPCH);
        spal_XcorComplexConvolution_Ext_8809(  (INT16*)(UncachedBuffers->EgprsPrefilterBufs.InArray + 74),// format 3.9
                                               74,
                                               (INT16*)UncachedBuffers->EgprsPrefilterBufs.PrefiltMin,      // format Q(LPCH)
                                               QP + SPP_LSE_TAP_NB,
                                               (INT16*)(prefilterOut + 74),          // format 3.9
                                               SPP_LSE_TAP_NB - 2 + QP,
                                               74,
                                               LPCH);
        for (i=0; i< 5; i++)
        {
            *((INT16*)(&burst->taps[i])) = UncachedBuffers->EgprsPrefilterBufs.LpChan[i].i>>(LPCH-9);
            *(1+(INT16*)(&burst->taps[i])) = UncachedBuffers->EgprsPrefilterBufs.LpChan[i].q>>(LPCH-9);
        }
        for (i=0; i<3; i++)
        {
            *((INT16*)(&LpChanExt[i])) = UncachedBuffers->EgprsPrefilterBufs.LpChan[i+5].i>>(LPCH-9);
            *(1+(INT16*)(&LpChanExt[i])) = UncachedBuffers->EgprsPrefilterBufs.LpChan[i+5].q>>(LPCH-9);
        }
        for (i=0; i<19; i++)
        {
            if (i<16)
                burst->ref[58+6+i] = *(UINT32*)(&UncachedBuffers->EgprsPrefilterBufs.LocalTsc[i]);
        }
    }
    else
    {
#if 1
        // the same as #else ... #end
        // use original algo
        INT16 *taps = (INT16*)(&burst->taps[0]);

        burst->samples -= TOf;
        if (burst->power > (1<<22) )
            burst->samples += 4;
        else
            burst->samples += burst->samples_offset ;

        spal_XcorChest((INT16*)&burst->samples[63 + CHIP_CHEST_LEN- SPAL_ISI], (INT16*)&g_sppChestTab[burst->tsc][0][0], taps, SPAL_ISI, 15);

        // approximative weightening

        taps[0]=(taps[0]*25231)>>15;
        taps[1]=(taps[1]*25231)>>15;
        taps[2]=(taps[2]*30749)>>15;
        taps[3]=(taps[3]*30749)>>15;
        taps[4]= taps[4];
        taps[5]= taps[5];
        taps[6]=(taps[6]*30749)>>15;
        taps[7]=(taps[7]*30749)>>15;
        taps[8]=(taps[8]*25231)>>15;
        taps[9]=(taps[9]*25231)>>15;
        // SNR Estimation
        // --------------
        // First reconstruct TrainingSequence (minus 2 tail symbols on each side)
        spal_XcorSrec( (INT16*)burst->taps, // Channel Taps
                       (NB_TS_SIZE-4), // Reconstruction length
                       (UINT32*) &(g_sppGsmTscNburst[burst->tsc]), // ref. hardbit sequence
                       &(burst->ref[NB_TS_START+2]), // reconstructed output
                       SPAL_ISI); // channel tap number

        // Run power of the error calculation upon Training Sequence
        // The three guard bits are not included in the reference (reconstructed) burst
        burst->noise = spal_XcorErrCalc(
                           (UINT32*)&(burst->samples[NB_TS_START+3+2]), // original
                           (UINT32*)&(burst->ref[NB_TS_START  +2]), // reference
                           (NB_TS_SIZE-4)); // Number of samples

        // Normalise NoisePower and calculate Snr
        burst->noise /= (NB_TS_SIZE-4);
#else
        burst->samples -= TOf;
        // Channel Estimation Process
        // --------------------------
        spp_ChannelEstimation(GSM_WIN_TYPE_NBURST, burst, UncachedBuffers);

        // SNR Estimation
        // --------------
        // First reconstruct TrainingSequence (minus 2 tail symbols on each side)
        spal_XcorSrec( (INT16*)burst->taps, // Channel Taps
                       (NB_TS_SIZE-4), // Reconstruction length
                       (UINT32*) &(g_sppGsmTscNburst[burst->tsc]), // ref. hardbit sequence
                       &(burst->ref[NB_TS_START+2]), // reconstructed output
                       SPAL_ISI); // channel tap number

        // Run power of the error calculation upon Training Sequence
        // The three guard bits are not included in the reference (reconstructed) burst
        burst->noise = spal_XcorErrCalc(
                           (UINT32*)&(burst->samples[NB_TS_START+3+2]), // original
                           (UINT32*)&(burst->ref[NB_TS_START  +2]), // reference
                           (NB_TS_SIZE-4)); // Number of samples

        // Normalise NoisePower and calculate Snr
        burst->noise /= (NB_TS_SIZE-4);
#endif

    }


    for (i=0; i<8; i++)
    {
        *((INT16*)(&tap8[i])) = UncachedBuffers->EgprsPrefilterBufs.RLS_W[i].i>>(LPCH-9);
        *(1+(INT16*)(&tap8[i])) = UncachedBuffers->EgprsPrefilterBufs.RLS_W[i].q>>(LPCH-9);
    }

    SPC_PROFILE_FUNC_EXIT(spp_LsePrefilter_Ext_8809);

}



//=============================================================================
// spp_3tapRebuild_Prefilter_Ext_8809()
//-----------------------------------------------------------------------------
// This function rebuild data based on 3 taps
//=============================================================================

PUBLIC VOID spp_3tapRebuild_Prefilter_Ext_8809(SPP_EQU_BURST_T* burst, UINT32* tap, UINT32* sample_out)
{
    UINT32 NewTap[5];
    INT16 *NewTap_cmp = (INT16*)(MEM_ACCESS_UNCACHED(&NewTap[0]));
    UINT16 i;
    INT16 *sample_in = (INT16*)(&(burst->samples[0]));

    UINT32 hard_refBurst_tab[4];
    UINT32* hard_refBurst;
    UINT32 tmp_hardRef[2];
    UINT32 refout[74];
    INT16 *ref = (INT16*)(MEM_ACCESS_UNCACHED(&refout[0]));
    INT16* out = (sample_out)?((INT16*)sample_out):(sample_in);
    hard_refBurst = MEM_ACCESS_UNCACHED(hard_refBurst_tab);




    // First extract hard values from equalized output
    // 116 SoftValues => 29 words
    spal_XcorHvextract( burst->sv,
                        (UINT8) 29,
                        hard_refBurst);

    // Build the first part of the burst
    tmp_hardRef[0] = hard_refBurst[0];
    // ...to the first 57bits of payload...
    tmp_hardRef[1] = hard_refBurst[1];
    // ...mask...
    tmp_hardRef[1] &= 0xFFFFFFFFU >> (32-(57-32));
    // ...and add the first stealing flag...
    tmp_hardRef[1] |= (hard8(burst->sv[114]) >> 7 ) << (57-32);
    // ...and the beginning of the TS
    tmp_hardRef[1] |= (g_sppGsmTscNburst[burst->tsc]) << (57-32+1);
    //tmp_hardRef[2] = (g_sppGsmTscNburst[burst->tsc]) >> (32-(57-32+1));




    for (i=0; i<3; i++)
    {
        NewTap_cmp[2*(i)] = *((INT16*)(&tap[0])+2*(2-i));
        NewTap_cmp[2*(i)+1] = -*((INT16*)(&tap[0])+2*(2-i)+1);
    }
    for(i=0; i<2; i++)
    {
        *((UINT32*)(NewTap_cmp)+(i+3))= 0;
    }
#if (GPRS_3TAP_REB_DEBUG)
    if  ( g_tcj_RebINIT== 0x00)
    {
        UINT32 *tmppt = MEM_ACCESS_UNCACHED(burst->samples);
        UINT32 *tmpdes = MEM_ACCESS_UNCACHED(&g_tcj_RebBuf[0]);
        UINT16 kk;
        g_tcj_RebCnt = burst->tsc;
        for(kk=0; kk<148; kk++)
        {
            *tmpdes++ = *tmppt++;
        }
        tmppt = MEM_ACCESS_UNCACHED(burst->sv);
        for(kk=0; kk<116/4; kk++)
            *tmpdes++ = *tmppt++;
        tmppt = MEM_ACCESS_UNCACHED(tap);
        for(kk=0; kk<3; kk++)
            *tmpdes++ = *tmppt++;
    }

#endif

    spal_XcorSrec((INT16*)NewTap_cmp,
                  60,
                  tmp_hardRef,
                  (UINT32*)ref,
                  5);
    tmp_hardRef[0] = (g_sppGsmTscNburst[burst->tsc]) >> 2;
    spal_XcorSrec((INT16*)NewTap_cmp,
                  14,
                  tmp_hardRef,
                  ((UINT32*)ref)+60,
                  5);

    for (i=0; i<2*(74); i++)
    {
        *out++ = *sample_in++ - *ref++;
    }


#if (GPRS_3TAP_REB_DEBUG)
    if  ( g_tcj_RebINIT== 0x00)
    {
        UINT32 *tmppt = MEM_ACCESS_UNCACHED(refout);
        UINT32 *tmpdes = MEM_ACCESS_UNCACHED(&g_tcj_RebBuf[148+29+3]);
        UINT16 kk;

        for(kk=0; kk<74; kk++)
        {
            *tmpdes++ = *tmppt++;
        }
    }

#endif


    // Build the second part of the burst
    // add the 4 last TS bits
    tmp_hardRef[0] = (g_sppGsmTscNburst[burst->tsc]) >> 6;
    // add the second stealing flag
    tmp_hardRef[0] |= (hard8(burst->sv[115])>>7) << 20;
    // ...to the second 57bits of payload...
    tmp_hardRef[0] |= ((hard_refBurst[1] >> (NB_HPAYLD_SIZE-32-21)) & 0x0FE00000);
    tmp_hardRef[0] |= hard_refBurst[2] << (64-NB_HPAYLD_SIZE+21);
    tmp_hardRef[1] = (hard_refBurst[2] >> (NB_HPAYLD_SIZE-32-21));
    tmp_hardRef[1] |= (hard_refBurst[3] << (64-NB_HPAYLD_SIZE+21));
    //tmp_hardRef[2] = (hard_refBurst[3]>> (NB_HPAYLD_SIZE-32-21));
    // add mask for the 2 needed tail bits ...
    //tmp_hardRef[2] &= 0xFFFFFFFFU >> (32-14);

    for (i=0; i<3; i++)
    {
        NewTap_cmp[2*(i+2)] = *((INT16*)(&tap[0])+2*(i));
        NewTap_cmp[2*(i+2)+1] = *((INT16*)(&tap[0])+2*(i)+1);
    }
    for(i=0; i<2; i++)
    {
        *((UINT32*)(NewTap_cmp)+(i))= 0;
    }

    ref = (INT16*)(MEM_ACCESS_UNCACHED(&refout[0]));
    spal_XcorSrec((INT16*)NewTap_cmp,
                  60,
                  tmp_hardRef,
                  (UINT32*)ref,
                  5);
    tmp_hardRef[0] = hard_refBurst[3]&(0xFFFFFFFF>>(32-18));
    spal_XcorSrec((INT16*)NewTap_cmp,
                  14,
                  tmp_hardRef,
                  ((UINT32*)ref)+60,
                  5);


    for (i=0; i<2*(74); i++)
    {
        *out++ = *sample_in++ - *ref++;
    }

#if (GPRS_3TAP_REB_DEBUG)
    if  ( g_tcj_RebINIT== 0x00)
    {
        UINT32 *tmppt = MEM_ACCESS_UNCACHED(refout);
        UINT32 *tmpdes = MEM_ACCESS_UNCACHED(&g_tcj_RebBuf[148+29+3+74]);
        UINT16 kk;

        for(kk=0; kk<74; kk++)
        {
            *tmpdes++ = *tmppt++;
        }
        tmppt = MEM_ACCESS_UNCACHED((sample_out)?sample_out:(burst->samples));
        for(kk=0; kk<148; kk++)
        {
            *tmpdes++ = *tmppt++;
        }
        while(1);
    }

#endif

}


//=============================================================================
// spp_Equalize_Prefilter_Ext_8809
//-----------------------------------------------------------------------------
/// This function performs Viterbi equalization of a burst.
///
/// @param WinType UINT8. INPUT. Toggles channel estimation function to either
///                       normal burst or SCH.
/// @param burst SPP_EQU_BURST_T*. INPUT/OUTPUT Pointer to the burst structure.
/// @param UncachedBuffers SPP_UNCACHED_BUFFERS_T*. INPUT/OUTPUT. Pointer to
/// the work buffers designated by SPC layer.
//=============================================================================
PUBLIC VOID spp_Equalize_Prefilter_Ext_8809(UINT8 WinType,
        UINT8 offset,
        SPP_EQU_BURST_T* burst,
        SPP_UNCACHED_BUFFERS_T* UncachedBuffers)
{
    INT16 H[4];
    int i,survivor;
    SPAL_VITAC_EQU_PARAM_T equ_param;
    UINT32 SumTable[2*2*8/2];
    volatile UINT8  *path_diff_bkw;
    volatile UINT8  *path_diff_fwd;
    volatile UINT16 *PathMetric;
    UINT8* tb_wr_ptr = &(burst->sv[1]);
    UINT8   tb_start,tb_end,tb_idx,tb_size;
    INT16* taps = (INT16*) burst->taps;
    SPAL_DC_OFFSET_T Dco;
    BOOL fwd_vitac_done = FALSE;

#define BM_SHIFT_MAX    17
#define MAXU_NBIT(n) ((1<<n)-1)
#define SV_MAX MAXU_NBIT(7)

    // SPP_PROFILE_FUNCTION_ENTRY(spp_Equalize);
    // ***********************************
    // Derive the VITAC shifters settings
    // ***********************************
    equ_param.shift_bm = spp_Log2(burst->noise);
    if (equ_param.shift_bm>SHIFT_BM_OFFSET) equ_param.shift_bm -= SHIFT_BM_OFFSET;

    equ_param.shift_sb = SHIFT_SB_OFFSET;

    if (equ_param.shift_bm>BM_SHIFT_MAX)
    {
        equ_param.shift_sb += (equ_param.shift_bm-BM_SHIFT_MAX);
        equ_param.shift_bm=BM_SHIFT_MAX;
    }
    else if (equ_param.shift_bm<4) equ_param.shift_bm = 4;
    if (equ_param.shift_sb>9) equ_param.shift_sb = 9;
#if 1
    equ_param.shift_bm -= 3;
#endif

    // ***********************************
    // Precalculation of Sum terms for BMs
    // ***********************************
    // The branch metric for a given node can be precalculated
    // since the register state is directly linked to the node.
    spal_XcorBmsum(taps, (INT16*)SumTable);
    // change SumTable's Content:
    // SumTable:
    // 0->7: for forward bm
    // 8->15:for backward bm. conjugate of 0->7
    for (i=0; i<8; i++)
    {
        *(UINT16*)(&SumTable[i+8]) = *(UINT16*)(&SumTable[i]); // real
        *((UINT16*)(&SumTable[i+8])+1) = -*((UINT16*)(&SumTable[i])+1); // image
    }

    // Compute H0 and HL for Viterbi Accelerator
    H[0] = taps[0];
    H[1] = taps[1];
    H[2] = taps[SPAL_DELAY*2];
    H[3] = taps[SPAL_DELAY*2+1];

    // Saturation on SYMB_SIZE bits signed
    for (i=0; i<4; i++)
    {
        H[i] = (H[i] > SYMB_MAX) ? SYMB_MAX : H[i];
        H[i] = (H[i] < SYMB_MIN) ? SYMB_MIN : H[i];
    }

    // backward H0 and HL
    // backward CIR: conj(h4,h3,h2,h1,h0)
    // tap value: h0,h1,h2,h3,h4
    equ_param.H0 = (UINT32)(((-H[3])<<16)|((0xFFFF)&H[2]));
    equ_param.HL = (UINT32)(((-H[1])<<16)|((0xFFFF)&H[0]));
    equ_param.bmSum     = (INT16*)SumTable;

    // Setup for first half-burst equalization (backward)
    if (WinType == GSM_WIN_TYPE_NBURST)
    {
        equ_param.length = 3 + NB_HPAYLD_SIZE + 1;
        equ_param.in     = (burst->samples +3 +NB_TS_START +SPAL_DELAY -1 -2 - offset);
        equ_param.out    =
            (UINT8*)UncachedBuffers->NbVitac.path_diff_bkw;
        path_diff_bkw    =
            (volatile UINT8*)UncachedBuffers->NbVitac.path_diff_bkw;
        path_diff_fwd    =
            (volatile UINT8*)UncachedBuffers->NbVitac.path_diff_fwd;
        equ_param.pm     =
            (UINT16*)UncachedBuffers->NbVitac.PathMetric;
        PathMetric       =
            (volatile UINT16*)UncachedBuffers->NbVitac.PathMetric;
        equ_param.first_state = g_sppNbBwequStartSt[burst->tsc];
    }
    else // SCH
    {
        equ_param.length = 3 + SCH_HPAYLD_SIZE;
        equ_param.in     = (burst->samples +3 +SCH_TS_START +SPAL_DELAY -1 -2);
        equ_param.out    =
            (UINT8*)UncachedBuffers->SchVitac.path_diff_bkw;
        path_diff_bkw    =
            (volatile UINT8*)UncachedBuffers->SchVitac.path_diff_bkw;
        path_diff_fwd    =
            (volatile UINT8*)UncachedBuffers->SchVitac.path_diff_fwd;
        equ_param.pm     =
            (UINT16*)UncachedBuffers->SchVitac.PathMetric;
        PathMetric       =
            (volatile UINT16*)UncachedBuffers->SchVitac.PathMetric;
        equ_param.first_state = SCH_BWEQU_START_ST;
    }

    // Run the viterbi for the backward part of the burst (1st half)
    spal_VitacEqualize(BACKWARD_TRELLIS, &equ_param);

    // Poll till end of processing
    while(spal_VitacBusy());
    //-------------------------------------------------------------------

    // ****************************************
    // Traceback Processing of backward Viterbi
    // ****************************************
    // Update node 0 for transition 62 with node 0 and 8
    // And get the first payload bit
    // Due to Vitac architecture (PM double buffering), the actual
    // PM position depends on nb_symbol parity.
    PathMetric = (equ_param.length & 0x1) ? PathMetric + VIT_NSTATES:
                 PathMetric;

    if (PathMetric[0]<PathMetric[8])
    {
        UINT16 diff;
        //diff = (PathMetric[8]-PathMetric[0]) >>  equ_param.shift_sb;
        diff = (PathMetric[8]-PathMetric[0]);
        if (equ_param.shift_sb<=4)
            diff <<= (4-equ_param.shift_sb);
        else
            diff >>= (equ_param.shift_sb-4);
        burst->sv[0]=((diff>SV_MAX)? SV_MAX: diff);
        survivor=0;
    }
    else
    {
        UINT16 diff;
        //diff = (PathMetric[0]-PathMetric[8]) >>  equ_param.shift_sb;
        diff = (PathMetric[0]-PathMetric[8]);
        if (equ_param.shift_sb<=4)
            diff <<= (4-equ_param.shift_sb);
        else
            diff >>= (equ_param.shift_sb-4);

        burst->sv[0]=0x80|((diff>SV_MAX)? SV_MAX: diff);
        survivor=8;
    }

    // ***************************************************
    // Launching HW Viterbi Trellis processing of the 2nd
    // burst during the Trace-back of the first half-burst
    // if we are not in EQU_HBURST_MODE
    // ***************************************************
    if (WinType == GSM_WIN_TYPE_NBURST)
    {
        // Setup for second half-burst equalization (forward)
        // No change needed for the other parameters
        equ_param.in     = (burst->samples +3 +NB_TS_START +NB_TS_SIZE -2 + offset);
        equ_param.out    = (UINT8 *)path_diff_fwd;
        equ_param.first_state = g_sppNbFwequStartSt[burst->tsc];
        equ_param.H0 = (UINT32)((H[1]<<16)|((0xFFFF)&H[0]));
        equ_param.HL = (UINT32)((H[3]<<16)|((0xFFFF)&H[2]));
        if (!EQU_HBURST_MODE)
        {
            // Run the viterbi now only in non-HalfBurst mode
            spal_VitacEqualize(FORWARD_TRELLIS, &equ_param);
        }
        // Start now even in EQU_BURST_MODE if the full Nburst
        // has been received
        else if (spp_FullBurstReady() && (WinType == GSM_WIN_TYPE_NBURST))
        {
            // Run DCOC and Derotation on remaining samples
            // (take into account offset induced by TOf)
            Dco.I=burst->dco.i;
            Dco.Q=burst->dco.q;
            spal_XcorDcocHburst((UINT32*)&(burst->samples[BB_BURST_HALF_SIZE-
                                           burst->samples_offset]),
                                BB_BURST_TOTAL_SIZE-BB_BURST_HALF_SIZE,
                                TRUE, // Derotation
                                BYPASS_DCOC,
                                Dco);
            // Run the viterbi (Setup for second half-burst
            // equalization has already been done)
            spal_VitacEqualize(FORWARD_TRELLIS, &equ_param);
            // Update "servicing" flags
            spp_OneFullBurstDone();
            fwd_vitac_done = TRUE;
        }
    }
    else // SCH
    {
        // SCH doesn't support HalfBurst mode
        equ_param.in     = (burst->samples +3 +SCH_TS_START +SCH_TS_SIZE -2);
        equ_param.out    = (UINT8 *)path_diff_fwd;
        equ_param.first_state = SCH_FWEQU_START_ST;
        // Run the viterbi
        spal_VitacEqualize(FORWARD_TRELLIS, &equ_param);
    }


    // ****************************************
    // Back to the trace-back of 1st half-burst
    // ****************************************
    //  Setup the traceback loop bounds
    //  Note that for NB stealing bits are handled separately
    if (WinType == GSM_WIN_TYPE_NBURST)
    {
        tb_start = (56 + 1) + SPAL_DELAY - 1; // +1 for steal bit shift
        tb_end   = ( 0 + 1) + SPAL_DELAY - 1;
        tb_size  = 2*57; // steal bits not accounted for
    }
    else //SCH
    {
        tb_start = 38 + SPAL_DELAY - 1;
        tb_end   =  0 + SPAL_DELAY - 1;
        tb_size  = 2*39;
    }

    // Move traceback pointer to beginning of first part of payload
    tb_wr_ptr = &(burst->sv[1]);

    // Get the remaining payload bits
    for (tb_idx=tb_start ; tb_idx>tb_end ; tb_idx--)
    {
        *tb_wr_ptr = (path_diff_bkw[VIT_NSTATES*(tb_idx) + survivor]);

        if (hard8(*tb_wr_ptr++))    survivor = survivor/2+(VIT_NSTATES/2);
        else                        survivor = survivor/2;
    }
    // Store the first stealing bit (hl) at the end of the burst
    if (WinType == GSM_WIN_TYPE_NBURST)
        burst->sv[114] = (path_diff_bkw[VIT_NSTATES*SPAL_DELAY+survivor]);
    //-------------------------------------------------------------------

    // In case of EQU_HBURST_MODE we need to wait for the samples of
    // the second half burst to be ready
    if (EQU_HBURST_MODE && (!fwd_vitac_done))
    {
        if (WinType == GSM_WIN_TYPE_NBURST)
        {
            // Wait for EndOfBurst Interrupt
            while(!spp_FullBurstReady());
            // Run DCOC and Derotation on remaining samples
            // (take into account offset induced by TOf)
            Dco.I=burst->dco.i;
            Dco.Q=burst->dco.q;
            spal_XcorDcocHburst((UINT32*)&(burst->samples[BB_BURST_HALF_SIZE-
                                           burst->samples_offset]),
                                BB_BURST_TOTAL_SIZE-BB_BURST_HALF_SIZE,
                                TRUE, // Derotation
                                BYPASS_DCOC,
                                Dco);

            // Run the viterbi (Setup for second half-burst
            // equalization has already been done)
            spal_VitacEqualize(FORWARD_TRELLIS, &equ_param);
            // Update "servicing" flag
            spp_OneFullBurstDone();
        }
    }

    // Poll till end of processing
    while(spal_VitacBusy());
    //-------------------------------------------------------------------

    // ****************************************
    // Traceback Processing of forward Viterbi
    // ****************************************
    // Update node 0 for transition 62 with node 0 and 8
    // And get first bit of payload
    if (PathMetric[0]<PathMetric[8])
    {
        UINT16 diff;

        //diff = (PathMetric[8]-PathMetric[0]) >>  equ_param.shift_sb;
        diff = (PathMetric[8]-PathMetric[0]);
        if (equ_param.shift_sb<=4)
            diff <<= (4-equ_param.shift_sb);
        else
            diff >>= (equ_param.shift_sb-4);
        burst->sv[tb_size-1]=((diff>SV_MAX)? SV_MAX: diff);
        survivor=0;
    }
    else
    {
        UINT16 diff;

        //diff = (PathMetric[0]-PathMetric[8]) >>  equ_param.shift_sb;
        diff = (PathMetric[0]-PathMetric[8]);
        if (equ_param.shift_sb<=4)
            diff <<= (4-equ_param.shift_sb);
        else
            diff >>= (equ_param.shift_sb-4);
        burst->sv[tb_size-1]=0x80|((diff>SV_MAX)? SV_MAX: diff);
        survivor=8;
    }

    // Move traceback pointer to the end of second half of payload
    tb_wr_ptr = &(burst->sv[tb_size - 2]);

    for (tb_idx=tb_start ; tb_idx>tb_end ; tb_idx--)
    {
        *tb_wr_ptr = (path_diff_fwd[VIT_NSTATES*tb_idx + survivor]);

        if (hard8(*tb_wr_ptr--))    survivor = survivor/2+(VIT_NSTATES/2);
        else                        survivor = survivor/2;
    }
    // Store the second stealing bit (hu) at the end of the burst
    if (WinType == GSM_WIN_TYPE_NBURST)
        burst->sv[115] = (path_diff_fwd[VIT_NSTATES*SPAL_DELAY+survivor]);

    //SPP_PROFILE_FUNCTION_EXIT(spp_Equalize);
}

//=============================================================================
// spp_EqualizeNBurst
//-----------------------------------------------------------------------------
// Performs normal burst equalization including:
// - DC offset estimation and correction,
// - channel estimation,
// - SNR estimation,
// - Viterbi equalization and traceback,
// - Frequency Offset estimation.
// .
//
// @param RxBuffer UINT32*. Address of the received samples.
// @param Tsc UINT8. Training sequence number.
// @param EqBufferOut UINT32*.
// @param Res SPP_N_BURST_RESULT_T*. Pointer to the burst equalization structure.
//
//=============================================================================
// this is a virtual 8taps version of spp_EqualizeNBurst,
// CHIP_CHEST_LEN==5, and CE module estimate 8taps,
// select (or not) largest 5taps for Eualization.
PUBLIC VOID spp_EqualizeNBurst_Prefilter_Ext_8809(UINT32* RxBuffer,
        UINT8 Tsc,
        UINT32* EqBufferOut,
        SPP_UNCACHED_BUFFERS_T* UncachedBuffers,
        SPP_N_BURST_RESULT_T* Res)
{
    UINT32 refBurst[142]; // stores reconstructed burst
    UINT32 prefltOut[74*2];// stores prefiltered data
    UINT32 tap8[8];
    INT16 i;
    INT16 Shft;
    UINT32 SignBits;
    UINT32 Msk;
    UINT8 LPValid;
    UINT32 LpChanExt[3];

    SPC_PROFILE_FUNC_ENTER(spp_Equalize);


    // TODO : see if this structure can be used one level higher
    // to avoid passing too much parameters and keep structured datas.
    SPP_EQU_BURST_T burst; // equalisation structure
    SPAL_DC_OFFSET_T Dco;

    //SPP_PROFILE_FUNCTION_ENTRY(spp_EqualizeNBurst);
    burst.tsc       = Tsc;
    burst.samples   = RxBuffer;
    burst.ref       = refBurst;
    burst.sv        = (UINT8*)EqBufferOut;

#if (GPRS_EQPREFLT_DEBUG)
    if  ( g_tcj_EqINIT== 0x00)
    {
        UINT32 *tmppt = MEM_ACCESS_UNCACHED(RxBuffer);
        UINT32 *tmpdes = MEM_ACCESS_UNCACHED(&g_tcj_EqBuf[0]);
        UINT16 kk;
        g_tcj_EqCnt = Tsc;
        for(kk=0; kk<156; kk++)
        {
            *tmpdes++ = *tmppt++;
        }
    }
#endif

    // DC Offset Correction
    // --------------------
    if (0)// bypass this branch to save patch space, EQU_HBURST_MODE never activated
    {
        burst.power = spal_XcorDcoc((UINT32*)RxBuffer,
                                    BB_BURST_HALF_SIZE,
                                    TRUE, // Derotation
                                    BYPASS_DCOC,
                                    &Dco);
        burst.dco.i=Dco.I;
        burst.dco.q=Dco.Q;
        burst.power /= BB_BURST_HALF_SIZE; // Normalised power
        Res->Pwr = spp_10log10(burst.power);
    }
    else
    {
#define DGC_PWR_TARGET          (1<<g_dgc_pwr_target_shift)
        UINT8 dgcShift = 0;
        INT16* dgcPt = (INT16*)RxBuffer;

        burst.power = spal_XcorDcoc((UINT32*)RxBuffer,
                                    BB_BURST_TOTAL_SIZE,
                                    TRUE, // Derotation
                                    BYPASS_DCOC,
                                    &Dco);
        burst.dco.i=Dco.I;
        burst.dco.q=Dco.Q;
        burst.power /= BB_BURST_TOTAL_SIZE; // Normalised power
        Res->Pwr = spp_10log10(burst.power);


        // auto gain control. for better prefilter fixed point calculation
        // NOTE: DGC_PWR_TARGET is related to agc table
        if (burst.power)
        {
            while(burst.power < DGC_PWR_TARGET)
            {
                burst.power <<= 2;
                dgcShift ++;
            }
//SPAL_BUFFER_PROFILE_WINDOW_ENTER_RAW(0x7d00|(dgcShift));
            if (dgcShift)
            {
                for (i=0; i<2*BB_BURST_TOTAL_SIZE; i++)
                {
                    (*dgcPt)<<=dgcShift;
                    dgcPt++;
                }
            }
//SPAL_BUFFER_PROFILE_WINDOW_EXIT_RAW(0x7d00|(dgcShift));
        }
    }


//SPAL_BUFFER_PROFILE_WINDOW_ENTER_RAW(0x7f01);
    // --------------------------
    // lse channel and prefilter estimation
    spp_GprsLsePrefilter_Ext_8809(&burst, UncachedBuffers, MEM_ACCESS_UNCACHED(&prefltOut[0]),tap8, LpChanExt, &LPValid);

//SPAL_BUFFER_PROFILE_WINDOW_EXIT_RAW(0x7f01);

    if (burst.power > burst.noise)
        Res->Snr =  spp_10log10(burst.power-burst.noise)-spp_10log10(burst.noise);
    else
        Res->Snr =- spp_10log10(burst.noise);



#if (GPRS_EQPREFLT_DEBUG)
    if  ( g_tcj_EqINIT== 0x00)
    {
        UINT32 *tmppt = MEM_ACCESS_UNCACHED(&UncachedBuffers->EgprsPrefilterBufs.RLS_W[0]);
        UINT32 *tmpdes = MEM_ACCESS_UNCACHED(&g_tcj_EqBuf[156]);
        UINT16 kk;
        for(kk=0; kk<8+18+18+8+8+10+11+10+15; kk++)
        {
            *tmpdes++ = *tmppt++;
        }
        tmppt = MEM_ACCESS_UNCACHED(&prefltOut[0]);
        for (kk=0; kk<144; kk++)
            *tmpdes++ = *tmppt++;

        for (kk=0; kk<19; kk++)
            *tmpdes++ = burst.ref[58+6+kk];
        *tmpdes++ =  ((Res->Snr<<16) | (burst.samples_offset));
    }

#endif

//SPAL_BUFFER_PROFILE_WINDOW_ENTER_RAW(0x7f02);
    // Equalize : Viterbi + traceback
    // ------------------------------
    // Only first 5 taps are used in spp_Euqalize
    // leave EgprsPrefilterBufs.InArray for lator ref rebuild
    if (LPValid)
    {
        burst.samples = MEM_ACCESS_UNCACHED(&prefltOut[0]);
        spp_Equalize_Prefilter_Ext_8809(GSM_WIN_TYPE_NBURST,2, &burst,
                                        (SPP_UNCACHED_BUFFERS_T*)&(UncachedBuffers->EgprsPrefilterBufs.RLS_W[0]));
        if (LPValid > 1)// LpChan's last 3taps large. 3 tap removal algo
        {
            spp_3tapRebuild_Prefilter_Ext_8809(&burst, LpChanExt, RxBuffer); burst.samples = RxBuffer;

#if (GPRS_EQPREFLT_DEBUG)
            if  ( g_tcj_EqINIT== 0x00)
            {
                UINT32 *tmppt = MEM_ACCESS_UNCACHED(EqBufferOut);
                UINT32 *tmpdes = MEM_ACCESS_UNCACHED(&g_tcj_EqBuf[156+8+18+18+8+8+10+11+10+15+144+19+1+116/4+142+4]);
                UINT16 kk;
                for(kk=0; kk<116/4; kk++)
                {
                    *tmpdes++ = *tmppt++;
                }
                tmppt = burst.samples;
                for(kk=0; kk<148; kk++)
                {
                    *tmpdes++ = *tmppt++;
                }
            }
#endif
            spp_Equalize_Prefilter_Ext_8809(GSM_WIN_TYPE_NBURST,2, &burst,
                                            (SPP_UNCACHED_BUFFERS_T*)&(UncachedBuffers->EgprsPrefilterBufs.RLS_W[0]));
#if 1
            // try 2nd 3tap remove process and re-eq
            burst.samples = MEM_ACCESS_UNCACHED(&prefltOut[0]);
            spp_3tapRebuild_Prefilter_Ext_8809(&burst, LpChanExt, RxBuffer); burst.samples = RxBuffer;
            spp_Equalize_Prefilter_Ext_8809(GSM_WIN_TYPE_NBURST,2, &burst,
                                            (SPP_UNCACHED_BUFFERS_T*)&(UncachedBuffers->EgprsPrefilterBufs.RLS_W[0]));
#if 1
            burst.samples = MEM_ACCESS_UNCACHED(&prefltOut[0]);
            spp_3tapRebuild_Prefilter_Ext_8809(&burst, LpChanExt, RxBuffer); burst.samples = RxBuffer;
            spp_Equalize_Prefilter_Ext_8809(GSM_WIN_TYPE_NBURST,2, &burst,
                                            (SPP_UNCACHED_BUFFERS_T*)&(UncachedBuffers->EgprsPrefilterBufs.RLS_W[0]));
#endif


#endif
        }

        burst.samples = (UINT32*)(&UncachedBuffers->EgprsPrefilterBufs.InArray[0]);
        spp_FofEstimation_Prefilter_Ext_8809(&burst, tap8);
    }
    else
    {
        spp_Equalize(GSM_WIN_TYPE_NBURST, &burst, ((SPP_UNCACHED_BUFFERS_T*)&(UncachedBuffers->EgprsPrefilterBufs.RLS_W[0])));
        spp_FofEstimation(GSM_WIN_TYPE_NBURST, &burst);
        // use the whole burst for snr re-calculation. burst.ref has been reconstruct in
        // spal_XcorSrec and spp_FofEstimation
        burst.noise = spal_XcorErrCalc(
                          (UINT32*)&(burst.samples[3]), // original, 3 tail bits removed
                          (UINT32*)&(burst.ref[0]), // reference
                          142); // Number of samples. 58+26+58

        // Normalise NoisePower and calculate Snr
        burst.noise /= 142;

    }

//SPAL_BUFFER_PROFILE_WINDOW_EXIT_RAW(0x7f02);

    if (burst.power > burst.noise)
        Res->Snr =  spp_10log10(burst.power-burst.noise)-spp_10log10(burst.noise);
    else
        Res->Snr =- spp_10log10(burst.noise);
#if (GPRS_EQPREFLT_DEBUG)
    if  ( g_tcj_EqINIT== 0x00)
    {
        UINT32 *tmppt = MEM_ACCESS_UNCACHED(EqBufferOut);
        UINT32 *tmpdes = MEM_ACCESS_UNCACHED(&g_tcj_EqBuf[156+8+18+18+8+8+10+11+10+15+144+19+1]);
        UINT16 kk;
        for(kk=0; kk<116/4; kk++)
        {
            *tmpdes++ = *tmppt++;
        }
        tmppt = MEM_ACCESS_UNCACHED(refBurst);
        for(kk=0; kk<142; kk++)
        {
            *tmpdes++ = *tmppt++;
        }
        *tmpdes++ = burst.power;
        *tmpdes++ = burst.noise;
        *tmpdes++ = Res->Snr;
        *tmpdes++ = burst.fof;
        while(1);
    }
#endif

    // Fill result structure
    //Res->Pwr = spp_10log10(burst.power);// this line moved tightly after burst.power calculated
    Res->TOf = burst.tof;
    Res->FOf = burst.fof;
    Res->Dco.i = burst.dco.i;
    Res->Dco.q = burst.dco.q;


    if (Res->Snr < EQ_SHIFT_MAX_NUM)
    {
        Shft = g_EqShiftTable_Ext_8809[(Res->Snr<0) ? 0: Res->Snr];

        switch(Shft)
        {
            case 1:
                Msk = 0x7E7E7E7E;
                break;
            case 2:
                Msk = 0x7C7C7C7C;
                break;
            case 3:
                Msk = 0x78787878;
                break;
            case 4:
                Msk = 0x70707070;
                break;
            case 5:
                Msk = 0x60606060;
                break;
            case 6:
                Msk = 0x40404040;
                break;
            default:
                Msk = 0x0;
                break;
        }
        if (Shft)
        {
            for (i=0; i<29; i++)
            {
                SignBits = (EqBufferOut[i] & 0x80808080);

                EqBufferOut[i] &= Msk;
                EqBufferOut[i] >>= Shft;
                EqBufferOut[i] |=SignBits;
            }
        }
    }

    if ((Res->Snr <= 0) && (g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.nBurst.bufIdx == 4))
    {
        for (i=0; i<29; i++)
        {
            EqBufferOut[i] &= 0x80808080;
        }
    }
    SPC_PROFILE_FUNC_ENTER(spp_Equalize);
}

#endif


//=============================================================================
// spc_CCHRxProcess(SPC_RX_LOCALS_T *spRx)
//-----------------------------------------------------------------------------
/// This function processes the Normal Burst Equalization for Control Channel
//=============================================================================
PUBLIC VOID spc_CCHRxProcess_Ext_8809(SPC_RX_LOCALS_T *spRx)
{
    UINT32 *ItlvOutput=(UINT32*)(g_spcCtx->Malloc) ;
    UINT32* SoftBitAddress=g_spcCtx->ItlvBufRxCtrl;
    UINT32 *EqualizerInput;
    UINT32 *EqualizerOutput;
    UINT8 BuffIdx = spRx->buffIdx+spRx->slotIdx;
    UINT8 i=0;

    //SPC_PROFILE_FUNCTION_ENTER(spc_CCHRxProcess);

    if (g_spcCtx->DedicatedActive==FALSE)
    {
        SoftBitAddress=g_spcCtx->ItlvBufRxCtrl+BuffIdx*SPC_ITLV_RX_BUFF_OFFSET;
    }
    //----------------------------- EQUALIZE  ---------------------------------
    EqualizerInput=(UINT32 *) &(g_spcStaticBufTab.RxIfcBuffer[spRx->slotIdx*
                                (BB_BURST_ACTIVE_SIZE+2*BB_RX_NBURST_MARGIN)]);
    ////////////////////////////////////////////////////////
    // dump the input samples if asked
    if ((g_spcCtx->dumpMask==SPC_DBG_DUMP_GSM_CCH_EQU_IN)&&
            (g_spcCtx->dumpPtr!=NULL))
    {
        ((VOID(*)(UINT16*,UINT16,UINT16))g_spcCtx->dumpPtr)(
            (UINT16*)EqualizerInput,
            312,
            SPC_DBG_DUMP_EQU_IN_MARKER);
    }
    ////////////////////////////////////////////////////////
    EqualizerOutput = (UINT32 *) (SoftBitAddress + spRx->burstIdx * 32);
#if (GPRS_CCHRX_DEBUG)
    if  ( g_tcj_CCHINIT== 0x00)
    {
        UINT32 *tmppt = MEM_ACCESS_UNCACHED(EqualizerInput);
        UINT32 *tmpdes = MEM_ACCESS_UNCACHED(&g_tcj_CCHBuf[spRx->burstIdx*156]);
        UINT16 kk;
        for(kk=0; kk<156; kk++)
        {
            //g_tcj_CCHBuf[kk + spRx->burstIdx*156] = *tmppt++;
            *tmpdes++ = *tmppt++;
        }
        g_tcj_CCHCnt = (spRx->slotIdx<<16) |(spRx->burstIdx);
        if (spRx->burstIdx == 3)
        {
            //*(UINT32*)0x0 = 0x1;
        }
    }

#endif


    if (g_mailbox_2ndEq_Flag == 2)
    {
        spp_EqualizeNBurst_Prefilter_Ext_8809(EqualizerInput,
                                              g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.nBurst.tsc,
                                              EqualizerOutput,
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


#if (GPRS_CCHRX_DEBUG)
    if  ( g_tcj_CCHINIT== 0x00)
    {
        UINT32 *tmppt = MEM_ACCESS_UNCACHED(EqualizerOutput);
        UINT16 kk;
        UINT32 *tmpdes ;//= MEM_ACCESS_UNCACHED(Eq2ndOut);
        for(kk=0; kk<32; kk++)
        {
            g_tcj_CCHBuf[156*4 + kk+spRx->burstIdx*32] = *tmppt++;
            //*tmpdes++ = *tmppt++;
        }

        tmpdes = MEM_ACCESS_UNCACHED(&g_tcj_CCHBuf[4*156+4*32+spRx->burstIdx*3]);
        *tmpdes++ = g_spcCtx->NBurst_result.Snr;
        *tmpdes++ = g_spcCtx->NBurst_result.Pwr;
        *tmpdes++ = g_spcCtx->NBurst_result.FOf;

        tmppt = MEM_ACCESS_UNCACHED(&g_tcj_CCHBuf[4*156+4*32+4*3+4*25]);
        tmpdes = MEM_ACCESS_UNCACHED(&g_tcj_CCHBuf[4*156+4*32+4*3+spRx->burstIdx*25]);
        for(kk=0; kk<25; kk++)
            *tmpdes++ = *tmppt++;
    }

#endif

    g_spcCtx->Rx_q_CCH[spRx->slotIdx][ spRx->burstIdx << 1     ] =
        ((EqualizerOutput[28]) >> 16) & 0x0FF;
    g_spcCtx->Rx_q_CCH[spRx->slotIdx][(spRx->burstIdx << 1) + 1] =
        ( EqualizerOutput[28]) >> 24;
    ////////////////////////////////////////////////////////
    // dump the output softbits if asked
    if ((g_spcCtx->dumpMask==SPC_DBG_DUMP_GSM_CCH_EQU_OUT)&&
            (g_spcCtx->dumpPtr!=NULL))
    {
        ((VOID(*)(UINT16*,UINT16,UINT16))g_spcCtx->dumpPtr)(
            (UINT16*)EqualizerOutput,
            58,
            SPC_DBG_DUMP_EQU_OUT_MARKER);
    }
    ////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////
    // check the dummy PCH burst detection is enabled
    if (g_spcCtx->statWinRxCfg[BuffIdx].itr&MBOX_DSP_PCH_READY)
    {
        UINT8 result=0;
        result = spp_DetectDummyPCH(EqualizerOutput,
                                    spRx->burstIdx,
                                    ((g_mailbox.dummyPchThresh)&0xFF00)>>8,
                                    (g_mailbox.dummyPchThresh)&0xFF);
        g_mailbox.dummyPchResult = (UINT32) result;
        spal_ComregsSetIrq(MBOX_DSP_PCH_READY);
    }
    ////////////////////////////////////////////////////////

    //----------------------------- EQUALIZE  ---------------------------------
    //----------------------------  RESULTS  ----------------------------------
    if (spRx->slotIdx==0)
    {
        g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.tof=
            (INT8)((int) g_spcCtx->NBurst_result.TOf - 4*BB_RX_NBURST_MARGIN);
        g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.fof=
            g_spcCtx->NBurst_result.FOf;
        g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.snr=
            (UINT8)(g_spcCtx->NBurst_result.Snr>=0)?g_spcCtx->NBurst_result.Snr:0;
        g_mailbox.spc2pal.win[g_spcCtx->currentSnap].rx.burstRes.pwr=
            g_spcCtx->NBurst_result.Pwr;
    }
    //----------------------------  RESULTS  ----------------------------------
    //---------------------------- CIPHERING ----------------------------------
    if (g_mailbox.pal2spc.statWin.ciphMode!=NO_CIPHER)
    {
        spp_CipherDecipherNb(EqualizerOutput,EqualizerOutput);
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


                ////////////////////////////////////////////////////////
                // dump the decoder input softbits if asked
                if ((g_spcCtx->dumpMask==SPC_DBG_DUMP_GSM_CCH_DEC_IN)&&
                        (g_spcCtx->dumpPtr!=NULL))
                {
                    ((VOID(*)(UINT16*,UINT16,UINT16))g_spcCtx->dumpPtr)(
                        (UINT16*)ItlvOutput,
                        228,
                        SPC_DBG_DUMP_DEC_IN_MARKER);
                }
                ////////////////////////////////////////////////////////
#if (GPRS_CCHRX_DEBUG)
                /*
                                 if  ( g_tcj_CCHINIT== 0x00)
                                 {
                                     UINT32 *tmppt = MEM_ACCESS_UNCACHED(ItlvOutput);
                                     UINT16 kk;
                                     UINT32 *tmpdes = MEM_ACCESS_UNCACHED(&g_tcj_CCHBuf[4*156+4*32+4*3]);
                                     for(kk=0; kk<128;kk++)
                                     {
                                         *tmpdes++ = *tmppt++;
                                     }
                                 }
                */
#endif


                spp_DecodeBlock((UINT32*)ItlvOutput,
                                spRx->Copy_ChMode,
                                0,
                                0,
                                spRx->BlockDecodedAdd,
                                (g_mailbox.spc2pal.statWin.rx.decodedUsf[k])&0x7,
                                (SPP_UNCACHED_BUFFERS_T*) g_spcCtx->Malloc,
                                &g_spcCtx->NBlock_result);
#if 1
                // CS4 will perform CRC error correction which give bfi==2 if correction succeeds.
                g_spcCtx->NBlock_result.Bfi &= 0x1;
#endif


#if (GPRS_CCHRX_DEBUG)
                if ( (g_spcCtx->Rx_CS[k] == GSM_CS3) && (g_mailbox_2ndEq_Flag>=0x10) )
                {
                    if (g_spcCtx->NBlock_result.Bfi==0)
                    {
                        if (ItlvOutput==NULL)
                        {
                            g_tcj_CCH2ndEqOK++;
                            if (g_tcj_CCHINIT == 0x5)
                            {
                                for (i=0; i<11; i++)
                                    g_tcj_CCHBuf[16+i] = spRx->BlockDecodedAdd[i];
                                //*(UINT32*)0x0 = 0x2;
                                g_tcj_CCHINIT = 0x06;
                            }
                        }
                        else
                        {
                            g_tcj_CCH1ndEqOK++;
                            if (g_tcj_CCHINIT == 0x5)
                            {
                                for (i=0; i<11; i++)
                                    g_tcj_CCHBuf[i] = spRx->BlockDecodedAdd[i];
                            }
                        }
                    }
                    else
                        g_tcj_CCHFail++;
                }
#endif


#if (GPRS_CCHRX_DEBUG)
                if  ( ( g_tcj_CCHINIT== 0x00)  && (g_tcj_CCHCnt == 3) )
                {
                    if ((g_spcCtx->NBlock_result.Bfi) &&(g_mailbox_2ndEq_Flag == 2))
                        g_tcj_CCHINIT = 0xF0|g_spcCtx->NBlock_result.Bfi;
                }
                else if (g_tcj_CCHINIT == 0x02)// external triger
                {
                    if (g_tcj_CCHCnt == 3) // g_tcj_CCHCnt == 3 is required, PCH may only dump data less than 4 bursts
                        g_tcj_CCHINIT = (0xF0|g_spcCtx->NBlock_result.Bfi);
                    else
                        g_tcj_CCHINIT = 0;
                }


#endif
                ////////////////////////////////////////////////////////
                // dump the decoder output bits if asked
                if ((g_spcCtx->dumpMask==SPC_DBG_DUMP_GSM_CCH_DEC_OUT)&&
                        (g_spcCtx->dumpPtr!=NULL))
                {
                    ((VOID(*)(UINT16*,UINT16,UINT16))g_spcCtx->dumpPtr)(
                        (UINT16*)spRx->BlockDecodedAdd,
                        28,
                        SPC_DBG_DUMP_DEC_OUT_MARKER);
                }
                ////////////////////////////////////////////////////////

                g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].meanBep =
                    g_spcCtx->NBlock_result.MeanBEP;
                g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].cvBep =
                    g_spcCtx->NBlock_result.CvBEP;
                g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].bitError =
                    g_spcCtx->NBlock_result.BitError;
                g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].bitTotal =
                    g_spcCtx->NBlock_result.BitTotal;
                g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].bfi =
                    g_spcCtx->NBlock_result.Bfi;
                g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].cs =
                    g_spcCtx->Rx_CS[k];
                g_mailbox.spc2pal.statWin.rx.nBlock[BuffIdx].sid =
                    g_spcCtx->NBlock_result.Voc_Sid |
                    (spRx->RxFrameType==SPP_RX_SID_UPDATE);


            }
            // reset the Rx Ctrl buffer to allow decoding with only two bursts
            for (i=0; i<4*32; i++) g_spcCtx->ItlvBufRxCtrl[i]=0;
        }
    }
    //----------------------------  DECODE  -----------------------------------
    //SPC_PROFILE_FUNCTION_EXIT(spc_CCHRxProcess);
}

#endif // 8809

