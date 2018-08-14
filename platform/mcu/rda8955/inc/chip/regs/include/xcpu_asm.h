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


#ifndef _XCPU_ASM_H_
#define _XCPU_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'xcpu'."
#endif



//==============================================================================
// xcpu
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_XCPU_BASE              0x01A2B000
#define REG_BCPU_BASE              0x0190A000

#define REG_XCPU_BASE_HI           BASE_HI(REG_XCPU_BASE)
#define REG_XCPU_BASE_LO           BASE_LO(REG_XCPU_BASE)

#define REG_XCPU_RF0_ADDR          REG_XCPU_BASE_LO + 0x00000004
#define REG_XCPU_CP0_ADR_BADVADDR  REG_XCPU_BASE_LO + 0x0000018C
#define REG_XCPU_CP0_CAUSE         REG_XCPU_BASE_LO + 0x00000190
#define REG_XCPU_CP0_STATUS        REG_XCPU_BASE_LO + 0x00000194
#define REG_XCPU_CP0_EPC           REG_XCPU_BASE_LO + 0x00000198
#define REG_XCPU_CP0_BADVADDR      REG_XCPU_BASE_LO + 0x000001B0
#define REG_XCPU_RF0_DATA          REG_XCPU_BASE_LO + 0x00000200
#define REG_XCPU_DC0_RESULT        REG_XCPU_BASE_LO + 0x00000218
#define REG_XCPU_REGFILE_AT        REG_XCPU_BASE_LO + 0x00000384
#define REG_XCPU_REGFILE_V0        REG_XCPU_BASE_LO + 0x00000388
#define REG_XCPU_REGFILE_V1        REG_XCPU_BASE_LO + 0x0000038C
#define REG_XCPU_REGFILE_A0        REG_XCPU_BASE_LO + 0x00000390
#define REG_XCPU_REGFILE_A1        REG_XCPU_BASE_LO + 0x00000394
#define REG_XCPU_REGFILE_A2        REG_XCPU_BASE_LO + 0x00000398
#define REG_XCPU_REGFILE_A3        REG_XCPU_BASE_LO + 0x0000039C
#define REG_XCPU_REGFILE_T0        REG_XCPU_BASE_LO + 0x000003A0
#define REG_XCPU_REGFILE_T1        REG_XCPU_BASE_LO + 0x000003A4
#define REG_XCPU_REGFILE_T2        REG_XCPU_BASE_LO + 0x000003A8
#define REG_XCPU_REGFILE_T3        REG_XCPU_BASE_LO + 0x000003AC
#define REG_XCPU_REGFILE_T4        REG_XCPU_BASE_LO + 0x000003B0
#define REG_XCPU_REGFILE_T5        REG_XCPU_BASE_LO + 0x000003B4
#define REG_XCPU_REGFILE_T6        REG_XCPU_BASE_LO + 0x000003B8
#define REG_XCPU_REGFILE_T7        REG_XCPU_BASE_LO + 0x000003BC
#define REG_XCPU_REGFILE_S0        REG_XCPU_BASE_LO + 0x000003C0
#define REG_XCPU_REGFILE_S1        REG_XCPU_BASE_LO + 0x000003C4
#define REG_XCPU_REGFILE_S2        REG_XCPU_BASE_LO + 0x000003C8
#define REG_XCPU_REGFILE_S3        REG_XCPU_BASE_LO + 0x000003CC
#define REG_XCPU_REGFILE_S4        REG_XCPU_BASE_LO + 0x000003D0
#define REG_XCPU_REGFILE_S5        REG_XCPU_BASE_LO + 0x000003D4
#define REG_XCPU_REGFILE_S6        REG_XCPU_BASE_LO + 0x000003D8
#define REG_XCPU_REGFILE_S7        REG_XCPU_BASE_LO + 0x000003DC
#define REG_XCPU_REGFILE_T8        REG_XCPU_BASE_LO + 0x000003E0
#define REG_XCPU_REGFILE_T9        REG_XCPU_BASE_LO + 0x000003E4
#define REG_XCPU_REGFILE_K0        REG_XCPU_BASE_LO + 0x000003E8
#define REG_XCPU_REGFILE_K1        REG_XCPU_BASE_LO + 0x000003EC
#define REG_XCPU_REGFILE_GP        REG_XCPU_BASE_LO + 0x000003F0
#define REG_XCPU_REGFILE_SP        REG_XCPU_BASE_LO + 0x000003F4
#define REG_XCPU_REGFILE_S8        REG_XCPU_BASE_LO + 0x000003F8
#define REG_XCPU_REGFILE_RA        REG_XCPU_BASE_LO + 0x000003FC
#define REG_XCPU_DEBUG_PAGE_ADDRESS REG_XCPU_BASE_LO + 0x00000440
#define REG_XCPU_CACHE_CONTROL     REG_XCPU_BASE_LO + 0x00000444

