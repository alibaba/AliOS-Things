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

#include "efpg_i.h"
#include "efpg_debug.h"
#include "driver/chip/hal_efuse.h"

static int efpg_boot_hash_cmp(const uint8_t *data, const uint8_t *buf, uint8_t *err_cnt,
							  uint8_t *err_1st_no, uint8_t *err_2nd_no)
{
	uint8_t byte_cnt;
	uint8_t bit_cnt;

	if (efpg_memcmp(data, buf, EFPG_BOOT_BUF_LEN) == 0)
		return 0;

	*err_cnt = 0;
	for (byte_cnt = 0; byte_cnt < EFPG_BOOT_BUF_LEN; byte_cnt++) {
		if (data[byte_cnt] == buf[byte_cnt])
			continue;

		for (bit_cnt = 0; bit_cnt < 8; bit_cnt++) {
			if ((data[byte_cnt] & (0x1 << bit_cnt)) == (buf[byte_cnt] & (0x1 << bit_cnt)))
				continue;

			if (*err_cnt == 0) {
				*err_1st_no = (byte_cnt << 3) + bit_cnt;
				*err_cnt = 1;
			} else if (*err_cnt == 1) {
				*err_2nd_no = (byte_cnt << 3) + bit_cnt;
				*err_cnt = 2;
			} else {
				return -1;
			}
		}
	}

	return 0;
}

static uint16_t efpg_read_hosc(uint8_t *data)
{
	if (HAL_EFUSE_Read(EFPG_HOSC_TYPE_START, EFPG_HOSC_TYPE_NUM, data) != HAL_OK)
		return EFPG_ACK_RW_ERR;

	return EFPG_ACK_OK;
}

static uint16_t efpg_read_boot(uint8_t *data)
{
	uint8_t tmp = 0;
	uint8_t byte_cnt;
	uint8_t bit_cnt;

	/* flag */
	if (HAL_EFUSE_Read(EFPG_BOOT_FLAG_START, EFPG_BOOT_FLAG_NUM, &tmp) != HAL_OK)
		return EFPG_ACK_RW_ERR;

	if (tmp == 0) {
		EFPG_WARN("%s(), %d, boot flag 0\n", __func__, __LINE__);
		return EFPG_ACK_NODATA_ERR;
	}

	/* hash */
	if (HAL_EFUSE_Read(EFPG_BOOT_HASH_START, EFPG_BOOT_HASH_NUM, data) != HAL_OK)
		return EFPG_ACK_RW_ERR;

	/* correct bit error */
	if (HAL_EFUSE_Read(EFPG_BOOT_1ST_EN_START, EFPG_BOOT_1ST_EN_NUM, &tmp) != HAL_OK)
		return EFPG_ACK_RW_ERR;

	if (tmp != 0) {
		if (HAL_EFUSE_Read(EFPG_BOOT_1ST_NO_START, EFPG_BOOT_1ST_NO_NUM, &tmp) != HAL_OK)
			return EFPG_ACK_RW_ERR;
		byte_cnt = tmp >> 3;
		bit_cnt = tmp & 0x07;
		data[byte_cnt] ^= (0x1 << bit_cnt);

		if (HAL_EFUSE_Read(EFPG_BOOT_2ND_EN_START, EFPG_BOOT_2ND_EN_NUM, &tmp) != HAL_OK)
			return EFPG_ACK_RW_ERR;

		if (tmp != 0) {
			if (HAL_EFUSE_Read(EFPG_BOOT_2ND_NO_START, EFPG_BOOT_2ND_NO_NUM, &tmp) != HAL_OK)
				return EFPG_ACK_RW_ERR;
			byte_cnt = tmp >> 3;
			bit_cnt = tmp & 0x07;
			data[byte_cnt] ^= (0x1 << bit_cnt);
		}
	}

	return EFPG_ACK_OK;
}

