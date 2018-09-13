/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _DRIVER_CHIP_SDMMC_SDHOST_H_
#define _DRIVER_CHIP_SDMMC_SDHOST_H_

#include "../hal_debug.h"

#include "driver/chip/hal_util.h"
#include "driver/chip/sdmmc/card.h"
#include "driver/chip/sdmmc/hal_sdhost.h"
#ifdef CONFIG_USE_SDIO
#include "driver/chip/sdmmc/sdio.h"
#endif
#include "driver/chip/sdmmc/sdmmc.h"
#include "pm/pm.h"

#define CONFIG_SDC_OS_USED

#ifdef SD_SUPPORT_VERSION3
#define CONFIG_SDC_SUPPORT_1V8     /* board not support */
#endif
#ifndef __CONFIG_ARCH_DUAL_CORE
//#define CONFIG_SDC_DMA_USED
#endif
#ifdef SD_SUPPORT_WRITEPROTECT
#define CONFIG_SDC_READONLY_USED   /* check readonly */
#endif

#define SDC_DEBUG               1

#ifdef CONFIG_SDC_OS_USED
#include "./../hal_os.h"
#include "kernel/os/os_semaphore.h"
#include "kernel/os/os_mutex.h"
#ifdef __CONFIG_ARCH_APP_CORE
#include "kernel/os/os_timer.h"
#endif

#define SDC_Semaphore OS_Semaphore_t
#define SDC_Mutex OS_Mutex_t
#ifdef __CONFIG_ARCH_APP_CORE
#define SDC_Timer OS_Timer_t
#endif
#endif

#if SDC_DEBUG
#define SDC_LOGD(format, args...) HAL_LOG(0, format, ##args)
#define SDC_LOGN(format, args...) HAL_LOG(SDC_DEBUG, format, ##args)
#define SDC_LOGW(format, args...) HAL_LOG(SDC_DEBUG, format, ##args)
#define SDC_LOGE(format, args...) HAL_LOG(SDC_DEBUG, format, ##args)
#define SDC_LOGA(format, args...) HAL_LOG(SDC_DEBUG, format, ##args)
#else
#define SDC_LOGD(x...)
#define SDC_LOGN(x...)
#define SDC_LOGW(x...)
#define SDC_LOGE(x...)
#define SDC_LOGA(x...)
#endif
#define SDC_BUG_ON(v) do {if(v) {printf("BUG at %s:%d!\n", __func__, __LINE__); while (1);}} while (0)
#define SDC_WARN_ON(v) do {if(v) {printf("WARN at %s:%d!\n", __func__, __LINE__);}} while (0)

#define mmc_mdelay(ms) OS_MSleep(ms)
#define mmc_udelay(us) HAL_UDelay(us)

#ifdef CONFIG_PM
#define CONFIG_SD_PM
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if ((defined SDIO_EXCLUSIVE_HOST) || (defined SD_EXCLUSIVE_HOST) || (defined MMC_EXCLUSIVE_HOST))
#define CONFIG_SDC_EXCLUSIVE_HOST
#endif

struct scatterlist {
	void *buffer;
	uint32_t len;
};

/* IDMC structure */
typedef struct {
	uint32_t config;

#define SDXC_IDMAC_DES0_DIC             BIT(1) /* disable interrupt on completion */
#define SDXC_IDMAC_DES0_LD              BIT(2) /* 1-this data buffer is the last buffer */
#define SDXC_IDMAC_DES0_FD              BIT(3) /* 1-data buffer is the first buffer, 0-data buffer contained in the next descriptor is the first data buffer */
#define SDXC_IDMAC_DES0_CH              BIT(4) /* 1-the 2nd address in the descriptor is the next descriptor address */
#define SDXC_IDMAC_DES0_ER              BIT(5) /* 1-last descriptor flag when using dual data buffer in descriptor */
#define SDXC_IDMAC_DES0_CES             BIT(30) /* transfer error flag */
#define SDXC_IDMAC_DES0_OWN             BIT(31) /* des owner:1-idma owns it, 0-host owns it */

	uint32_t data_buf1_sz   :16,
	         data_buf2_sz   :16;

	uint32_t buf_addr_ptr1;
	uint32_t buf_addr_ptr2;
} smc_idma_des;

