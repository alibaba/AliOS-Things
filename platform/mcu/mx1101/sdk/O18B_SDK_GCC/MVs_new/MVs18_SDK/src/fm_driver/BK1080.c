//  maintainer: Sam
#include <string.h>
#include "type.h"
#include "app_config.h"
#include "i2c.h"
#include "clk.h"
#include "gpio.h"
#include "bk1080.h"
#include "dev_detect_driver.h"
#ifdef FUNC_RADIO_BK1080_EN

#ifdef REG_RW_I2C_MODE
//BK1080 initialization value
uint8_t  HW_Reg[] =
{
	0x00, 0x08, //reg0
	0x10, 0x80, //reg1

//select crystal or reference clock
//there select crystal
#ifdef XTAL_CLOCK	//reg2
	0x92, 0x01,	//for crystal clock	  bit15 1
#else
	0x82, 0x01,	//for extern clock	 bit15 1
#endif

	0x00, 0x00, //reg3
	0x40, 0x90, //reg4
	0x0A, 0x1F, //reg5//vol
	0x00, 0x2E, //reg6
	0x02, 0xFF, //reg7
	0x5B, 0x11, //reg8
	0x00, 0x00, //reg9
	0x41, 0x1E, //reg10
	0x00, 0x00, //reg11
	0xCE, 0x00, //reg12
	0x00, 0x00, //reg13
	0x00, 0x00, //reg14
	0x10, 0x00, //reg15
	0x00, 0x10, //reg16
	0x00, 0x00,	//0x00, 0x02,
	0x13, 0xFF, //reg18
	0x98, 0x52, //reg19
	0x00, 0x00, //reg20
	0x00, 0x00, //reg21
	0x00, 0x08, //reg22
	0x00, 0x00, //reg23
	0x51, 0xE1, //reg24
	0x38, 0xBC,	//0x20, 0xBC,
	0x26, 0x45, //reg26
	0x00, 0xE4, //reg27
	0x1C, 0xD8, //reg28
	0x3A, 0x50, //reg29
	0xEA, 0xE0, //reg30
	0x33, 0x90, //31
	0x02, 0x00, //reg32
	0x00, 0x01, //reg33
};
//BK1080_WriteBuff[2 * 2] = 0x42;
//BK1080_WriteBuff[16 * 2] = 0x16;
//BK1080_WriteBuff[16 * 2 + 1] = 0xE3;
//BK1080_WriteBuff[17 * 2] = 0x80;
//uint8_t CODE Ext12m_Reg[]=
//{
//	0x82, 0x01,//for extern clock
//	0x16, 0xE3,//reg16
//	0x80, 0x00,//reg16
//};
static void* FMI2cHandler = NULL;

uint8_t BK1080_WriteBuff[2];

bool BEKEN_I2C_Write(uint8_t reg, uint8_t* pBuf, uint8_t len)
{
#if 1
	uint8_t count;

	I2cStart(FMI2cHandler);
	if(!I2cWriteByte(FMI2cHandler, BK1080_CHIP_ADDR))
	{
		I2cStop(FMI2cHandler);
		DBG("Op BK1080 err! 3\n");
		return FALSE;
	}

	if(!I2cWriteByte(FMI2cHandler, reg << 1))
	{
		I2cStop(FMI2cHandler);
		DBG("Op BK1080 err !4\n");
		return FALSE;
	}

	for(count = 0; count < len; count++)
	{
		if(!I2cWriteByte(FMI2cHandler, pBuf[count]))
		{
			I2cStop(FMI2cHandler);
			return FALSE;
		}
	}
	I2cStop(FMI2cHandler);

	return TRUE;
#else
	return I2C_WriteNByte(BK1080_CHIP_ADDR, (uint16_t)(reg << 1), pBuf, len);
#endif
}