static uint16_t efpg_read_dcxo(uint8_t *data)
{
	uint8_t idx = 1;
	uint8_t flag = 0;
	uint32_t start_bit;

	/* flag */
	if (HAL_EFUSE_Read(EFPG_DCXO_FLAG_START, EFPG_DCXO_FLAG_NUM, &flag) != HAL_OK)
		return EFPG_ACK_RW_ERR;

	if (flag == 0) {
		EFPG_WARN("%s(), %d, dcxo flag 0\n", __func__, __LINE__);
		return EFPG_ACK_NODATA_ERR;
	}

	while ((flag & 0x3) == 0) {
		flag = flag >> 2;
		idx++;
	}

	/* DCXO TRIM */
	start_bit = EFPG_DCXO_TRIM_START + (idx - 1) * EFPG_DCXO_TRIM_NUM;
	if (HAL_EFUSE_Read(start_bit, EFPG_DCXO_TRIM_NUM, data) != HAL_OK)
		return EFPG_ACK_RW_ERR;

	return EFPG_ACK_OK;
}

static uint16_t efpg_read_pout(uint8_t *data)
{
	uint8_t idx = 1;
	uint8_t flag = 0;
	uint32_t start_bit;

	/* flag */
	if (HAL_EFUSE_Read(EFPG_POUT_FLAG_START, EFPG_POUT_FLAG_NUM, &flag) != HAL_OK)
		return EFPG_ACK_RW_ERR;

	if (flag == 0) {
		EFPG_WARN("%s(), %d, pout flag 0\n", __func__, __LINE__);
		return EFPG_ACK_NODATA_ERR;
	}

	while ((flag & 0x3) == 0) {
		flag = flag >> 2;
		idx++;
	}

	/* POUT CAL */
	start_bit = EFPG_POUT_CAL_START + (idx - 1) * EFPG_POUT_CAL_NUM;
	if (HAL_EFUSE_Read(start_bit, EFPG_POUT_CAL_NUM, data) != HAL_OK)
		return EFPG_ACK_RW_ERR;

	return EFPG_ACK_OK;
}

static uint16_t efpg_read_mac(uint8_t *data)
{
	uint8_t idx = 1;
	uint32_t flag = 0;
	uint32_t start_bit;

	/* flag */
	if (HAL_EFUSE_Read(EFPG_MAC_FLAG_START, EFPG_MAC_FLAG_NUM, (uint8_t *)&flag) != HAL_OK)
		return EFPG_ACK_RW_ERR;

	if (flag == 0) {
		EFPG_WARN("%s(), %d, mac flag 0\n", __func__, __LINE__);
		return EFPG_ACK_NODATA_ERR;
	}

	while ((flag & 0x3) == 0) {
		flag = flag >> 2;
		idx++;
	}

	/* MAC */
	start_bit = EFPG_MAC_ADDR_START + (idx - 1) * EFPG_MAC_ADDR_NUM;
	if (HAL_EFUSE_Read(start_bit, EFPG_MAC_ADDR_NUM, data) != HAL_OK)
		return EFPG_ACK_RW_ERR;

	return EFPG_ACK_OK;
}

static uint16_t efpg_read_chipid(uint8_t *data)
{
	uint8_t flag = 0;

	/* flag */
	if (HAL_EFUSE_Read(EFPG_CHIPID_FLAG_START, EFPG_CHIPID_FLAG_NUM, &flag) != HAL_OK)
		return EFPG_ACK_RW_ERR;

	/* chipid */
	if (flag == 0) {
		if (HAL_EFUSE_Read(EFPG_CHIPID_1ST_START, EFPG_CHIPID_1ST_NUM, data) != HAL_OK)
			return EFPG_ACK_RW_ERR;
	} else {
		if (HAL_EFUSE_Read(EFPG_CHIPID_2ND_START, EFPG_CHIPID_2ND_NUM, data) != HAL_OK)
			return EFPG_ACK_RW_ERR;
	}

	return EFPG_ACK_OK;
}

static uint16_t efpg_read_user_area(uint16_t start, uint16_t num, uint8_t *data)
{
	if ((start >= EFPG_USER_AREA_NUM)
		|| (num == 0)
		|| (num > EFPG_USER_AREA_NUM)
		|| (start + num > EFPG_USER_AREA_NUM)) {
		EFPG_ERR("start %d, num %d\n", start, num);
		return EFPG_ACK_RW_ERR;
	}

	if (HAL_EFUSE_Read(start + EFPG_USER_AREA_START, num, data) != HAL_OK) {
		EFPG_ERR("eFuse read failed\n");
		return EFPG_ACK_RW_ERR;
	}

	return EFPG_ACK_OK;
}