#define REG_BCPU_BASE_HI           BASE_HI(REG_BCPU_BASE)
#define REG_BCPU_BASE_LO           BASE_LO(REG_BCPU_BASE)

#define REG_BCPU_RF0_ADDR          REG_BCPU_BASE_LO + 0x00000004
#define REG_BCPU_CP0_ADR_BADVADDR  REG_BCPU_BASE_LO + 0x0000018C
#define REG_BCPU_CP0_CAUSE         REG_BCPU_BASE_LO + 0x00000190
#define REG_BCPU_CP0_STATUS        REG_BCPU_BASE_LO + 0x00000194
#define REG_BCPU_CP0_EPC           REG_BCPU_BASE_LO + 0x00000198
#define REG_BCPU_CP0_BADVADDR      REG_BCPU_BASE_LO + 0x000001B0
#define REG_BCPU_RF0_DATA          REG_BCPU_BASE_LO + 0x00000200
#define REG_BCPU_DC0_RESULT        REG_BCPU_BASE_LO + 0x00000218
#define REG_BCPU_REGFILE_AT        REG_BCPU_BASE_LO + 0x00000384
#define REG_BCPU_REGFILE_V0        REG_BCPU_BASE_LO + 0x00000388
#define REG_BCPU_REGFILE_V1        REG_BCPU_BASE_LO + 0x0000038C
#define REG_BCPU_REGFILE_A0        REG_BCPU_BASE_LO + 0x00000390
#define REG_BCPU_REGFILE_A1        REG_BCPU_BASE_LO + 0x00000394
#define REG_BCPU_REGFILE_A2        REG_BCPU_BASE_LO + 0x00000398
#define REG_BCPU_REGFILE_A3        REG_BCPU_BASE_LO + 0x0000039C
#define REG_BCPU_REGFILE_T0        REG_BCPU_BASE_LO + 0x000003A0
#define REG_BCPU_REGFILE_T1        REG_BCPU_BASE_LO + 0x000003A4
#define REG_BCPU_REGFILE_T2        REG_BCPU_BASE_LO + 0x000003A8
#define REG_BCPU_REGFILE_T3        REG_BCPU_BASE_LO + 0x000003AC
#define REG_BCPU_REGFILE_T4        REG_BCPU_BASE_LO + 0x000003B0
#define REG_BCPU_REGFILE_T5        REG_BCPU_BASE_LO + 0x000003B4
#define REG_BCPU_REGFILE_T6        REG_BCPU_BASE_LO + 0x000003B8
#define REG_BCPU_REGFILE_T7        REG_BCPU_BASE_LO + 0x000003BC
#define REG_BCPU_REGFILE_S0        REG_BCPU_BASE_LO + 0x000003C0
#define REG_BCPU_REGFILE_S1        REG_BCPU_BASE_LO + 0x000003C4
#define REG_BCPU_REGFILE_S2        REG_BCPU_BASE_LO + 0x000003C8
#define REG_BCPU_REGFILE_S3        REG_BCPU_BASE_LO + 0x000003CC
#define REG_BCPU_REGFILE_S4        REG_BCPU_BASE_LO + 0x000003D0
#define REG_BCPU_REGFILE_S5        REG_BCPU_BASE_LO + 0x000003D4
#define REG_BCPU_REGFILE_S6        REG_BCPU_BASE_LO + 0x000003D8
#define REG_BCPU_REGFILE_S7        REG_BCPU_BASE_LO + 0x000003DC
#define REG_BCPU_REGFILE_T8        REG_BCPU_BASE_LO + 0x000003E0
#define REG_BCPU_REGFILE_T9        REG_BCPU_BASE_LO + 0x000003E4
#define REG_BCPU_REGFILE_K0        REG_BCPU_BASE_LO + 0x000003E8
#define REG_BCPU_REGFILE_K1        REG_BCPU_BASE_LO + 0x000003EC
#define REG_BCPU_REGFILE_GP        REG_BCPU_BASE_LO + 0x000003F0
#define REG_BCPU_REGFILE_SP        REG_BCPU_BASE_LO + 0x000003F4
#define REG_BCPU_REGFILE_S8        REG_BCPU_BASE_LO + 0x000003F8
#define REG_BCPU_REGFILE_RA        REG_BCPU_BASE_LO + 0x000003FC
#define REG_BCPU_DEBUG_PAGE_ADDRESS REG_BCPU_BASE_LO + 0x00000440
#define REG_BCPU_CACHE_CONTROL     REG_BCPU_BASE_LO + 0x00000444

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
