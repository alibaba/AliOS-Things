// maintainer: Sam
#include <string.h>
#include "type.h"
#include "app_config.h"
#include "i2c.h"
#include "clk.h"
#include "gpio.h"
#include "RDA5876A.h"
#include "dev_detect_driver.h"
#ifdef FUNC_RADIO_RDA5876A_EN
#define FM_PATCH_I2C_ADDR 	0x2c
#define FM_WRITE_I2C_ADDR 	0x22

#define BT_ACTIVE

#ifdef BT_ACTIVE
const uint16_t Rda5876hpFmInitBtOn[][2] =
{
	{0x3f, 0x0001},//page 1
	{0x22, 0x0e93},
	{0x25, 0x07ff},
	{0x26, 0x23b3},
	{0x28, 0x6800},
	{0x2D, 0x006A},
	{0x2f, 0x1100},
	{0x32, 0x88f9},
	{0x3f, 0x0000},//page 1
};

const uint16_t Rda5876hpFmDeInitBtOn[][2] =
{
	{0x3f, 0x0001},//page 1
	{0x22, 0x0e93},
	{0x25, 0x03e1},
	{0x26, 0x43a5},
	{0x28, 0x6800},
	{0x2D, 0x00AA},
	{0x2f, 0x1100},
	{0x32, 0x88f9},
	{0x3f, 0x0000},//page 1
};
#endif // BT_ACTIVE

const uint16_t Rda5876hpFmInitBtOff[][2] =
{
	// turn on ldo_on before wirte these regs
	{0x3f, 0x0001},//page 1
	{0x22, 0x0ea3},
	{0x25, 0x67e5},
	{0x26, 0x23a3},
	{0x28, 0x2800},
	{0x2D, 0x006A},
	{0x2f, 0x2100},
	{0x32, 0x88f9},
	{0x3f, 0x0000},//page 1
};

const uint16_t Rda5876hpFmInit[][2] =
{
	{0x3f, 0x0001}, //page 1
	{0x2D, 0x002A}, //page 1
	{0x3f, 0x0000}, //page 1
};

const uint16_t Rda5876hpFmDeinit[][2] =
{
	{0x3f, 0x0001}, //page 1
	{0x2D, 0x00AA}, //page 1
	{0x3f, 0x0000}, //page 1
};

const uint16_t Rda5876hpInitializationReg[] =
{
	0xc405,//02h
	0x1a00,
	0x0400,
#if 1//__RDA_CHIP_R12_5876__ 天线FM_IP
	0x8b6d,//05h	//0xad 如果搜台有问题 0x6d改为0xad
#else
	0x88ad,//05h
#endif
	0x6000,
	0x42c6,
	0x0000,
	0x0000,
	0x4468,
	0x5d8f,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0xf017,//10h
	0x4779,
	0x2d00,
	0xb042,
	0x2211,
	0xf831,//15h
	0xc3c4,
	0x1fce,
	0x9500,//0x94, 0x12,//18h
	0x00a8,
	0xc400,
	0xe7df,
	0x2414,//0x34, 0xdc,提升VIO VDD之间压差太大引起的不良
	0x816a,
	0x4608,
	0x0086,
	0x0661,//20h
	0x0000,
	0x109e,
	0x23c8,
	0x0406,
	0x060b,//25h
	0xe105,
	0xbb6c,
	0x2bec,
	0x090f,
	0x3414,
	0x1450,
	0x096d,
	0x2d96,
	0x01da,
	0x2a7b,
	0x0821,//30h
	0x13d5,
	0x48f1,
	0x00bc,
	0x0820,
	0x0a24,//35h
	0x0b82,
	0x25c7,
	0x2be0,
	0x3edd,
	0x2cb9,
	0x0c95,
	0x088d,
	0x0485,
	0x0080,
	0x0000,
	0x5804,//40h
	0x5804,
	0x3835,
	0x2f2b,
	0x2722,
	0x1b15,
	0x0f08,
	0x03fd,
	0xfdfd,
	0x17af,
	0xffff,
	0xff00,
	0x2e94,
	0xc84e,
	0x00fd,
	0xd498,
};

static void * FMI2cHandler = NULL;

uint16_t Rda5876hpRead(uint8_t Register_Address)
{
	uint16_t	TempData = 0;
	uint16_t    Ret;

	I2cReadNByte(FMI2cHandler, FM_WRITE_I2C_ADDR, Register_Address, (uint8_t*)(&TempData), 2);
	Ret = (*(uint8_t*)(&TempData) << 8) + *((uint8_t*)(&TempData) + 1);
	return Ret;
}

bool Rda5876hpWrite(uint16_t addr, uint16_t dat)
{
	bool Rv;
	uint8_t LittleEndianData[2];
	LittleEndianData[0] = dat >> 8;
	LittleEndianData[1] = dat & 0xFF;
	Rv = I2cWriteNByte(FMI2cHandler, FM_WRITE_I2C_ADDR, addr, LittleEndianData, 2);
	return Rv;
}

bool Rda5876hpWritePatch(uint16_t addr, uint16_t dat)
{
	bool Rv;
	uint8_t LittleEndianData[2];
	LittleEndianData[0] = dat >> 8;
	LittleEndianData[1] = dat & 0xFF;
	Rv = I2cWriteNByte(FMI2cHandler, FM_PATCH_I2C_ADDR, addr, LittleEndianData, 2); //2c
	return Rv;
}