static uint16_t efpg_write_hosc(uint8_t *data)
{
	uint8_t buf[EFPG_HOSC_BUF_LEN] = {0};

	if ((HAL_EFUSE_Write(EFPG_HOSC_TYPE_START, EFPG_HOSC_TYPE_NUM, data) != HAL_OK)
		|| (HAL_EFUSE_Read(EFPG_HOSC_TYPE_START, EFPG_HOSC_TYPE_NUM, buf) != HAL_OK))
		return EFPG_ACK_RW_ERR;

	if (efpg_memcmp(data, buf, EFPG_HOSC_BUF_LEN)) {
		EFPG_WARN("%s(), %d, hosc: write %#04x, read %#04x\n",
				  __func__, __LINE__, data[0], buf[0]);
		return EFPG_ACK_DI_ERR;
	}

	return EFPG_ACK_OK;
}

static uint16_t efpg_write_boot(uint8_t *data)
{
	uint8_t tmp;
	uint8_t err_cnt = 0;
	uint8_t err_1st_no = 0;
	uint8_t err_2nd_no = 0;
	uint8_t buf[EFPG_BOOT_BUF_LEN] = {0};

	/* flag */
	tmp = 0x03;
	if ((HAL_EFUSE_Write(EFPG_BOOT_FLAG_START, EFPG_BOOT_FLAG_NUM, &tmp) != HAL_OK)
		|| (HAL_EFUSE_Read(EFPG_BOOT_FLAG_START, EFPG_BOOT_FLAG_NUM, &tmp) != HAL_OK))
		return EFPG_ACK_RW_ERR;

	if (tmp == 0) {
		EFPG_WARN("%s(), %d, boot flag 0\n", __func__, __LINE__);
		return EFPG_ACK_DI_ERR;
	}

	/* hash */
	if ((HAL_EFUSE_Write(EFPG_BOOT_HASH_START, EFPG_BOOT_HASH_NUM, data) != HAL_OK)
		|| (HAL_EFUSE_Read(EFPG_BOOT_HASH_START, EFPG_BOOT_HASH_NUM, buf) != HAL_OK))
		return EFPG_ACK_RW_ERR;

	if (efpg_boot_hash_cmp(data, buf, &err_cnt, &err_1st_no, &err_2nd_no) < 0) {
		EFPG_WARN("%s(), %d, boot hash: compare failed\n", __func__, __LINE__);
		return EFPG_ACK_DI_ERR;
	}

	/* error bit */
	if (err_cnt > 0) {
		/* bit en */
		tmp = 0x01;
		if ((HAL_EFUSE_Write(EFPG_BOOT_1ST_EN_START, EFPG_BOOT_1ST_EN_NUM, &tmp) != HAL_OK)
			|| (HAL_EFUSE_Read(EFPG_BOOT_1ST_EN_START, EFPG_BOOT_1ST_EN_NUM, &tmp) != HAL_OK))
			return EFPG_ACK_RW_ERR;

		if (tmp != 0x01) {
			EFPG_WARN("%s(), %d, boot 1st en %d\n", __func__, __LINE__, tmp);
			return EFPG_ACK_DI_ERR;
		}

		/* bit no */
		if ((HAL_EFUSE_Write(EFPG_BOOT_1ST_NO_START, EFPG_BOOT_1ST_NO_NUM, &err_1st_no) != HAL_OK)
			|| (HAL_EFUSE_Read(EFPG_BOOT_1ST_NO_START, EFPG_BOOT_1ST_NO_NUM, &tmp) != HAL_OK))
			return EFPG_ACK_RW_ERR;

		if (err_1st_no != tmp) {
			EFPG_WARN("%s(), %d, boot 1st no: write %d, read %d\n",
					  __func__, __LINE__, err_1st_no, tmp);
			return EFPG_ACK_DI_ERR;
		}

		if (err_cnt == 2) {
			/* bit en */
			tmp = 0x01;
			if ((HAL_EFUSE_Write(EFPG_BOOT_2ND_EN_START, EFPG_BOOT_2ND_EN_NUM, &tmp) != HAL_OK)
				|| (HAL_EFUSE_Read(EFPG_BOOT_2ND_EN_START, EFPG_BOOT_2ND_EN_NUM, &tmp) != HAL_OK))
				return EFPG_ACK_RW_ERR;

			if (tmp != 0x01) {
				EFPG_WARN("%s(), %d, boot 2nd en %d\n", __func__, __LINE__, tmp);
				return EFPG_ACK_DI_ERR;
			}

			/* bit no */
			if ((HAL_EFUSE_Write(EFPG_BOOT_2ND_NO_START, EFPG_BOOT_2ND_NO_NUM, &err_2nd_no) != HAL_OK)
				|| (HAL_EFUSE_Read(EFPG_BOOT_2ND_NO_START, EFPG_BOOT_2ND_NO_NUM, &tmp) != HAL_OK))
				return EFPG_ACK_RW_ERR;

			if (err_2nd_no != tmp) {
				EFPG_WARN("%s(), %d, boot 2nd no: write %d, read %d\n",
						  __func__, __LINE__, err_2nd_no, tmp);
				return EFPG_ACK_DI_ERR;
			}
		}
	}

	return EFPG_ACK_OK;
}

