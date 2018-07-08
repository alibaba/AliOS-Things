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


#ifndef _CAMERA_ASM_H_
#define _CAMERA_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'camera'."
#endif


#define FIFORAM_SIZE                            (80)

//==============================================================================
// camera
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_CAMERA_BASE            0x01A18000

#define REG_CAMERA_BASE_HI         BASE_HI(REG_CAMERA_BASE)
#define REG_CAMERA_BASE_LO         BASE_LO(REG_CAMERA_BASE)

#define REG_CAMERA_CTRL            REG_CAMERA_BASE_LO + 0x00000000
#define REG_CAMERA_STATUS          REG_CAMERA_BASE_LO + 0x00000004
#define REG_CAMERA_DATA            REG_CAMERA_BASE_LO + 0x00000008
#define REG_CAMERA_IRQ_MASK        REG_CAMERA_BASE_LO + 0x0000000C
#define REG_CAMERA_IRQ_CLEAR       REG_CAMERA_BASE_LO + 0x00000010
#define REG_CAMERA_IRQ_CAUSE       REG_CAMERA_BASE_LO + 0x00000014
#define REG_CAMERA_CMD_SET         REG_CAMERA_BASE_LO + 0x00000018
#define REG_CAMERA_CMD_CLR         REG_CAMERA_BASE_LO + 0x0000001C
#define REG_CAMERA_DSTWINCOL       REG_CAMERA_BASE_LO + 0x00000020
#define REG_CAMERA_DSTWINROW       REG_CAMERA_BASE_LO + 0x00000024
#define REG_CAMERA_SCALE_COFIG     REG_CAMERA_BASE_LO + 0x00000028
#define REG_CAMERA_CAM_SPI_REG_0   REG_CAMERA_BASE_LO + 0x0000002C
#define REG_CAMERA_CAM_SPI_REG_1   REG_CAMERA_BASE_LO + 0x00000030
#define REG_CAMERA_CAM_SPI_REG_2   REG_CAMERA_BASE_LO + 0x00000034
#define REG_CAMERA_CAM_SPI_REG_3   REG_CAMERA_BASE_LO + 0x00000038
#define REG_CAMERA_CAM_SPI_REG_4   REG_CAMERA_BASE_LO + 0x0000003C
#define REG_CAMERA_CAM_SPI_REG_5   REG_CAMERA_BASE_LO + 0x00000040
#define REG_CAMERA_CAM_SPI_REG_6   REG_CAMERA_BASE_LO + 0x00000044
#define REG_CAMERA_CAM_SPI_OBSERVE_REG_0 REG_CAMERA_BASE_LO + 0x00000048
#define REG_CAMERA_CAM_SPI_OBSERVE_REG_1 REG_CAMERA_BASE_LO + 0x0000004C
#define REG_CAMERA_FIFORAM_0_RAMDATA REG_CAMERA_BASE_LO + 0x00000200
#define REG_CAMERA_FIFORAM_1_RAMDATA REG_CAMERA_BASE_LO + 0x00000204
#define REG_CAMERA_FIFORAM_2_RAMDATA REG_CAMERA_BASE_LO + 0x00000208
#define REG_CAMERA_FIFORAM_3_RAMDATA REG_CAMERA_BASE_LO + 0x0000020C
#define REG_CAMERA_FIFORAM_4_RAMDATA REG_CAMERA_BASE_LO + 0x00000210
#define REG_CAMERA_FIFORAM_5_RAMDATA REG_CAMERA_BASE_LO + 0x00000214
#define REG_CAMERA_FIFORAM_6_RAMDATA REG_CAMERA_BASE_LO + 0x00000218
#define REG_CAMERA_FIFORAM_7_RAMDATA REG_CAMERA_BASE_LO + 0x0000021C
#define REG_CAMERA_FIFORAM_8_RAMDATA REG_CAMERA_BASE_LO + 0x00000220
#define REG_CAMERA_FIFORAM_9_RAMDATA REG_CAMERA_BASE_LO + 0x00000224
#define REG_CAMERA_FIFORAM_10_RAMDATA REG_CAMERA_BASE_LO + 0x00000228
#define REG_CAMERA_FIFORAM_11_RAMDATA REG_CAMERA_BASE_LO + 0x0000022C
#define REG_CAMERA_FIFORAM_12_RAMDATA REG_CAMERA_BASE_LO + 0x00000230
#define REG_CAMERA_FIFORAM_13_RAMDATA REG_CAMERA_BASE_LO + 0x00000234
#define REG_CAMERA_FIFORAM_14_RAMDATA REG_CAMERA_BASE_LO + 0x00000238
#define REG_CAMERA_FIFORAM_15_RAMDATA REG_CAMERA_BASE_LO + 0x0000023C
#define REG_CAMERA_FIFORAM_16_RAMDATA REG_CAMERA_BASE_LO + 0x00000240
#define REG_CAMERA_FIFORAM_17_RAMDATA REG_CAMERA_BASE_LO + 0x00000244
#define REG_CAMERA_FIFORAM_18_RAMDATA REG_CAMERA_BASE_LO + 0x00000248
#define REG_CAMERA_FIFORAM_19_RAMDATA REG_CAMERA_BASE_LO + 0x0000024C
#define REG_CAMERA_FIFORAM_20_RAMDATA REG_CAMERA_BASE_LO + 0x00000250
#define REG_CAMERA_FIFORAM_21_RAMDATA REG_CAMERA_BASE_LO + 0x00000254
#define REG_CAMERA_FIFORAM_22_RAMDATA REG_CAMERA_BASE_LO + 0x00000258
#define REG_CAMERA_FIFORAM_23_RAMDATA REG_CAMERA_BASE_LO + 0x0000025C
#define REG_CAMERA_FIFORAM_24_RAMDATA REG_CAMERA_BASE_LO + 0x00000260
#define REG_CAMERA_FIFORAM_25_RAMDATA REG_CAMERA_BASE_LO + 0x00000264
#define REG_CAMERA_FIFORAM_26_RAMDATA REG_CAMERA_BASE_LO + 0x00000268
#define REG_CAMERA_FIFORAM_27_RAMDATA REG_CAMERA_BASE_LO + 0x0000026C
#define REG_CAMERA_FIFORAM_28_RAMDATA REG_CAMERA_BASE_LO + 0x00000270
#define REG_CAMERA_FIFORAM_29_RAMDATA REG_CAMERA_BASE_LO + 0x00000274
#define REG_CAMERA_FIFORAM_30_RAMDATA REG_CAMERA_BASE_LO + 0x00000278
#define REG_CAMERA_FIFORAM_31_RAMDATA REG_CAMERA_BASE_LO + 0x0000027C
#define REG_CAMERA_FIFORAM_32_RAMDATA REG_CAMERA_BASE_LO + 0x00000280
#define REG_CAMERA_FIFORAM_33_RAMDATA REG_CAMERA_BASE_LO + 0x00000284
#define REG_CAMERA_FIFORAM_34_RAMDATA REG_CAMERA_BASE_LO + 0x00000288
#define REG_CAMERA_FIFORAM_35_RAMDATA REG_CAMERA_BASE_LO + 0x0000028C
#define REG_CAMERA_FIFORAM_36_RAMDATA REG_CAMERA_BASE_LO + 0x00000290
#define REG_CAMERA_FIFORAM_37_RAMDATA REG_CAMERA_BASE_LO + 0x00000294
#define REG_CAMERA_FIFORAM_38_RAMDATA REG_CAMERA_BASE_LO + 0x00000298
#define REG_CAMERA_FIFORAM_39_RAMDATA REG_CAMERA_BASE_LO + 0x0000029C
#define REG_CAMERA_FIFORAM_40_RAMDATA REG_CAMERA_BASE_LO + 0x000002A0
#define REG_CAMERA_FIFORAM_41_RAMDATA REG_CAMERA_BASE_LO + 0x000002A4
#define REG_CAMERA_FIFORAM_42_RAMDATA REG_CAMERA_BASE_LO + 0x000002A8
#define REG_CAMERA_FIFORAM_43_RAMDATA REG_CAMERA_BASE_LO + 0x000002AC
#define REG_CAMERA_FIFORAM_44_RAMDATA REG_CAMERA_BASE_LO + 0x000002B0
#define REG_CAMERA_FIFORAM_45_RAMDATA REG_CAMERA_BASE_LO + 0x000002B4
#define REG_CAMERA_FIFORAM_46_RAMDATA REG_CAMERA_BASE_LO + 0x000002B8
#define REG_CAMERA_FIFORAM_47_RAMDATA REG_CAMERA_BASE_LO + 0x000002BC
#define REG_CAMERA_FIFORAM_48_RAMDATA REG_CAMERA_BASE_LO + 0x000002C0
#define REG_CAMERA_FIFORAM_49_RAMDATA REG_CAMERA_BASE_LO + 0x000002C4
#define REG_CAMERA_FIFORAM_50_RAMDATA REG_CAMERA_BASE_LO + 0x000002C8
#define REG_CAMERA_FIFORAM_51_RAMDATA REG_CAMERA_BASE_LO + 0x000002CC
#define REG_CAMERA_FIFORAM_52_RAMDATA REG_CAMERA_BASE_LO + 0x000002D0
#define REG_CAMERA_FIFORAM_53_RAMDATA REG_CAMERA_BASE_LO + 0x000002D4
#define REG_CAMERA_FIFORAM_54_RAMDATA REG_CAMERA_BASE_LO + 0x000002D8
#define REG_CAMERA_FIFORAM_55_RAMDATA REG_CAMERA_BASE_LO + 0x000002DC
#define REG_CAMERA_FIFORAM_56_RAMDATA REG_CAMERA_BASE_LO + 0x000002E0
#define REG_CAMERA_FIFORAM_57_RAMDATA REG_CAMERA_BASE_LO + 0x000002E4
#define REG_CAMERA_FIFORAM_58_RAMDATA REG_CAMERA_BASE_LO + 0x000002E8
#define REG_CAMERA_FIFORAM_59_RAMDATA REG_CAMERA_BASE_LO + 0x000002EC
#define REG_CAMERA_FIFORAM_60_RAMDATA REG_CAMERA_BASE_LO + 0x000002F0
#define REG_CAMERA_FIFORAM_61_RAMDATA REG_CAMERA_BASE_LO + 0x000002F4
#define REG_CAMERA_FIFORAM_62_RAMDATA REG_CAMERA_BASE_LO + 0x000002F8
#define REG_CAMERA_FIFORAM_63_RAMDATA REG_CAMERA_BASE_LO + 0x000002FC
#define REG_CAMERA_FIFORAM_64_RAMDATA REG_CAMERA_BASE_LO + 0x00000300
#define REG_CAMERA_FIFORAM_65_RAMDATA REG_CAMERA_BASE_LO + 0x00000304
#define REG_CAMERA_FIFORAM_66_RAMDATA REG_CAMERA_BASE_LO + 0x00000308
#define REG_CAMERA_FIFORAM_67_RAMDATA REG_CAMERA_BASE_LO + 0x0000030C
#define REG_CAMERA_FIFORAM_68_RAMDATA REG_CAMERA_BASE_LO + 0x00000310
#define REG_CAMERA_FIFORAM_69_RAMDATA REG_CAMERA_BASE_LO + 0x00000314
#define REG_CAMERA_FIFORAM_70_RAMDATA REG_CAMERA_BASE_LO + 0x00000318
#define REG_CAMERA_FIFORAM_71_RAMDATA REG_CAMERA_BASE_LO + 0x0000031C
#define REG_CAMERA_FIFORAM_72_RAMDATA REG_CAMERA_BASE_LO + 0x00000320
#define REG_CAMERA_FIFORAM_73_RAMDATA REG_CAMERA_BASE_LO + 0x00000324
#define REG_CAMERA_FIFORAM_74_RAMDATA REG_CAMERA_BASE_LO + 0x00000328
#define REG_CAMERA_FIFORAM_75_RAMDATA REG_CAMERA_BASE_LO + 0x0000032C
#define REG_CAMERA_FIFORAM_76_RAMDATA REG_CAMERA_BASE_LO + 0x00000330
#define REG_CAMERA_FIFORAM_77_RAMDATA REG_CAMERA_BASE_LO + 0x00000334
#define REG_CAMERA_FIFORAM_78_RAMDATA REG_CAMERA_BASE_LO + 0x00000338
#define REG_CAMERA_FIFORAM_79_RAMDATA REG_CAMERA_BASE_LO + 0x0000033C

