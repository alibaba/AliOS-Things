	/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#ifndef _RTL8721D_SDIO_HOST_H_
#define _RTL8721D_SDIO_HOST_H_

#define SDIO_HOST_REG_BASE 0x40026000


#define HAL_SDIO_HOST_READ32(addr)				HAL_READ32(SDIO_HOST_REG_BASE, addr)
#define HAL_SDIO_HOST_WRITE32(addr, value)		HAL_WRITE32(SDIO_HOST_REG_BASE, addr, value)
#define HAL_SDIO_HOST_READ16(addr)				HAL_READ16(SDIO_HOST_REG_BASE, addr)
#define HAL_SDIO_HOST_WRITE16(addr, value)		HAL_WRITE16(SDIO_HOST_REG_BASE, addr, value)
#define HAL_SDIO_HOST_READ8(addr)				HAL_READ8(SDIO_HOST_REG_BASE, addr)
#define HAL_SDIO_HOST_WRITE8(addr, value)		HAL_WRITE8(SDIO_HOST_REG_BASE, addr, value)

/* =============== Register Offset Definition =============== */
#define REG_SDIO_HOST_SDMA_SYS_ADDR				0x00  // 4byte
#define REG_SDIO_HOST_BLK_SIZE					0x04  // 2byte
#define REG_SDIO_HOST_BLK_CNT					0x06  // 2byte
#define REG_SDIO_HOST_ARG						0x08  // 4byte
#define REG_SDIO_HOST_XFER_MODE					0x0C  // 2byte
#define REG_SDIO_HOST_CMD						0x0E  // 2byte
#define REG_SDIO_HOST_RSP0						0x10  // 4byte
#define REG_SDIO_HOST_RSP2						0x14  // 4byte
#define REG_SDIO_HOST_RSP4						0x18  // 4byte
#define REG_SDIO_HOST_RSP6						0x1C  // 4byte
#define REG_SDIO_HOST_BUF_DATA_PORT				0x20  // 4byte
#define REG_SDIO_HOST_PRESENT_STATE				0x24  // 4byte
#define REG_SDIO_HOST_HOST_CTRL					0x28  // 1byte
#define REG_SDIO_HOST_PWR_CTRL					0x29  // 1byte
#define REG_SDIO_HOST_BLK_GAP_CTRL				0x2A  // 1byte
#define REG_SDIO_HOST_WAKEUP_CTRL				0x2B  // 1byte
#define REG_SDIO_HOST_CLK_CTRL					0x2C  // 2byte
#define REG_SDIO_HOST_TIMEOUT_CTRL				0x2E  // 1byte
#define REG_SDIO_HOST_SW_RESET					0x2F  // 1byte
#define REG_SDIO_HOST_NORMAL_INT_STATUS			0x30  // 2byte
#define REG_SDIO_HOST_ERROR_INT_STATUS			0x32  // 2byte
#define REG_SDIO_HOST_NORMAL_INT_STATUS_EN		0x34  // 2byte
#define	REG_SDIO_HOST_ERROR_INT_STATUS_EN		0x36  // 2byte
#define REG_SDIO_HOST_NORMAL_INT_SIG_EN			0x38  // 2byte
#define REG_SDIO_HOST_ERROR_INT_SIG_EN			0x3A  // 2byte
#define REG_SDIO_HOST_CAPABILITIES				0x40  // 8byte
#define REG_SDIO_HOST_ADMA_SYS_ADDR				0x58  // 8byte
/* =============================================== */

/* Block Count Register (0x06) */
#define BLK_CNT_REG_MAX					0xFFFF  // 65535 blocks

/* Transfer Mode Register (0x0C) */
#define XFER_MODE_DMA_EN				BIT0
#define XFER_MODE_BLK_CNT_EN			BIT1
#define XFER_MODE_AUTO_CMD12_EN			BIT2
#define XFER_MODE_DATA_XFER_DIR			BIT4
#define XFER_MODE_MULT_SINGLE_BLK		BIT5

/* Present State Register (0x24) */
#define PRES_STATE_CMD_INHIBIT_CMD		BIT0
#define PRES_STATE_CMD_INHIBIT_DAT		BIT1
#define PRES_STATE_DAT_LINE_ACTIVE		BIT2
#define PRES_STATE_CARD_INSERTED		BIT16
#define PRES_STATE_DAT0_SIGNAL_LEVEL	BIT20

