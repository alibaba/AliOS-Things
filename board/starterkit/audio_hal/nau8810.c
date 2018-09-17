#include <k_api.h>
#include <stdio.h>
#include "nau8810.h"
#include "stm32l4xx_hal.h"
#include "soc_init.h"

#define NAU8810_I2C_SLAVE_ADDR                            0x1a
#define NAU8810_ADDR_TRANS(n)                             ((n) << 1)
#define NAU8810_I2C_ADDR                                  NAU8810_ADDR_TRANS(NAU8810_I2C_SLAVE_ADDR)

#define AUDIO_NAU8810_REG_RESET                                 0x00
#define AUDIO_NAU8810_REG_POWER1                                0x01
#define AUDIO_NAU8810_REG_POWER2                                0x02
#define AUDIO_NAU8810_REG_POWER3                                0x03
#define AUDIO_NAU8810_REG_IFACE                                 0x04
#define AUDIO_NAU8810_REG_COMP                                  0x05
#define AUDIO_NAU8810_REG_CLOCK                                 0x06
#define AUDIO_NAU8810_REG_SMPLR                                 0x07
#define AUDIO_NAU8810_REG_DAC                                   0x0A
#define AUDIO_NAU8810_REG_DACGAIN                               0x0B
#define AUDIO_NAU8810_REG_ADC                                   0x0E
#define AUDIO_NAU8810_REG_ADCGAIN                               0x0F
#define AUDIO_NAU8810_REG_EQ1                                   0x12
#define AUDIO_NAU8810_REG_EQ2                                   0x13
#define AUDIO_NAU8810_REG_EQ3                                   0x14
#define AUDIO_NAU8810_REG_EQ4                                   0x15
#define AUDIO_NAU8810_REG_EQ5                                   0x16
#define AUDIO_NAU8810_REG_DACLIM1                               0x18
#define AUDIO_NAU8810_REG_DACLIM2                               0x19
#define AUDIO_NAU8810_REG_NOTCH1                                0x1B
#define AUDIO_NAU8810_REG_NOTCH2                                0x1C
#define AUDIO_NAU8810_REG_NOTCH3                                0x1D
#define AUDIO_NAU8810_REG_NOTCH4                                0x1E
#define AUDIO_NAU8810_REG_ALC1                                  0x20
#define AUDIO_NAU8810_REG_ALC2                                  0x21
#define AUDIO_NAU8810_REG_ALC3                                  0x22
#define AUDIO_NAU8810_REG_NOISEGATE                             0x23
#define AUDIO_NAU8810_REG_PLLN                                  0x24
#define AUDIO_NAU8810_REG_PLLK1                                 0x25
#define AUDIO_NAU8810_REG_PLLK2                                 0x26
#define AUDIO_NAU8810_REG_PLLK3                                 0x27
#define AUDIO_NAU8810_REG_ATTEN                                 0x28
#define AUDIO_NAU8810_REG_INPUT_SIGNAL                          0x2C
#define AUDIO_NAU8810_REG_PGAGAIN                               0x2D
#define AUDIO_NAU8810_REG_ADCBOOST                              0x2F
#define AUDIO_NAU8810_REG_OUTPUT                                0x31
#define AUDIO_NAU8810_REG_SPKMIX                                0x32
#define AUDIO_NAU8810_REG_SPKGAIN                               0x36
#define AUDIO_NAU8810_REG_MONOMIX                               0x38
#define AUDIO_NAU8810_REG_POWER4                                0x3A
#define AUDIO_NAU8810_REG_TSLOTCTL1                             0x3B
#define AUDIO_NAU8810_REG_TSLOTCTL2                             0x3C
#define AUDIO_NAU8810_REG_DEVICE_REVID                          0x3E
#define AUDIO_NAU8810_REG_I2C_DEVICEID                          0x3F
#define AUDIO_NAU8810_REG_ADDITIONID                            0x40
#define AUDIO_NAU8810_REG_RESERVE                               0x41
#define AUDIO_NAU8810_REG_OUTCTL                                0x45
#define AUDIO_NAU8810_REG_ALC1ENHAN1                            0x46
#define AUDIO_NAU8810_REG_ALC1ENHAN2                            0x47
#define AUDIO_NAU8810_REG_MISCCTL                               0x49
#define AUDIO_NAU8810_REG_OUTTIEOFF                             0x4B
#define AUDIO_NAU8810_REG_AGCP2POUT                             0x4C
#define AUDIO_NAU8810_REG_AGCPOUT                               0x4D
#define AUDIO_NAU8810_REG_AMTCTL                                0x4E
#define AUDIO_NAU8810_REG_OUTTIEOFFMAN                          0x4F
#define NAU8810_REG_MAX                                   AUDIO_NAU8810_REG_OUTTIEOFFMAN

#define NAU8810_I2C_DEVICEID_VAL                          0x01a
#define NAU8810_ADDITIONID_VAL                            0x0ca


