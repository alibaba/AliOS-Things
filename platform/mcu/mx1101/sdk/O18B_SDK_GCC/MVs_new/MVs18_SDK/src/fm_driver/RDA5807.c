/*
*****************************************************************************
*					Mountain View Silicon Tech. Inc.
*	Copyright 2009, Mountain View Silicon Tech. Inc., ShangHai, China
*					All rights reserved.
*
* Filename:			
* Description:		
* maintainer: Sam
******************************************************************************
*/
#include<string.h>
#include "type.h"
#include "app_config.h"
#include "i2c.h"
#include "clk.h"
#include "gpio.h"
#include "QN8035.h"
#include "dev_detect_driver.h"
#include "rda5807.h"
#include "i2c.h"


#ifdef FUNC_RADIO_RDA5807_EN

static void* FMI2cHandler = NULL;
uint8_t ChipVer;	
	  
const uint16_t RDA5807HP_REG[66]=
{
//#ifdef(_SHARE_CRYSTAL_12MHz_)	  //5801
//    0xC4, 0x11, //02H:
//#else
//    0xC0, 0x01,
//#endif
	0x03,0x0000,
	0x04,0x0400,
	0x05,0x86BF,  //83 84 85 86 87 88 89 8A 8B;	
	0x06,0x4000,
	0x07,0x56C6,
	0x09,0x0000,
	0x12,0x2A11,
	0x13,0x002E,
	0x16,0x9000,
	0x17,0x2A91,
	0x18,0x8412,
	0x1B,0xE000,
	0x1C,0x301D,
	0x1D,0x816A,
	0x1E,0x4608,
	0x1F,0x0086,
	0x20,0x0661, //
	0x23,0x24c9, //
	0x24,0x0408,
	0x25,0x0608,
	0x2A,0x3414,
	0x2C,0x096D,
	0x2D,0x2D96,
	0x2e,0x01DA,
	0x31,0x13D5,
	0x32,0x4891,
	0x38,0x0000,
	0x39,0x3C58,
	0x3a,0x2A50,
	0x3b,0x262C,
	0x3c,0x0608,
	0x3d,0x0200,
	0x3e,0x0000,
};

const uint16_t RDA5807PE_REG[32]=   //sp	 5804
{	
	0x03,0x0000, //freq 0//band 875--1080//space 100k//turn off//turn on 0x0010
	0x04,0x0400,
	0x05,0x86AF, //0x8AAF//SEEK threshold //VOL
	0x06,0x8000,
	0x07,0x5F1A,
	0x12,0xF000,
	0x13,0x7F0B,
	0x14,0x0071,
	0x15,0x42C0,
	0x1A,0x10C0,
	0x1C,0x6015,
	0x21,0x4580,
	0x24,0x0601,
	0x26,0x0D04,
	0x2C,0x3C3B,
	0x35,0x3877
};

const uint16_t RDA5807N_REG[34]=	  //5808
{
	0x03,0x0000,
	0x04,0x0400,
    0x05,0xC6ad, //05h snr //83 84 85 86 87 88 89 8A 8B;
	0x06,0x6000,
    0x07,0x4216, //rssi 0x42, 0xC6,
	0x09,0x0000,
	0x12,0x2a11,
	0x13,0xB042,  
	0x14,0x2A11,  
	0x15,0xb831, 
	0x16,0xc000,
	0x17,0x2a91,
	0x18,0x9400,
	0x1B,0xF7cF,   
	0x1c,0x2ADC,
	0x1d,0x806F, 
	0x25,0x0E1C,
};

/*
*****************************************************
*      
******************************************************
*/
bool RDA5807Write(uint8_t addr, uint16_t dat)
{
	  	
	bool Rv;
	uint8_t LittleEndianData[2];
	LittleEndianData[0] = dat >> 8;
	LittleEndianData[1] = dat & 0xFF;
	Rv = I2cWriteNByte(FMI2cHandler, RDA5807_CHIP_ADDR, addr, LittleEndianData, 2);	
	
	return Rv;
}

uint16_t RDA5807Read(uint8_t Register_Address)	
{
	uint16_t	TempData = 0;
	uint16_t	Ret;
	
	//I2C_ReadNByte(RDA5807_CHIP_ADDR, Register_Address, (uint8_t*)(&TempData), 2);
	I2cReadNByte(FMI2cHandler, RDA5807_CHIP_ADDR, Register_Address, (uint8_t*)(&TempData), 2);
	Ret = (*(uint8_t*)(&TempData) << 8) + *((uint8_t*)(&TempData) + 1);
	return Ret;
}

