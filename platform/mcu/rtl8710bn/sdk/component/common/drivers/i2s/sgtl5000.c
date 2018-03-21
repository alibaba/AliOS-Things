#include "sgtl5000.h"
#include "PinNames.h"
#include "basic_types.h"

#include "i2c_api.h"
#include "i2c_ex_api.h"
#include "pinmap.h"
#include "wait_api.h"

//#define I2C_MTR_SDA			PC_4//PB_3
//#define I2C_MTR_SCL			PC_5//PB_2
#define I2C_MTR_SDA			PB_3
#define I2C_MTR_SCL			PB_2
#define I2C_BUS_CLK			100000  //100K HZ

i2c_t   sgtl5000_i2c;

uint16_t ana_ctrl;
uint8_t i2c_addr;

bool muted;



u8 sgtl5000_reg_write(u16 reg, u16 val)
{
	int length = 0;
	char buf[4];
	buf[0] = (char)(reg >> 8);
	buf[1] = (char)(reg&0xff);
	buf[2] = (char)(val>>8);
	buf[3] = (char)(val&0xff);

	length = i2c_write(&sgtl5000_i2c, i2c_addr, &buf[0], 4, 1);
	return (length==4)?0:1;
}

u8 sgtl5000_reg_read(u16 reg, u16* val)
{
	int tmp;
	char *buf = (char*)&tmp;	
	u8 ret = 0;
	
	buf[0] = (char)(reg >> 8);
	buf[1] = (char)(reg&0xff);

	if(i2c_write(&sgtl5000_i2c, i2c_addr, &buf[0], 2, 1) != 2){
		DBG_8195A("sgtl5000_reg_read(): write register addr fail\n");
		ret = 1;
	}
	
	buf[0] = 0xaa;
	buf[1] = 0xaa;
	
	if(i2c_read(&sgtl5000_i2c, i2c_addr, &buf[0], 2, 1) < 2){
		DBG_8195A("sgtl5000_reg_read(): read register value fail\n");
		ret = 1;
	}else
		*val = ((buf[0]&0xFF)<<8)|(buf[1]&0xFF); 
	return ret;
}

u16 sgtl5000_reg_modify(u16 reg, u16 val, u16 iMask)
{
	u16 val1;
	
	sgtl5000_reg_read(reg, &val1);
	
	u16 val2 = (val1 &(~iMask))|val;
	if(!sgtl5000_reg_write(reg, val2)) return 0;
	return val2;
}

void sgtl5000_setAddress(uint8_t level)
{
	if (level == 0) {
		i2c_addr = SGTL5000_I2C_ADDR_CS_LOW;
	} else {
		i2c_addr = SGTL5000_I2C_ADDR_CS_HIGH;
	}
}


void sgtl5000_i2c_master_txc_callback(void *userdata)
{

}

void sgtl5000_i2c_master_rxc_callback(void *userdata)
{

}

void sgtl5000_i2c_master_err_callback(void *userdata)
{	
    //DBG_8195A("I2C Ack address:%d", (sgtl5000_i2c.I2Cx->IC_TAR)& BIT_CTRL_IC_TAR);//sgtl5000_i2c.SalI2CHndPriv.SalI2CHndPriv.I2CAckAddr);
    //DBG_8195A("I2C Error:%x\n", sgtl5000_i2c.I2Cx->IC_TX_ABRT_SOURCE);//sgtl5000_i2c.SalI2CHndPriv.SalI2CHndPriv.ErrType);
}

void sgtl5000_reg_dump(void);