/* Power Control Register (0x29) */
#define PWR_CTRL_SD_BUS_PWR				BIT0

/* Clock Control Register (0x2C) */
#define CLK_CTRL_INTERAL_CLK_EN			BIT0
#define CLK_CTRL_INTERAL_CLK_STABLE		BIT1
#define CLK_CTRL_SD_CLK_EN				BIT2

/* Software Reset Register (0x2F) */
#define SW_RESET_FOR_ALL				BIT0
#define SW_RESET_FOR_CMD				BIT1
#define SW_RESET_FOR_DAT				BIT2

/* Normal Interrupt Status (0x30) */
#define NOR_INT_STAT_CMD_COMP			BIT0
#define NOR_INT_STAT_XFER_COMP			BIT1
#define NOR_INT_STAT_BLK_GAP_EVENT		BIT2
#define NOR_INT_STAT_DMA_INT			BIT3
#define NOR_INT_STAT_BUF_WR_RDY			BIT4
#define NOR_INT_STAT_BUF_RD_RDY			BIT5
#define NOR_INT_STAT_CARD_INSERT		BIT6
#define NOR_INT_STAT_CARD_REMOVAL		BIT7
#define NOR_INT_STAT_CARD_INT			BIT8
#define NOR_INT_STAT_ERR_INT			BIT15

/* Error Interrupt Status (0x32) */
#define ERR_INT_STAT_CMD_TIMEOUT		BIT0
#define ERR_INT_STAT_CMD_CRC			BIT1
#define ERR_INT_STAT_CMD_END_BIT		BIT2
#define ERR_INT_STAT_CMD_IDX			BIT3
#define ERR_INT_STAT_DATA_TIMEOUT		BIT4
#define ERR_INT_STAT_DATA_CRC			BIT5
#define ERR_INT_STAT_DATA_END_BIT		BIT6
#define ERR_INT_STAT_CUR_LIMIT			BIT7
#define ERR_INT_STAT_AUTO_CMD12			BIT8
#define ERR_INT_STAT_ADMA				BIT9

/* Normal Interrupt Status Enable (0x34) */
#define NOR_INT_STAT_EN_CMD_COMP		BIT0
#define NOR_INT_STAT_EN_XFER_COMP		BIT1
#define NOR_INT_STAT_EN_BLK_GAP_EVENT	BIT2
#define NOR_INT_STAT_EN_DMA_INT			BIT3
#define NOR_INT_STAT_EN_BUF_WR_RDY		BIT4
#define NOR_INT_STAT_EN_BUF_RD_RDY		BIT5
#define NOR_INT_STAT_EN_CARD_INSERT		BIT6
#define NOR_INT_STAT_EN_CARD_REMOVAL	BIT7
#define NOR_INT_STAT_EN_CARD_INT		BIT8

/* Error Interrupt Status Enable (0x36) */
#define ERR_INT_STAT_EN_CMD_TIMEOUT		BIT0
#define ERR_INT_STAT_EN_CMD_CRC			BIT1
#define ERR_INT_STAT_EN_CMD_END_BIT		BIT2
#define ERR_INT_STAT_EN_CMD_IDX			BIT3
#define ERR_INT_STAT_EN_DATA_TIMEOUT	BIT4
#define ERR_INT_STAT_EN_DATA_CRC		BIT5
#define ERR_INT_STAT_EN_DATA_END_BIT	BIT6
#define ERR_INT_STAT_EN_CUR_LIMIT		BIT7
#define ERR_INT_STAT_EN_AUTO_CMD		BIT8
#define ERR_INT_STAT_EN_ADMA			BIT9

/* Normal Interrupt Signal Enable (0x38) */
#define NOR_INT_SIG_EN_CMD_COMP			BIT0
#define NOR_INT_SIG_EN_XFER_COMP		BIT1
#define NOR_INT_SIG_EN_BLK_GAP_EVENT	BIT2
#define NOR_INT_SIG_EN_DMA_INT			BIT3
#define NOR_INT_SIG_EN_BUF_WR_RDY		BIT4
#define NOR_INT_SIG_EN_BUF_RD_RDY		BIT5
#define NOR_INT_SIG_EN_CARD_INSERT		BIT6
#define NOR_INT_SIG_EN_CARD_REMOVAL		BIT7
#define NOR_INT_SIG_EN_CARD_INT			BIT8