/*******************************************************/
bool RDA5807Init(void)
{	
	uint8_t	i;
#if 0
//select crystal or reference clock
//	if((gRadioData.BandXtalSel & MASK_RADIO_XTAL) == RADIO_XTAL_EXCLK_12MHZ)
	{
		RDA5807Write(0x02, 0xC411);//12m
	}
	else
#endif
	{
		RDA5807Write(0x02, 0xC001);		// 32K
	}
	WaitMs(600);

	if(ChipVer == VER_PE)//5804
	{
		for(i = 0; i < RDA5807PE_REG_LEN;)
		{
			RDA5807Write(*(RDA5807PE_REG + i), *(RDA5807PE_REG+1 + i));
			i += 2;
		}
	}

	if(ChipVer == VER_HP)//5801
	{
		for(i = 0; i < RDA5807PH_REG_LEN;)
		{
			RDA5807Write(*(RDA5807HP_REG + i), *(RDA5807HP_REG + 1 + i));
			i += 2;
		}	
	}

	if(ChipVer == VER_N)//5808
	{
		for(i = 0; i < RDA5807N_REG_LEN;)
		{
			RDA5807Write(*(RDA5807N_REG + i), *(RDA5807N_REG + 1 + i));
			i += 2;
		}	
	}
	return TRUE;
}

bool RDA5807ReadID(void)
{
	uint16_t	id;
	
	ChipVer = 0xff;	
	RDA5807Write(0x02, 0x0002);//reset
	WaitMs(60);
	id = RDA5807Read(0x0E);
	DBG("RD5807 ID: %X\t", id);
	if(id == 0x5801)
	{
		DBG("HP\n");
		ChipVer = VER_HP;//yes debug	
	}
	else if(id == 0x5804)
	{
		DBG("PE\n");
		ChipVer = VER_PE;
	}
	else if(id == 0x5808)//yes debug 5807m
	{
		DBG("N\n");
		ChipVer = VER_N;
	}
	else
	{
		DBG("ERR\n");
		return FALSE;
	}

	//RDA5807Init();
	RDA5807Mute(TRUE);
	return TRUE;		
}

void RDA5807Mute(bool mute)
{
	uint16_t temp;

	temp = RDA5807Read(0x02);

	if(mute) 
	{
		temp &= 0xBFFF;			//mute on
	}
	else 
	{
		temp |= 0x4000;			// mute oFF
	}
	
	RDA5807Write(0x02, temp);
}

void RDA5807OutputHIZ(bool HIZOnOff)   //litao add SDKV215
{
#ifdef	MUTE_WITH_HIZ
	uint16_t temp;
	temp = RDA5807Read(0x02);

	if(HIZOnOff) 
	{	
		temp &= 0x7FFF; 		//Audio Ouput High Impedance
	}
	else 
	{
		temp |= 0x8000; 		//Audio Ouput Normal Operation
	}
	
	RDA5807Write(0x02, temp);
#endif
}
bool RDA5807PowerOn(void)
{
	if(!FMI2cHandler)
	{
		FMI2cHandler = I2cMasterCreate(PORT_C, 3, PORT_C, 4);
	}
	//RDA5807Write(0x02, (RDA5807Read(0x02) | 0xC000));  
	return RDA5807ReadID();
}

bool RDA5807PowerDown(void)
{
	//RDA5807Write(0x02, (RDA5807Read(0x02) & 0x3FFE)); 
	RDA5807Mute(TRUE);
	return 1;
}

void RDA5807SetFreq(uint16_t freq)
{	
	//RDA5807Mute(TRUE);
	freq = ((freq - 870) << 6) | 0x0010;
	RDA5807Write(0x03, freq); //FREQ //tune//band//space
	//RDA5807Mute(FALSE);
}


void RDA5807SearchSet(uint16_t freq)
{
	DBG("RDA5807_SearchSet\n");
	RDA5807SetFreq(freq);
}

uint8_t RDA5807SearchRead(uint16_t freg)
{
	uint16_t u16reg;

	u16reg = RDA5807Read(0x0B); 
		  
	if(u16reg & 0x0100)
	{
		if((freg == 1080) || (freg == 1020) || (freg == 960) || (freg == 885) || (freg == 1000))
		{
			u16reg = RDA5807Read(0x0A);
			if(u16reg & 0x0400)	  
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		return TRUE;
	}
	return FALSE;
}

void RDA5807VolSet(uint8_t vol)
{
	uint16_t u16reg;	
	u16reg = RDA5807Read(0x05); 	
	vol &= 0x0F;
	u16reg &= 0xFFF0;
	u16reg |= vol;
	RDA5807Write(0x05, u16reg); 
}


#endif	//#ifdef FUNC_RADIO_RDA5807_EN

