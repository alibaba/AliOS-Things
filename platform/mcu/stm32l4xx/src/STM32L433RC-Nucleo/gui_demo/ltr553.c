#include "stm32l4xx_hal.h"
#include "soc_init.h"

#define LTR553_I2C_ADDR 0x46
#define LTR553_WAIT_TIME 100
#define LTR553_WAIT_INTERVAL 50

#ifdef ALIOS_HAL
static int alsreg_mask_write(uint8_t reg, uint8_t mask, uint8_t data)
{
	uint8_t tdata = 0;
	uint8_t *pd = &tdata;
	int ret = 0;

	ret = hal_i2c_mem_read(&brd_i2c2_dev, LTR553_I2C_ADDR, (uint16_t)reg, 1, pd, 1, 1000);
	if (ret != 0)
		return ret;
	tdata &= ~mask;
	tdata |= data & mask;
	ret = hal_i2c_mem_write(&brd_i2c2_dev, LTR553_I2C_ADDR, (uint16_t)reg, 1, pd, 1, 1000);
	return ret;
}

static int alsreg_write(uint8_t reg, uint8_t data)
{
	uint8_t tdata = data;
	uint8_t *pd = &tdata;

	return hal_i2c_mem_write(&brd_i2c2_dev, LTR553_I2C_ADDR, (uint16_t)reg, 1, pd, 1, 1000);
}

static int alsreg_read(uint8_t reg, uint8_t *data)
{
	return  hal_i2c_mem_read(&brd_i2c2_dev, LTR553_I2C_ADDR, (uint16_t)reg, 1, data, 1, 1000);
}
#else
extern I2C_HandleTypeDef hi2c2;

static int alsreg_mask_write(uint8_t reg, uint8_t mask, uint8_t data)
{
	uint8_t tdata = 0;
	uint8_t *pd = &tdata;
	int ret = 0;

	ret = (int)HAL_I2C_Mem_Read(&hi2c2, LTR553_I2C_ADDR, (uint16_t)reg, 1, pd, 1, HAL_MAX_DELAY);
	if (ret != 0)
		return ret;
	tdata &= ~mask;
	tdata |= data & mask;
	ret = (int)HAL_I2C_Mem_Write(&hi2c2, LTR553_I2C_ADDR, (uint16_t)reg, 1, pd, 1, HAL_MAX_DELAY);
	return ret;
}

static int alsreg_write(uint8_t reg, uint8_t data)
{
	uint8_t tdata = data;
	uint8_t *pd = &tdata;

	return (int)HAL_I2C_Mem_Write(&hi2c2, LTR553_I2C_ADDR, (uint16_t)reg, 1, pd, 1, HAL_MAX_DELAY);
}

static int alsreg_read(uint8_t reg, uint8_t *data)
{
	return (int)HAL_I2C_Mem_Read(&hi2c2, LTR553_I2C_ADDR, (uint16_t)reg, 1, data, 1, HAL_MAX_DELAY);
}
#endif

static int ltr553_is_ready(FLAG_ALS_PS flag)
{
	uint8_t data = 0;
	const uint8_t als_ready_mask = 0x84;
	const uint8_t ps_ready_mask = 0x81;
	const uint8_t both_ready_mask = 0x85;
	int res = 0;

	res = alsreg_read(LTR553_ALS_PS_STATUS, &data);
	if (res != 0)
		return 0;
	switch ((int)flag) {
		case FLAG_ALS:
			res = (data & als_ready_mask) == 0x4 ? 1 : 0;
			break;
		case FLAG_PS:
			res = (data & ps_ready_mask) == 0x1 ? 1 : 0;
			break;
		case FLAG_BOTH:
			res = (data & both_ready_mask) == 0x5 ? 1 : 0;
			break;
		default:
			res = 0;
	}

	return res;
}