/* Error Interrupt Signal Enable (0x3A) */
#define ERR_INT_SIG_EN_CMD_TIMEOUT		BIT0
#define ERR_INT_SIG_EN_CMD_CRC			BIT1
#define ERR_INT_SIG_EN_CMD_END_BIT		BIT2
#define ERR_INT_SIG_EN_CMD_IDX			BIT3
#define ERR_INT_SIG_EN_DATA_TIMEOUT		BIT4
#define ERR_INT_SIG_EN_DATA_CRC			BIT5
#define ERR_INT_SIG_EN_DATA_END_BIT		BIT6
#define ERR_INT_SIG_EN_CUR_LIMIT		BIT7
#define ERR_INT_SIG_EN_AUTO_CMD			BIT8
#define ERR_INT_SIG_EN_ADMA				BIT9

/* Capabilities Register (0x40) */
#define CAPA_TIMEOUT_CLK_UNIT			BIT7
#define CAPA_ADMA2_SUPPORT				BIT19
#define CAPA_HIGH_SPEED_SUPPORT			BIT21
#define CAPA_VOLT_SUPPORT_33V			BIT24
#define CAPA_VOLT_SUPPORT_30V			BIT25
#define CAPA_VOLT_SUPPORT_18V			BIT26

#define DATA_BLK_LEN					512
#define SCR_REG_LEN						8		// 64 bits
#define SWITCH_FN_STATUS_LEN			64		// 512 bits
#define SD_STATUS_LEN					64		// 512 bits
#define CSD_REG_LEN						16		// 128 bits

/* Switch Function (CMD6) Group */
#define SWITCH_FN_GRP1_DEFAULT			BIT0
#define SWITCH_FN_GRP1_HIGH_SPEED		BIT1
#define SWITCH_FN_GRP2_DEFAULT			BIT0
#define SWITCH_FN_GRP2_FOR_EC			BIT1
#define SWITCH_FN_GRP2_VENDOR_SPECIFIC	BIT14

/* Operating Condition (ACMD41) */
#define ACMD41_POLL_INTERVAL			10000	// 10 ms
#define ACMD41_INIT_TIMEOUT				1000000	// 1 sec	

/* Card Status (R1) */
#define R1_APP_CMD						BIT5
#define R1_WP_VIOLATION					BIT26

/* Error Interrupt Recovery */
#define HAL_SDH_RECOVERED				0x10


/* 0x0C */
typedef enum
{
	WRITE_OP			= 0,
	READ_OP				= 1
}DATA_OPERATION;

/* 0x0E */
typedef enum
{
	CMD_GO_IDLE_STATE			= 0,
	CMD_ALL_SEND_CID			= 2,
	CMD_SEND_RELATIVE_ADDR		= 3,
	CMD_SET_DSR 				= 4,
	CMD_SWITCH_FUNC				= 6,
	CMD_SELECT_DESELECT_CARD	= 7,
	CMD_SEND_IF_COND			= 8,
	CMD_SEND_CSD				= 9,
	CMD_SEND_CID				= 10,
	CMD_VOLTAGE_SWITCH			= 11,
	CMD_STOP_TRANSMISSION		= 12,
	CMD_SEND_STATUS				= 13,
	CMD_GO_INACTIVE_STATE 		= 15,
	CMD_SET_BLOCKLEN			= 16,
	CMD_READ_SINGLE_BLOCK   	= 17,
	CMD_READ_MULTIPLE_BLOCK 	= 18,
	CMD_SET_BLOCK_COUNT			= 23,	
	CMD_WRITE_BLOCK				= 24,
	CMD_WRITE_MULTIPLE_BLOCK	= 25,
	CMD_PROGRAM_CSD 			= 27,
	CMD_ERASE_WR_BLK_START		= 32,
	CMD_ERASE_WR_BLK_END		= 33,
	CMD_ERASE					= 38,
	CMD_SD_SEND_OP_COND 		= 41,
	CMD_LOCK_UNLOCK				= 42,
	CMD_SEND_SCR 				= 51,
	CMD_APP_CMD					= 55
}CMD_IDX;

