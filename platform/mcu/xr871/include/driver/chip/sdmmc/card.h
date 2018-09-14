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

#ifndef _DRIVER_CHIP_HAL_SDMMC_CARD_H_
#define _DRIVER_CHIP_HAL_SDMMC_CARD_H_

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_USE_SD

struct mmc_ocr {
	union {
		uint32_t vol_window      : 24,
		         to_1v8_acpt     : 1,
		                         : 5,
		         high_capacity   : 1,
		                         : 1;
		uint32_t ocr;
	};
};

struct mmc_cid {
	uint16_t   oemid;
	uint8_t    manfid;
	uint32_t   serial;
	uint16_t   year;
	uint8_t    month;
	uint8_t    hwrev;
	uint8_t    fwrev;
	uint8_t    prod_name[6];
};

struct mmc_csd {
	uint8_t    csd_ver;
	uint8_t    read_blk_len;
	uint8_t    c_size_mult;
	uint32_t   max_dtr;
	uint16_t   c_size;
	uint16_t   cmdclass;
	uint32_t capacity;
};

struct sd_scr {
	uint8_t sda_vsn;
	uint8_t sda_spec3;
	uint8_t sda_spec4;
	uint8_t sda_spec5;
	uint8_t bus_widths;
	uint8_t security_sup;
#define SD_SCR_BUS_WIDTH_1      (1<<0)
#define SD_SCR_BUS_WIDTH_4      (1<<2)
	uint8_t cmds;
#define SD_SCR_CMD20_SUPPORT    (1<<0)
#define SD_SCR_CMD23_SUPPORT    (1<<1)
};

struct sd_switch_caps {
	uint32_t                hs_max_dtr;
#define HIGH_SPEED_MAX_DTR      50000000
#define UHS_SDR104_MAX_DTR      208000000
#define UHS_SDR50_MAX_DTR       100000000
#define UHS_DDR50_MAX_DTR       50000000
#define UHS_SDR25_MAX_DTR       UHS_DDR50_MAX_DTR
#define UHS_SDR12_MAX_DTR       25000000
	uint32_t                uhs_max_dtr;
#define UHS_SDR12_BUS_SPEED     0
#define HIGH_SPEED_BUS_SPEED    1
#define UHS_SDR25_BUS_SPEED     1
#define UHS_SDR50_BUS_SPEED     2
#define UHS_SDR104_BUS_SPEED    3
#define UHS_DDR50_BUS_SPEED     4
#define SD_MODE_HIGH_SPEED      (1 << HIGH_SPEED_BUS_SPEED)
#define SD_MODE_UHS_SDR12       (1 << UHS_SDR12_BUS_SPEED)
#define SD_MODE_UHS_SDR25       (1 << UHS_SDR25_BUS_SPEED)
#define SD_MODE_UHS_SDR50       (1 << UHS_SDR50_BUS_SPEED)
#define SD_MODE_UHS_SDR104      (1 << UHS_SDR104_BUS_SPEED)
#define SD_MODE_UHS_DDR50       (1 << UHS_DDR50_BUS_SPEED)
	uint32_t                sd3_bus_mode;

#define SD_DRIVER_TYPE_B        0x01
#define SD_DRIVER_TYPE_A        0x02
#define SD_DRIVER_TYPE_C        0x04
#define SD_DRIVER_TYPE_D        0x08
	uint32_t                sd3_drv_type;
#define SD_SET_CURRENT_LIMIT_200        0
#define SD_SET_CURRENT_LIMIT_400        1
#define SD_SET_CURRENT_LIMIT_600        2
#define SD_SET_CURRENT_LIMIT_800        3

#define SD_MAX_CURRENT_200      (1 << SD_SET_CURRENT_LIMIT_200)
#define SD_MAX_CURRENT_400      (1 << SD_SET_CURRENT_LIMIT_400)
#define SD_MAX_CURRENT_600      (1 << SD_SET_CURRENT_LIMIT_600)
#define SD_MAX_CURRENT_800      (1 << SD_SET_CURRENT_LIMIT_800)
	uint32_t                sd3_curr_limit;
};

struct mmc_ext_csd {
	uint8_t    version;
	uint8_t    card_type;
	uint8_t    csd_struc;
	uint8_t    hs_timing;
	uint8_t    bus_width;
	uint8_t    part_config;
	uint8_t    boot_bus_cond;
};

