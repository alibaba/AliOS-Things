/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#ifndef __REG_USBPHY_BEST2001_H__
#define __REG_USBPHY_BEST2001_H__

#include "plat_types.h"

// REG_00
#define REVID_SHIFT                         0
#define REVID_MASK                          (0xF << REVID_SHIFT)
#define REVID(n)                            BITFIELD_VAL(REVID, n)
#define CHIPID_SHIFT                        4
#define CHIPID_MASK                         (0xFFF << CHIPID_SHIFT)
#define CHIPID(n)                           BITFIELD_VAL(CHIPID, n)

// REG_01
#define CFG_ANAPHY_RESETN                   (1 << 0)
#define CFG_CKCDR_EN                        (1 << 1)
#define CFG_CKPLL_EN                        (1 << 2)
#define CFG_EN_CLKMAC                       (1 << 3)
#define CFG_POL_CDRCLK                      (1 << 4)
#define CFG_RESETN_MAC                      (1 << 5)
#define CFG_RESETNCDR                       (1 << 6)
#define CFG_RESETNPLL                       (1 << 7)
#define CFG_BYPASS_CDR_ENDRESETN            (1 << 8)
#define CFG_INTR_EN_ALL                     (1 << 9)
#define CFG_LOW_SPEED_MODE                  (1 << 10)
#define CFG_POL_CLKPLL                      (1 << 11)
#define CFG_BYPASS_RDY                      (1 << 12)
#define CFG_MODE_HS_LINES_TX                (1 << 13)
#define CFG_MODE_FS_LINES_TX                (1 << 14)
#define CFG_ENUM_MODE                       (1 << 15)

// REG_02
#define CFG_TARGET_SYNC_TOUT_SHIFT          0
#define CFG_TARGET_SYNC_TOUT_MASK           (0xF << CFG_TARGET_SYNC_TOUT_SHIFT)
#define CFG_TARGET_SYNC_TOUT(n)             BITFIELD_VAL(CFG_TARGET_SYNC_TOUT, n)
#define CFG_SEL_SYNCPATTERN_SHIFT           4
#define CFG_SEL_SYNCPATTERN_MASK            (0x7 << CFG_SEL_SYNCPATTERN_SHIFT)
#define CFG_SEL_SYNCPATTERN(n)              BITFIELD_VAL(CFG_SEL_SYNCPATTERN, n)
#define CFG_BYPASS_SQL_VALID                (1 << 7)
#define CFG_EN_SYNCTOUT                     (1 << 8)
#define CFG_FORCERX                         (1 << 9)
#define CFG_RXRESET                         (1 << 10)
#define CFG_EBUF_THRD_SHIFT                 11
#define CFG_EBUF_THRD_MASK                  (0x1F << CFG_EBUF_THRD_SHIFT)
#define CFG_EBUF_THRD(n)                    BITFIELD_VAL(CFG_EBUF_THRD, n)

// REG_03
#define CFG_TARGET_TAIL_SHIFT               0
#define CFG_TARGET_TAIL_MASK                (0xFF << CFG_TARGET_TAIL_SHIFT)
#define CFG_TARGET_TAIL(n)                  BITFIELD_VAL(CFG_TARGET_TAIL, n)
#define CFG_TXPATTERN_SHIFT                 8
#define CFG_TXPATTERN_MASK                  (0xFF << CFG_TXPATTERN_SHIFT)
#define CFG_TXPATTERN(n)                    BITFIELD_VAL(CFG_TXPATTERN, n)

// REG_04
#define CFG_EN_HSTSOF                       (1 << 0)
#define CFG_FORCETX                         (1 << 1)
#define CFG_MODE_BITEN                      (1 << 2)
#define CFG_TXSTATE_RESET                   (1 << 3)
#define CFG_EMPTY_DLY_SEL_SHIFT             4
#define CFG_EMPTY_DLY_SEL_MASK              (0x7 << CFG_EMPTY_DLY_SEL_SHIFT)
#define CFG_EMPTY_DLY_SEL(n)                BITFIELD_VAL(CFG_EMPTY_DLY_SEL, n)
#define CFG_TP_SEL_SHIFT                    8
#define CFG_TP_SEL_MASK                     (0xFF << CFG_TP_SEL_SHIFT)
#define CFG_TP_SEL(n)                       BITFIELD_VAL(CFG_TP_SEL, n)

// REG_05
#define CFG_ADP_PRB                         (1 << 0)
#define CFG_ADP_SNS                         (1 << 1)
#define CFG_RID_A                           (1 << 2)
#define CFG_RID_B                           (1 << 3)
#define CFG_RID_C                           (1 << 4)
#define CFG_RID_FLOAT                       (1 << 5)
#define CFG_RID_GND                         (1 << 6)
#define CFG_UTMIOTG_AVALID                  (1 << 7)
#define CFG_UTMISRP_BVALID                  (1 << 8)
#define CFG_UTMIOTG_VBUSVALID               (1 << 9)
#define CFG_UTMISRP_SESSEND                 (1 << 10)
#define CFG_EOP_MODE_RX                     (1 << 11)
#define CFG_SEL_HS_LSTATE_REG               (1 << 12)
#define CFG_FS_DRV_SEL_SHIFT                13
#define CFG_FS_DRV_SEL_MASK                 (0x7 << CFG_FS_DRV_SEL_SHIFT)
#define CFG_FS_DRV_SEL(n)                   BITFIELD_VAL(CFG_FS_DRV_SEL, n)

// REG_06
#define CFG_RTERM_HS_REG_SHIFT              0
#define CFG_RTERM_HS_REG_MASK               (0xF << CFG_RTERM_HS_REG_SHIFT)
#define CFG_RTERM_HS_REG(n)                 BITFIELD_VAL(CFG_RTERM_HS_REG, n)
#define CFG_HS_IBIAS_TRIM_REG_SHIFT         4
#define CFG_HS_IBIAS_TRIM_REG_MASK          (0xF << CFG_HS_IBIAS_TRIM_REG_SHIFT)
#define CFG_HS_IBIAS_TRIM_REG(n)            BITFIELD_VAL(CFG_HS_IBIAS_TRIM_REG, n)
#define CFG_HS_SL_REG                       (1 << 8)
#define CFG_DICCON_VTHSEL_REG_SHIFT         9
#define CFG_DICCON_VTHSEL_REG_MASK          (0x7 << CFG_DICCON_VTHSEL_REG_SHIFT)
#define CFG_DICCON_VTHSEL_REG(n)            BITFIELD_VAL(CFG_DICCON_VTHSEL_REG, n)
#define CFG_HS_SRC_SEL_REG                  (1 << 12)
#define CFG_EN_HS_S2P_REG                   (1 << 13)
#define CFG_HS_EN_IOFF_REG                  (1 << 14)
#define CFG_SEL_HS_GAIN_REG                 (1 << 15)

