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


#ifndef _XCPU_H_
#define _XCPU_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'xcpu'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// XCPU_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_XCPU_BASE              0x01A2B000
#define REG_BCPU_BASE              0x0190A000

typedef volatile struct
{
    REG32 Reserved_00000000;                    //0x00000000
    REG32                          rf0_addr;                     //0x00000004
    REG32 Reserved_00000008[97];                //0x00000008
    REG32                          cp0_adr_BadVAddr;             //0x0000018C
    REG32                          cp0_Cause;                    //0x00000190
    REG32                          cp0_Status;                   //0x00000194
    REG32                          cp0_EPC;                      //0x00000198
    REG32 Reserved_0000019C[5];                 //0x0000019C
    REG32                          cp0_BadVAddr;                 //0x000001B0
    REG32 Reserved_000001B4[19];                //0x000001B4
    REG32                          rf0_data;                     //0x00000200
    REG32 Reserved_00000204[5];                 //0x00000204
    REG32                          dc0_result;                   //0x00000218
    REG32 Reserved_0000021C[89];                //0x0000021C
    REG32 Reserved_00000380;                    //0x00000380
    REG32                          Regfile_AT;                   //0x00000384
    REG32                          Regfile_V0;                   //0x00000388
    REG32                          Regfile_V1;                   //0x0000038C
    REG32                          Regfile_A0;                   //0x00000390
    REG32                          Regfile_A1;                   //0x00000394
    REG32                          Regfile_A2;                   //0x00000398
    REG32                          Regfile_A3;                   //0x0000039C
    REG32                          Regfile_T0;                   //0x000003A0
    REG32                          Regfile_T1;                   //0x000003A4
    REG32                          Regfile_T2;                   //0x000003A8
    REG32                          Regfile_T3;                   //0x000003AC
    REG32                          Regfile_T4;                   //0x000003B0
    REG32                          Regfile_T5;                   //0x000003B4
    REG32                          Regfile_T6;                   //0x000003B8
    REG32                          Regfile_T7;                   //0x000003BC
    REG32                          Regfile_S0;                   //0x000003C0
    REG32                          Regfile_S1;                   //0x000003C4
    REG32                          Regfile_S2;                   //0x000003C8
    REG32                          Regfile_S3;                   //0x000003CC
    REG32                          Regfile_S4;                   //0x000003D0
    REG32                          Regfile_S5;                   //0x000003D4
    REG32                          Regfile_S6;                   //0x000003D8
    REG32                          Regfile_S7;                   //0x000003DC
    REG32                          Regfile_T8;                   //0x000003E0
    REG32                          Regfile_T9;                   //0x000003E4
    REG32                          Regfile_K0;                   //0x000003E8
    REG32                          Regfile_K1;                   //0x000003EC
    REG32                          Regfile_GP;                   //0x000003F0
    REG32                          Regfile_SP;                   //0x000003F4
    REG32                          Regfile_S8;                   //0x000003F8
    REG32                          Regfile_RA;                   //0x000003FC
    REG32 Reserved_00000400[16];                //0x00000400
    REG32                          Debug_Page_Address;           //0x00000440
    REG32                          Cache_Control;                //0x00000444
} HWP_XCPU_T;

#define hwp_xcpu                   ((HWP_XCPU_T*) KSEG1(REG_XCPU_BASE))
#define hwp_bcpu                   ((HWP_XCPU_T*) KSEG1(REG_BCPU_BASE))


//rf0_addr
#define XCPU_RF0_ADDR(n)           (((n)&0xFFFFFFFF)<<0)

//cp0_adr_BadVAddr
#define XCPU_CP0_ADR_BADVADDR(n)   (((n)&0xFFFFFFFF)<<0)