typedef enum
{
	SDC_STATE_RESET        = 0x00, /* Peripheral is not yet Initialized */
	SDC_STATE_READY        = 0x02, /* Peripheral Initialized and ready for use */
	SDC_STATE_BUSY         = 0x04, /* An internal process is ongoing */
	SDC_STATE_ERROR        = 0x08  /* Error */
} SDC_StateTypeDef;

struct mmc_bus_ops {
	int (*suspend)(struct mmc_host *);
	int (*resume)(struct mmc_host *);
};

struct mmc_host {
	uint32_t        sdc_id;
	struct mmc_card *card;
#ifdef CONFIG_SDC_SUPPORT_1V8
	uint32_t        voltage;
#define SDC_WOLTAGE_3V3 (0)
#define SDC_WOLTAGE_1V8 (1)
#define SDC_WOLTAGE_1V2 (2)
#define SDC_WOLTAGE_OFF (3)
#define SDC_WOLTAGE_ON  (4)
	uint32_t        voltage_switching;
#endif
	volatile uint32_t present;
	uint32_t        power_on;
	uint32_t        suspend;
	uint32_t        int_err;       /* for Interrupt Controller */

	uint32_t        int_use;       /* Control */
	uint32_t        int_sum;       /* interrupt summary */
	uint32_t        trans_done;
	uint32_t        dma_done;
	uint32_t        buswidth;      /* current card bus width */
	uint32_t        blkcnt;

	smc_idma_des    *dma_hdle;

	/* host specific block data */
	uint32_t        max_seg_size;   /* see blk_queue_max_segment_size */
	uint32_t        max_segs;       /* see blk_queue_max_segments */
	uint32_t        max_req_size;   /* maximum number of bytes in one req */
	uint32_t        max_blk_size;   /* maximum size of one mmc block */
	uint32_t        max_blk_count;  /* maximum number of blocks in one req */
	uint32_t        ocr_avail;

#define MMC_VDD_165_195         0x00000080      /* VDD voltage 1.65 - 1.95 */
#define MMC_VDD_20_21           0x00000100      /* VDD voltage 2.0 ~ 2.1 */
#define MMC_VDD_21_22           0x00000200      /* VDD voltage 2.1 ~ 2.2 */
#define MMC_VDD_22_23           0x00000400      /* VDD voltage 2.2 ~ 2.3 */
#define MMC_VDD_23_24           0x00000800      /* VDD voltage 2.3 ~ 2.4 */
#define MMC_VDD_24_25           0x00001000      /* VDD voltage 2.4 ~ 2.5 */
#define MMC_VDD_25_26           0x00002000      /* VDD voltage 2.5 ~ 2.6 */
#define MMC_VDD_26_27           0x00004000      /* VDD voltage 2.6 ~ 2.7 */
#define MMC_VDD_27_28           0x00008000      /* VDD voltage 2.7 ~ 2.8 */
#define MMC_VDD_28_29           0x00010000      /* VDD voltage 2.8 ~ 2.9 */
#define MMC_VDD_29_30           0x00020000      /* VDD voltage 2.9 ~ 3.0 */
#define MMC_VDD_30_31           0x00040000      /* VDD voltage 3.0 ~ 3.1 */
#define MMC_VDD_31_32           0x00080000      /* VDD voltage 3.1 ~ 3.2 */
#define MMC_VDD_32_33           0x00100000      /* VDD voltage 3.2 ~ 3.3 */
#define MMC_VDD_33_34           0x00200000      /* VDD voltage 3.3 ~ 3.4 */
#define MMC_VDD_34_35           0x00400000      /* VDD voltage 3.4 ~ 3.5 */
#define MMC_VDD_35_36           0x00800000      /* VDD voltage 3.5 ~ 3.6 */