//CTRL
#define CAMERA_ENABLE              (1<<0)
#define CAMERA_ENABLE_ENABLE       (1<<0)
#define CAMERA_ENABLE_DISABLE      (0<<0)
#define CAMERA_DCTENABLE           (1<<1)
#define CAMERA_DCTENABLE_1_BUFENABLE_ENABLE (1<<1)
#define CAMERA_DCTENABLE_1_BUFENABLE_DISABLE (0<<1)
#define CAMERA_DATAFORMAT(n)       (((n)&3)<<4)
#define CAMERA_DATAFORMAT_RGB565   (0<<4)
#define CAMERA_DATAFORMAT_YUV422   (1<<4)
#define CAMERA_DATAFORMAT_JPEG     (2<<4)
#define CAMERA_DATAFORMAT_RESERVE  (3<<4)
#define CAMERA_RESET_POL           (1<<8)
#define CAMERA_RESET_POL_INVERT    (1<<8)
#define CAMERA_RESET_POL_NORMAL    (0<<8)
#define CAMERA_PWDN_POL            (1<<9)
#define CAMERA_PWDN_POL_INVERT     (1<<9)
#define CAMERA_PWDN_POL_NORMAL     (0<<9)
#define CAMERA_VSYNC_POL           (1<<10)
#define CAMERA_VSYNC_POL_INVERT    (1<<10)
#define CAMERA_VSYNC_POL_NORMAL    (0<<10)
#define CAMERA_HREF_POL            (1<<11)
#define CAMERA_HREF_POL_INVERT     (1<<11)
#define CAMERA_HREF_POL_NORMAL     (0<<11)
#define CAMERA_PIXCLK_POL          (1<<12)
#define CAMERA_PIXCLK_POL_INVERT   (1<<12)
#define CAMERA_PIXCLK_POL_NORMAL   (0<<12)
#define CAMERA_VSYNC_DROP          (1<<14)
#define CAMERA_VSYNC_DROP_DROP     (1<<14)
#define CAMERA_VSYNC_DROP_NORMAL   (0<<14)
#define CAMERA_DECIMFRM(n)         (((n)&3)<<16)
#define CAMERA_DECIMFRM_ORIGINAL   (0<<16)
#define CAMERA_DECIMFRM_DIV_2      (1<<16)
#define CAMERA_DECIMFRM_DIV_3      (2<<16)
#define CAMERA_DECIMFRM_DIV_4      (3<<16)
#define CAMERA_DECIMCOL(n)         (((n)&3)<<18)
#define CAMERA_DECIMCOL_ORIGINAL   (0<<18)
#define CAMERA_DECIMCOL_DIV_2      (1<<18)
#define CAMERA_DECIMCOL_DIV_3      (2<<18)
#define CAMERA_DECIMCOL_DIV_4      (3<<18)
#define CAMERA_DECIMROW(n)         (((n)&3)<<20)
#define CAMERA_DECIMROW_ORIGINAL   (0<<20)
#define CAMERA_DECIMROW_DIV_2      (1<<20)
#define CAMERA_DECIMROW_DIV_3      (2<<20)
#define CAMERA_DECIMROW_DIV_4      (3<<20)
#define CAMERA_REORDER(n)          (((n)&7)<<24)
#define CAMERA_CROPEN              (1<<28)
#define CAMERA_CROPEN_ENABLE       (1<<28)
#define CAMERA_CROPEN_DISABLE      (0<<28)
#define CAMERA_BIST_MODE           (1<<30)
#define CAMERA_BIST_MODE_BIST      (1<<30)
#define CAMERA_BIST_MODE_NORMAL    (0<<30)
#define CAMERA_TEST                (1<<31)
#define CAMERA_TEST_TEST           (1<<31)
#define CAMERA_TEST_NORMAL         (0<<31)