int ltr553_get_adc_als_data(uint16_t *ch0, uint16_t *ch1)
{
	int wait_count = LTR553_WAIT_TIME;
	uint8_t reg_ch0[2] = {0};
	uint8_t reg_ch1[2] = {0};
	int res = 0;

	if (ch0 == NULL || ch1 == NULL)
		return -1;

	do {
		res = ltr553_is_ready(FLAG_ALS);
		if (res)
			break;
		wait_count -= LTR553_WAIT_INTERVAL;
	} while (wait_count > 0);

	if (res != 1)
		return -1;

	res = 0;
	res |= alsreg_read(LTR553_ALS_DATA1_L, &reg_ch1[0]);
	res |= alsreg_read(LTR553_ALS_DATA1_H, &reg_ch1[1]);
	res |= alsreg_read(LTR553_ALS_DATA0_L, &reg_ch0[0]);
	res |= alsreg_read(LTR553_ALS_DATA0_H, &reg_ch0[1]);

	if (res != 0)
		return -1;

	*ch0 = (uint16_t)reg_ch0[1] << 8 | reg_ch0[0];
	*ch1 = (uint16_t)reg_ch1[1] << 8 | reg_ch1[0];

	return 0;
}

int ltr553_get_adc_ps_data(uint16_t *data)
{
	int wait_count = LTR553_WAIT_TIME;
	uint8_t reg_data[2] = {0};
	int res = 0;

	if (data == NULL)
		return -1;

	do {
		res = ltr553_is_ready(FLAG_PS);
		if (res)
			break;
		wait_count -= LTR553_WAIT_INTERVAL;
	} while (wait_count > 0);

	if (res != 1)
		return -1;

	res = 0;
	res |= alsreg_read(LTR553_PS_DATA_L, &reg_data[0]);
	res |= alsreg_read(LTR553_PS_DATA_H, &reg_data[1]);

	if (res != 0)
		return -1;

	*data = (uint16_t)reg_data[1] << 8 | reg_data[0];

	return 0;
}

int ltr553_init(FLAG_ALS_PS flag)
{
	int res = 0;
	uint8_t part_id = 0;
	uint8_t manufac_id = 0;

	res |= alsreg_read(LTR553_PART_ID, &part_id);
	res |= alsreg_read(LTR553_MANUFAC_ID, &manufac_id);
	if (res != 0 || part_id != 0x92 || manufac_id != 0x5)
	{
		_Error_Handler(__FILE__, __LINE__);
		return res;
	}
	switch (flag) {
		case FLAG_ALS:
			res |= alsreg_write(LTR553_ALS_MEAS_RATE, AIT_TIME_100 << 3 | AMR_RATE_500);
			res |= alsreg_mask_write(LTR553_ALS_CONTR, 0x1d, AG_GAIN_1X << 2 | 0x1);
			break;
		case FLAG_PS:
			res |= alsreg_write(LTR553_PS_MEAS_RATE, PMR_RATE_100);
			res |= alsreg_write(LTR553_PS_LED, LPMF_PERIOD_60K << 5 | LCD_PER_100 << 3 | LC_LEVEL_100);
			res |= alsreg_mask_write(LTR553_PS_CONTR, 0xf, PG_GAIN_X16 << 2 | 0x2);
			break;
		case FLAG_BOTH:
			res |= alsreg_write(LTR553_ALS_MEAS_RATE, AIT_TIME_100 << 3 | AMR_RATE_500);
			res |= alsreg_mask_write(LTR553_ALS_CONTR, 0x1d, AG_GAIN_1X << 2 | 0x1);
			res |= alsreg_write(LTR553_PS_MEAS_RATE, PMR_RATE_100);
			res |= alsreg_write(LTR553_PS_LED, LPMF_PERIOD_60K << 5 | LCD_PER_100 << 3 | LC_LEVEL_100);
			res |= alsreg_mask_write(LTR553_PS_CONTR, 0xf, PG_GAIN_X16 << 2 | 0x2);
		default:
			return -1;
	}
	return res;
}