typedef enum
{
	NORMAL, 				// 00b
	SUSPEND,				// 01b
	RESUME, 				// 10b
	ABORT					// 11b
}CMD_TYPE;
	
typedef enum
{
	NO_DATA,				// 00b
	WITH_DATA				// 01b
}DATA_PRESENT_SEL;

typedef enum
{
	NO_RSP,					// 00b
	RSP_LEN_136,			// 01b
	RSP_LEN_48,				// 10b
	RSP_LEN_48_CHK_BUSY		// 11b
}RSP_TYPE;

/* 0x28 */
typedef enum
{
	SDMA,					// 00b
	RESERVED,				// 01b
	ADMA2_32BIT,			// 10b
	ADMA2_64BIT				// 11b
}HOST_DMA_SELECT;

typedef enum
{
	MODE_1_BIT			= 0,
	MODE_4_BIT			= 1
}HOST_DATA_WIDTH;

/* 0x29 */
typedef enum
{
	VOLT_33V			= 7,// 111b
	VOLT_30V			= 6,// 110b
	VOLT_18V			= 5 // 101b
}HOST_SD_BUS_VOLT;

/* 0x2C */
typedef enum
{
	BASE_CLK					= 0x00,
	BASE_CLK_DIVIDED_BY_2		= 0x01,
	BASE_CLK_DIVIDED_BY_4		= 0x02,
	BASE_CLK_DIVIDED_BY_8		= 0x04,
	BASE_CLK_DIVIDED_BY_16		= 0x08,
	BASE_CLK_DIVIDED_BY_32		= 0x10,
	BASE_CLK_DIVIDED_BY_64		= 0x20,
	BASE_CLK_DIVIDED_BY_128		= 0x40,
	BASE_CLK_DIVIDED_BY_256		= 0x80
}SD_CLK_DIVISOR;

typedef enum
{
    SD_CLK_162KHZ,
    SD_CLK_325KHZ,
    SD_CLK_650KHZ,
    SD_CLK_1_3MHZ,
    SD_CLK_2_6MHZ,
    SD_CLK_5_2MHZ,
    SD_CLK_10_4MHZ,
    SD_CLK_20_8MHZ,
	SD_CLK_41_6MHZ
}SD_CLK_FREQUENCY;

/* Card Status Register */
typedef enum
{
	IDLE,					// 0
	READY,					// 1
	IDENTIFICATION,			// 2
	STAND_BY,				// 3
	TRANSFER,				// 4
	SENDING_DATA,			// 5
	RECEIVE_DATA,			// 6
	PROGRAMMING,			// 7
	DISCONNECT,				// 8
	UNKNOWN = 0xFF
}CURRENT_STATE;

/* OCR Register */
typedef enum
{
	VDD_27_28			= BIT15,
	VDD_28_29			= BIT16,
	VDD_29_30			= BIT17,
	VDD_30_31			= BIT18,
	VDD_31_32			= BIT19,
	VDD_32_33			= BIT20,
	VDD_33_34			= BIT21,
	VDD_34_35			= BIT22,
	VDD_35_36			= BIT23,
	CARD_CAPA_STATUS	= BIT30,
	CARD_PWR_UP_STATUS	= BIT31
}OCR_VOLTAGE_PROFILE;

/* SCR Register */
typedef enum
{
	SD_VER_10			= 0,
	SD_VER_110			= 1,
	SD_VER_200			= 2
}PHYSICAL_LAYER_SPEC_VER;

/* CSD Register */
typedef enum
{
	CLEAR_WRITE_PROTECT			= 0,
	SET_WRITE_PROTECT			= 1
}TEMPORARY_WRITE_PROTECT_STATUS;

/* Switch Function (CMD6) Status Data Structure Version */
typedef enum
{
	BUSY_STATUS_UNDEFINED	= 0,  // bits [511:376] are defined
	BUSY_STATUS_DEFINED		= 1   // bits [511:272] are defined
}SWITCH_FN_STATUS_DATA_STRUCTURE_VER;

