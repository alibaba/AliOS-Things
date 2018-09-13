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

#ifndef _DRIVER_CHIP_SDMMC_CORE_H_
#define _DRIVER_CHIP_SDMMC_CORE_H_

#include "../hal_base.h"

#include "driver/chip/sdmmc/card.h"

#ifdef CONFIG_USE_SDIO
#include "driver/chip/sdmmc/sdio.h"
#endif
#include "driver/chip/sdmmc/sdmmc.h"

#include "sdhost.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SD_DEBUG 1

#if SD_DEBUG
#define SD_DEBUG_DETAIL 0
#define SD_LOGD(format, args...) HAL_LOG(0, format, ##args)
#define SD_LOGN(format, args...) HAL_LOG(SD_DEBUG_DETAIL, format, ##args)
#define SD_LOGW(format, args...) HAL_LOG(SD_DEBUG, format, ##args)
#define SD_LOGE(format, args...) HAL_LOG(SD_DEBUG, format, ##args)
#define SD_LOGA(format, args...) HAL_LOG(SD_DEBUG, format, ##args)
#if SD_DEBUG_DETAIL
extern void print_hex_dump_bytes(const void *addr, unsigned int len);
#define sd_hex_dump_bytes(a, l) print_hex_dump_bytes(a, l)
#else
#define sd_hex_dump_bytes(a, l)
#endif
#else
#define SD_LOGD(x...)
#define SD_LOGN(x...)
#define SD_LOGW(x...)
#define SD_LOGE(x...)
#define SD_LOGA(x...)
#define sd_hex_dump_bytes(a, l)
#endif
#define SD_BUG_ON(v) do {if(v) {printf("BUG at %s:%d!\n", __func__, __LINE__); while (1);}} while (0)
#define SD_WARN_ON(v) do {if(v) {printf("WARN at %s:%d!\n", __func__, __LINE__);}} while (0)

#define MMC_CMD_RETRIES        3

struct mmc_data {
	uint32_t blksz; 	/* data block size */
	uint32_t blocks;	/* number of blocks */
	uint32_t flags;

#define MMC_DATA_WRITE          (1 << 8)
#define MMC_DATA_READ           (1 << 9)
#define MMC_DATA_STREAM         (1 << 10)

	uint32_t                bytes_xfered;
	uint32_t                sg_len;         /* size of scatter list */
	struct scatterlist      *sg;            /* I/O scatter list */
};

struct mmc_command {
	uint32_t opcode;
	uint32_t arg;
	uint32_t resp[4];
	uint32_t flags;                         /* expected response type */
	/* data transfer */
	volatile uint32_t stop        :1,
	                  boot        :1,
	                  vol_switch  :1;

#define MMC_RSP_MASK            (0x1f << 0)
#define MMC_RSP_PRESENT         (1 << 0)
#define MMC_RSP_136             (1 << 1)        /* 136 bit response */
#define MMC_RSP_CRC             (1 << 2)        /* expect valid crc */
#define MMC_RSP_BUSY            (1 << 3)        /* card may send busy */
#define MMC_RSP_OPCODE          (1 << 4)        /* response contains opcode */

#define MMC_CMD_MASK            (3 << 5)        /* non-SPI command type */
#define MMC_CMD_AC              (0 << 5)        /* addressed comamnd without data transfer */
#define MMC_CMD_ADTC            (1 << 5)        /* addressed command with data transfer */
#define MMC_CMD_BC              (2 << 5)        /* broadcast command without response */
#define MMC_CMD_BCR             (3 << 5)        /* broadcast command with response */

#define MMC_RSP_SPI_S1          (1 << 7)        /* one status byte */
#define MMC_RSP_SPI_S2          (1 << 8)        /* second byte */
#define MMC_RSP_SPI_B4          (1 << 9)        /* four data bytes */
#define MMC_RSP_SPI_BUSY        (1 << 10)       /* card may send busy */

/* These are the native response types, and correspond to valid bit
 * patterns of the above flags.  One additional valid pattern
 * is all zeros, which means we don't expect a response.
 */
#define MMC_RSP_NONE            (0)
#define MMC_RSP_R1              (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R1B             (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE|MMC_RSP_BUSY)
#define MMC_RSP_R2              (MMC_RSP_PRESENT|MMC_RSP_136|MMC_RSP_CRC)
#define MMC_RSP_R3              (MMC_RSP_PRESENT)
#define MMC_RSP_R4              (MMC_RSP_PRESENT)
#define MMC_RSP_R5              (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R6              (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R7              (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)

#define mmc_resp_type(cmd)      ((cmd)->flags & MMC_RSP_MASK)

/*
 * These are the SPI response types for MMC, SD, and SDIO cards.
 * Commands return R1, with maybe more info.  Zero is an error type;
 * callers must always provide the appropriate MMC_RSP_SPI_Rx flags.
 */
#define MMC_RSP_SPI_R1          (MMC_RSP_SPI_S1)
#define MMC_RSP_SPI_R1B         (MMC_RSP_SPI_S1|MMC_RSP_SPI_BUSY)
#define MMC_RSP_SPI_R2          (MMC_RSP_SPI_S1|MMC_RSP_SPI_S2)
#define MMC_RSP_SPI_R3          (MMC_RSP_SPI_S1|MMC_RSP_SPI_B4)
#define MMC_RSP_SPI_R4          (MMC_RSP_SPI_S1|MMC_RSP_SPI_B4)
#define MMC_RSP_SPI_R5          (MMC_RSP_SPI_S1|MMC_RSP_SPI_S2)
#define MMC_RSP_SPI_R7          (MMC_RSP_SPI_S1|MMC_RSP_SPI_B4)

/* These are the command types. */
#define mmc_cmd_type(cmd)       ((cmd)->flags & MMC_CMD_MASK)