	uint32_t        caps;                   /* Host capabilities */

#define MMC_CAP_4_BIT_DATA      (1 << 0)        /* Can the host do 4 bit transfers */
#define MMC_CAP_MMC_HIGHSPEED   (1 << 1)        /* Can do MMC high-speed timing */
#define MMC_CAP_SD_HIGHSPEED    (1 << 2)        /* Can do SD high-speed timing */
#define MMC_CAP_SDIO_IRQ        (1 << 3)        /* Can signal pending SDIO IRQs */
#define MMC_CAP_SPI             (1 << 4)        /* Talks only SPI protocols */
#define MMC_CAP_NEEDS_POLL      (1 << 5)        /* Needs polling for card-detection */
#define MMC_CAP_8_BIT_DATA      (1 << 6)        /* Can the host do 8 bit transfers */

#define MMC_CAP_NONREMOVABLE    (1 << 8)        /* Nonremovable e.g. eMMC */
#define MMC_CAP_WAIT_WHILE_BUSY (1 << 9)        /* Waits while card is busy */
#define MMC_CAP_ERASE           (1 << 10)       /* Allow erase/trim commands */
#define MMC_CAP_1_8V_DDR        (1 << 11)       /* can support */
                                                /* DDR mode at 1.8V */
#define MMC_CAP_1_2V_DDR        (1 << 12)       /* can support */
                                                /* DDR mode at 1.2V */
#define MMC_CAP_POWER_OFF_CARD  (1 << 13)       /* Can power off after boot */
#define MMC_CAP_BUS_WIDTH_TEST  (1 << 14)       /* CMD14/CMD19 bus width ok */
#define MMC_CAP_UHS_SDR12       (1 << 15)       /* Host supports UHS SDR12 mode */
#define MMC_CAP_UHS_SDR25       (1 << 16)       /* Host supports UHS SDR25 mode */
#define MMC_CAP_UHS_SDR50       (1 << 17)       /* Host supports UHS SDR50 mode */
#define MMC_CAP_UHS_SDR104      (1 << 18)       /* Host supports UHS SDR104 mode */
#define MMC_CAP_UHS_DDR50       (1 << 19)       /* Host supports UHS DDR50 mode */
#define MMC_CAP_SET_XPC_330     (1 << 20)       /* Host supports >150mA current at 3.3V */
#define MMC_CAP_SET_XPC_300     (1 << 21)       /* Host supports >150mA current at 3.0V */
#define MMC_CAP_SET_XPC_180     (1 << 22)       /* Host supports >150mA current at 1.8V */
#define MMC_CAP_DRIVER_TYPE_A   (1 << 23)       /* Host supports Driver Type A */
#define MMC_CAP_DRIVER_TYPE_C   (1 << 24)       /* Host supports Driver Type C */
#define MMC_CAP_DRIVER_TYPE_D   (1 << 25)       /* Host supports Driver Type D */
#define MMC_CAP_MAX_CURRENT_200 (1 << 26)       /* Host max current limit is 200mA */
#define MMC_CAP_MAX_CURRENT_400 (1 << 27)       /* Host max current limit is 400mA */
#define MMC_CAP_MAX_CURRENT_600 (1 << 28)       /* Host max current limit is 600mA */
#define MMC_CAP_MAX_CURRENT_800 (1 << 29)       /* Host max current limit is 800mA */
#define MMC_CAP_CMD23           (1 << 30)       /* CMD23 supported. */
#define MMC_CAP_HW_RESET        (1 << 31)       /* Hardware reset */

