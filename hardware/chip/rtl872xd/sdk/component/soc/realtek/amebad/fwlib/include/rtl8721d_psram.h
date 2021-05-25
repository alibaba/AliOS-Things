/**
  ******************************************************************************
  * @file    rtl8721d_psram.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for the PSRAM firmware
  *          library.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#ifndef _RTL8710B_PSRAM_H
#define _RTL8710B_PSRAM_H

/** @addtogroup AmebaD_Periph_Driver
  * @{
  */

/** @defgroup PSRAM 
  * @brief PSRAM driver modules
  * @{
  */

/** @addtogroup PSRAM
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * PSRAM_Controller is used to communicate with PSRAM
  * PSRAM:
  *		- IPclk: 50Mhz
  *		- Memory Size: 32M bits 
  *		- Address Mapping: 0x0200_0000 ~ 0x0240_0000
  *		- Access: direct access or dpin mode
  * 		- IRQ: PSRAMC_IRQ
  * 		- GDMA: Support
  *
  *****************************************************************************************
  * How to use PSRAM_Controller to direct access PSRAM memory
  *****************************************************************************************
  *      To direct access PSRAM memory, the following steps are mandatory:
  *       
  *      1. Program read/write Latency, access mode, refresh_rate
  *			PSRAM_CTRL_StructInit(PCTL_InitTypeDef *PCTL_InitStruct)
  *
  *      2. Init Hardware use step1 parameters:
  *			PSRAM_CTRL_Init(PCTL_InitTypeDef *PCTL_InitStruct)
  *
  *      3. Calibration the best rwds delay line:
  *			PSRAM_calibration()
  *
  *      4. Access PSRAM memory normally
  *
  *****************************************************************************************
  * How to use PSRAM_Controller to access PSRAM by Dpin mode
  *****************************************************************************************
  *      To access PSRAM by Dpin mode, the following steps are mandatory:
  *       
  *      1. Program read/write Latency, access mode, refresh_rate
  *			PSRAM_CTRL_StructInit(PCTL_InitTypeDef *PCTL_InitStruct)
  *
  *      2. Init Hardware use step1 parameters:
  *			PSRAM_CTRL_Init(PCTL_InitTypeDef *PCTL_InitStruct)
  *
  *      3. Calibration the best rwds delay line:
  *			PSRAM_calibration()
  *
  *      4. Generate PSRAM command address value for dpin mode
  *			PSRAM_CTRL_CA_Gen()
  *
  *      5. Access PSRAM memory/registers by dpin mode
  *			PSRAM_CTRL_DPin_Mem():Access PSRAM memory
  *			PSRAM_CTRL_DPin_Reg():Access PSRAM register
  *
  *      @note: Cache should be disable during DPIN mode
  *
  *****************************************************************************************
  * How to use PSRAM_Controller to access PSRAM by DMA mode
  *****************************************************************************************
  *      To access PSRAM by DMA mode, the following steps are mandatory:
  *       
  *      1. Program read/write Latency, access mode, refresh_rate
  *			PSRAM_CTRL_StructInit(PCTL_InitTypeDef *PCTL_InitStruct)
  *
  *      2. Init Hardware use step1 parameters:
  *			PSRAM_CTRL_Init(PCTL_InitTypeDef *PCTL_InitStruct)
  *
  *      3. Calibration the best rwds delay line:
  *			PSRAM_calibration()
  *
  *      4. Alloc a free channel for PSRAM GDMA and register GDMA IRQ callback function and data
  *			GDMA_ChnlAlloc()
  *
  *      5. GDMA related configurations(source address/destination address/block size etc.)
  *			GDMA_StructInit():Fills GDMA_InitStruct member with its default value
  *			GDMA_Init():Init GDMA
  *			GDMA_BurstEnable():Enable GDMA Burst Transmission
  *
  *      6. Active the PSRAM DMA Request using the function GDMA_Cmd()
  *
  *      @note: The Maximum Msize of Channel0 is different from other Channels
  *			Maximum Msize of Channel0 is 256
  *			Maximum Msize of Channel1 ~ Channel5 is 8
  *
  * @endverbatim
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup PSRAM_Exported_Types PSRAM Exported Types
  * @{
  */

/** 
  * @brief  PSRAM Init structure definition  
  */