//STATUS
#define CAMERA_OVFL                (1<<0)
#define CAMERA_VSYNC_R             (1<<1)
#define CAMERA_VSYNC_F             (1<<2)
#define CAMERA_DMA_DONE            (1<<3)
#define CAMERA_FIFO_EMPTY          (1<<4)

//DATA
#define CAMERA_RX_DATA(n)          (((n)&0xFFFFFFFF)<<0)

//IRQ_MASK
//#define CAMERA_OVFL              (1<<0)
//#define CAMERA_VSYNC_R           (1<<1)
//#define CAMERA_VSYNC_F           (1<<2)
//#define CAMERA_DMA_DONE          (1<<3)

//IRQ_CLEAR
//#define CAMERA_OVFL              (1<<0)
//#define CAMERA_VSYNC_R           (1<<1)
//#define CAMERA_VSYNC_F           (1<<2)
//#define CAMERA_DMA_DONE          (1<<3)

//IRQ_CAUSE
//#define CAMERA_OVFL              (1<<0)
//#define CAMERA_VSYNC_R           (1<<1)
//#define CAMERA_VSYNC_F           (1<<2)
//#define CAMERA_DMA_DONE          (1<<3)

//CMD_SET
#define CAMERA_PWDN                (1<<0)
#define CAMERA_RESET               (1<<4)
#define CAMERA_FIFO_RESET          (1<<8)