        uint32_t        caps2;                  /* More host capabilities */

#define MMC_CAP2_BOOTPART_NOACC (1 << 0)        /* Boot partition no access */
#define MMC_CAP2_CACHE_CTRL     (1 << 1)        /* Allow cache control */
#define MMC_CAP2_POWEROFF_NOTIFY (1 << 2)       /* Notify poweroff supported */
#define MMC_CAP2_NO_MULTI_READ  (1 << 3)        /* Multiblock reads don't work */
#define MMC_CAP2_NO_SLEEP_CMD   (1 << 4)        /* Don't allow sleep command */
#define MMC_CAP2_HS200_1_8V_SDR (1 << 5)        /* can support */
#define MMC_CAP2_HS200_1_2V_SDR (1 << 6)        /* can support */
#define MMC_CAP2_HS200          (MMC_CAP2_HS200_1_8V_SDR | MMC_CAP2_HS200_1_2V_SDR)
#define MMC_CAP2_BROKEN_VOLTAGE (1 << 7)        /* Use the broken voltage */
#define MMC_CAP2_DETECT_ON_ERR  (1 << 8)        /* On I/O err check card removal */
#define MMC_CAP2_HC_ERASE_SZ    (1 << 9)        /* High-capacity erase size */

#ifdef CONFIG_SDC_OS_USED
	SDC_Semaphore           lock;
	SDC_Mutex               thread_lock;
#ifdef CONFIG_DETECT_CARD
	SDC_Timer               cd_timer;
#endif
#ifdef CONFIG_SDC_EXCLUSIVE_HOST
	SDC_Semaphore           exclusive_lock; /* lock for claim and bus ops */
#endif
#endif

	//uint8_t                 bus_width;         /* data bus width */
	uint32_t                clk;

#define MMC_BUS_WIDTH_1         0
#define MMC_BUS_WIDTH_4         2
#define MMC_BUS_WIDTH_8         3

	struct mmc_request      *mrq;

#define SDC_WAIT_NONE                   BIT(0)
#define SDC_WAIT_CMD_DONE               BIT(1)
#define SDC_WAIT_DATA_OVER              BIT(2)
#define SDC_WAIT_AUTOCMD_DONE           BIT(3)
#define SDC_WAIT_IDMA_DONE              BIT(4)
#define SDC_WAIT_IDMA_ERR               BIT(5)
#define SDC_WAIT_ERROR                  BIT(6)
#define SDC_WAIT_RXDATA_OVER            (SDC_WAIT_DATA_OVER|SDC_WAIT_IDMA_DONE)
#define SDC_WAIT_RXAUTOCMD_DONE         (SDC_WAIT_AUTOCMD_DONE|SDC_WAIT_IDMA_DONE)
#define SDC_WAIT_SWITCH1V8              BIT(7)
#define SDC_WAIT_FINALIZE               BIT(8)
	volatile uint32_t       smc_cmd;

	uint32_t                wait;
#ifdef CONFIG_SDIO_IRQ_SUPPORT
	uint32_t                sdio_int;
#endif
#ifdef CONFIG_SD_PM
	const struct mmc_bus_ops *bus_ops;      /* current bus driver */
	uint32_t                pm_flags;       /* requested pm features */
	uint32_t                pm_caps;        /* supported pm features */
#endif
#ifdef __CONFIG_ARCH_APP_CORE
	SDC_InitTypeDef         param;
	GPIO_Port               cd_port;
	GPIO_Pin                cd_pin;
#endif
#ifdef CONFIG_SDC_READONLY_USED
	uint32_t                read_only;
	GPIO_PinMuxParam        ro_gpio;
#endif
	SDC_StateTypeDef        State;
};

#define SDC_MAX_CPU_TRANS_LEN           64

//#define SMC_LOW_POWER_MODE            0       /* 1--Close Clock when Idle, 0--Clock always on */
/* IDMA control */
#define IDMAC_DES_MODE                  0       /* 0-chain mode, 1-fix skip length */

#define SMC_RX_WLEVEL                   7
#define SMC_TX_WLEVEL                   8
#define BURST_SIZE                      2

#define IDMA_MAX_TBKNUM_ONETIME         16

/* registers define */
#ifdef __CONFIG_ARCH_APP_CORE
#define SMC0_BASE                       (0x40002000)
#else
#define SMC0_BASE                       (0xA0001000)
#endif