typedef struct {
	u8 PCTL_dfi_cs_wr_dly;			/*!< Specifies the latency contrast between PHY write data path latency 
								and PHY command path latency.
								This parameter must be set to a value in the 0-15 range */
	u8 PCTL_dfi_cs_rd_dly; 			/*!< Specifies the latency contrast between PHY read data enable path
								latency and PHY command path latency  .
								This parameter must be set to a value in the 0-15 range */
	u8 PCTL_tphy_wrdata; 			/*!< Specifies the delay latency from DFI write command to DFI write data.
								This parameter is proposed to set PCTL_wl+2 */
	u8 PCTL_fix_tphy_lat; 				/*!< Specifies thePSRAM_LPC_CTRL uses TPHY_WRDATA or TPHY_RDDATA only .
								This parameter can be a value of @ref PSRAM_TPHY_LAT_definitions */
	u8 PCTL_tphy_rddata; 			/*!< Specifies the delay latency from DFI read command to DFI read data.
								This parameter is proposed to set PCTL_rl+3  */
	u8 PCTL_dfi_path_dly; 			/*!< Specifies which TPHY_WRATA/TPHY_RDDATA cycle to sample DFI latency.
								This parameter is proposed to set PCTL_wl */
	u8 PCTL_wl;						/*!< Specifies the PSRAM write latency .
								This parameter must be set to a value in the 3-6 range */
	u8 PCTL_rl; 						/*!< Specifies the PSRAM read latency.
								This parameter must be set to a value in the 3-6 range */
	u32 PCTL_tcph_ps;			/*!< Specifies the PSRAM CE# pin HIGH cycles between subsequent command.
								This parameter unit is ps */
	u32 PCTL_tpu_ps;					/*!< Specifies the time between access PSRAM and RESET# pin high.
								This parameter unit is ps */
	u32 PCTL_tcem_ps;				/*!< Specifies the maximum average Refresh commands delay cycles.
								This parameter unit is ps */
	u32 PCTL_clk_ps;					/*!< Specifies the PSRAM clock cycle.
								This parameter unit is ps */
	u8 PCTL_mr0_burst_len; 			/*!< Specifies the PSRAM burst length.
								This parameter can be a value of @ref PSRAM_BURST_LENGTH_definitions */
	u8 PCTL_mr0_burst_type; 			/*!< Specifies the PSRAM wrapped burst type.
								This parameter can be a value of @ref PSRAM_BURST_TYPE_definitions */
	u8 PCTL_mr0_lat_mode; 			/*!< Specifies the PSRAM whether fix latency.
								This parameter can be a value of @ref PSRAM_LATENCY_MODE_definitions */
	u8 PCTL_mr0_init_lat; 				/*!< Specifies the PSRAM initial latency.
								This parameter can be a value of @ref PSRAM_INIT_LATENCY_definitions */
	u8 PCTL_mr0_drv_strength; 		/*!< Specifies the PSRAM drive strength.
								This parameter can be a value of @ref PSRAM_DRV_STRENGTH_definitions */
	u8 PCTL_mr0_dpd_en; 			/*!< Specifies the PSRAM whether enter deep power pown.
								This parameter can be a value of @ref PSRAM_DPD_definitions */
	u8 PCTL_mr1_pasr; 				/*!< Specifies the PSRAM partial array self refresh.
								This parameter can be a value of @ref PSRAM_PASR_definitions */
	u8 PCTL_mr1_half_slp; 			/*!< Specifies the PSRAM half sleep mode set.
								This parameter can be a value of @ref PSRAM_HALF_SLP_definitions */
	u8 PCTL_mr1_refresh_rate; 		/*!< Specifies the PSRAM refresh rate.
								This parameter can be a value of @ref PSRAM_REFRESH_RATE_definitions */
} PCTL_InitTypeDef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup PSRAM_Exported_Constants PSRAM Exported Constants
  * @{
  */ 

/** @defgroup PSRAM_Peripheral_definitions 
  * @{
  */
#define IS_PSRAM_ALL_PERIPH(PERIPH) ((PERIPH) == PSRAM_DEV)
/**
  * @}
  */

/** @defgroup  PSRAM_TPHY_LAT_definitions 
  * @{
  */ 
#define PSRAM_FIX_TPHY_LATENCY			1
#define PSRAM_UNSET_TPHY_LATENCY		0
/**
  * @}
  */
  
/** @defgroup PSRAM_BURST_LENGTH_definitions 
  * @{
  */ 
#define PSRAM_BURST_LENGTH_128B		0
#define PSRAM_BURST_LENGTH_64B		1
#define PSRAM_BURST_LENGTH_16B		2
#define PSRAM_BURST_LENGTH_32B		3
/**
  * @}
  */

/** @defgroup PSRAM_BURST_TYPE_definitions 
  * @{
  */ 
#define PSRAM_BURST_VENDOR_TYPE		0
#define PSRAM_BURST_LEGACY_TYPE		1
/**
  * @}
  */

/** @defgroup  PSRAM_LATENCY_MODE_definitions 
  * @{
  */ 
#define PSRAM_2TIMES_LATENCY_FIXED	1
#define PSRAM_VARIABLE_LATENCY			0
/**
  * @}
  */

/** @defgroup PSRAM_INIT_LATENCY_definitions 
  * @{
  */ 
#define PSRAM_INIT_LATENCY_5CLK		0
#define PSRAM_INIT_LATENCY_6CLK		1
#define PSRAM_INIT_LATENCY_3CLK		0xe
#define PSRAM_INIT_LATENCY_4CLK		0xf
/**
  * @}
  */

/** @defgroup PSRAM_DRV_STRENGTH_definitions 
  * @{
  */ 
#define PSRAM_DRV_STRENGTH_50OHMS		0
#define PSRAM_DRV_STRENGTH_35OHMS		1
#define PSRAM_DRV_STRENGTH_100OHMS		2
#define PSRAM_DRV_STRENGTH_200OHMS		3
/**
  * @}
  */

/** @defgroup  PSRAM_DPD_definitions 
  * @{
  */ 
#define PSRAM_DPD_MODE				0
#define PSRAM_NORMAL_MODE			1
/**
  * @}
  */

/** @defgroup  PSRAM_PASR_definitions 
  * @{
  */ 
#define PSRAM_PASR_FULL_ARRAY				0
#define PSRAM_PASR_BOT_HALF_ARRAY			1
#define PSRAM_PASR_BOT_QUARTER_ARRAY		2
#define PSRAM_PASR_BOT_EIGHTH_ARRAY		3
#define PSRAM_PASR_NONE					4
#define PSRAM_PASR_TOP_HALF_ARRAY			5
#define PSRAM_PASR_TOP_QUARTER_ARRAY		6
#define PSRAM_PASR_TOP_EIGHTH_ARRAY		7
/**
  * @}
  */

/** @defgroup  PSRAM_HALF_SLP_definitions 
  * @{
  */ 
#define PSRAM_HALF_SLP_DIS				0
#define PSRAM_HALF_SLP_EN				1
/**
  * @}
  */

/** @defgroup  PSRAM_REFRESH_RATE_definitions 
  * @{
  */ 
#define PSRAM_REFRESH_RATE_FAST			0
#define PSRAM_REFRESH_RATE_NORMAL		1
/**
  * @}
  */

/** @defgroup  PSRAM_BURST_TYPE_definitions 
  * @{
  */ 
#define PSRAM_WRAPPED_TYPE		0
#define PSRAM_LINEAR_TYPE			1
#define IS_PSRAM_BURST_TYPE(TYPE) (((TYPE) == PSRAM_WRAPPED_TYPE) || \
                                   ((TYPE) == PSRAM_LINEAR_TYPE))