//CMD_CLR
//#define CAMERA_PWDN              (1<<0)
//#define CAMERA_RESET             (1<<4)

//DSTWINCOL
#define CAMERA_DSTWINCOLSTART(n)   (((n)&0xFFF)<<0)
#define CAMERA_DSTWINCOLEND(n)     (((n)&0xFFF)<<16)

//DSTWINROW
#define CAMERA_DSTWINROWSTART(n)   (((n)&0xFFF)<<0)
#define CAMERA_DSTWINROWEND(n)     (((n)&0xFFF)<<16)

//SCALE_COFIG
#define CAMERA_SCALE_EN            (1<<0)
#define CAMERA_SCALE_COL(n)        (((n)&3)<<8)
#define CAMERA_SCALE_ROW(n)        (((n)&3)<<16)

//CAM_SPI_REG_0
#define CAMERA_LINE_NUM_PER_FRAME(n) (((n)&0x3FF)<<22)
#define CAMERA_BLOCK_NUM_PER_LINE(n) (((n)&0x3FF)<<12)
#define CAMERA_VSYNC_INV           (1<<11)
#define CAMERA_HREF_INV            (1<<10)
#define CAMERA_OVERFLOW_INV        (1<<9)
#define CAMERA_BIG_END_DIS         (1<<8)
#define CAMERA_OVERFLOW_RST_EN     (1<<7)
#define CAMERA_OVERFLOW_OBSERVE_ONLY_VSYNC_LOW (1<<6)
#define CAMERA_OVERFLOW_RSTN_ONLY_VSYNC_LOW (1<<5)
#define CAMERA_YUV_OUT_FORMAT(n)   (((n)&7)<<2)
#define CAMERA_CAMERA_SPI_MASTER_EN (1<<1)
#define CAMERA_CAMERA_SPI_SLAVE_EN (1<<0)