/* Switch Function (CMD6) Busy Status */
typedef enum
{
	READY_STATUS		= 0,
	BUSY_STATUS			= 1
}SWITCH_FN_BUSY_STATUS;

/* Switch Function (CMD6) Mode */
typedef enum
{
	CHECK_FN			= 0x0,
	SWITCH_FN 			= 0x1
}SWITCH_FN_MODE;

/* Switch Function (CMD6) Group 1 */
typedef enum
{
	FN1_DEFAULT			= 0x0,
	FN1_HIGH_SPEED 		= 0x1,
	FN1_KEEP_CURRENT 	= 0xF
}SWITCH_FN_GROUP_1;

/* Switch Function (CMD6) Group 2 */
typedef enum
{
	FN2_DEFAULT			= 0x0,
	FN2_FOR_EC 			= 0x1,
	FN2_VENDOR_SPECIFIC	= 0xE,
	FN2_KEEP_CURRENT 	= 0xF	
}SWITCH_FN_GROUP_2;

typedef enum
{
	DESEL_CARD			= 0,
	SEL_CARD			= 1
}CARD_SELECTION;

typedef enum
{
	SDSC_ONLY			= 0,
	SDHC_SUPPORT		= 1
}HOST_CAPACITY_SUPPORT;

typedef enum
{
	BUS_1_BIT			= 0,
	BUS_4_BIT			= 2
}DATA_BUS_WIDTH;


typedef struct _ADMA2_ATTRIB_
{
	u16 Valid:1;
	u16 End:1;
	u16 Int:1;
	u16 Rsvd1:1;
	u16 Act1:1;
	u16 Act2:1;
	u16 Rsvd2:10;
}ADMA2_ATTRIB, *PADMA2_ATTRIB;

typedef struct _ADMA2_DESC_FMT_
{
	ADMA2_ATTRIB Attrib1;
	u16	Len1;
	u32 Addr1;
	/* Link to next descriptor (if needed) */
	ADMA2_ATTRIB Attrib2;
	u16 Len2;
	u32 Addr2;
}ADMA2_DESC_FMT, *PADMA2_DESC_FMT;

/* 0x0E */
typedef struct _SDIO_HOST_CMD_FMT_
{
	u16 RespType:2;
	u16 Rsvd0:1;
	u16 CmdCrcChkEn:1;
	u16 CmdIdxChkEn:1;
	u16 DataPresent:1;
	u16 CmdType:2;
	u16 CmdIdx:6;
	u16 Rsvd1:2;
}SDIO_HOST_CMD_FMT, *PSDIO_HOST_CMD_FMT;

typedef struct _SDIO_HOST_CMD_
{
	SDIO_HOST_CMD_FMT CmdFmt;
	u32 Arg;
}SDIO_HOST_CMD, *PSDIO_HOST_CMD;


HAL_Status
HalSdioHostInitHostRtl8721d(
	IN VOID *Data
);

HAL_Status
HalSdioHostInitCardRtl8721d(
	IN VOID *Data
);

HAL_Status
HalSdioHostDeInitRtl8721d(
	IN VOID *Data
);

HAL_Status
HalSdioHostEnableRtl8721d(
	IN VOID *Data
);

HAL_Status
HalSdioHostDisableRtl8721d(
	IN VOID *Data
);

HAL_Status
HalSdioHostIrqInitRtl8721d(
	IN VOID *Data
);

HAL_Status
HalSdioHostReadBlocksDmaRtl8721d(
	IN VOID *Data,
	IN u64 ReadAddr,
	IN u32 BlockCnt
);

HAL_Status
HalSdioHostWriteBlocksDmaRtl8721d(
	IN VOID *Data,
	IN u64 WriteAddr,
	IN u32 BlockCnt
);

HAL_Status
HalSdioHostStopTransferRtl8721d(
	IN VOID *Data
);

HAL_Status
HalSdioHostGetCardStatusRtl8721d(
	IN VOID *Data
);

HAL_Status
HalSdioHostGetSdStatusRtl8721d(
	IN VOID *Data
);

HAL_Status
HalSdioHostChangeSdClockRtl8721d(
	IN VOID *Data,
	IN u8 Frequency
);