bool Rda5876hpPatchPowerOn(void)
{
	uint8_t i = 0;
	bool RV = TRUE;

	if(!FMI2cHandler)
	{
		FMI2cHandler = I2cMasterCreate(PORT_B, 22, PORT_B, 23);
	}

	if(FMI2cHandler)
	{
		//GpioClk32kIoConfig(1);
#ifdef BT_ACTIVE
		for(i = 0; i < sizeof(Rda5876hpFmInitBtOn) / sizeof(Rda5876hpFmInitBtOn[0]); i++)
#else
		for(i = 0; i < sizeof(Rda5876hpFmInit) / sizeof(Rda5876hpFmInit[0]); i++)
#endif //BT_ACTIVE		
		{
#ifdef BT_ACTIVE
			if(!Rda5876hpWritePatch(Rda5876hpFmInitBtOn[i][0], Rda5876hpFmInitBtOn[i][1]))
#else
			if(!Rda5876hpWritePatch(Rda5876hpFmInit[i][0], Rda5876hpFmInit[i][1]))
#endif //BT_ACTIVE		
			{
				APP_DBG("Rda5876hpPatchPowerOn Fail!\n");
				RV = FALSE;
			}
		}
		WaitMs(10);
	}
	else
	{
		APP_DBG("I2cMasterCreate Fail!\n");
		RV = FALSE;
	}

	return RV;
}

bool Rda5876hpPatchPowerOff(void)
{
	uint8_t i = 0;
	bool RV = TRUE;
#ifdef BT_ACTIVE
	for(i = 0; i < sizeof(Rda5876hpFmDeInitBtOn) / sizeof(Rda5876hpFmDeInitBtOn[0]); i++)
#else
	for(i = 0; i < sizeof(Rda5876hpFmDeinit) / sizeof(Rda5876hpFmDeinit[0]); i++)
#endif //BT_ACTIVE	
	{
#ifdef BT_ACTIVE
		if(!Rda5876hpWritePatch(Rda5876hpFmDeInitBtOn[i][0], Rda5876hpFmDeInitBtOn[i][1]))
#else
		if(!Rda5876hpWritePatch(Rda5876hpFmDeinit[i][0], Rda5876hpFmDeinit[i][1]))
#endif //BT_ACTIVE	
		{
			APP_DBG("Rda5876hpPatchPowerOff Fail!\n");
			RV = FALSE;
		}
	}
	WaitMs(10);
	return RV;
}


uint16_t Rda5876hpReadID(void)
{
	uint16_t cChipID = 0;

	if(Rda5876hpPatchPowerOn())
	{
		Rda5876hpWrite(0x02, 0x0002); //reset  //0x22

		WaitMs(100);

		cChipID = Rda5876hpRead(0x0e); //5808

		cChipID = Rda5876hpRead(0x0c);	//0a 0b 0c	 //0x23   5803

		if(0x5802 == cChipID || 0x5803 == cChipID)
		{
			return(0x5802);
		}

	}

	return(cChipID);
}

bool Rda5876hpInit(void)
{

	uint16_t i = 0;
	bool RV = TRUE;

	Rda5876hpWrite(0x02, 0xC401);
	Rda5876hpWrite(0x02, 0x0002); //reset  //0x22
	Rda5876hpWrite(0x02, 0xC401);

	WaitMs(500);

	for(i = 0; i < sizeof(Rda5876hpInitializationReg) / sizeof(Rda5876hpInitializationReg[0]); i++)
	{
		if(!Rda5876hpWrite(0x02 + i, Rda5876hpInitializationReg[i]))
		{
			APP_DBG("Rda5876hpInit Fail!\n");
			RV = FALSE;
		}
	}

	return RV;
}

void Rda5876hpFreqSet(uint16_t freq)
{

	freq = ((freq - 870) << 6) | 0x0010;
	Rda5876hpWrite(0x03, freq); //FREQ //tune//band//space

}

bool Rda5876hpGetTuneResult(uint16_t Freq)
{
	uint16_t TempA = Rda5876hpRead(0x0A);
	uint16_t TempB = Rda5876hpRead(0x0B);

	if((TempA & 0x4000) && (TempB & 0x0080) && (TempB & 0x0100))
	{
		if((Freq == 1080) || (Freq == 1020) || (Freq == 960))
		{
			if(Rda5876hpRead(0x0A) & 0x0400)
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

void Rda5876hpMute(bool MuteFg)
{
	uint16_t temp;

	temp = Rda5876hpRead(0x02);
	if(MuteFg)
	{
		temp &= 0xBFFF;			//mute on
	}
	else
	{
		temp |= 0x4000;			// mute oFF
	}
	Rda5876hpWrite(0x02, temp);
}

bool Rda5876hpIsStereo(void)
{
	uint16_t temp;

	temp = Rda5876hpRead(0x02);

	return (temp & 0x2000) ? FALSE : TRUE;
}

void Rda5876hpSetStereo(bool StereoFlag)
{
	uint16_t temp;

	temp = Rda5876hpRead(0x02);
	if(StereoFlag)
	{
		temp &= 0xDFFF;			//Stereo
	}
	else
	{
		temp |= 0x2000;			//Force Mono
	}
	Rda5876hpWrite(0x02, temp);
}

void Rda5876hpSetHighImpedance(bool Flag)
{
	uint16_t temp;

	temp = Rda5876hpRead(0x02);
	if(Flag)
	{
		temp &= 0x7FFF;			//HighImpedance
	}
	else
	{
		temp |= 0x8000;
	}
	Rda5876hpWrite(0x02, temp);
}
#endif