// REG_07
#define CFG_FS_RPD_SEL_REG_SHIFT            0
#define CFG_FS_RPD_SEL_REG_MASK             (0xF << CFG_FS_RPD_SEL_REG_SHIFT)
#define CFG_FS_RPD_SEL_REG(n)               BITFIELD_VAL(CFG_FS_RPD_SEL_REG, n)
#define CFG_FS_RPU_SEL_REG_SHIFT            4
#define CFG_FS_RPU_SEL_REG_MASK             (0xF << CFG_FS_RPU_SEL_REG_SHIFT)
#define CFG_FS_RPU_SEL_REG(n)               BITFIELD_VAL(CFG_FS_RPU_SEL_REG, n)
#define CFG_FS_CDR_BYPASS_REG               (1 << 8)
#define CFG_FS_DRV_SL_REG                   (1 << 9)
#define CFG_FS_EDGE_SEL_IN_REG              (1 << 10)
#define CFG_DTESTEN1_FS_IN_REG              (1 << 11)
#define CFG_DTESTEN2_FS_IN_REG              (1 << 12)
#define CFG_DTEST_SEL_FS_IN_REG_SHIFT       13
#define CFG_DTEST_SEL_FS_IN_REG_MASK        (0x3 << CFG_DTEST_SEL_FS_IN_REG_SHIFT)
#define CFG_DTEST_SEL_FS_IN_REG(n)          BITFIELD_VAL(CFG_DTEST_SEL_FS_IN_REG, n)
#define CFG_MODE_LINESTATE                  (1 << 15)

// REG_08
#define CFG_RES_GN_RCV_REG_SHIFT            0
#define CFG_RES_GN_RCV_REG_MASK             (0xF << CFG_RES_GN_RCV_REG_SHIFT)
#define CFG_RES_GN_RCV_REG(n)               BITFIELD_VAL(CFG_RES_GN_RCV_REG, n)
#define CFG_I_RCV_REG_SHIFT                 4
#define CFG_I_RCV_REG_MASK                  (0x3 << CFG_I_RCV_REG_SHIFT)
#define CFG_I_RCV_REG(n)                    BITFIELD_VAL(CFG_I_RCV_REG, n)
#define CFG_PI_GN_REG_SHIFT                 6
#define CFG_PI_GN_REG_MASK                  (0x3 << CFG_PI_GN_REG_SHIFT)
#define CFG_PI_GN_REG(n)                    BITFIELD_VAL(CFG_PI_GN_REG, n)
#define CFG_ISEL_SQ_REG_SHIFT               8
#define CFG_ISEL_SQ_REG_MASK                (0x7 << CFG_ISEL_SQ_REG_SHIFT)
#define CFG_ISEL_SQ_REG(n)                  BITFIELD_VAL(CFG_ISEL_SQ_REG, n)
#define CFG_DISCON_DET_EN                   (1 << 11)
#define CFG_BYP_SUSPENDM_REG                (1 << 12)
#define CFG_CDR_GN_REG_SHIFT                13
#define CFG_CDR_GN_REG_MASK                 (0x3 << CFG_CDR_GN_REG_SHIFT)
#define CFG_CDR_GN_REG(n)                   BITFIELD_VAL(CFG_CDR_GN_REG, n)
#define CFG_EN_HOLD_LAST_REG                (1 << 15)

// REG_09
#define CFG_EN_ZPS_REG                      (1 << 0)
#define CFG_HYS_EN_IN_REG                   (1 << 1)
#define CFG_LOOP_BACK_REG                   (1 << 2)
#define CFG_SEL_TERR_REG                    (1 << 3)
#define CFG_ATEST_EN_DISCON_REG             (1 << 4)
#define CFG_ATEST_SELX_DISCON_REG           (1 << 5)
#define CFG_ATEST_EN_REG                    (1 << 6)
#define CFG_ATEST_SELX_REG_SHIFT            7
#define CFG_ATEST_SELX_REG_MASK             (0x3 << CFG_ATEST_SELX_REG_SHIFT)
#define CFG_ATEST_SELX_REG(n)               BITFIELD_VAL(CFG_ATEST_SELX_REG, n)
#define CFG_DTESTEN_TX_REG                  (1 << 9)
#define CFG_DTEST_SEL_TX_REG_SHIFT          10
#define CFG_DTEST_SEL_TX_REG_MASK           (0x3 << CFG_DTEST_SEL_TX_REG_SHIFT)
#define CFG_DTEST_SEL_TX_REG(n)             BITFIELD_VAL(CFG_DTEST_SEL_TX_REG, n)
#define CFG_DTESTEN1_REG                    (1 << 12)
#define CFG_DTESTEN2_REG                    (1 << 13)
#define CFG_DTESTEN3_REG                    (1 << 14)

// REG_0A
#define CFG_DR_HOSTMODE                     (1 << 0)
#define CFG_DR_FSLS_SEL                     (1 << 1)
#define CFG_DR_SDATA                        (1 << 2)
#define CFG_DR_TERMSEL                      (1 << 3)
#define CFG_DR_XCVRSEL                      (1 << 4)
#define CFG_DR_SON                          (1 << 5)
#define CFG_DR_OPMODE                       (1 << 6)
#define CFG_DR_HS_TXEN                      (1 << 8)
#define CFG_DR_FSTXEN                       (1 << 9)
#define CFG_DR_FSTXD                        (1 << 10)
#define CFG_DR_FSTXDB                       (1 << 11)
#define CFG_EN_RST_INTP                     (1 << 12)
#define CFG_BISTEN                          (1 << 13)
#define CFG_DTEST_SEL_REG_SHIFT             14
#define CFG_DTEST_SEL_REG_MASK              (0x3 << CFG_DTEST_SEL_REG_SHIFT)
#define CFG_DTEST_SEL_REG(n)                BITFIELD_VAL(CFG_DTEST_SEL_REG, n)