static int nau8810_reg_read(uint8_t reg, uint16_t *pdata)
{
	uint8_t reg_raw = 0;
	uint8_t buf[2] = {0};
	int ret = 0;
	
	if (pdata == NULL) {
		KIDS_A10_PRT("parameters is invalid\n");
		return -1;
	}
	reg_raw = reg << 1 & 0xfe;
	ret = hal_i2c_mem_read(&brd_i2c2_dev, NAU8810_I2C_ADDR, (uint16_t)reg_raw, 1, buf, 2, 1000);
	if (ret != 0) {
		KIDS_A10_PRT("hal_i2c_mem_read return %d\n", reg);
		return ret;
	}
	*pdata = (uint16_t)((buf[0] << 8 & 0xff00) | (buf[1] & 0xff));
	
	return 0;
}

static int nau8810_reg_write(uint8_t reg, uint16_t data)
{
	uint8_t reg_raw = 0;
	uint8_t data_raw = 0;
	
	reg_raw = reg << 1;
	reg_raw |= (uint8_t)(data >> 1 & 0x01);
	data_raw = (uint8_t)(data & 0xff);
	
	return hal_i2c_mem_write(&brd_i2c2_dev, NAU8810_I2C_ADDR, (uint16_t)reg_raw, 1, &data_raw, 1, 1000);
}

static int drv_codec_nau8810_validate_id(void)
{
	int ret               = 0;
	uint16_t device_revid = 0;
	uint16_t i2c_deviceid = 0;
	uint16_t additionid   = 0;
	
	ret = nau8810_reg_read(AUDIO_NAU8810_REG_I2C_DEVICEID, &i2c_deviceid);
	if (ret != 0) {
		KIDS_A10_PRT("nau8810_reg_read return %d\n", ret);
		return ret;
	}
	if (i2c_deviceid != NAU8810_I2C_DEVICEID_VAL) {
		KIDS_A10_PRT("read AUDIO_NAU8810_REG_I2C_DEVICEID: %hu\n", i2c_deviceid);
		return -1;
	}
	
	ret = nau8810_reg_read(AUDIO_NAU8810_REG_ADDITIONID, &additionid);
	if (ret != 0) {
		KIDS_A10_PRT("nau8810_reg_read return %d\n", ret);
		return ret;
	}
	if (additionid != NAU8810_ADDITIONID_VAL) {
		KIDS_A10_PRT("read AUDIO_NAU8810_REG_ADDITIONID: %hu\n", additionid);
		return -1;
	}
	
	return 0;
}

#if 0
static int drv_codec_nau8810_set_da_config(void)
{
	int ret = 0;
	
	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_RESET, 0x000);                  /* Reset all registers */ 
	krhino_task_sleep(krhino_ms_to_ticks(50));

	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_POWER1, 0x009);
	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_POWER2, 0x000);                 /* Enable L/R Headphone, ADC Mix/Boost, ADC */

	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_POWER3, 0x0fd);                 /* Enable L/R main mixer, DAC */			
	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_IFACE, 0x010);                  /* 16-bit word length, I2S format, Stereo */			
	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_COMP, 0x000);                   /* Companding control and loop back mode (all disable) */

	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_CLOCK, 0x000);                  /* Slave mode */
	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_SMPLR, 0x006);                  /* 16K for internal filter cofficients */

	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_DAC, 0x000);                    /* DAC softmute is disabled, DAC oversampling rate is 128x */
	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_DACGAIN, 0x0ff);                /* ADC left digital volume control */

	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_OUTPUT, 0x002);
	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_SPKMIX, 0x001);                 /* Left DAC connected to LMIX */

	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_SPKGAIN, 0x039);                /* LSPKOUT Volume */
	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_MONOMIX, 0x001);

	if (ret != 0) {
		KIDS_A10_PRT("nau8810_reg_write return %d\n", ret);
	}

	return ret;
}

static int drv_codec_nau8810_set_ad_config(void)
{
	int ret = 0;
	
	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_RESET, 0x000);                  /* Reset all registers */ 
	krhino_task_sleep(krhino_ms_to_ticks(50));

	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_POWER1, 0x019);
	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_POWER2, 0x015);                 /* Enable L/R Headphone, ADC Mix/Boost, ADC */

	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_POWER3, 0x000);                 /* Enable L/R main mixer, DAC */			
	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_IFACE, 0x010);                  /* 16-bit word length, I2S format, Stereo */			
	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_COMP, 0x000);                   /* Companding control and loop back mode (all disable) */

	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_CLOCK, 0x000);                  /* Slave mode */
	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_SMPLR, 0x006);                  /* 16K for internal filter cofficients */

	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_ADC, 0x000);                    /* DAC softmute is disabled, DAC oversampling rate is 128x */
	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_ADCGAIN, 0x0ff);                /* ADC left digital volume control */

	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_INPUT_SIGNAL, 0x003);
	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_PGAGAIN, 0x010);
	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_ADCBOOST, 0x100);

	if (ret != 0) {
		KIDS_A10_PRT("nau8810_reg_write return %d\n", ret);
	}

	return ret;
}
#endif