	struct mmc_data         *data;          /* data segment associated with cmd */
};

struct mmc_request {
	struct mmc_command      *cmd;
	struct mmc_data         *data;
};

#define UNSTUFF_BITS(resp,start,size)                                   \
	({                                                              \
		const int32_t __size = size;                            \
		const uint32_t __mask = (__size < 32 ? 1 << __size : 0) - 1;    \
		const int32_t __off = 3 - ((start) / 32);                       \
		const int32_t __shft = (start) & 31;                    \
		uint32_t __res;                                         \
		                                                        \
		__res = resp[__off] >> __shft;                          \
		if (__size + __shft > 32)                               \
			__res |= resp[__off-1] << ((32 - __shft) % 32); \
		__res & __mask;                                         \
	})

void mmc_attach_bus(struct mmc_host *host, const struct mmc_bus_ops *ops);
void mmc_detach_bus(struct mmc_host *host);

extern int32_t mmc_align_data_size(struct mmc_card *card, uint32_t sz);
extern int32_t mmc_sd_switch(struct mmc_card *card, uint8_t mode, uint8_t group,
                             uint16_t value, uint8_t *resp);
extern void mmc_enumerate_card_info(struct mmc_card *card);
extern int32_t mmc_switch_to_high_speed(struct mmc_card *card);
extern int32_t mmc_sd_switch_hs(struct mmc_card *card);
extern int32_t mmc_app_set_bus_width(struct mmc_card *card, uint32_t width);
extern int32_t mmc_wait_for_cmd(struct mmc_host *host, struct mmc_command *cmd);
extern int32_t mmc_wait_for_req(struct mmc_host *host, struct mmc_request *mrq);
extern int32_t mmc_attach_sd(struct mmc_card *card, struct mmc_host *host);
extern void mmc_deattach_sd(struct mmc_card *card, struct mmc_host *host);
extern int32_t mmc_select_card(struct mmc_card *card, uint32_t select);
extern int32_t mmc_all_send_cid(struct mmc_host *host, uint32_t *cid);
extern int32_t mmc_send_relative_addr(struct mmc_host *host, uint32_t *rca);
extern void mmc_add_card(struct mmc_card *card);

/**
 * @brief Exclusively claim a host.
 * @note Claim a host for a set of operations.
 * @param host:
 *        @host->mmc host to claim.
 * @retval  None.
 */
static inline void mmc_claim_host(struct mmc_host *host)
{
	HAL_SDC_Claim_Host(host);
}

/**
 * @brief Release a host.
 * @note Release a MMC host, allowing others to claim the host for their operations.
 * @param host:
 *        @host->mmc host to release.
 * @retval  None.
 */
static inline void mmc_release_host(struct mmc_host *host)
{
	HAL_SDC_Release_Host(host);
}

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CHIP_SDMMC_CORE_H_ */