// REG_0B
#define CFG_REG_HOSTMODE                    (1 << 0)
#define CFG_REG_FSLS_SEL                    (1 << 1)
#define CFG_REG_SDATA                       (1 << 2)
#define CFG_REG_TERM                        (1 << 3)
#define CFG_REG_XCVRSEL                     (1 << 4)
#define CFG_REG_SON                         (1 << 5)
#define CFG_REG_OPMODE_SHIFT                6
#define CFG_REG_OPMODE_MASK                 (0x3 << CFG_REG_OPMODE_SHIFT)
#define CFG_REG_OPMODE(n)                   BITFIELD_VAL(CFG_REG_OPMODE, n)
#define CFG_REG_HS_TXEN                     (1 << 8)
#define CFG_REG_FSTXEN                      (1 << 9)
#define CFG_REG_FSTXD                       (1 << 10)
#define CFG_REG_FSTXDB                      (1 << 11)

// REG_0C
#define CFG_RXINTR_MSK_SHIFT                0
#define CFG_RXINTR_MSK_MASK                 (0xFFFF << CFG_RXINTR_MSK_SHIFT)
#define CFG_RXINTR_MSK(n)                   BITFIELD_VAL(CFG_RXINTR_MSK, n)

// REG_0D
#define CFG_HSHOST_DISC_MSK                 (1 << 0)

// REG_0E
#define RB_RXERRSTATUS_RAW_SHIFT            0
#define RB_RXERRSTATUS_RAW_MASK             (0xFFFF << RB_RXERRSTATUS_RAW_SHIFT)
#define RB_RXERRSTATUS_RAW(n)               BITFIELD_VAL(RB_RXERRSTATUS_RAW, n)

// REG_0F
#define RB_RXERRSTATUS_MSKD_SHIFT           0
#define RB_RXERRSTATUS_MSKD_MASK            (0xFFFF << RB_RXERRSTATUS_MSKD_SHIFT)
#define RB_RXERRSTATUS_MSKD(n)              BITFIELD_VAL(RB_RXERRSTATUS_MSKD, n)

// REG_10
#define RB_HSHOST_DISC_RAW                  (1 << 0)

// REG_11
#define RB_HSHOST_DISC_MSKED                (1 << 0)
#define CFG_RXP_DATA_SHIFT                  1
#define CFG_RXP_DATA_MASK                   (0xFF << CFG_RXP_DATA_SHIFT)
#define CFG_RXP_DATA(n)                     BITFIELD_VAL(CFG_RXP_DATA, n)

// REG_12
#define CFG_EN_SETERR_RX_SHIFT              0
#define CFG_EN_SETERR_RX_MASK               (0xFFFF << CFG_EN_SETERR_RX_SHIFT)
#define CFG_EN_SETERR_RX(n)                 BITFIELD_VAL(CFG_EN_SETERR_RX, n)

// REG_13
#define CFG_TXSTART_DLY_SEL_SHIFT           0
#define CFG_TXSTART_DLY_SEL_MASK            (0x7 << CFG_TXSTART_DLY_SEL_SHIFT)
#define CFG_TXSTART_DLY_SEL(n)              BITFIELD_VAL(CFG_TXSTART_DLY_SEL, n)
#define CFG_PEBUF_THRD_SHIFT                3
#define CFG_PEBUF_THRD_MASK                 (0xF << CFG_PEBUF_THRD_SHIFT)
#define CFG_PEBUF_THRD(n)                   BITFIELD_VAL(CFG_PEBUF_THRD, n)
#define CFG_TXP_DATA_SHIFT                  7
#define CFG_TXP_DATA_MASK                   (0xFF << CFG_TXP_DATA_SHIFT)
#define CFG_TXP_DATA(n)                     BITFIELD_VAL(CFG_TXP_DATA, n)

// REG_14
#define CFG_DIG_SLOOP                       (1 << 0)
#define CFG_DIG_PLOOP                       (1 << 1)
#define CFG_RX_P_SEL                        (1 << 2)
#define CFG_HS_TEST                         (1 << 3)
#define CFG_FS_TEST                         (1 << 4)
#define CFG_PHY_TEST_GOON                   (1 << 5)
#define CFG_RESETN_HSTXP                    (1 << 7)
#define CFG_RESETN_HSRXP                    (1 << 8)
#define CFG_POL_HSTXP                       (1 << 9)
#define CFG_POL_HSRXP                       (1 << 10)
#define CFG_TXP_MODE                        (1 << 11)

// REG_15
#define CFG_HS_PDATA_SHIFT                  0
#define CFG_HS_PDATA_MASK                   (0xFF << CFG_HS_PDATA_SHIFT)
#define CFG_HS_PDATA(n)                     BITFIELD_VAL(CFG_HS_PDATA, n)
#define CFG_HS_P_ON_SHIFT                   8
#define CFG_HS_P_ON_MASK                    (0xFF << CFG_HS_P_ON_SHIFT)
#define CFG_HS_P_ON(n)                      BITFIELD_VAL(CFG_HS_P_ON, n)

// REG_16
#define RESERVED_16_15_0_SHIFT              0
#define RESERVED_16_15_0_MASK               (0xFFFF << RESERVED_16_15_0_SHIFT)
#define RESERVED_16_15_0(n)                 BITFIELD_VAL(RESERVED_16_15_0, n)

// REG_17
#define RESERVED_17_15_0_SHIFT              0
#define RESERVED_17_15_0_MASK               (0xFFFF << RESERVED_17_15_0_SHIFT)
#define RESERVED_17_15_0(n)                 BITFIELD_VAL(RESERVED_17_15_0, n)