//CAM_SPI_REG_1
#define CAMERA_CLK_DIV_NUM(n)      (((n)&0xFFFF)<<0)
#define CAMERA_CTS_SPI_MASTER_REG  (1<<16)
#define CAMERA_SSN_CM_INV          (1<<17)
#define CAMERA_SCK_CM_INV          (1<<18)
#define CAMERA_SSN_SPI_OENB_DR     (1<<19)
#define CAMERA_SSN_SPI_OENB_REG    (1<<20)
#define CAMERA_SCK_SPI_OENB_DR     (1<<21)
#define CAMERA_SCK_SPI_OENB_REG    (1<<22)

//CAM_SPI_REG_2
#define CAMERA_SSN_CS_INV          (1<<29)
#define CAMERA_READY_CS_INV        (1<<28)
#define CAMERA_DATA_RCV_CHOOSE_BIT(n) (((n)&3)<<26)
#define CAMERA_SSN_CS_DELAY(n)     (((n)&3)<<24)
#define CAMERA_WORD_NUM_PER_BLOCK(n) (((n)&0xFF)<<16)
#define CAMERA_INIT_WAIT_LENGTH(n) (((n)&0xFF)<<8)
#define CAMERA_SSN_WAIT_LENGTH(n)  (((n)&0xFF)<<0)