bool BEKEN_I2C_Read(uint8_t reg, uint8_t* pBuf, uint8_t len)
{
	// @Note: BK1080协议与IIC有区别，读数据前不需要发Start和器件地址。
	uint8_t	count;

	I2cStart(FMI2cHandler);
	if(!I2cWriteByte(FMI2cHandler, BK1080_CHIP_ADDR))
	{
		I2cStop(FMI2cHandler);
		DBG("Op BK1080 1err!\n");
		return FALSE;
	}

	if(!I2cWriteByte(FMI2cHandler, (reg << 1) | 0x01))
	{
		I2cStop(FMI2cHandler);
		DBG("Op BK1080 2err!\n");
		return FALSE;
	}

	for(count = 0; count < len - 1; count++)
	{
		pBuf[count] = I2cReadByte(FMI2cHandler);
		I2cSendAck(FMI2cHandler);
	}
	pBuf[count] = I2cReadByte(FMI2cHandler);
	I2cSendNoAck(FMI2cHandler);
	I2cStop(FMI2cHandler);

	return TRUE;
}

uint16_t BK1080FreqToChan(uint16_t frequency)
{
	return ((frequency - RADIO_LOWER_BOUND) / 1);
}
/*
{
	uint16_t bottomOfBand;
	uint8_t channelSpacing;
	uint16_t channel;

	bottomOfBand = RADIO_LOWER_BOUND;
	channelSpacing = 1;

	channel = (frequency - bottomOfBand) / channelSpacing;

	return (channel);
}
*/

void BK1080Mute(bool MuteFg)
{
///	uint8_t BK1080_WriteBuff[2];
//DBG("xxxx1\n");
	BEKEN_I2C_Read(2, BK1080_WriteBuff, 2);
	if(MuteFg)
	{
		BK1080_WriteBuff[0] |= 0x40;
	}
	else
	{
		BK1080_WriteBuff[0] &= 0xbf;
	}

	BEKEN_I2C_Write(2, BK1080_WriteBuff, 2); //write reg5,with 4 bytes
}

void  BK1080SetFreq(uint16_t Freq)
{
	uint16_t curChan;
	uint8_t TmpData8[4];

//	DBG("Bk1080_FreqSet\n");

	//BK1080Mute(TRUE);
//DBG("xxxx2\n");
	BEKEN_I2C_Read(2, TmpData8, 4); //read reg2,with 4 bytes

	curChan = BK1080FreqToChan(Freq); /*stop seek and tune*/

	TmpData8[0] &= 0xfe;
	TmpData8[2] = 0x00;//reset tune
	TmpData8[3] = curChan & 0xff;
	BEKEN_I2C_Write(2, TmpData8, 4); //write reg2,with 4 bytes

	TmpData8[2] = (curChan >> 8) & 0xff;
	TmpData8[2] |= 0x80;//start tune
	TmpData8[3] = curChan & 0xff;
	BEKEN_I2C_Write(2, TmpData8, 4); //write reg2,with 4 bytes
	//BK1080Mute(FALSE);
//	WaitMs(100);
}

bool BK1080Init(void)
{
	DBG("Bk1080 Init\n");

	if(FMI2cHandler)
	{
		//GpioClk32kIoConfig(2);
		if(!BEKEN_I2C_Write(0, &(HW_Reg[0]), sizeof(HW_Reg))) //start from reg2,total 60 byte
		{
			APP_DBG("BEKEN_I2C_Write Fail!\n");
			return FALSE;
		}

		//select crystal or reference clock
		//see it the init array buffer
		//	if(Radio.ExtOsc12m)
		//	{
		//		BEKEN_I2C_Write(2, &Ext12m_Reg[0], 2);
		//		BEKEN_I2C_Write(16, &Ext12m_Reg[2], 2);
		//		BEKEN_I2C_Write(17, &Ext12m_Reg[4], 2);//修正一处笔误BEKEN_I2c_Write(17, &Ext12m_Reg[4], 4);
		//	}

		//select crystal or reference clock
		BK1080_WriteBuff[1] = 0x01;
		//if((gRadioData.BandXtalSel & MASK_RADIO_XTAL) == RADIO_XTAL_EXCLK_12MHZ)
		//{
		//	BK1080_WriteBuff[0] = 0x82;
		//}
		//else
		//{
		BK1080_WriteBuff[0] = 0x92;
		//}
		if(!BEKEN_I2C_Write(0, &(BK1080_WriteBuff[0]), 2))
		{
			APP_DBG("BEKEN_I2C_Write Fail!\n");
			return FALSE;
		}
		WaitMs(300);//delay 250ms
		BK1080_WriteBuff[0] = HW_Reg[25 * 2];
		BK1080_WriteBuff[1] = HW_Reg[25 * 2 + 1];
		BK1080_WriteBuff[1] &= 0x7f;
		if(!BEKEN_I2C_Write(25, &(BK1080_WriteBuff[0]), 2))
		{
			APP_DBG("BEKEN_I2C_Write Fail!\n");
			return FALSE;
		}
		BK1080_WriteBuff[1] |= 0x80;
		if(!BEKEN_I2C_Write(25, &(BK1080_WriteBuff[0]), 2))
		{
			APP_DBG("BEKEN_I2C_Write Fail!\n");
			return FALSE;
		}
		WaitMs(80);//delay 60ms
		return TRUE;
	}
	else
	{
		APP_DBG("I2cMasterCreate Fail!\n");
		return FALSE;
	}
}