#define SDXC_REG_GCTRL                  (SMC0_BASE + 0x00)      /* SMC Global Control Register */
#define SDXC_REG_CLKCR                  (SMC0_BASE + 0x04)      /* SMC Clock Control Register */
#define SDXC_REG_TMOUT                  (SMC0_BASE + 0x08)      /* SMC Time Out Register */
#define SDXC_REG_WIDTH                  (SMC0_BASE + 0x0C)      /* SMC Bus Width Register */
#define SDXC_REG_BLKSZ                  (SMC0_BASE + 0x10)      /* SMC Block Size Register */
#define SDXC_REG_BCNTR                  (SMC0_BASE + 0x14)      /* SMC Byte Count Register */
#define SDXC_REG_CMDR                   (SMC0_BASE + 0x18)      /* SMC Command Register */
#define SDXC_REG_CARG                   (SMC0_BASE + 0x1C)      /* SMC Argument Register */
#define SDXC_REG_RESP0                  (SMC0_BASE + 0x20)      /* SMC Response Register 0 */
#define SDXC_REG_RESP1                  (SMC0_BASE + 0x24)      /* SMC Response Register 1 */
#define SDXC_REG_RESP2                  (SMC0_BASE + 0x28)      /* SMC Response Register 2 */
#define SDXC_REG_RESP3                  (SMC0_BASE + 0x2C)      /* SMC Response Register 3 */
#define SDXC_REG_IMASK                  (SMC0_BASE + 0x30)      /* SMC Interrupt Mask Register */
#define SDXC_REG_MISTA                  (SMC0_BASE + 0x34)      /* SMC Masked Interrupt Status Register */
#define SDXC_REG_RINTR                  (SMC0_BASE + 0x38)      /* SMC Raw Interrupt Status Register */
#define SDXC_REG_STAS                   (SMC0_BASE + 0x3C)      /* SMC Status Register */
#define SDXC_REG_FTRGL                  (SMC0_BASE + 0x40)      /* SMC FIFO Threshold Watermark Register */
#define SDXC_REG_FUNS                   (SMC0_BASE + 0x44)      /* SMC Function Select Register */
#define SDXC_REG_CBCR                   (SMC0_BASE + 0x48)      /* SMC CIU Byte Count Register */
#define SDXC_REG_BBCR                   (SMC0_BASE + 0x4C)      /* SMC BIU Byte Count Register */
#define SDXC_REG_DBGC                   (SMC0_BASE + 0x50)      /* SMC Debug Enable Register */
#define SDXC_REG_A12A                   (SMC0_BASE + 0x58)      /* SMC auto command 12 argument */
#define SDXC_REG_NTSR                   (SMC0_BASE + 0x5C)      /* SMC NewTiming Set Register(RX TX) */
#define SDXC_REG_SDEG                   (SMC0_BASE + 0x60)      /* SMC NewTiming Set debg */
#define SDXC_REG_HWST                   (SMC0_BASE + 0x78)      /* SMC SMC hardware reset register */
#define SDXC_REG_DMAC                   (SMC0_BASE + 0x80)      /* SMC IDMAC Control Register */
#define SDXC_REG_DLBA                   (SMC0_BASE + 0x84)      /* SMC IDMAC Descriptor List Base Address Register */
#define SDXC_REG_IDST                   (SMC0_BASE + 0x88)      /* SMC IDMAC Status Register */
#define SDXC_REG_IDIE                   (SMC0_BASE + 0x8C)      /* SMC IDMAC Interrupt Enable Register */
#define SDXC_REG_CHDA                   (SMC0_BASE + 0x90)      /* SMC Current Host Descriptor Address Register */
#define SDXC_REG_CBDA                   (SMC0_BASE + 0x94)      /* SMC Current Buffer Descriptor Address Register */
#define SDXC_REG_THLDC                  (SMC0_BASE + 0x100)     /* SMC Threshold Control Register */
#define SDXC_REG_DSBD                   (SMC0_BASE + 0x10C)
#define SDXC_REG_RESP_CRC               (SMC0_BASE + 0x110)
#define SDXC_REG_DAT7_CRC               (SMC0_BASE + 0x114)
#define SDXC_REG_DAT6_CRC               (SMC0_BASE + 0x118)
#define SDXC_REG_DAT5_CRC               (SMC0_BASE + 0x11C)
#define SDXC_REG_DAT4_CRC               (SMC0_BASE + 0x120)
#define SDXC_REG_DAT3_CRC               (SMC0_BASE + 0x124)
#define SDXC_REG_DAT2_CRC               (SMC0_BASE + 0x128)
#define SDXC_REG_DAT1_CRC               (SMC0_BASE + 0x12C)
#define SDXC_REG_DAT0_CRC               (SMC0_BASE + 0x130)
#define SDXC_REG_CRC_STA                (SMC0_BASE + 0x134)
#define SDXC_REG_FIFO                   (SMC0_BASE + 0x200)     /* SMC FIFO Access Address */