u8 sgtl5000_enable(void){
	u16 temp = 0;
	u8 ret = 0;
	muted = 1;
	memset(&sgtl5000_i2c, 0x00, sizeof(sgtl5000_i2c));
	i2c_init(&sgtl5000_i2c, I2C_MTR_SDA, I2C_MTR_SCL);
	i2c_frequency(&sgtl5000_i2c, I2C_BUS_CLK);
    i2c_set_user_callback(&sgtl5000_i2c, I2C_ERR_OCCURRED, sgtl5000_i2c_master_err_callback);

	// set I2C address
	sgtl5000_setAddress(0);  // CTRL_ADR0_CS is tied to GND
	wait_ms(5);
	
	ret = sgtl5000_reg_read(CHIP_ID, &temp);
	if(ret == 0)
		DBG_8195A("SGTL5000 CHIP ID:0x%04X\n", temp);
	else
		DBG_8195A("Get SGTL5000 CHIP ID fail\n");

	sgtl5000_reg_write(CHIP_ANA_POWER, 0x4060);	// VDDD is externally driven with 1.8V
	sgtl5000_reg_write(CHIP_LINREG_CTRL, 0x006C);  // VDDA & VDDIO both over 3.1V
	sgtl5000_reg_write(CHIP_REF_CTRL, 0x01F2); // VAG=1.575, normal ramp, +12.5% bias current
	sgtl5000_reg_write(CHIP_LINE_OUT_CTRL, 0x0F22); // LO_VAGCNTRL=1.65V, OUT_CURRENT=0.54mA
	sgtl5000_reg_write(CHIP_SHORT_CTRL, 0x4446);  // allow up to 125mA
	sgtl5000_reg_write(CHIP_ANA_CTRL, 0x0137);  // enable zero cross detectors
	sgtl5000_reg_write(CHIP_ANA_POWER, 0x40FF); // power up: lineout, hp, adc, dac
	sgtl5000_reg_write(CHIP_DIG_POWER, 0x0073); // power up all digital stuff
	wait_ms(400);
	sgtl5000_reg_write(CHIP_LINE_OUT_VOL, 0x1D1D); // default approx 1.3 volts peak-to-peak
	sgtl5000_reg_write(CHIP_CLK_CTRL, 0x0004);  // 44.1 kHz, 256*Fs
	sgtl5000_reg_write(CHIP_I2S_CTRL, 0x0130); // SCLK=32*Fs, 16bit, I2S format
	// default signal routing is ok?
	sgtl5000_reg_write(CHIP_SSS_CTRL, 0x0010); // ADC->I2S, I2S->DAC
	sgtl5000_reg_write(CHIP_ADCDAC_CTRL, 0x0000); // disable dac mute
	sgtl5000_reg_write(CHIP_DAC_VOL, 0x3C3C); // digital gain, 0dB
	sgtl5000_reg_write(CHIP_ANA_HP_CTRL, 0x7F7F); // set volume (lowest level)
	sgtl5000_reg_write(CHIP_ANA_CTRL, 0x0036);  // enable zero cross detectors
	//semi_automated = true;
	
	//sgtl5000_reg_dump();

    return 0;
}

bool sgtl5000_muteHeadphone(void) { 
	return sgtl5000_reg_write(CHIP_ANA_CTRL, ana_ctrl | (1<<4)); 
}
bool sgtl5000_unmuteHeadphone(void) { 
	return sgtl5000_reg_write(CHIP_ANA_CTRL, ana_ctrl & ~(1<<4)); 
}
bool sgtl5000_muteLineout(void) { 
	return sgtl5000_reg_write(CHIP_ANA_CTRL, ana_ctrl | (1<<8)); 
}
bool sgtl5000_unmuteLineout(void) { 
	return sgtl5000_reg_write(CHIP_ANA_CTRL, ana_ctrl & ~(1<<8)); 
}

u8 sgtl5000_setVolume(float val){
	int volumeInt = 0; 

	volumeInt = (int)(val * 129 + 0.499);

	if (volumeInt == 0) {
		muted = 1;
		sgtl5000_reg_write(CHIP_ANA_HP_CTRL, 0x7F7F);
		return sgtl5000_muteHeadphone();
	} else if (volumeInt > 0x80) {
		volumeInt = 0;
	} else {
		volumeInt = 0x80 - volumeInt;
	}
	if (muted) {
		muted = 0;
		sgtl5000_unmuteHeadphone();
	}
	volumeInt = volumeInt | (volumeInt << 8);

	return sgtl5000_reg_write(CHIP_ANA_HP_CTRL, volumeInt);	// set volume
}


void sgtl5000_reg_dump(void)
{
	u16 reg;
	u8 ret = 0;
	u16 value;
  
	DBG_8195A("SGTL5000 codec reg dump\n\r");
	DBG_8195A("------------------------\n\r");
	for(reg = 0; reg <= 0x013A; ){
		ret = sgtl5000_reg_read(reg, &value);
		if(ret == 0)
			DBG_8195A("0x%04X : 0x%04X\n\r", reg, value);
        reg += 0x02;
	}
	DBG_8195A("------------------------\n\r");
}