// REG_18
#define DIG_PU_USBPLL                       (1 << 0)
#define USB_SEL_480M                        (1 << 1)
#define USB_CLK960M_SEL                     (1 << 2)
#define REG_USBPLL_LDO_BYPASS               (1 << 4)
#define REG_USBPLL_REFCLK_SEL               (1 << 5)
#define DIG_USBPLL_USB_EN                   (1 << 6)
#define DIG_USBPLL_BB_AD_SEL                (1 << 7)
#define DIG_USBPLL_BT_CLK_EN                (1 << 8)
#define DIG_USBPLL_FANA960M_SEL             (1 << 9)
#define REG_USBPLL_2PATH_EN                 (1 << 10)
#define DIG_USBPLL_CODEC_CLKDIV2_EN         (1 << 11)
#define REG_USBPLL_MDLL_RESETB_DR           (1 << 12)
#define REG_USBPLL_CPMODE                   (1 << 13)
#define REG_USBPLL_MDLL_RESETB              (1 << 14)
#define DIG_USBPLL_MDLL_CALEN               (1 << 15)

// REG_19
#define REG_USBPLL_TSTBUF_PLUS              (1 << 0)
#define DIG_USBPLL_FANA960M_DIV2_EN         (1 << 1)
#define DIG_USBPLL_MDLL_CALCLKEN            (1 << 2)
#define REG_USBPLL_LDO_PRE_CHARGE           (1 << 3)
#define DIG_USBPLL_LCKDET_BYPS              (1 << 4)
#define DIG_USBPLL_SDM_CLK_SEL              (1 << 5)
#define REG_USBPLL_PFD_RESETB               (1 << 6)
#define REG_USBPLL_TST_EN                   (1 << 7)
#define TST_USBPLL_VCTRL_COMP               (1 << 8)
#define REG_USBPLL_UP_TIMER_SHIFT           9
#define REG_USBPLL_UP_TIMER_MASK            (0x1F << REG_USBPLL_UP_TIMER_SHIFT)
#define REG_USBPLL_UP_TIMER(n)              BITFIELD_VAL(REG_USBPLL_UP_TIMER, n)

// REG_1A
#define REG_USBPLL_IOFST_SHIFT              0
#define REG_USBPLL_IOFST_MASK               (0xF << REG_USBPLL_IOFST_SHIFT)
#define REG_USBPLL_IOFST(n)                 BITFIELD_VAL(REG_USBPLL_IOFST, n)
#define REG_USBPLL_LDO_PFD_SHIFT            4
#define REG_USBPLL_LDO_PFD_MASK             (0x7 << REG_USBPLL_LDO_PFD_SHIFT)
#define REG_USBPLL_LDO_PFD(n)               BITFIELD_VAL(REG_USBPLL_LDO_PFD, n)
#define REG_USBPLL_VCAP_SHIFT               7
#define REG_USBPLL_VCAP_MASK                (0x7 << REG_USBPLL_VCAP_SHIFT)
#define REG_USBPLL_VCAP(n)                  BITFIELD_VAL(REG_USBPLL_VCAP, n)
#define REG_USBPLL_LDO_FBRES_SHIFT          10
#define REG_USBPLL_LDO_FBRES_MASK           (0x7 << REG_USBPLL_LDO_FBRES_SHIFT)
#define REG_USBPLL_LDO_FBRES(n)             BITFIELD_VAL(REG_USBPLL_LDO_FBRES, n)
#define REG_USBPLL_LDO_DIG_SHIFT            13
#define REG_USBPLL_LDO_DIG_MASK             (0x7 << REG_USBPLL_LDO_DIG_SHIFT)
#define REG_USBPLL_LDO_DIG(n)               BITFIELD_VAL(REG_USBPLL_LDO_DIG, n)

// REG_1B
#define REG_USBPLL_BW_SEL_SHIFT             0
#define REG_USBPLL_BW_SEL_MASK              (0xF << REG_USBPLL_BW_SEL_SHIFT)
#define REG_USBPLL_BW_SEL(n)                BITFIELD_VAL(REG_USBPLL_BW_SEL, n)
#define DIG_USBPLL_POST_DIVN_SHIFT          4
#define DIG_USBPLL_POST_DIVN_MASK           (0x3F << DIG_USBPLL_POST_DIVN_SHIFT)
#define DIG_USBPLL_POST_DIVN(n)             BITFIELD_VAL(DIG_USBPLL_POST_DIVN, n)
#define DIG_USBPLL_DIVN_480M_SHIFT          10
#define DIG_USBPLL_DIVN_480M_MASK           (0x7 << DIG_USBPLL_DIVN_480M_SHIFT)
#define DIG_USBPLL_DIVN_480M(n)             BITFIELD_VAL(DIG_USBPLL_DIVN_480M, n)
#define REG_USBPLL_LDO_CP_SHIFT             13
#define REG_USBPLL_LDO_CP_MASK              (0x7 << REG_USBPLL_LDO_CP_SHIFT)
#define REG_USBPLL_LDO_CP(n)                BITFIELD_VAL(REG_USBPLL_LDO_CP, n)

// REG_1C
#define DIG_USBPLL_MDLL_DIV_SHIFT           0
#define DIG_USBPLL_MDLL_DIV_MASK            (0x3F << DIG_USBPLL_MDLL_DIV_SHIFT)
#define DIG_USBPLL_MDLL_DIV(n)              BITFIELD_VAL(DIG_USBPLL_MDLL_DIV, n)

// REG_1D
#define DIG_USBPLL_DIV_DLY_CAP_SHIFT        0
#define DIG_USBPLL_DIV_DLY_CAP_MASK         (0xF << DIG_USBPLL_DIV_DLY_CAP_SHIFT)
#define DIG_USBPLL_DIV_DLY_CAP(n)           BITFIELD_VAL(DIG_USBPLL_DIV_DLY_CAP, n)
#define DIG_USBPLL_FBC_SEL_SHIFT            4
#define DIG_USBPLL_FBC_SEL_MASK             (0x7 << DIG_USBPLL_FBC_SEL_SHIFT)
#define DIG_USBPLL_FBC_SEL(n)               BITFIELD_VAL(DIG_USBPLL_FBC_SEL, n)
#define REG_USBPLL_LDO_MDLL_SHIFT           7
#define REG_USBPLL_LDO_MDLL_MASK            (0x7 << REG_USBPLL_LDO_MDLL_SHIFT)
#define REG_USBPLL_LDO_MDLL(n)              BITFIELD_VAL(REG_USBPLL_LDO_MDLL, n)
#define REG_USBPLL_LDO_VCO_SHIFT            10
#define REG_USBPLL_LDO_VCO_MASK             (0x7 << REG_USBPLL_LDO_VCO_SHIFT)
#define REG_USBPLL_LDO_VCO(n)               BITFIELD_VAL(REG_USBPLL_LDO_VCO, n)
#define DIG_USBPLL_FDIV_SEL_SHIFT           13
#define DIG_USBPLL_FDIV_SEL_MASK            (0x3 << DIG_USBPLL_FDIV_SEL_SHIFT)
#define DIG_USBPLL_FDIV_SEL(n)              BITFIELD_VAL(DIG_USBPLL_FDIV_SEL, n)