HAL_Status
HalSdioHostEraseRtl8721d(
	IN VOID *Data,
	IN u64 StartAddr,
	IN u64 EndAddr
);

HAL_Status
HalSdioHostGetWriteProtectRtl8721d(
	IN VOID *Data
);

HAL_Status
HalSdioHostSetWriteProtectRtl8721d(
	IN VOID *Data,
	IN u8 Setting
);


#ifdef CONFIG_SDIO_HOST_VERIFY

#define HAL_MMC_HOST_READ32(addr)            			HAL_READ32(SDIO_HOST_REG_BASE, addr)
#define HAL_MMC_HOST_WRITE32(addr, value)    			HAL_WRITE32(SDIO_HOST_REG_BASE, addr, value)
#define HAL_MMC_HOST_READ16(addr)						HAL_READ16(SDIO_HOST_REG_BASE, addr)
#define HAL_MMC_HOST_WRITE16(addr, value)    			HAL_WRITE16(SDIO_HOST_REG_BASE, addr, value)
#define HAL_MMC_HOST_READ8(addr)						HAL_READ8(SDIO_HOST_REG_BASE, addr)
#define HAL_MMC_HOST_WRITE8(addr, value)    			HAL_WRITE8(SDIO_HOST_REG_BASE, addr, value)

/* RTL8721D Register */
// REG_SOC_HCI_COM_FUNC_EN (0x214)
#define SD_DEVICE_IP_ON_BLK					BIT0
#define SD_DEVICE_IP_OFF_BLK				BIT1
#define SD_HOST_IP_BLK						BIT2

// REG_PESOC_HCI_CLK_CTRL0 (0x240)
#define SD_HOST_CLKEN_IN_CPU_RUN_MODE		BIT2

// REG_HCI_PINMUX_CTRL (0x2A0)
#define SD_DEVICE_MODE_PINMUX_EN			BIT0
#define SD_HOST_MODE_PINMUX_EN				BIT1

// 0x40059000
#define SD_HOST_CARD_DETECT_CIRCUIT			BIT10


	
/* SD Host Register */
#define REG_SDMA_SYS_ADDR_ARG				0x00 // 4byte
#define REG_BLOCK_SIZE						0x04 // 2byte
#define REG_BLOCK_COUNT						0x06 // 2byte
#define REG_ARGUMENT1						0x08 // 4byte
#define REG_TRANSFER_MODE					0x0C // 2byte
#define REG_COMMAND							0x0E // 2byte
#define REG_RESPONSE0						0x10 // 4byte
#define REG_RESPONSE2						0x14 // 4byte
#define REG_RESPONSE4						0x18 // 4byte
#define REG_RESPONSE6						0x1C // 4byte
#define REG_BUFFER_DATA_PORT				0x20 // 4byte
#define REG_PRESENT_STATE					0x24 // 4byte
#define REG_HOST_CONTROL1					0x28 // 1byte
#define REG_POWER_CONTROL					0x29 // 1byte
#define REG_BLOCK_GAP_CONTROL				0x2A // 1byte
#define REG_WAKEUP_CONTROL					0x2B // 1byte
#define REG_CLOCK_CONTROL					0x2C // 2byte
#define REG_TIMEOUT_CONTROL					0x2E // 1byte
#define REG_SW_RESET						0x2F // 1byte
#define REG_NORMAL_INT_STATUS				0x30 // 2byte
#define REG_ERROR_INT_STATUS				0x32 // 2byte
#define REG_NORMAL_INT_STATUS_ENABLE		0x34 // 2byte
#define	REG_ERROR_INT_STATUS_ENABLE			0x36 // 2byte
#define REG_NORMAL_INT_SIGNAL_ENABLE		0x38 // 2byte
#define REG_ERROR_INT_SIGNAL_ENABLE			0x3A // 2byte
#define REG_CAPABILITIES					0x40 // 8byte
#define REG_ADMA_ADDRESS					0x58 // 8byte

// Transfer Mode (0x0C)
#define BIT_DMA_EN							BIT0
#define BIT_BLK_CNT_EN						BIT1
#define BIT_AUTO_CMD12_EN					BIT2
#define BIT_AUTO_CMD23_EN					BIT3
#define BIT_READ_TRANS						BIT4
#define BIT_MULTI_BLK						BIT5