//CAM_SPI_REG_3
#define CAMERA_SSN_HIGH_LENGTH(n)  (((n)&0xFF)<<24)
#define CAMERA_BLOCK_WAIT_LENGTH(n) (((n)&0xFF)<<16)
#define CAMERA_LINE_WAIT_LENGTH(n) (((n)&0xFFFF)<<0)

//CAM_SPI_REG_4
#define CAMERA_BLOCK_NUM_PER_PACKET(n) (((n)&0x3FF)<<6)
#define CAMERA_IMAGE_WIDTH_CHOOSE_BIT (1<<5)
#define CAMERA_IMAGE_HEIGHT_CHOOSE_BIT (1<<4)
#define CAMERA_DATA_SIZE_CHOOSE_BIT (1<<3)
#define CAMERA_SDO_LINE_CHOOSE_BIT(n) (((n)&3)<<1)
#define CAMERA_CAMERA_SPI_MASTER_EN_2 (1<<0)

//CAM_SPI_REG_5
#define CAMERA_SYNC_CODE(n)        (((n)&0xFFFFFF)<<0)

//CAM_SPI_REG_6
#define CAMERA_PACKET_ID_FRAME_START(n) (((n)&0xFF)<<24)
#define CAMERA_PACKET_ID_FRAME_END(n) (((n)&0xFF)<<16)
#define CAMERA_PACKED_ID_LINE_START(n) (((n)&0xFF)<<8)
#define CAMERA_PACKED_ID_DATA_START(n) (((n)&0xFF)<<0)

//CAM_SPI_OBSERVE_REG_0
#define CAMERA_OBS_DATA_ID_WRONG   (1<<28)
#define CAMERA_OBS_LN_NUM_WRONG    (1<<27)
#define CAMERA_OBS_IMG_WIDTH_WRONG (1<<26)
#define CAMERA_OBS_IMG_HEIGHT_WRONG (1<<25)
#define CAMERA_OBS_DATA_SIZE_WRONG (1<<24)
#define CAMERA_DATA_ID(n)          (((n)&0xFF)<<16)
#define CAMERA_LINE_ID(n)          (((n)&0xFFFF)<<0)

//CAM_SPI_OBSERVE_REG_1
#define CAMERA_IMAGE_WIDTH(n)      (((n)&0xFFFF)<<16)
#define CAMERA_IMAGE_HEIGHT(n)     (((n)&0xFFFF)<<0)

//RAMData
#define CAMERA_DATA(n)             (((n)&0xFFFFFFFF)<<0)
#define CAMERA_DATA_MASK           (0xFFFFFFFF<<0)
#define CAMERA_DATA_SHIFT          (0)




#endif