#define SDXC_REG_FCTL                   (SMC0_BASE + 0x64)      /* SMC FIFO Access Address */
#define SDXC_REG_FCTL_OS                (0x64)                  /* SMC FIFO Access Address */

/* global control register */
#define SDXC_SoftReset                  BIT(0 )
#define SDXC_FIFOReset                  BIT(1 )
#define SDXC_DMAReset                   BIT(2 )
#define SDXC_HWReset                    (SDXC_SoftReset|SDXC_FIFOReset|SDXC_DMAReset)
#define SDXC_INTEnb                     BIT(4 )
#define SDXC_DMAEnb                     BIT(5 )
#define SDXC_DebounceEnb                BIT(8 )
#define SDXC_DDR_MODE                   BIT(10)
#define SDXC_MemAccessDone              BIT(29)
#define SDXC_AccessDoneDirect           BIT(30)
#define SDXC_ACCESS_BY_AHB              BIT(31)
#define SDXC_ACCESS_BY_DMA              (0x0U << 31)

/* Clock control */
#define SDXC_CardClkOn                  (0x1U << 16)
#define SDXC_LowPowerOn                 (0x1U << 17)

/* bus width */
#define SDXC_WIDTH1                     (0)
#define SDXC_WIDTH4                     (1)
#define SDXC_WIDTH8                     (2)

/* Struct for SMC Commands */
#define SDXC_CMD_OPCODE                 (0x3F )         /* 0x00000040 */
#define SDXC_RspExp                     BIT(6 )         /* 0x00000080 */
#define SDXC_LongRsp                    BIT(7 )         /* 0x00000100 */
#define SDXC_CheckRspCRC                BIT(8 )         /* 0x00000200 */
#define SDXC_DataExp                    BIT(9 )         /* 0x00000000 */
#define SDXC_Read                       (0x0U<<10 )     /* 0x00000400 */
#define SDXC_Write                      BIT(10)         /* 0x00000000 */
#define SDXC_Blockmod                   (0x0U<<11 )     /* 0x00000800 */
#define SDXC_Seqmod                     BIT(11)         /* 0x00001000 */
#define SDXC_SendAutoStop               BIT(12)         /* 0x00002000 */
#define SDXC_WaitPreOver                BIT(13)         /* 0x00004000 */
#define SDXC_StopAbortCMD               BIT(14)         /* 0x00008000 */
#define SDXC_SendInitSeq                BIT(15)         /* 0x00200000 */
#define SDXC_UPCLKOnly                  BIT(21)         /* 0x00400000 */
#define SDXC_RdCEATADev                 BIT(22)         /* 0x00800000 */
#define SDXC_CCSExp                     BIT(23)         /* 0x01000000 */
#define SDXC_EnbBoot                    BIT(24)         /* 0x02000000 */
#define SDXC_AltBootOpt                 BIT(25)         /* 0x00000000 */
#define SDXC_MandBootOpt                (0x0U<<25)      /* 0x04000000 */
#define SDXC_BootACKExp                 BIT(26)         /* 0x08000000 */
#define SDXC_DisableBoot                BIT(27)         /* 0x10000000 */
#define SDXC_VolSwitch                  BIT(28)         /* 0x80000000 */
#define SDXC_Start                      BIT(31)