// REG_1E
#define DIG_USBPLL_LCKDET_WIN_SHIFT         0
#define DIG_USBPLL_LCKDET_WIN_MASK          (0x7 << DIG_USBPLL_LCKDET_WIN_SHIFT)
#define DIG_USBPLL_LCKDET_WIN(n)            BITFIELD_VAL(DIG_USBPLL_LCKDET_WIN, n)
#define REG_USBPLL_ICP_SHIFT                3
#define REG_USBPLL_ICP_MASK                 (0xF << REG_USBPLL_ICP_SHIFT)
#define REG_USBPLL_ICP(n)                   BITFIELD_VAL(REG_USBPLL_ICP, n)
#define DIG_USBPLL_MDLL_LATCH_SHIFT         7
#define DIG_USBPLL_MDLL_LATCH_MASK          (0x7 << DIG_USBPLL_MDLL_LATCH_SHIFT)
#define DIG_USBPLL_MDLL_LATCH(n)            BITFIELD_VAL(DIG_USBPLL_MDLL_LATCH, n)
#define REG_USBPLL_SPEED_SHIFT              10
#define REG_USBPLL_SPEED_MASK               (0x7 << REG_USBPLL_SPEED_SHIFT)
#define REG_USBPLL_SPEED(n)                 BITFIELD_VAL(REG_USBPLL_SPEED, n)
#define REG_USBPLL_MDLL_ICP_SHIFT           13
#define REG_USBPLL_MDLL_ICP_MASK            (0x3 << REG_USBPLL_MDLL_ICP_SHIFT)
#define REG_USBPLL_MDLL_ICP(n)              BITFIELD_VAL(REG_USBPLL_MDLL_ICP, n)

// REG_1F
#define DIG_USBPLL_DIVN_320M_SHIFT          0
#define DIG_USBPLL_DIVN_320M_MASK           (0x7 << DIG_USBPLL_DIVN_320M_SHIFT)
#define DIG_USBPLL_DIVN_320M(n)             BITFIELD_VAL(DIG_USBPLL_DIVN_320M, n)
#define DIG_USBPLL_MDLL_SPEED_SHIFT         3
#define DIG_USBPLL_MDLL_SPEED_MASK          (0x7 << DIG_USBPLL_MDLL_SPEED_SHIFT)
#define DIG_USBPLL_MDLL_SPEED(n)            BITFIELD_VAL(DIG_USBPLL_MDLL_SPEED, n)
#define REG_USBPLL_TST_VREF_SHIFT           6
#define REG_USBPLL_TST_VREF_MASK            (0x7 << REG_USBPLL_TST_VREF_SHIFT)
#define REG_USBPLL_TST_VREF(n)              BITFIELD_VAL(REG_USBPLL_TST_VREF, n)
#define REG_USBPLL_TST_SEL_SHIFT            9
#define REG_USBPLL_TST_SEL_MASK             (0x3 << REG_USBPLL_TST_SEL_SHIFT)
#define REG_USBPLL_TST_SEL(n)               BITFIELD_VAL(REG_USBPLL_TST_SEL, n)
#define REG_USBPLL_DIGCLK_EN                (1 << 11)
#define REG_USBPLL_DIGCLKDIV2_EN            (1 << 12)
#define REG_USB_PLL_DCO_CLK480M_EN          (1 << 13)

// REG_20
#define RB_BISTERR_CAUSE_SHIFT              0
#define RB_BISTERR_CAUSE_MASK               (0x7 << RB_BISTERR_CAUSE_SHIFT)
#define RB_BISTERR_CAUSE(n)                 BITFIELD_VAL(RB_BISTERR_CAUSE, n)
#define UTMI_DATAIN_SHIFT                   3
#define UTMI_DATAIN_MASK                    (0xFF << UTMI_DATAIN_SHIFT)
#define UTMI_DATAIN(n)                      BITFIELD_VAL(UTMI_DATAIN, n)

// REG_21
#define RB_BIST_FAIL                        (1 << 0)
#define RB_BIST_DONE                        (1 << 1)
#define RB_CHIRP_ON                         (1 << 2)
#define RB_ADP_SNS_EN                       (1 << 3)
#define RB_ADP_PRB_EN                       (1 << 4)
#define RB_ADP_DISCHRG                      (1 << 5)
#define RB_ADP_CHRG                         (1 << 6)
#define RB_UTMISRP_DISCHRGVBUS              (1 << 7)
#define RB_UTMISRP_CHRGVBUS                 (1 << 8)
#define RB_UTMIOTG_IDPULLUP                 (1 << 9)
#define RB_UTMIOTG_DPPULLDOWN               (1 << 10)
#define RB_UTMIOTG_DMPULLDOWN               (1 << 11)
#define RB_UTMIOTG_DRVVBUS                  (1 << 12)
#define P2S_PUSH_ERR                        (1 << 13)
#define P2S_POP_ERR                         (1 << 14)

// REG_22
#define CFG_RESETNTX                        (1 << 0)
#define CFG_RESETNRX                        (1 << 1)
#define CFG_CKTX_EN                         (1 << 2)
#define CFG_CKRX_EN                         (1 << 3)
#define CFG_FORCE_TXCK                      (1 << 4)
#define CFG_FORCE_RXCK                      (1 << 5)
#define CFG_FORCE_CDRCK                     (1 << 6)
#define CFG_MODE_S_ON                       (1 << 7)
#define CFG_INT_FLT_IN_SHIFT                8
#define CFG_INT_FLT_IN_MASK                 (0x3 << CFG_INT_FLT_IN_SHIFT)
#define CFG_INT_FLT_IN(n)                   BITFIELD_VAL(CFG_INT_FLT_IN, n)
#define CFG_CKTXP_EN                        (1 << 10)
#define CFG_CKRXP_EN                        (1 << 11)
#define CFG_FORCE_TXPCK                     (1 << 12)
#define CFG_FORCE_CDRPCK                    (1 << 13)