bool BK1080ReadID(void)
{
	uint16_t	reg1;
	uint16_t	TempData = 0;
	DBG("Bk1080 Probe\n");
	if(BEKEN_I2C_Read(1, (uint8_t*)&TempData, 2))	// Read ID.
	{
		reg1 = (*(uint8_t*)(&TempData) << 8) + *((uint8_t*)(&TempData) + 1);
		if(reg1 == 0x1080)
		{
			DBG("BK1080 ID : 0x1080\n");
			//BK1080Init();
			return TRUE;
		}
	}
	DBG("BK1080 ID ERR = %x\n", reg1);
	BK1080Mute(TRUE);
	return FALSE;
}
void BK1080OutputHIZ(bool HIZOnOff)   //litao add SDKV215 [BK1080没有高阻抗设置寄存器，通过验证可以通过设置power 模式来实现高阻抗]
{
	BEKEN_I2C_Read(2, BK1080_WriteBuff, 2);
	if(HIZOnOff)
	{
		BK1080_WriteBuff[1] |= 0x41;  //power down
	}
	else
	{
		BK1080_WriteBuff[1] &= 0xBF;  //power up
	}

	BEKEN_I2C_Write(2, BK1080_WriteBuff, 2); //write reg5,with 4 bytes

	if(HIZOnOff == FALSE)
	{
		WaitMs(200);    // wait for power stable
	}
}

bool BK1080PowerDown(void)
{
//	BK1080_WriteBuff[2*2] |= 0xC0;// mute on
//	BEKEN_I2C_Write(2, &(BK1080_WriteBuff[2*2]), 2);
//	BK1080_WriteBuff[2*2 + 1] &= 0x01;		//power UP dis
//	BK1080_WriteBuff[2*2 + 1] |= 0x40;		//power down enable
//	BEKEN_I2C_Write(2, &(BK1080_WriteBuff[2*2]), 2);
	return TRUE;
}
bool BK1080PowerOn(void)
{
	if(!FMI2cHandler)
	{
		FMI2cHandler = I2cMasterCreate(PORT_B, 22, PORT_B, 23);
	}
	if(FMI2cHandler)
	{

		if(BK1080ReadID())
		{
			return TRUE;
		}
	}
	else
	{
		APP_DBG("I2cMasterCreate Fail!\n");
		return FALSE;
	}
}

void Bk1080SearchSet(uint16_t Freq)
{
	DBG("Bk1080_SearchSet\n");
	BK1080SetFreq(Freq);
}