/**
  * @}
  */

/** @defgroup  PSRAM_ADDR_SPACE_definitions 
  * @{
  */ 
#define PSRAM_MEM_SPACE		0
#define PSRAM_REG_SPACE		1
#define IS_PSRAM_ADDR_SPACE(MODE) (((MODE) == PSRAM_MEM_SPACE) || \
                                   ((MODE) == PSRAM_REG_SPACE))
/**
  * @}
  */

/** @defgroup  PSRAM_WR_TRANSACTION_definitions 
  * @{
  */ 
#define PSRAM_WRITE_TRANSACTION		0
#define PSRAM_READ_TRANSACTION		1
#define IS_PSRAM_WR_TRANSACTION(MODE) (((MODE) == PSRAM_WRITE_TRANSACTION) || \
                                   ((MODE) == PSRAM_READ_TRANSACTION))
/**
  * @}
  */


/** @defgroup  PSRAM_DPIN_WR_MODE_definitions 
  * @{
  */ 
#define PSRAM_DPIN_READ_MODE		0 << 17
#define PSRAM_DPIN_WRITE_MODE		1 << 17

/**
  * @}
  */



/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup PSRAM_Exported_Functions PSRAM Exported Functions
  * @{
  */
_LONG_CALL_ void PSRAM_CTRL_StructInit(PCTL_InitTypeDef *PCTL_InitStruct);
_LONG_CALL_ void PSRAM_CTRL_Init(PCTL_InitTypeDef *PCTL_InitStruct);
_LONG_CALL_ void PSRAM_CTRL_CA_Gen(u8* PSRAM_CA, u32 StartAddr, u8 BurstType, u8 AddSpace, u8 RW);
_LONG_CALL_ void PSRAM_CTRL_DPin_Mem(u8* PSRAM_CA, u32* PSRAM_data, u32 PSRAM_byteen, u8 RW);
_LONG_CALL_ void PSRAM_CTRL_DPin_Reg(u8* PSRAM_CA, u32* PSRAM_data, u8 RW);
_LONG_CALL_ u32 PSRAM_PHY_REG_Read(u8 offset);
_LONG_CALL_ void PSRAM_PHY_REG_Write(u8 offset, u32 reg_val);
_LONG_CALL_ BOOL PSRAM_calibration(VOID);
/**
  * @}
  */

/* Registers Definitions --------------------------------------------------------*/
/** @defgroup PSRAM_Register_Definitions PSRAM Register Definitions
  * @{
  */
/**************************************************************************//**
 * @defgroup PSRAM_CCR
 * @{
 *****************************************************************************/
/********************  Bits definition for CCR register  *******************/
#define BIT_PSRAM_CR_UPDATE			((u32)0x00000001 << 31)	/*Bit[31],bit for update the internal timing/control registers or Quick INIT done*/
#define BIT_PSRAM_FLUSH_FIFO			((u32)0x00000001 << 8)		/*Bit[8],bit for flush all FIFO in PSRAM_LPC_CTRL*/
#define BIT_PSRAM_DPIN					((u32)0x00000001 << 3)		/*Bit[3],bit for start to set PSRAM command function*/
#define BIT_PSRAM_INIT					((u32)0x00000001)			/*Bit[0],bit for start to issue PSRAM initialization sequence*/
/** @} */

/**************************************************************************//**
 * @defgroup PSRAM_DCR
 * @{
 *****************************************************************************/
/********************  Bits definition for DCR register  *******************/
#define BIT_PSRAM_AADMUX 				((u32)0x00000001 << 20)	/*Bit[20], bit for indicate the target PSRAM is AADMUX type or ADMUX or mormal type*/
#define BIT_PSRAM_PAGE_SIZE			((u32)0x0000000f << 16)	/*Bit[19:16], bits for indicate the target PSRAM page size*/
#define BIT_PSRAM_SUSPEND_EN 			((u32)0x00000001 << 9)		/*Bit[9], bit for indicate PSRAM_CTRL support SUSPEND command*/
#define BIT_PSRAM_CLK_EN				((u32)0x00000001 << 8)		/*Bit[8], bit for indicate PSRAM at asynchronous mode clock enable*/
#define BIT_PSRAM_CRE_EN				((u32)0x00000001 << 7)		/*Bit[7], bit for indicate access PSRAM register mode*/
#define BIT_PSRAM_DFI_RATE				((u32)0x00000007 << 4)		/*Bit[6:4], bits for set DFI ratio*/
#define BIT_PSRAM_DDR_MODE				((u32)0x00000001 << 3)		/*Bit[3], bit for indicate the PSRAM work at DDR mode or SDR mode*/
#define BIT_PSRAM_ASYNC_MODE			((u32)0x00000001 << 2)		/*Bit[2], bit for indicate the PSRAM work at asynchronous mode or synchronous mode*/
#define BIT_PSRAM_DQ					((u32)0x00000003)			/*Bit[1:0], bits for set the PSRAM architecture*/
/** @} */

/**************************************************************************//**
 * @defgroup PSRAM_IOCR0
 * @{
 *****************************************************************************/
/********************  Bits definition for IOCR0 register  *******************/
#define BIT_PSRAM_DFI_PATH_DLY_MASK		((u32)0x0000001f << 25)	/*Bit[29:25], bits for select which TPHY_WRATA/TPHY_RDDATA cycle to sample dfi_latency_en*/
#define BIT_PSRAM_DFI_PATH_DLY_SHIFT		25
#define BIT_PSRAM_TPHY_RDDATA_EN_MASK	((u32)0x0000001f << 20)	/*Bit[24:20], bits for setting the delay latency from DFI read command to dfi_rddata_en signal*/
#define BIT_PSRAM_TPHY_RDDATA_EN_SHIFT	20
#define BIT_PSRAM_FIX_TPHY_LAT_MASK		((u32)0x00000001 << 19)	/*Bit[19], bit for PSRAM_LPC_CTRL uses TPHY_WRDATA or TPHY_RDDATA only*/
#define BIT_PSRAM_FIX_TPHY_LAT_SHIFT		19
#define BIT_PSRAM_TPHY_WRDATA_MASK		((u32)0x0000001f << 12)	/*Bit[16:12], bits for setting the delay latency from DFI write command to DFI write data*/
#define BIT_PSRAM_TPHY_WRDATA_SHIFT		12
#define BIT_PSRAM_RD_PIPE_MASK				((u32)0x0000000f << 8)		/*Bit[11:8], bits for SDR mode read data delay setting*/
#define BIT_PSRAM_RD_PIPE_SHIFT			8
#define BIT_PSRAM_DFI_CS_RD_DLY_MASK		((u32)0x0000000f << 4)		/*Bit[7:4], bits for setting the latency contrast between PHY write data path latency and 
																	PHY command path latency*/
#define BIT_PSRAM_DFI_CS_RD_DLY_SHIFT		4
#define BIT_PSRAM_DFI_CS_WR_DLY_MASK		((u32)0x0000000f)			/*Bit[3:0], bits for setting the latency contrast between PHY read data enable path latency 
																	and PHY command path latency*/
#define BIT_PSRAM_DFI_CS_WR_DLY_SHIFT		0
/** @} */

/**************************************************************************//**
 * @defgroup PSRAM_CSR
 * @{
 *****************************************************************************/
/********************  Bits definition for CSR register  *******************/
#define BIT_PSRAM_DPIN_MODE			((u32)0x00000003 << 17)	/*Bit[18:17], bit for DPIN mode decode*/
#define BIT_PSRAM_MEM_IDLE 				((u32)0x00000001 << 8)		/*Bit[8], bit for disable memory access state*/
/** @} */

/**************************************************************************//**
 * @defgroup PSRAM_DRR
 * @{
 *****************************************************************************/
/********************  Bits definition for DRR register  *******************/
#define BIT_PSRAM_PU_TIME_MASK		((u32)0x0000007f << 15)	/*Bit[21:15], bits for If non-defined CR_INTI_RMBIT configuration*/
#define BIT_PSRAM_PU_TIME_SHIFT		15
#define BIT_PSRAM_CEM_TIME_MASK		((u32)0x000007ff << 4)		/*Bit[14:4], bits for Maximum average Refresh commands delay cycles*/
#define BIT_PSRAM_CEM_TIME_SHIFT		4
#define BIT_PSRAM_CPH_TIME_MASK 		((u32)0x0000000f)			/*Bit[3:0], bits for PSRAM CE# pin HIGH cycles between subsequent command*/
#define BIT_PSRAM_CPH_TIME_SHIFT 		0
/** @} */

/**************************************************************************//**
 * @defgroup PSRAM_CMD_DPIN_NDGE
 * @{
 *****************************************************************************/
/********************  Bits definition for CMD_DPIN_NDGE register  *******************/
#define BIT_PSRAM_DPIN_ADDR_NDGE 		((u32)0x00ffffff)			/*Bit[23:0], bits for PSRAM command address value in DPIN function at negative edge*/
/** @} */

/**************************************************************************//**
 * @defgroup PSRAM_CMD_DPIN
 * @{
 *****************************************************************************/
/********************  Bits definition for CMD_DPIN register  *******************/
#define BIT_PSRAM_DPIN_ADDR_POSI 		((u32)0x00ffffff)			/*Bit[23:0], bits for PSRAM command address value in DPIN function at positive edge*/
/** @} */

/**************************************************************************//**
 * @defgroup PSRAM_CR_TDPIN
 * @{
 *****************************************************************************/
/********************  Bits definition for CR_TDPIN register  *******************/
#define BIT_PSRAM_DFI_RESET_N 			((u32)0x00000001)			/*Bit[0], bit for set the PSRAM TDPIN register value*/
/** @} */

/**************************************************************************//**
 * @defgroup PSRAM_MR_INFO
 * @{
 *****************************************************************************/
/********************  Bits definition for MR_INFO register  *******************/
#define BIT_PSRAM_RL_LATENCY_MASK 	((u32)0x0000001f << 5)		/*Bit[9:5], bits for indicate PSRAM read latency counter*/
#define BIT_PSRAM_RL_LATENCY_SHIFT		5
#define BIT_PSRAM_WL_LATENCY_MASK 	((u32)0x0000001f)			/*Bit[4:0], bits for indicate PSRAM write latency counter*/
#define BIT_PSRAM_WL_LATENCY_SHIFT 	0
/** @} */

/**************************************************************************//**
 * @defgroup PSRAM_MR0
 * @{
 *****************************************************************************/
/********************  Bits definition for MR0 register  *******************/
#define BIT_PSRAM_MR0_MASK							((u32)0x0000ffff)			/*Bit[15:0], bits for setting the value to PSRAM CR0 register in initialization flow*/
#define BIT_PSRAM_MR0_BURST_DPD_MODE_MASK		((u32)0x00000001)	
#define BIT_PSRAM_MR0_BURST_DPD_MODE_SHIFT		15
#define BIT_PSRAM_MR0_BURST_DRV_STRENGTH_MASK	((u32)0x00000007)	
#define BIT_PSRAM_MR0_BURST_DRV_STRENGTH_SHIFT	12
#define BIT_PSRAM_MR0_BURST_RSVD_MASK			((u32)0x0000000f)	
#define BIT_PSRAM_MR0_BURST_RSVD_SHIFT			8
#define BIT_PSRAM_MR0_BURST_INIT_LAT_MASK		((u32)0x0000000f)	
#define BIT_PSRAM_MR0_BURST_INIT_LAT_SHIFT		4
#define BIT_PSRAM_MR0_BURST_LAT_MODE_MASK		((u32)0x00000001)	
#define BIT_PSRAM_MR0_BURST_LAT_MODE_SHIFT		3
#define BIT_PSRAM_MR0_BURST_TYPE_MASK			((u32)0x00000001)	
#define BIT_PSRAM_MR0_BURST_TYPE_SHIFT			2
#define BIT_PSRAM_MR0_BURST_LENGTH_MASK			((u32)0x00000003)	
#define BIT_PSRAM_MR0_BURST_LENGTH_SHIFT			0
/** @} */

/**************************************************************************//**
 * @defgroup PSRAM_MR1
 * @{
 *****************************************************************************/
/********************  Bits definition for MR1 register  *******************/
#define BIT_PSRAM_MR1_MASK 					((u32)0x0000ffff)			/*Bit[15:0], bits for setting the value to PSRAM CR1 register in initialization flow*/
#define BIT_PSRAM_MR1_REFRESH_RATE_MASK		((u32)0x00000001)	
#define BIT_PSRAM_MR1_REFRESH_RATE_SHIFT		6
#define BIT_PSRAM_MR1_HALF_SLP_MODE_MASK	((u32)0x00000001)	
#define BIT_PSRAM_MR1_HALF_SLP_MODE_SHIFT	5
#define BIT_PSRAM_MR1_PASR_MASK				((u32)0x00000007)	
#define BIT_PSRAM_MR1_PASR_SHIFT				0
/** @} */

/**************************************************************************//**
 * @defgroup PSRAM_DPDRI
 * @{
 *****************************************************************************/
/********************  Bits definition for DPDRI register  *******************/
#define BIT_PSRAM_DPIN_DATA_INDEX 		((u32)0x0000000f)			/*Bit[3:0], bits for indicate select which DPIN DATA register .*/
/** @} */

/**************************************************************************//**
 * @defgroup PSRAM_DPDR
 * @{
 *****************************************************************************/
/********************  Bits definition for DPDR register  *******************/
#define BIT_PSRAM_DPIN_DATA 			((u32)0xffffffff)			/*Bit[31:0], bits for indicate the data which will be written to PSRAM or the data
																	read from PSRAM.*/
/** @} */

/**************************************************************************//**
 * @defgroup PSRAM_PCTL_SVN_ID
 * @{
 *****************************************************************************/
/********************  Bits definition for PCTL_SVN_ID register  *******************/
#define BIT_PSRAM_CTRL_GIT_CNT			((u32)0x0000ffff << 16)	/*Bit[31:16], bits for indicate the Git counter of the released RTL code.*/
#define BIT_PSRAM_CTRL_RELEASE_DATE 	((u32)0x0000ffff)			/*Bit[15:0], bits for indicate the IP release date for the particular project.*/
/** @} */

/**************************************************************************//**
 * @defgroup PSRAM_PCTL_IDR
 * @{
 *****************************************************************************/
/********************  Bits definition for PCTL_IDR register  *******************/
#define BIT_PSRAM_CTRL_CR_VER 			((u32)0x0000ffff << 16)	/*Bit[31:16], bits for control register version number.*/
#define BIT_PSRAM_CTRL_CR_PCTL_DEF	((u32)0x0000ffff)			/*Bit[15:0], bits for user definition or support PSRAM type.*/
/** @} */

/**
  * @}
  */
/**
  * @}
  */	

/**
  * @}
  */

/********************  Bits definition for REG_PSRAM_CAL_CTRL register  *******************/
#define BIT_PSRAM_CFG_CAL_INTR 		((u32)0x00000001 << 16)	/*Bit[16], bit for Interrupt flag raised by calibration.*/
#define BIT_PSRAM_CFG_CAL_INTR_MASK	((u32)0x00000001 << 8)		/*Bit[8], bit for Calibration fail interrupt mask.*/
#define BIT_PSRAM_CFG_CAL_THD_MASK	((u32)0x0000000f)			/*Bit[7:4], bits for Error check threshold value , used as a filter*/
#define BIT_PSRAM_CFG_CAL_THD_SHIFT	4
#define BIT_PSRAM_CFG_CAL_EN			((u32)0x00000001)			/*Bit[0], bit for Hardware auto calibration enable.*/

/********************  Bits definition for REG_PSRAM_CAL_PARA register  *******************/
#define BIT_PSRAM_CFG_CAL_JMIN_MASK	((u32)0x00000007)			/*Bit[26:24], bits for Minimum J.*/
#define BIT_PSRAM_CFG_CAL_JMIN_SHIFT	24
#define BIT_PSRAM_CFG_CAL_JMAX_MASK	((u32)0x0000000f)			/*Bit[19:16], bits for Maximum J.*/
#define BIT_PSRAM_CFG_CAL_JMAX_SHIFT	16
#define BIT_PSRAM_CFG_CAL_J_MASK		((u32)0x0000000f)			/*Bit[11:8], bits for Initial value J calibrated by software.*/
#define BIT_PSRAM_CFG_CAL_J_SHIFT		8
#define BIT_PSRAM_CFG_CAL_N_MASK		((u32)0x0000001f)			/*Bit[4:0], bits for Initial value N calibrated by software.*/
#define BIT_PSRAM_CFG_CAL_N_SHIFT		0

/********************  Bits definition for REG_PSRAM_CAL_STATUS register  *******************/
#define BIT_PSRAM_CFG_CAL_CUR_ST_MASK	((u32)0x0000001f)		/*Bit[28:24], bits for Calibration current state.*/
#define BIT_PSRAM_CFG_CAL_CUR_ST_SHIFT	24
#define BIT_PSRAM_CFG_CAL_PDST_MASK		((u32)0x00000007)		/*Bit[18:16], bits for Calibration check result for last read burst.*/
#define BIT_PSRAM_CFG_CAL_PDST_SHIFT		16
#define BIT_PSRAM_CFG_CAL_CUR_J_MASK		((u32)0x0000000f)		/*Bit[11:8], bits for Current value J read from hardware.*/
#define BIT_PSRAM_CFG_CAL_CUR_J_SHIFT		8
#define BIT_PSRAM_CFG_CAL_CUR_N_MASK		((u32)0x0000001f)		/*Bit[4:0], bits for Current value N read from hardware.*/
#define BIT_PSRAM_CFG_CAL_CUR_N_SHIFT		0

/********************  Bits definition for REG_PSRAM_CMD_ADDR_INFO_L register  *******************/
#define BIT_PSRAM_CMD_ADDR_INFO_L_MASK	((u32)0xffffffff)		/*Bit[31:0], bits for Command/Address information[31:0].*/

/********************  Bits definition for REG_PSRAM_CMD_ADDR_INFO_H register  *******************/
#define BIT_PSRAM_CMD_ADDR_INFO_H_MASK	((u32)0x0000ffff)		/*Bit[15:0], bits for Command/Address information[47:32].*/

/********************  Bits definition for REG_PSRAM_BYTE_CNT_INFO register  *******************/
#define BIT_PSRAM_WBYTE_ACCU_MASK		((u32)0x000000ff)		/*Bit[31:24], bits for length of write data given by psram controller, will be cleared at the end of this Transaction.*/
#define BIT_PSRAM_WBYTE_ACCU_SHIFT		24
#define BIT_PSRAM_WBYTE_CNT_MASK			((u32)0x000000ff)		/*Bit[23:16], bits for length of data written to psram controller, will be cleared at the end of this Transaction.*/
#define BIT_PSRAM_WBYTE_CNT_SHIFT			16
#define BIT_PSRAM_RBYTE_ACCU_MASK			((u32)0x000000ff)		/*Bit[15:8], bits for length of read data given by psram controller, will be cleared at the beginning of next Transaction.*/
#define BIT_PSRAM_RBYTE_ACCU_SHIFT		8
#define BIT_PSRAM_RBYTE_CNT_MASK			((u32)0x000000ff)		/*Bit[7:0], bits for length of data read from psram controller, will be cleared at the beginning of next Transaction.*/
#define BIT_PSRAM_RBYTE_CNT_SHIFT			0

/********************  Bits definition for REG_PSRAM_TIME_OUT_CTRL register  *******************/
#define BIT_PSRAM_TIME_OUT_INTR				((u32)0x00000001 << 31) 	/*Bit[31], bit for read transaction time out interrupt flag.*/
#define BIT_PSRAM_TIME_OUT_INTR_MASK			((u32)0x00000001 << 23)	/*Bit[23], bit for read transaction time out interrupt mask.*/
#define BIT_PSRAM_TIME_OUT_THOLD_MASK		((u32)0x000000ff)			/*Bit[7:0], bits for read transaction time out threshold.*/
#define BIT_PSRAM_TIME_OUT_THOLD_SHIFT		0

/********************  Bits definition for REG_PSRAM_DBG_SEL register  *******************/
#define BIT_PSRAM_PSRAM_PHY_DBG_SEL_MASK		((u32)0xff)				/*Bit[7:0], bits for psram phy debug select signal.*/
#define BIT_PSRAM_PSRAM_PHY_DBG_SEL_SHIFT	0

/********************  Bits definition for REG_PSRAM_DBG_INFO register  *******************/
#define BIT_PSRAM_PSRAM_PHY_DBG_MASK			((u32)0xffffffff)			/*Bit[31:0], bits for psram phy debug signals.*/
#define BIT_PSRAM_PSRAM_PHY_DBG_SHIFT			0

/********************  PSRAM PHY register  *******************/
#define REG_PSRAM_CAL_CTRL				0x000	/*!< PSRAM PHY Calibration control register */
#define REG_PSRAM_CAL_PARA				0x004	/*!< PSRAM PHY Calibration parameter register*/
#define REG_PSRAM_CAL_STATUS			0x008	/*!< PSRAM PHY Calibration status register*/
#define REG_PSRAM_CMD_ADDR_INFO_L		0x010	/*!< PSRAM Command/address information register[31:0]*/
#define REG_PSRAM_CMD_ADDR_INFO_H	0x014	/*!< PSRAM Command/address information register[48:32]*/
#define REG_PSRAM_BYTE_CNT_INFO		0x018	/*!< PSRAM Byte Counts register*/
#define REG_PSRAM_TIME_OUT_CTRL		0x01C	/*!< PSRAM Timeout Control register*/
#define REG_PSRAM_DBG_SEL				0x0F0	/*!< PSRAM Debug select register*/
#define REG_PSRAM_DBG_INFO				0x0F8	/*!< PSRAM Debug information register*/

/* Other definations --------------------------------------------------------*/

typedef struct
{
	u32 psram_dev_enable;		/*enable psram*/
	u32 psram_dev_cal_enable;	/*enable psram calibration function*/
	u32 psram_dev_retention;		/*enable psram retention when km4 enter low power mode*/
	u32 psram_heap_start_address;	/*config psram heap start address*/
	u32 psram_heap_size;			/*config psram heap size*/
} PSRAMCFG_TypeDef;

extern PSRAMCFG_TypeDef psram_dev_config;

#endif  //_RTL8710B_PSRAM_H
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