// REG_30
#define REV_REG30_SHIFT                     0
#define REV_REG30_MASK                      (0xFFFF << REV_REG30_SHIFT)
#define REV_REG30(n)                        BITFIELD_VAL(REV_REG30, n)

// REG_31
#define REV_REG31_SHIFT                     0
#define REV_REG31_MASK                      (0xFFFF << REV_REG31_SHIFT)
#define REV_REG31(n)                        BITFIELD_VAL(REV_REG31, n)

// REG_32
#define REV_REG32_SHIFT                     0
#define REV_REG32_MASK                      (0xFFFF << REV_REG32_SHIFT)
#define REV_REG32(n)                        BITFIELD_VAL(REV_REG32, n)

// REG_33
#define CFG_ANA_REV_SHIFT                   0
#define CFG_ANA_REV_MASK                    (0xFFFF << CFG_ANA_REV_SHIFT)
#define CFG_ANA_REV(n)                      BITFIELD_VAL(CFG_ANA_REV, n)

// REG_34
#define REG_USBPLL_PU_DR                    (1 << 0)
#define REG_USBPLL_PU                       (1 << 1)
#define REG_USBPLL_POSTDIV_RST_TMP          (1 << 2)
#define REG_USBPLL_POSTDIV_RST_DR           (1 << 3)
#define REG_USBPLL_POSTDIV_RST              (1 << 4)

// REG_35
#define REG_USBPLL_LDOPRECHG_TIMER_SHIFT    0
#define REG_USBPLL_LDOPRECHG_TIMER_MASK     (0x1F << REG_USBPLL_LDOPRECHG_TIMER_SHIFT)
#define REG_USBPLL_LDOPRECHG_TIMER(n)       BITFIELD_VAL(REG_USBPLL_LDOPRECHG_TIMER, n)
#define REG_USBPLL_PRECHG_TIMER_SHIFT       5
#define REG_USBPLL_PRECHG_TIMER_MASK        (0x1F << REG_USBPLL_PRECHG_TIMER_SHIFT)
#define REG_USBPLL_PRECHG_TIMER(n)          BITFIELD_VAL(REG_USBPLL_PRECHG_TIMER, n)
#define REG_USBPLL_UP_TIMER_PD_SHIFT        10
#define REG_USBPLL_UP_TIMER_PD_MASK         (0x1F << REG_USBPLL_UP_TIMER_PD_SHIFT)
#define REG_USBPLL_UP_TIMER_PD(n)           BITFIELD_VAL(REG_USBPLL_UP_TIMER_PD, n)

// REG_36
#define PLL_RSTN_DR                         (1 << 0)
#define PLL_RSTN                            (1 << 1)
#define PLL_PRESCALER_DEL_SEL_SHIFT         2
#define PLL_PRESCALER_DEL_SEL_MASK          (0xF << PLL_PRESCALER_DEL_SEL_SHIFT)
#define PLL_PRESCALER_DEL_SEL(n)            BITFIELD_VAL(PLL_PRESCALER_DEL_SEL, n)
#define PLL_CLK_FBC_EDGE                    (1 << 6)
#define PLL_INT_DEC_SEL_SHIFT               7
#define PLL_INT_DEC_SEL_MASK                (0x7 << PLL_INT_DEC_SEL_SHIFT)
#define PLL_INT_DEC_SEL(n)                  BITFIELD_VAL(PLL_INT_DEC_SEL, n)
#define PLL_DITHER_BYPASS                   (1 << 10)
#define PLL_DIV_DR                          (1 << 11)
#define PLL_DIV_FRAC_SHIFT                  12
#define PLL_DIV_FRAC_MASK                   (0x3 << PLL_DIV_FRAC_SHIFT)
#define PLL_DIV_FRAC(n)                     BITFIELD_VAL(PLL_DIV_FRAC, n)
#define PLL_FREQ_EN                         (1 << 14)

// REG_37
#define PLL_DIV_INT_SHIFT                   0
#define PLL_DIV_INT_MASK                    (0x7F << PLL_DIV_INT_SHIFT)
#define PLL_DIV_INT(n)                      BITFIELD_VAL(PLL_DIV_INT, n)
#define PLL_FREQ_34_32_SHIFT                7
#define PLL_FREQ_34_32_MASK                 (0x7 << PLL_FREQ_34_32_SHIFT)
#define PLL_FREQ_34_32(n)                   BITFIELD_VAL(PLL_FREQ_34_32, n)

// REG_38
#define PLL_FREQ_OFFSET_SHIFT               0
#define PLL_FREQ_OFFSET_MASK                (0xFFFF << PLL_FREQ_OFFSET_SHIFT)
#define PLL_FREQ_OFFSET(n)                  BITFIELD_VAL(PLL_FREQ_OFFSET, n)

// REG_39
#define PLL_FREQ_15_0_SHIFT                 0
#define PLL_FREQ_15_0_MASK                  (0xFFFF << PLL_FREQ_15_0_SHIFT)
#define PLL_FREQ_15_0(n)                    BITFIELD_VAL(PLL_FREQ_15_0, n)

// REG_3A
#define PLL_FREQ_31_16_SHIFT                0
#define PLL_FREQ_31_16_MASK                 (0xFFFF << PLL_FREQ_31_16_SHIFT)
#define PLL_FREQ_31_16(n)                   BITFIELD_VAL(PLL_FREQ_31_16, n)

// REG_41
#define REG_MDLL_CAL_START                  (1 << 0)
#define REG_MDLL_CAL_MODE_SHIFT             1
#define REG_MDLL_CAL_MODE_MASK              (0x3 << REG_MDLL_CAL_MODE_SHIFT)
#define REG_MDLL_CAL_MODE(n)                BITFIELD_VAL(REG_MDLL_CAL_MODE, n)
#define REG_MDLL_COUNT_ADJ_SHIFT            3
#define REG_MDLL_COUNT_ADJ_MASK             (0xF << REG_MDLL_COUNT_ADJ_SHIFT)
#define REG_MDLL_COUNT_ADJ(n)               BITFIELD_VAL(REG_MDLL_COUNT_ADJ, n)
#define REG_MDLL_CAL_SEL                    (1 << 7)