//cp0_Cause
#define XCPU_CP0_CAUSE_EXCCODE(n)  (((n)&15)<<2)
#define XCPU_CP0_CAUSE_EXCCODE_MASK (15<<2)
#define XCPU_CP0_CAUSE_EXCCODE_SHIFT (2)
#define XCPU_CP0_CAUSE_EXCCODE_INT (0<<2)
#define XCPU_CP0_CAUSE_EXCCODE_MOD (1<<2)
#define XCPU_CP0_CAUSE_EXCCODE_TLBL (2<<2)
#define XCPU_CP0_CAUSE_EXCCODE_TLBS (3<<2)
#define XCPU_CP0_CAUSE_EXCCODE_ADEL (4<<2)
#define XCPU_CP0_CAUSE_EXCCODE_ADES (5<<2)
#define XCPU_CP0_CAUSE_EXCCODE_IBE (6<<2)
#define XCPU_CP0_CAUSE_EXCCODE_DBE (7<<2)
#define XCPU_CP0_CAUSE_EXCCODE_SYS (8<<2)
#define XCPU_CP0_CAUSE_EXCCODE_PB  (9<<2)
#define XCPU_CP0_CAUSE_EXCCODE_RI  (10<<2)
#define XCPU_CP0_CAUSE_EXCCODE_CPU (11<<2)
#define XCPU_CP0_CAUSE_EXCCODE_OV  (12<<2)
#define XCPU_CP0_CAUSE_IP_SOFT(n)  (((n)&3)<<8)
#define XCPU_CP0_CAUSE_IP_SOFT_MASK (3<<8)
#define XCPU_CP0_CAUSE_IP_SOFT_SHIFT (8)
#define XCPU_CP0_CAUSE_IP_EXT(n)   (((n)&0x3F)<<10)
#define XCPU_CP0_CAUSE_IP_EXT_MASK (0x3F<<10)
#define XCPU_CP0_CAUSE_IP_EXT_SHIFT (10)
#define XCPU_CP0_CAUSE_CE(n)       (((n)&3)<<28)
#define XCPU_CP0_CAUSE_CE_MASK     (3<<28)
#define XCPU_CP0_CAUSE_CE_SHIFT    (28)
#define XCPU_CP0_CAUSE_BD_MASK     (1<<31)
#define XCPU_CP0_CAUSE_BD_SHIFT    (31)
#define XCPU_CP0_CAUSE_BD_NORMAL   (0<<31)
#define XCPU_CP0_CAUSE_BD_DELAY_SLOT (1<<31)
#define XCPU_CP0_CAUSE_IP(n)       (((n)&0xFF)<<8)
#define XCPU_CP0_CAUSE_IP_MASK     (0xFF<<8)
#define XCPU_CP0_CAUSE_IP_SHIFT    (8)

//cp0_Status
#define XCPU_CP0_STATUS_IEC_MASK   (1<<0)
#define XCPU_CP0_STATUS_IEC_SHIFT  (0)
#define XCPU_CP0_STATUS_IEC_DISABLE (0<<0)
#define XCPU_CP0_STATUS_IEC_ENABLE (1<<0)
#define XCPU_CP0_STATUS_KUC_MASK   (1<<1)
#define XCPU_CP0_STATUS_KUC_SHIFT  (1)
#define XCPU_CP0_STATUS_KUC_KERNEL (0<<1)
#define XCPU_CP0_STATUS_KUC_USER   (1<<1)
#define XCPU_CP0_STATUS_IEP_MASK   (1<<2)
#define XCPU_CP0_STATUS_IEP_SHIFT  (2)
#define XCPU_CP0_STATUS_IEP_DISABLE (0<<2)
#define XCPU_CP0_STATUS_IEP_ENABLE (1<<2)
#define XCPU_CP0_STATUS_KUP_MASK   (1<<3)
#define XCPU_CP0_STATUS_KUP_SHIFT  (3)
#define XCPU_CP0_STATUS_KUP_KERNEL (0<<3)
#define XCPU_CP0_STATUS_KUP_USER   (1<<3)
#define XCPU_CP0_STATUS_IEO_MASK   (1<<4)
#define XCPU_CP0_STATUS_IEO_SHIFT  (4)
#define XCPU_CP0_STATUS_IEO_DISABLE (0<<4)
#define XCPU_CP0_STATUS_IEO_ENABLE (1<<4)
#define XCPU_CP0_STATUS_KUO_MASK   (1<<5)
#define XCPU_CP0_STATUS_KUO_SHIFT  (5)
#define XCPU_CP0_STATUS_KUO_KERNEL (0<<5)
#define XCPU_CP0_STATUS_KUO_USER   (1<<5)
#define XCPU_CP0_STATUS_KUI        (1<<6)
#define XCPU_CP0_STATUS_INTMASK(n) (((n)&0xFF)<<8)
#define XCPU_CP0_STREAMING_DISABLE (1<<16)
#define XCPU_CP0_STATUS_CM         (1<<19)
#define XCPU_CP0_STATUS_TS         (1<<21)
#define XCPU_CP0_STATUS_BEV        (1<<22)
#define XCPU_CP0_STATUS_RE         (1<<25)
#define XCPU_CP0_STATUS_CU_0       (1<<28)
#define XCPU_CP0_STATUS_CU_1       (1<<29)
#define XCPU_CP0_STATUS_CU_2       (1<<30)
#define XCPU_CP0_STATUS_CU_3       (1<<31)
#define XCPU_CP0_STATUS_CU_123(n)  (((n)&7)<<29)
#define XCPU_CP0_STATUS_CU_123_MASK (7<<29)
#define XCPU_CP0_STATUS_CU_123_SHIFT (29)
#define XCPU_CP0_STATUS_CU(n)      (((n)&15)<<28)
#define XCPU_CP0_STATUS_CU_MASK    (15<<28)
#define XCPU_CP0_STATUS_CU_SHIFT   (28)