static uint16_t efpg_write_dcxo(uint8_t *data)
{
	uint8_t tmp;
	uint8_t idx = 0;
	uint8_t flag = 0;
	uint8_t buf[EFPG_DCXO_BUF_LEN] = {0};
	uint32_t start_bit;

	if (HAL_EFUSE_Read(EFPG_DCXO_FLAG_START, EFPG_DCXO_FLAG_NUM, &flag) != HAL_OK)
		return EFPG_ACK_RW_ERR;

	while (((flag & 0x3) == 0) && (idx < EFPG_DCXO_IDX_MAX)) {
		flag = flag >> 2;
		idx++;
	}

	while (idx > 0) {
		tmp = 0x3;
		start_bit = EFPG_DCXO_FLAG_START + (idx - 1) * 2;
		if ((HAL_EFUSE_Write(start_bit, 2, &tmp) != HAL_OK)
			|| (HAL_EFUSE_Read(start_bit, 2, &tmp) != HAL_OK))
			return EFPG_ACK_RW_ERR;

		if (tmp == 0) {
			idx--;
			continue;
		}

		start_bit = EFPG_DCXO_TRIM_START + (idx - 1) * EFPG_DCXO_TRIM_NUM;
		if ((HAL_EFUSE_Write(start_bit, EFPG_DCXO_TRIM_NUM, data) != HAL_OK)
			|| (HAL_EFUSE_Read(start_bit, EFPG_DCXO_TRIM_NUM, buf) != HAL_OK))
			return EFPG_ACK_RW_ERR;

		if (efpg_memcmp(data, buf, EFPG_DCXO_BUF_LEN)) {
			idx--;
			continue;
		}

		return EFPG_ACK_OK;
	}

	return EFPG_ACK_DI_ERR;
}

static uint16_t efpg_write_pout(uint8_t *data)
{
	uint8_t tmp;
	uint8_t idx = 0;
	uint8_t flag = 0;
	uint8_t buf[EFPG_POUT_BUF_LEN] = {0};
	uint32_t start_bit;

	if (HAL_EFUSE_Read(EFPG_POUT_FLAG_START, EFPG_POUT_FLAG_NUM, &flag) != HAL_OK)
		return EFPG_ACK_RW_ERR;

	while (((flag & 0x3) == 0) && (idx < EFPG_POUT_IDX_MAX)) {
		flag = flag >> 2;
		idx++;
	}

	while (idx > 0) {
		tmp = 0x3;
		start_bit = EFPG_POUT_FLAG_START + (idx - 1) * 2;
		if ((HAL_EFUSE_Write(start_bit, 2, &tmp) != HAL_OK)
			|| (HAL_EFUSE_Read(start_bit, 2, &tmp) != HAL_OK))
			return EFPG_ACK_RW_ERR;

		if (tmp == 0) {
			idx--;
			continue;
		}

		start_bit = EFPG_POUT_CAL_START + (idx - 1) * EFPG_POUT_CAL_NUM;
		if ((HAL_EFUSE_Write(start_bit, EFPG_POUT_CAL_NUM, data) != HAL_OK)
			|| (HAL_EFUSE_Read(start_bit, EFPG_POUT_CAL_NUM, buf) != HAL_OK))
			return EFPG_ACK_RW_ERR;

		if (efpg_memcmp(data, buf, EFPG_POUT_BUF_LEN)) {
			idx--;
			continue;
		}

		return EFPG_ACK_OK;
	}

	return EFPG_ACK_DI_ERR;
}