struct mmc_host;

#define SDIO_MAX_FUNCS		7

struct mmc_card {
	/*  register info. */
	struct mmc_cid          cid;
	struct mmc_csd          csd;
	struct sd_scr           scr;
	struct mmc_ext_csd      extcsd;
	struct sd_switch_caps   sw_caps;
	/*  card information  */
#define MMC_TYPE_MMC                    0               
#define MMC_TYPE_SD                     1               
#define MMC_TYPE_SDIO                   2               
#define MMC_TYPE_SD_COMBO               3               
	uint32_t type;                                  

	uint32_t                sd_bus_speed;           
#define MMC_STATE_PRESENT               (1 << 0)        
#define MMC_STATE_READONLY              (1 << 1)        
#define MMC_STATE_HIGHSPEED             (1 << 2)        
#define MMC_STATE_BLOCKADDR             (1 << 3)        
#define MMC_STATE_HIGHSPEED_DDR         (1 << 4)        
#define MMC_STATE_ULTRAHIGHSPEED        (1<<5)          
#define MMC_CARD_SDXC                   (1<<6)          
#define MMC_CARD_REMOVED                (1<<7)          
#define MMC_STATE_HIGHSPEED_200         (1<<8)          
#define MMC_STATE_SLEEP                 (1<<9)          

	uint32_t                state;                  

	uint8_t         bus_width;
	uint8_t         speed_class;
	uint32_t        cidno[4];

	uint32_t        rca;                           /* relative card address of device */
	struct mmc_ocr  ocr;
	struct mmc_host *host;                         /* the host this device belongs to */
};

#define mmc_card_mmc(c)         ((c)->type == MMC_TYPE_MMC)
#define mmc_card_sd(c)          ((c)->type == MMC_TYPE_SD)
#define mmc_card_sdio(c)        ((c)->type == MMC_TYPE_SDIO)

#define mmc_card_present(c)     ((c)->state & MMC_STATE_PRESENT)
#define mmc_card_readonly(c)    ((c)->state & MMC_STATE_READONLY)
#define mmc_card_highspeed(c)   ((c)->state & MMC_STATE_HIGHSPEED)
#define mmc_card_hs200(c)       ((c)->state & MMC_STATE_HIGHSPEED_200)
#define mmc_card_blockaddr(c)   ((c)->state & MMC_STATE_BLOCKADDR)
#define mmc_card_ddr_mode(c)    ((c)->state & MMC_STATE_HIGHSPEED_DDR)
#define mmc_card_uhs(c)         ((c)->state & MMC_STATE_ULTRAHIGHSPEED)
#define mmc_sd_card_uhs(c)      ((c)->state & MMC_STATE_ULTRAHIGHSPEED)
#define mmc_card_ext_capacity(c) ((c)->state & MMC_CARD_SDXC)
#define mmc_card_removed(c)     ((c) && ((c)->state & MMC_CARD_REMOVED))
#define mmc_card_is_sleep(c)    ((c)->state & MMC_STATE_SLEEP)

#define mmc_card_set_present(c) ((c)->state |= MMC_STATE_PRESENT)
#define mmc_card_set_readonly(c) ((c)->state |= MMC_STATE_READONLY)
#define mmc_card_set_highspeed(c) ((c)->state |= MMC_STATE_HIGHSPEED)
#define mmc_card_set_hs200(c)   ((c)->state |= MMC_STATE_HIGHSPEED_200)
#define mmc_card_set_blockaddr(c) ((c)->state |= MMC_STATE_BLOCKADDR)
#define mmc_card_set_ddr_mode(c) ((c)->state |= MMC_STATE_HIGHSPEED_DDR)
#define mmc_card_set_uhs(c) ((c)->state |= MMC_STATE_ULTRAHIGHSPEED)
#define mmc_sd_card_set_uhs(c) ((c)->state |= MMC_STATE_ULTRAHIGHSPEED)
#define mmc_card_set_ext_capacity(c) ((c)->state |= MMC_CARD_SDXC)
#define mmc_card_set_removed(c) ((c)->state |= MMC_CARD_REMOVED)
#define mmc_card_set_sleep(c)   ((c)->state |= MMC_STATE_SLEEP)

#define mmc_card_clr_sleep(c)   ((c)->state &= ~MMC_STATE_SLEEP)

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CHIP_HAL_SDMMC_CARD_H_ */