//cp0_EPC
#define XCPU_CP0_EPC(n)            (((n)&0xFFFFFFFF)<<0)

//cp0_BadVAddr
#define XCPU_CP0_BADVADDR(n)       (((n)&0xFFFFFFFF)<<0)

//rf0_data
#define XCPU_RF0_DATA(n)           (((n)&0xFFFFFFFF)<<0)

//dc0_result
#define XCPU_DC0_RESULT(n)         (((n)&0xFFFFFFFF)<<0)

//Regfile_AT
#define XCPU_AT(n)                 (((n)&0xFFFFFFFF)<<0)

//Regfile_V0
#define XCPU_V0(n)                 (((n)&0xFFFFFFFF)<<0)

//Regfile_V1
#define XCPU_V1(n)                 (((n)&0xFFFFFFFF)<<0)

//Regfile_A0
#define XCPU_A0(n)                 (((n)&0xFFFFFFFF)<<0)

//Regfile_A1
#define XCPU_A1(n)                 (((n)&0xFFFFFFFF)<<0)

//Regfile_A2
#define XCPU_A2(n)                 (((n)&0xFFFFFFFF)<<0)

//Regfile_A3
#define XCPU_A3(n)                 (((n)&0xFFFFFFFF)<<0)

//Regfile_T0
#define XCPU_T0(n)                 (((n)&0xFFFFFFFF)<<0)

//Regfile_T1
#define XCPU_T1(n)                 (((n)&0xFFFFFFFF)<<0)

//Regfile_T2
#define XCPU_T2(n)                 (((n)&0xFFFFFFFF)<<0)

//Regfile_T3
#define XCPU_T3(n)                 (((n)&0xFFFFFFFF)<<0)

//Regfile_T4
#define XCPU_T4(n)                 (((n)&0xFFFFFFFF)<<0)

//Regfile_T5
#define XCPU_T5(n)                 (((n)&0xFFFFFFFF)<<0)

//Regfile_T6
#define XCPU_T6(n)                 (((n)&0xFFFFFFFF)<<0)

//Regfile_T7
#define XCPU_T7(n)                 (((n)&0xFFFFFFFF)<<0)

//Regfile_S0
#define XCPU_S0(n)                 (((n)&0xFFFFFFFF)<<0)

//Regfile_S1
#define XCPU_S1(n)                 (((n)&0xFFFFFFFF)<<0)

//Regfile_S2
#define XCPU_S2(n)                 (((n)&0xFFFFFFFF)<<0)

//Regfile_S3
#define XCPU_S3(n)                 (((n)&0xFFFFFFFF)<<0)

//Regfile_S4
#define XCPU_S4(n)                 (((n)&0xFFFFFFFF)<<0)

//Regfile_S5
#define XCPU_S5(n)                 (((n)&0xFFFFFFFF)<<0)

//Regfile_S6
#define XCPU_S6(n)                 (((n)&0xFFFFFFFF)<<0)

//Regfile_S7
#define XCPU_S7(n)                 (((n)&0xFFFFFFFF)<<0)

//Regfile_T8
#define XCPU_T8(n)                 (((n)&0xFFFFFFFF)<<0)

//Regfile_T9
#define XCPU_T9(n)                 (((n)&0xFFFFFFFF)<<0)

//Regfile_K0
#define XCPU_K0(n)                 (((n)&0xFFFFFFFF)<<0)

//Regfile_K1
#define XCPU_K1(n)                 (((n)&0xFFFFFFFF)<<0)

//Regfile_GP
#define XCPU_GP(n)                 (((n)&0xFFFFFFFF)<<0)

//Regfile_SP
#define XCPU_SP(n)                 (((n)&0xFFFFFFFF)<<0)

//Regfile_S8
#define XCPU_S8(n)                 (((n)&0xFFFFFFFF)<<0)

//Regfile_RA
#define XCPU_RA(n)                 (((n)&0xFFFFFFFF)<<0)

//Debug_Page_Address
#define XCPU_DEBUG_PAGE_ADDRESS(n) (((n)&15)<<0)
#define XCPU_DEBUG_PAGE_ADDRESS_MASK (15<<0)
#define XCPU_DEBUG_PAGE_ADDRESS_SHIFT (0)
#define XCPU_DEBUG_PAGE_ADDRESS_ITAG (0<<0)
#define XCPU_DEBUG_PAGE_ADDRESS_DTAG (8<<0)

//Cache_Control
#define XCPU_DCACHE_INHIBIT        (1<<0)
#define XCPU_ICACHE_INHIBIT        (1<<1)
#define XCPU_CACHE_HIT_DISABLE     (1<<2)





#endif