// Present State (0x24)
#define BIT_CMD_INHIBIT_CMD					BIT0
#define BIT_CMD_INHIBIT_DAT					BIT1
#define BIT_CARD_INSERTED					BIT16
#define BIT_WRITE_PROTECT_SWITCH_PIN		BIT19

// Power Control (0x29)
#define BIT_POWER_33						0xE
#define BIT_POWER_30						0xC
#define BIT_POWER_18						0xA

// Clock Control (0x2C)
#define BIT_INTERNAL_CLK_EN					BIT0
#define BIT_INTERNAL_CLK_STABLE				BIT1
#define BIT_SD_CLK_EN						BIT2

// Software Reset (0x2F)
#define BIT_SW_RESET_ALL					BIT0
#define BIT_SW_RESET_CMD_LINE				BIT1
#define BIT_SW_RESET_DAT_LINE				BIT2
	
// Norma Interrupt Status (0x30)
#define BIT_COMMAND_COMPLETE				BIT0
#define BIT_TRANSFER_COMPLETE				BIT1
#define BIT_BLOCK_GAP_EVENT					BIT2
#define BIT_DMA_INT							BIT3
#define BIT_BUFFER_WRITE_RDY				BIT4
#define BIT_BUFFER_READ_RDY					BIT5
#define BIT_CARD_INSERTION					BIT6
#define BIT_CARD_REMOVAL					BIT7
#define BIT_CARD_INT						BIT8
#define BIT_ERROR_INT						BIT15

// Error Interrupt Status (0x32)
#define BIT_DATA_TIME_OUT_ERROR				BIT4
#define BIT_DATA_CRC_ERROR					BIT5
#define BIT_ADMA_ERROR						BIT9

// Capabilities (0x40)
#define BIT_VDD_33							BIT24
#define BIT_VDD_30							BIT25
#define BIT_VDD_18							BIT26


#define ENABLE								1
#define DISABLE 							0

#define ADMA_DESC_NUM						50

#define BUFFER_UNIT_SIZE					512
	
typedef enum _MMC_HOST_TEST_FUNC_ {
	MMC_HOST_TEST_HW_INIT,			// 0
	MMC_HOST_TEST_CARD_INIT,		// 1
	MMC_HOST_TEST_SEND_CMD, 		// 2
	MMC_HOST_TEST_DEBUG,			// 3
	MMC_HOST_TEST_SW_RESET,			// 4
	MMC_HOST_TEST_READ_SINGLE,		// 5
	MMC_HOST_TEST_WRITE_SINGLE,		// 6
	MMC_HOST_TEST_READ_MULTI,		// 7
	MMC_HOST_TEST_WRITE_MULTI,		// 8
	MMC_HOST_TEST_SINGLE_LONGRUN,	// 9
	MMC_HOST_TEST_MULTI_LONGRUN,	// 10
	MMC_HOST_TEST_CARD_DETECTION,	// 11
	MMC_HOST_TEST_WRITE_PROTECT,	// 12
	MMC_HOST_TEST_REGISTER_RW,		// 13
	SD_HOST_HAL_API_VERIFY			 = 20,
	SD_HOST_ERASE_TEST				 = 21,
	SD_HOST_WP_TEST 				 = 22,
	SD_HOST_MB_TEST					 = 23,
	SD_HOST_ADMA_MAX_TEST			 = 24
}MMC_HOST_TEST_FUNC;
	
typedef enum _RESPONSE_TYPE_ {
	No_Response,					// 00b
	Response_136,					// 01b
	Response_48,					// 10b
	Response_48_Busy				// 11b
}RESPONSE_TYPE;
	
typedef enum _COMMAND_TYPE_ {
	Normal, 						// 00b
	Suspend,						// 01b
	Resume, 						// 10b
	Abort							// 11b
}COMMAND_TYPE;
	
typedef enum _DATA_PRESENT_ {
	No_Data_Present,				// 00b
	Data_Present,					// 01b
}DATA_PRESENT;