// REG_42
#define REG_MDLL_WAIT_TIME_SHIFT            0
#define REG_MDLL_WAIT_TIME_MASK             (0xFF << REG_MDLL_WAIT_TIME_SHIFT)
#define REG_MDLL_WAIT_TIME(n)               BITFIELD_VAL(REG_MDLL_WAIT_TIME, n)
#define REG_MDLL_CAL_TIME_SHIFT             8
#define REG_MDLL_CAL_TIME_MASK              (0xFF << REG_MDLL_CAL_TIME_SHIFT)
#define REG_MDLL_CAL_TIME(n)                BITFIELD_VAL(REG_MDLL_CAL_TIME, n)

// REG_43
#define USB_SOF_NUM_SHIFT                   0
#define USB_SOF_NUM_MASK                    (0x3FFF << USB_SOF_NUM_SHIFT)
#define USB_SOF_NUM(n)                      BITFIELD_VAL(USB_SOF_NUM, n)

// REG_44
#define USB_SOF_DETECT_EN                   (1 << 0)
#define USB_SOF_INTR_MSK                    (1 << 1)
#define USB_SOF_INTR_CLR                    (1 << 2)
#define USB_SOF_FRM_NUM_CLR                 (1 << 3)
#define USB_SOF_HALT                        (1 << 4)
#define USB_SOF_TOGGLE_EN                   (1 << 5)
#define USB_SOF_INTR                        (1 << 15)

// REG_45
#define USB_SOF_START_NUM_SHIFT             0
#define USB_SOF_START_NUM_MASK              (0x3FFF << USB_SOF_START_NUM_SHIFT)
#define USB_SOF_START_NUM(n)                BITFIELD_VAL(USB_SOF_START_NUM, n)

// REG_46
#define USB_SOF_END_NUM_SHIFT               0
#define USB_SOF_END_NUM_MASK                (0x3FFF << USB_SOF_END_NUM_SHIFT)
#define USB_SOF_END_NUM(n)                  BITFIELD_VAL(USB_SOF_END_NUM, n)

// REG_47
#define USB_SOF_CAL_COUNT_15_0_SHIFT        0
#define USB_SOF_CAL_COUNT_15_0_MASK         (0xFFFF << USB_SOF_CAL_COUNT_15_0_SHIFT)
#define USB_SOF_CAL_COUNT_15_0(n)           BITFIELD_VAL(USB_SOF_CAL_COUNT_15_0, n)

// REG_48
#define USB_SOF_CAL_COUNT_27_16_SHIFT       0
#define USB_SOF_CAL_COUNT_27_16_MASK        (0xFFF << USB_SOF_CAL_COUNT_27_16_SHIFT)
#define USB_SOF_CAL_COUNT_27_16(n)          BITFIELD_VAL(USB_SOF_CAL_COUNT_27_16, n)

// REG_49
#define CALIB_TIME_15_0_SHIFT               0
#define CALIB_TIME_15_0_MASK                (0xFFFF << CALIB_TIME_15_0_SHIFT)
#define CALIB_TIME_15_0(n)                  BITFIELD_VAL(CALIB_TIME_15_0, n)

// REG_4A
#define CALIB_TIME_19_16_SHIFT              0
#define CALIB_TIME_19_16_MASK               (0xF << CALIB_TIME_19_16_SHIFT)
#define CALIB_TIME_19_16(n)                 BITFIELD_VAL(CALIB_TIME_19_16, n)

// REG_4B
#define CALIB_START                         (1 << 0)
#define INTR_MASK_SHIFT                     1
#define INTR_MASK_MASK                      (0x3 << INTR_MASK_SHIFT)
#define INTR_MASK(n)                        BITFIELD_VAL(INTR_MASK, n)

// REG_4C
#define O_CNT_PLL_15_0_SHIFT                0
#define O_CNT_PLL_15_0_MASK                 (0xFFFF << O_CNT_PLL_15_0_SHIFT)
#define O_CNT_PLL_15_0(n)                   BITFIELD_VAL(O_CNT_PLL_15_0, n)

// REG_4D
#define O_CNT_PLL_24_16_SHIFT               0
#define O_CNT_PLL_24_16_MASK                (0x1FF << O_CNT_PLL_24_16_SHIFT)
#define O_CNT_PLL_24_16(n)                  BITFIELD_VAL(O_CNT_PLL_24_16, n)
#define O_INTR_STATUS_SHIFT                 9
#define O_INTR_STATUS_MASK                  (0x3 << O_INTR_STATUS_SHIFT)
#define O_INTR_STATUS(n)                    BITFIELD_VAL(O_INTR_STATUS, n)

// REG_4E
#define O_CNT_OSC_15_0_SHIFT                0
#define O_CNT_OSC_15_0_MASK                 (0xFFFF << O_CNT_OSC_15_0_SHIFT)
#define O_CNT_OSC_15_0(n)                   BITFIELD_VAL(O_CNT_OSC_15_0, n)

// REG_4F
#define O_CNT_OSC_19_16_SHIFT               0
#define O_CNT_OSC_19_16_MASK                (0xF << O_CNT_OSC_19_16_SHIFT)
#define O_CNT_OSC_19_16(n)                  BITFIELD_VAL(O_CNT_OSC_19_16, n)
#define CHIP_ADDR_I2C_SHIFT                 9
#define CHIP_ADDR_I2C_MASK                  (0x7F << CHIP_ADDR_I2C_SHIFT)
#define CHIP_ADDR_I2C(n)                    BITFIELD_VAL(CHIP_ADDR_I2C, n)

// REG_50
#define RX_DATA_PID_SHIFT                   0
#define RX_DATA_PID_MASK                    (0xFF << RX_DATA_PID_SHIFT)
#define RX_DATA_PID(n)                      BITFIELD_VAL(RX_DATA_PID, n)
#define RX_DATA_DEBUG5_SHIFT                8
#define RX_DATA_DEBUG5_MASK                 (0xFF << RX_DATA_DEBUG5_SHIFT)
#define RX_DATA_DEBUG5(n)                   BITFIELD_VAL(RX_DATA_DEBUG5, n)