static uint16_t efpg_write_mac(uint8_t *data)
{
	uint8_t tmp;
	uint8_t idx = 0;
	uint32_t flag = 0;
	uint8_t buf[EFPG_MAC_BUF_LEN] = {0};
	uint32_t start_bit;

	if (HAL_EFUSE_Read(EFPG_MAC_FLAG_START, EFPG_MAC_FLAG_NUM, (uint8_t *)&flag) != HAL_OK)
		return EFPG_ACK_RW_ERR;

	while (((flag & 0x3) == 0) && (idx < EFPG_MAC_IDX_MAX)) {
		flag = flag >> 2;
		idx++;
	}

	while (idx > 0) {
		tmp = 0x3;
		start_bit = EFPG_MAC_FLAG_START + (idx - 1) * 2;
		if ((HAL_EFUSE_Write(start_bit, 2, &tmp) != HAL_OK)
			|| (HAL_EFUSE_Read(start_bit, 2, &tmp) != HAL_OK))
			return EFPG_ACK_RW_ERR;

		if (tmp == 0) {
			idx--;
			continue;
		}

		start_bit = EFPG_MAC_ADDR_START + (idx - 1) * EFPG_MAC_ADDR_NUM;
		if ((HAL_EFUSE_Write(start_bit, EFPG_MAC_ADDR_NUM, data) != HAL_OK)
			|| (HAL_EFUSE_Read(start_bit, EFPG_MAC_ADDR_NUM, buf) != HAL_OK))
			return EFPG_ACK_RW_ERR;

		if (efpg_memcmp(data, buf, EFPG_MAC_BUF_LEN)) {
			idx--;
			continue;
		}

		return EFPG_ACK_OK;
	}

	return EFPG_ACK_DI_ERR;
}

static uint16_t efpg_write_user_area(uint16_t start, uint16_t num, uint8_t *data)
{
	if ((start >= EFPG_USER_AREA_NUM)
		|| (num == 0)
		|| (num > EFPG_USER_AREA_NUM)
		|| (start + num > EFPG_USER_AREA_NUM)) {
		EFPG_ERR("start %d, num %d\n", start, num);
		return EFPG_ACK_RW_ERR;
	}

	if (HAL_EFUSE_Write(start + EFPG_USER_AREA_START, num, data) != HAL_OK) {
		EFPG_ERR("eFuse write failed\n");
		return EFPG_ACK_RW_ERR;
	}

	return EFPG_ACK_OK;
}

uint16_t efpg_read_field(efpg_field_t field, uint8_t *data, uint16_t start_bit_addr, uint16_t bit_len)
{
	switch (field) {
	case EFPG_FIELD_HOSC:
		return efpg_read_hosc(data);
	case EFPG_FIELD_BOOT:
		return efpg_read_boot(data);
	case EFPG_FIELD_DCXO:
		return efpg_read_dcxo(data);
	case EFPG_FIELD_POUT:
		return efpg_read_pout(data);
	case EFPG_FIELD_MAC:
		return efpg_read_mac(data);
	case EFPG_FIELD_CHIPID:
		return efpg_read_chipid(data);
	case EFPG_FIELD_UA:
		return efpg_read_user_area(start_bit_addr, bit_len, data);
	default:
		EFPG_WARN("%s(), %d, read field %d\n", __func__, __LINE__, field);
		return EFPG_ACK_RW_ERR;
	}
}

uint16_t efpg_write_field(efpg_field_t field, uint8_t *data, uint16_t start_bit_addr, uint16_t bit_len)
{
	switch (field) {
	case EFPG_FIELD_HOSC:
		return efpg_write_hosc(data);
	case EFPG_FIELD_BOOT:
		return efpg_write_boot(data);
	case EFPG_FIELD_DCXO:
		return efpg_write_dcxo(data);
	case EFPG_FIELD_POUT:
		return efpg_write_pout(data);
	case EFPG_FIELD_MAC:
		return efpg_write_mac(data);
	case EFPG_FIELD_UA:
		return efpg_write_user_area(start_bit_addr, bit_len, data);
	default:
		EFPG_WARN("%s(), %d, write field %d\n", __func__, __LINE__, field);
		return EFPG_ACK_RW_ERR;
	}
}