/* Struct for Intrrrupt Information */
#define SDXC_RespErr                    BIT(1)          /* 0x00000002 */
#define SDXC_CmdDone                    BIT(2)          /* 0x00000004 */
#define SDXC_DataOver                   BIT(3)          /* 0x00000008 */
#define SDXC_TxDataReq                  BIT(4)          /* 0x00000010 */
#define SDXC_RxDataReq                  BIT(5)          /* 0x00000020 */
#define SDXC_RespCRCErr                 BIT(6)          /* 0x00000040 */
#define SDXC_DataCRCErr                 BIT(7)          /* 0x00000080 */
#define SDXC_RespTimeout                BIT(8)          /* 0x00000100 */
#define SDXC_ACKRcv                     BIT(8)          /* 0x00000100 */
#define SDXC_DataTimeout                BIT(9)          /* 0x00000200 */
#define SDXC_BootStart                  BIT(9)          /* 0x00000200 */
#define SDXC_DataStarve                 BIT(10)         /* 0x00000400 */
#define SDXC_VolChgDone                 BIT(10)         /* 0x00000400 */
#define SDXC_FIFORunErr                 BIT(11)         /* 0x00000800 */
#define SDXC_HardWLocked                BIT(12)         /* 0x00001000 */
#define SDXC_StartBitErr                BIT(13)         /* 0x00002000 */
#define SDXC_AutoCMDDone                BIT(14)         /* 0x00004000 */
#define SDXC_EndBitErr                  BIT(15)         /* 0x00008000 */
#define SDXC_SDIOInt                    BIT(16)         /* 0x00010000 */
#define SDXC_CardInsert                 BIT(30)         /* 0x40000000 */
#define SDXC_CardRemove                 BIT(31)         /* 0x80000000 */
#define SDXC_IntErrBit                  (SDXC_RespErr | SDXC_RespCRCErr | SDXC_DataCRCErr | SDXC_RespTimeout | SDXC_DataTimeout | \
                                         SDXC_FIFORunErr | SDXC_HardWLocked | SDXC_StartBitErr | SDXC_EndBitErr) /* 0xbbc2 */
/* status */
#define SDXC_RXWLFlag                   BIT(0)
#define SDXC_TXWLFlag                   BIT(1)
#define SDXC_FIFOEmpty                  BIT(2)
#define SDXC_FIFOFull                   BIT(3)
#define SDXC_CardPresent                BIT(8)
#define SDXC_CardDataBusy               BIT(9)
#define SDXC_DataFSMBusy                BIT(10)
#define SDXC_DMAReq                     BIT(31)

/* Function select */
#define SDXC_CEATAOn                    (0xceaaU << 16)
#define SDXC_SendIrqRsp                 BIT(0)
#define SDXC_SDIORdWait                 BIT(1)
#define SDXC_AbtRdData                  BIT(2)
#define SDXC_SendCCSD                   BIT(8)
#define SDXC_SendAutoStopCCSD           BIT(9)
#define SDXC_CEATADevIntEnb             BIT(10)
/* status bit */
#define SDXC_CardBusy                   BIT(9)
/* IDMA controller bus mod bit field */
#define SDXC_IDMACSoftRST               BIT(0)
#define SDXC_IDMACFixBurst              BIT(1)
#define SDXC_IDMACIDMAOn                BIT(7)
#define SDXC_IDMACRefetchDES            BIT(31)

/* IDMA status bit field */
#define SDXC_IDMACTransmitInt           BIT(0)
#define SDXC_IDMACReceiveInt            BIT(1)
#define SDXC_IDMACFatalBusErr           BIT(2)
#define SDXC_IDMACDesInvalid            BIT(4)
#define SDXC_IDMACCardErrSum            BIT(5)
#define SDXC_IDMACNormalIntSum          BIT(8)
#define SDXC_IDMACAbnormalIntSum        BIT(9)
#define SDXC_IDMACHostAbtInTx           BIT(10)
#define SDXC_IDMACHostAbtInRx           BIT(10)
#define SDXC_IDMACIdle                  (0x0U << 13)
#define SDXC_IDMACSuspend               (0x1U << 13)
#define SDXC_IDMACDESCRd                (0x2U << 13)
#define SDXC_IDMACDESCCheck             (0x3U << 13)
#define SDXC_IDMACRdReqWait             (0x4U << 13)
#define SDXC_IDMACWrReqWait             (0x5U << 13)
#define SDXC_IDMACRd                    (0x6U << 13)
#define SDXC_IDMACWr                    (0x7U << 13)
#define SDXC_IDMACDESCClose             (0x8U << 13)