// REG_51
#define RX_DATA_DEBUG0_SHIFT                0
#define RX_DATA_DEBUG0_MASK                 (0xFF << RX_DATA_DEBUG0_SHIFT)
#define RX_DATA_DEBUG0(n)                   BITFIELD_VAL(RX_DATA_DEBUG0, n)
#define RX_DATA_DEBUG1_SHIFT                8
#define RX_DATA_DEBUG1_MASK                 (0xFF << RX_DATA_DEBUG1_SHIFT)
#define RX_DATA_DEBUG1(n)                   BITFIELD_VAL(RX_DATA_DEBUG1, n)

// REG_52
#define RX_DATA_DEBUG2_SHIFT                0
#define RX_DATA_DEBUG2_MASK                 (0xFF << RX_DATA_DEBUG2_SHIFT)
#define RX_DATA_DEBUG2(n)                   BITFIELD_VAL(RX_DATA_DEBUG2, n)
#define RX_DATA_DEBUG3_SHIFT                8
#define RX_DATA_DEBUG3_MASK                 (0xFF << RX_DATA_DEBUG3_SHIFT)
#define RX_DATA_DEBUG3(n)                   BITFIELD_VAL(RX_DATA_DEBUG3, n)

// REG_53
#define DEBUG_DATA_CLR                      (1 << 0)
#define USB_HS_PSQUELCH_CLR_EN              (1 << 1)
#define DEBUG_ERR_CLR                       (1 << 2)
#define RX_DATA_DEBUG4_SHIFT                8
#define RX_DATA_DEBUG4_MASK                 (0xFF << RX_DATA_DEBUG4_SHIFT)
#define RX_DATA_DEBUG4(n)                   BITFIELD_VAL(RX_DATA_DEBUG4, n)

// REG_54
#define TRI_G_CNT_MUX_SHIFT                 0
#define TRI_G_CNT_MUX_MASK                  (0x7FF << TRI_G_CNT_MUX_SHIFT)
#define TRI_G_CNT_MUX(n)                    BITFIELD_VAL(TRI_G_CNT_MUX, n)
#define TRI_STEP_MANUAL_SEL_SHIFT           11
#define TRI_STEP_MANUAL_SEL_MASK            (0x1F << TRI_STEP_MANUAL_SEL_SHIFT)
#define TRI_STEP_MANUAL_SEL(n)              BITFIELD_VAL(TRI_STEP_MANUAL_SEL, n)

// REG_55
#define SSC_ENABLE                          (1 << 0)
#define TRI_FREQ_OFST_SEL_SHIFT             1
#define TRI_FREQ_OFST_SEL_MASK              (0xF << TRI_FREQ_OFST_SEL_SHIFT)
#define TRI_FREQ_OFST_SEL(n)                BITFIELD_VAL(TRI_FREQ_OFST_SEL, n)
#define TRI_FREQ_SEL_SHIFT                  5
#define TRI_FREQ_SEL_MASK                   (0xF << TRI_FREQ_SEL_SHIFT)
#define TRI_FREQ_SEL(n)                     BITFIELD_VAL(TRI_FREQ_SEL, n)
#define TRI_STEP_SEL_SHIFT                  9
#define TRI_STEP_SEL_MASK                   (0x3 << TRI_STEP_SEL_SHIFT)
#define TRI_STEP_SEL(n)                     BITFIELD_VAL(TRI_STEP_SEL, n)
#define TRI_PLUS_STEP_NT_SHIFT              11
#define TRI_PLUS_STEP_NT_MASK               (0x7 << TRI_PLUS_STEP_NT_SHIFT)
#define TRI_PLUS_STEP_NT(n)                 BITFIELD_VAL(TRI_PLUS_STEP_NT, n)

enum USBPHY_REG_T {
    USBPHY_REG_00   = 0x00,
    USBPHY_REG_01,
    USBPHY_REG_02,
    USBPHY_REG_03,
    USBPHY_REG_04,
    USBPHY_REG_05,
    USBPHY_REG_06,
    USBPHY_REG_07,
    USBPHY_REG_08,
    USBPHY_REG_09,
    USBPHY_REG_0A,
    USBPHY_REG_0B,
    USBPHY_REG_0C,
    USBPHY_REG_0D,
    USBPHY_REG_0E,
    USBPHY_REG_0F,
    USBPHY_REG_10,
    USBPHY_REG_11,
    USBPHY_REG_12,
    USBPHY_REG_13,
    USBPHY_REG_14,
    USBPHY_REG_15,
    USBPHY_REG_16,
    USBPHY_REG_17,
    USBPHY_REG_18,
    USBPHY_REG_19,
    USBPHY_REG_1A,
    USBPHY_REG_1B,
    USBPHY_REG_1C,
    USBPHY_REG_1D,
    USBPHY_REG_1E,
    USBPHY_REG_1F,
    USBPHY_REG_20,
    USBPHY_REG_21,
    USBPHY_REG_22,

    USBPHY_REG_30   = 0x30,
    USBPHY_REG_31,
    USBPHY_REG_32,
    USBPHY_REG_33,
    USBPHY_REG_34,
    USBPHY_REG_35,
    USBPHY_REG_36,
    USBPHY_REG_37,
    USBPHY_REG_38,
    USBPHY_REG_39,
    USBPHY_REG_3A,

    USBPHY_REG_41   = 0x41,
    USBPHY_REG_42,
    USBPHY_REG_43,
    USBPHY_REG_44,
    USBPHY_REG_45,
    USBPHY_REG_46,
    USBPHY_REG_47,
    USBPHY_REG_48,
    USBPHY_REG_49,
    USBPHY_REG_4A,
    USBPHY_REG_4B,
    USBPHY_REG_4C,
    USBPHY_REG_4D,
    USBPHY_REG_4E,
    USBPHY_REG_4F,
    USBPHY_REG_50,
    USBPHY_REG_51,
    USBPHY_REG_52,
    USBPHY_REG_53,
    USBPHY_REG_54,
    USBPHY_REG_55,
};

#endif