static int drv_codec_nau8810_set_adda_config(void)
{
	int ret = 0;
	
	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_RESET, 0x000);                  /* Reset all registers */ 
	krhino_task_sleep(krhino_ms_to_ticks(50));

	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_POWER1, 0x019);
	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_POWER2, 0x015);                 /* Enable L/R Headphone, ADC Mix/Boost, ADC */

	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_POWER3, 0x0fd);                 /* Enable L/R main mixer, DAC */			
	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_IFACE, 0x010);                  /* 16-bit word length, I2S format, Stereo */			
	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_COMP, 0x03e);                   /* Companding control and loop back mode (all disable) */

	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_CLOCK, 0x000);                  /* Slave mode */
	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_SMPLR, 0x006);                  /* 16K for internal filter cofficients */

	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_ADC, 0x000);                    /* DAC softmute is disabled, DAC oversampling rate is 128x */
	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_ADCGAIN, 0x0ff);                /* ADC left digital volume control */

	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_INPUT_SIGNAL, 0x003);
	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_PGAGAIN, 0x03f);
	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_ADCBOOST, 0x100);

	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_DAC, 0x000);                    /* DAC softmute is disabled, DAC oversampling rate is 128x */
	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_DACGAIN, 0x0ff);                /* ADC left digital volume control */

	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_OUTPUT, 0x002);
	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_SPKMIX, 0x001);                 /* Left DAC connected to LMIX */

	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_SPKGAIN, 0x039);                /* LSPKOUT Volume */
	ret |= nau8810_reg_write(AUDIO_NAU8810_REG_MONOMIX, 0x001);

	if (ret != 0) {
		KIDS_A10_PRT("nau8810_reg_write return %d\n", ret);
	}

	return ret;
}

static int drv_codec_nau8810_set_loopback(void)
{
	int ret = 0;
	uint16_t reg_val = 0;
	
	ret = nau8810_reg_read(AUDIO_NAU8810_REG_COMP, &reg_val);
	if (ret != 0) {
		KIDS_A10_PRT("nau8810_reg_read return %d\n", ret);
		return ret;
	}
	reg_val |= 1 << 0;
	ret = nau8810_reg_write(AUDIO_NAU8810_REG_COMP, reg_val);
	if (ret != 0) {
		KIDS_A10_PRT("nau8810_reg_write return %d\n", ret);
		return ret;
	}
	
	ret = nau8810_reg_read(AUDIO_NAU8810_REG_SPKMIX, &reg_val);
	if (ret != 0) {
		KIDS_A10_PRT("nau8810_reg_read return %d\n", ret);
		return ret;
	}
	reg_val |= 1 << 1;
	ret = nau8810_reg_write(AUDIO_NAU8810_REG_SPKMIX, reg_val);
	if (ret != 0) {
		KIDS_A10_PRT("nau8810_reg_write return %d\n", ret);
		return ret;
	}
	
	ret = nau8810_reg_read(AUDIO_NAU8810_REG_MONOMIX, &reg_val);
	if (ret != 0) {
		KIDS_A10_PRT("nau8810_reg_read return %d\n", ret);
		return ret;
	}
	reg_val |= 1 << 1;
	ret = nau8810_reg_write(AUDIO_NAU8810_REG_MONOMIX, reg_val);
	if (ret != 0) {
		KIDS_A10_PRT("nau8810_reg_write return %d\n", ret);
		return ret;
	}
	
	return 0;
}

static int drv_codec_nau8810_set_spk_volume(uint8_t volume)
{
	int ret = 0;
	
	ret = nau8810_reg_write(AUDIO_NAU8810_REG_DACGAIN, volume);
	if (ret != 0)
		KIDS_A10_PRT("nau8810_reg_write return %d\n", ret);
	
	return ret;
}

static int drv_codec_nau8810_set_mic_volume(uint8_t volume)
{
	int ret = 0;
	
	ret = nau8810_reg_write(AUDIO_NAU8810_REG_ADCGAIN, volume);
	if (ret != 0)
		KIDS_A10_PRT("nau8810_reg_write return %d\n", ret);
	
	return ret;
}

int drv_codec_nau8810_init(void)
{
	int ret = 0;
	
	ret = drv_codec_nau8810_validate_id();
	if (ret != 0)
		return -1;
	
	ret = drv_codec_nau8810_set_adda_config();
	if (ret != 0)
		return -1;
	
	ret = drv_codec_nau8810_set_spk_volume(0xff);
	if (ret != 0)
		return -1;
	
	ret = drv_codec_nau8810_set_mic_volume(0xff);
	if (ret != 0)
		return -1;
	
#if 0
	ret = drv_codec_nau8810_set_loopback();
	if (ret != 0)
		return -1;
#endif
	
	return 0;
}