typedef enum _SUPPLY_VOLTAGE_ {
	MMC_VDD_27_28	= BIT15,
	MMC_VDD_28_29	= BIT16,
	MMC_VDD_29_30	= BIT17,
	MMC_VDD_30_31	= BIT18,
	MMC_VDD_31_32	= BIT19,
	MMC_VDD_32_33	= BIT20,
	MMC_VDD_33_34	= BIT21,
	MMC_VDD_34_35	= BIT22,
	MMC_VDD_35_36	= BIT23,
}SUPPLY_VOLTAGE;
	
typedef enum _COMMAND_INDEX_ {
	GO_IDLE_STATE			= 0,
	ALL_SEND_CID			= 2,
	SEND_RELATIVE_ADDR		= 3,
	SET_BUS_WIDTH			= 6,
	SELECT_CARD				= 7,
	SEND_IF_COND			= 8,
	SEND_CSD				= 9,
	STOP_TRANSMISSION		= 12,
	SEND_STATUS				= 13,
	READ_SINGLE_BLOCK   	= 17,
	READ_MULTIPLE_BLOCK 	= 18,
	WRITE_BLOCK				= 24,
	WRITE_MULTIPLE_BLOCK	= 25,
	SD_SEND_OP_COND 		= 41,
	APP_CMD					= 55,
}COMMAND_INDEX;

typedef enum _TRANSFER_CONFIG_ {
	Read_Data				= 0,
	Write_Data				= 1,
	Single_Block			= 0,
	Multiple_Block			= 1,
}TRANSFER_CONFIG;

typedef enum _ERROR_STATUS_ {
	General_Error,					// 0
	CRC_Error,						// 1	
	TIME_OUT_ERROR,					// 2
	CRC_Error_NeedCMD12,			// 3
	Transfer_OK						// 4
}ERROR_STATUS;

typedef enum _CARD_CURRENT_STATE_ {
	IDLE_STATE,
	READY_STATE,
	IDENT_STATE,
	STBY_STATE,
	TRAN_STATE,
	DATA_STATE,
	RCV_STATE,
	PRG_STATE,
	DIS_STATE,
	UNKNOWN_STATE
}CARD_CURRENT_STATE;

typedef struct _COMMAND_FORMAT_
{
	u16 Resp_Type:2;
	u16 Rsvd0:1;
	u16 CMD_CRC_Chk:1;
	u16 CMD_Idx_Chk:1;
	u16 Data_Present:1;
	u16 CMD_Type:2;
	u16 CMD_Idx:6;
	u16 Rsvd1:2;
}COMMAND_FORMAT, *PCOMMAND_FPRMAT;

typedef struct _MMC_COMMAND
{
	COMMAND_FORMAT	Cmd_Format;
	u32 Arg;
}MMC_COMMAND;

typedef struct _MMC_HOST_
{
	u32 OCR_Avail;
	u32 Resp[4];
	u32 CID[4];
	u32 RCA;
}MMC_HOST, *PMMC_HOST;

typedef struct _ADMA_ATTR_
{
	u16 Valid:1;
	u16 End:1;
	u16 Int:1;
	u16 Rsvd1:1;
	u16 Act1:1;
	u16 Act2:1;
	u16 Rsvd2:10;
}ADMA_ATTR, *PADMA_ATTR;
// 24 bytes
typedef struct _ADMA_DESC_TABLE_
{
	// 1st buffer desc
	ADMA_ATTR Attribute1;
	u16	Length1;
	u32 Address1;
	// 2nd buffer desc
	ADMA_ATTR Attribute2;
	u16	Length2;
	u32 Address2;
	// 3rd buffer desc
	ADMA_ATTR Attribute3;
	u16	Length3;
	u32 Address3;
}ADMA_DESC_TABLE, *PADMA_DESC_TABLE;
// 1024 bytes
typedef struct _ADMA_BUFFER_
{
	u8 Data1[512];  /* 1st buffer */
	u8 Data2[512];  /* 2nd buffer */
}ADMA_BUFFER, *PADMA_BUFFER;


VOID
SdHostTestApp(
    IN  u8      *argv[]
);
#endif	// end of "#ifdef CONFIG_SDIO_HOST_VERIFY"

#endif	/* #ifndef _RTL8721D_SDIO_HOST_H_ */