#define SDXC_IDMA_OVER                  (SDXC_IDMACTransmitInt|SDXC_IDMACReceiveInt|SDXC_IDMACNormalIntSum)
#define SDXC_IDMA_ERR                   (SDXC_IDMACFatalBusErr|SDXC_IDMACDesInvalid|SDXC_IDMACCardErrSum|SDXC_IDMACAbnormalIntSum)

/*
 * the max length of buffer which IDMA description supported is 8192,
 * transport data by several IDMA descriptions if data lenght more than 8192,
 * and the max number IDMA description support is 1024.
 * which meas the mas length transport data is 1024 * 8192 = 8MB in a signal transfer.
 */
#ifdef __CONFIG_ARCH_APP_CORE
#define SDXC_MAX_TRANS_LEN              (1 << 18)       /* max len is 256K */
#else
#define SDXC_MAX_TRANS_LEN              (1 << 14)       /* max len is 16K */
#endif
#define SDXC_DES_NUM_SHIFT              (13)
#define SDXC_DES_BUFFER_MAX_LEN         (1 << SDXC_DES_NUM_SHIFT)  /* 8192 == 1<<13; */
#define SDXC_MAX_DES_NUM                (SDXC_MAX_TRANS_LEN >> SDXC_DES_NUM_SHIFT)      /* 2 is the least */
#define SDXC_DES_MODE                   (0)             /* 0-chain mode, 1-fix length skip */

extern struct mmc_host *_mci_host;

/*
 * These flags are used to describe power management features that
 * some cards (typically SDIO cards) might wish to benefit from when
 * the host system is being suspended.  There are several layers of
 * abstractions involved, from the host controller driver, to the MMC core
 * code, to the SDIO core code, to finally get to the actual SDIO function
 * driver.  This file is therefore used for common definitions shared across
 * all those layers.
 */

#define MMC_PM_KEEP_POWER       (1 << 0)        /* preserve card power during suspend */
#define MMC_PM_WAKE_SDIO_IRQ    (1 << 1)        /* wake up host system on SDIO IRQ assertion */
#define MMC_PM_IGNORE_PM_NOTIFY (1 << 2)        /* ignore mmc pm notify */

#ifdef CONFIG_SD_PM
static inline int mmc_card_keep_power(struct mmc_host *host)
{
	return host->pm_flags & MMC_PM_KEEP_POWER;
}

static inline int mmc_card_wake_sdio_irq(struct mmc_host *host)
{
	return host->pm_flags & MMC_PM_WAKE_SDIO_IRQ;
}
#endif

#ifdef CONFIG_SDC_READONLY_USED
extern int32_t HAL_SDC_Get_ReadOnly(struct mmc_host *host);
#endif
extern void HAL_SDC_Set_BusWidth(struct mmc_host *host, uint32_t width);
extern int32_t HAL_SDC_Update_Clk(struct mmc_host *host, uint32_t clk);
extern uint32_t HAL_SDC_Is_Busy(struct mmc_host *host);
extern int32_t HAL_SDC_Clk_PWR_Opt(struct mmc_host *host, uint32_t oclk_en, uint32_t pwr_save);
#ifdef CONFIG_SDC_EXCLUSIVE_HOST
extern int32_t HAL_SDC_Claim_Host(struct mmc_host *host);
extern void HAL_SDC_Release_Host(struct mmc_host *host);
#else
static inline int32_t HAL_SDC_Claim_Host(struct mmc_host *host) { return 0; }
static inline void HAL_SDC_Release_Host(struct mmc_host *host) { ; }
#endif
extern int32_t HAL_SDC_PowerOn(struct mmc_host *host);
extern int32_t HAL_SDC_PowerOff(struct mmc_host *host);

extern int32_t HAL_SDC_Request(struct mmc_host *host, struct mmc_request *mrq);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CHIP_SDMMC_SDHOST_H_ */