bool BK1080SearchRead(uint16_t Freq)
{
	static uint16_t last_tuned_freq = 0;//static variable
	static uint16_t g_last_freq_deviation_value = 0;
	uint16_t cur_freq_deviation = 0;
	uint8_t TmpData8[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	uint16_t start_freq = RADIO_LOWER_BOUND;
	uint8_t	impc;
	uint16_t freq_input;

//	DBG("Bk1080_SearchRead\n");

	//freq_input = RadioFreq;
	freq_input = Freq;

	WaitMs(50);
//DBG("xxxx4\n");
	BEKEN_I2C_Read(7, TmpData8, 10); //start from reg7,with 10bytes

	cur_freq_deviation = TmpData8[0];
	cur_freq_deviation <<= 8;
	cur_freq_deviation |= TmpData8[1];
	cur_freq_deviation = cur_freq_deviation >> 4;

	impc = (uint8_t)(TmpData8[8] & 0x3C) >> 2;
	/*
	DBG("\nfreq: %d \n", freq_input);
	DBG("AFC: %d  ", (uint16_t)(TmpData8[6] & 0x10));
	DBG("rssi: %d   ", (uint16_t)(TmpData8[7]));
	DBG("snr: %d    ", (uint16_t)(TmpData8[1] & 0x0F));
	DBG("impc: %d   ", (uint16_t)(impc));
	DBG("deviation: %d   ", (uint16_t)(cur_freq_deviation));
	*/
	if((TmpData8[6] & 0x10)    //check AFCRL bit12
#if 0
	        || (TmpData8[7] < 40) //RSSI<10
	        || ((TmpData8[1] & 0x0F) < 8) //SNR<2
#else
	        || (TmpData8[7] < BK1080_DEFAULT_RSSI_VALUE) //RSSI<10
	        || ((TmpData8[1] & 0x0F) < 1) //SNR<2
#endif
	        || (impc > 14)					//impc
	        || ((cur_freq_deviation >= 200) && (cur_freq_deviation <= (0x0FFF - 200))))
	{
		last_tuned_freq = freq_input;//save last tuned freqency
		g_last_freq_deviation_value = cur_freq_deviation;
		return FALSE;//false
	}

#if 1
	//new added 2009-05-30
	if((freq_input > start_freq) && ((freq_input - last_tuned_freq) == 1))
	{
		if((g_last_freq_deviation_value & 0x800)
		        || (g_last_freq_deviation_value < 100))
		{
			last_tuned_freq = freq_input;//save last tuned freqency
			g_last_freq_deviation_value = cur_freq_deviation;
			return FALSE;
		}
	}
	if((freq_input >= start_freq) && ((last_tuned_freq - freq_input) == 1))
	{
		if(((g_last_freq_deviation_value & 0x800) == 0)
		        || (g_last_freq_deviation_value > (0xfff - 100)))
		{
			last_tuned_freq = freq_input;//save last tuned freqency
			g_last_freq_deviation_value = cur_freq_deviation;
			return FALSE;
		}
	}
#endif

	DBG("  Station: %d \n", freq_input);
#if 0
	if((freq_input == 880) || (freq_input == 909) || (freq_input == 920) || (freq_input == 921)
	        || (freq_input == 924) || (freq_input == 925) || (freq_input == 931)
	        || (freq_input == 940) || (freq_input == 960) || (freq_input == 989)
	        || (freq_input == 1000) || (freq_input == 1020) || (freq_input == 1038)
	        || (freq_input == 1040) || (freq_input == 1080))
	{
		if(!(TmpData8[7] & 0x80))
		{
			last_tuned_freq = freq_input;//save last tuned freqency
			g_last_freq_deviation_value = cur_freq_deviation;
			return FALSE;
		}
	}
#endif
	last_tuned_freq = freq_input;//save last tuned freqency
	g_last_freq_deviation_value = cur_freq_deviation;

	return TRUE; //OK
}

/*
uint8_t BK1080RSSIRead(void)
{
	uint8_t TmpData2[2]={0, 0};
	WaitMs(50);
	BEKEN_I2C_Read(10, TmpData2, 2);//start from reg7,with 10bytes
	return TmpData2[1];
}
*/

void BK1080VolSet(uint8_t vol)
{
	uint8_t TmpData8[2];

	BEKEN_I2C_Read(5, TmpData8, 2);
	//DBG("TmpData8[0]:%2BX\n", TmpData8[0]);
	//DBG("TmpData8[1]:%2BX\n", TmpData8[1]);
	TmpData8[1] &= 0xF0;
	TmpData8[1] |= vol;
	BEKEN_I2C_Write(5, TmpData8, 2);
}
#endif

#endif	//#ifdef FUNC_RADIO_EN
// end of file

