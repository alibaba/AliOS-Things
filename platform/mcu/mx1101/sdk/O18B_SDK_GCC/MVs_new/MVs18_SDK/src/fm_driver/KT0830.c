/*
*****************************************************************************
*					Mountain View Silicon Tech. Inc.
*	Copyright 2009, Mountain View Silicon Tech. Inc., ShangHai, China
*					All rights reserved.
*
* Filename:
* Description:
* maintainer Sam
******************************************************************************
*/
#include <string.h>
#include "type.h"
#include "utility.h"
#include "debug.h"
#include "timer.h"
#include "Sysctrl.h"
#include "KT0830.h"
#include "i2c.h"
#include "breakpoint.h"
#include "RadioCtrl.h"



#ifdef FUNC_RADIO_KT0830_EN


//global variables
int8_t mem_afc[3];			  //Rememberred afc values for previous, current and next stations
uint16_t mem_freq[3];			  //Rememberred channel frequencies for previous, current and next stations
#ifdef SEEK_WITH_SNR
uint8_t mem_snr[3];			  //Rememberred SNR values for previous, current and next stations
#endif

bool KT_Bus_Write(uint8_t Register_Address, uint16_t Word_Data)
{
	return I2C_WriteNByte(0x6E, Register_Address, (uint8_t*)&Word_Data, 2);
}

uint16_t KT_Bus_Read(uint8_t Register_Address)
{
	uint16_t	TempData = 0;

	I2C_ReadNByte(0x6E, Register_Address, (uint8_t*)(&TempData), 2);
	return TempData;
}

void KT_SetRegBit(uint8_t regaddr, uint16_t bitMask, uint16_t dataval)
{
	uint16_t  temp;

	temp = KT_Bus_Read(regaddr);
	temp &= (bitMask);
	temp |= dataval;
	KT_Bus_Write(regaddr, temp);
}

void KT_FMAntTuneOn(void)     //The chip will tune the antenna automatically whenever TUNE function is performed
{
	uint16_t regx;

	regx = KT_Bus_Read(0x23);
	KT_Bus_Write(0x23, regx | 0x0004);//antenna tuning enable
	regx = KT_Bus_Read(0x1D);
	KT_Bus_Write(0x1D, (regx & 0xff7f) | ((3 - 1) << 5)); //cali loop disable = 0
}

#if 0
bool KT_FMAntTuneOff(uint16_t cap)
//The antenna tuning function will be turn off and certain amount of caps will be connected to RFIN
//The unit of parameter cap is fF. The capactior value is range from 0fF to 49750fF.
{
	uint16_t capindex;
	if(cap < 2000)
	{
		KT_SetRegBit(0x23, 0xFFFB, 0x0);
		KT_SetRegBit(0x1D, 0x00FF, 0x0080);
	}
	else
	{
		KT_SetRegBit(0x23, 0xFFFF, 0x0004);
		capindex = (cap - 2000) / 250;
		if(capindex > 0xBF)
		{
			capindex = 0xBF;
		}
		KT_SetRegBit(0x1D, 0x00FF, (0x0080 | (capindex << 8)));
	}
	return TRUE;
}

#endif

void KT0830Mute(bool MuteFg)
{
	uint16_t reg5;

	if(MuteFg)
	{
		reg5 = KT_Bus_Read(0x04);
		//KT_Bus_Write(0x04, reg5 & 0xFFF0);   //Write volume to 0
		KT_Bus_Write(0x04, reg5 & 0x3FFF);
	}
	else
	{
		reg5 = KT_Bus_Read(0x04);
		//KT_Bus_Write(0x04, reg5 & 0xFFF0 | 0x0F);
		KT_Bus_Write(0x04, reg5 | 0xC000);
	}
}

void KT0830SetFreq(uint16_t Freq) //87.5MHz-->Frequency = 8750
{
	uint16_t channel;

	KT0830Mute(TRUE);
	channel = (Freq - 640) << 1;	//87.5Mhz --> 875
	KT_Bus_Write(0x03, 0x8800 | channel);		//set tune bit to 1
//	WaitMs(50);
}

void KT0830SearchSet(uint16_t Freq)  //litao add SDKV215
{
	DBG("Bk1080_SearchSet\n");
	KT0830SetFreq(Freq);
}
void KT0830Init(void)
{
	uint16_t reg5;
	uint8_t i;

	DBG("START INIT\n");

	KT_Bus_Write(0x0f, 0x8a00);

//select crystal or reference clock
//if(Radio.ExtOsc12m)
//	if(gRadioCtrl.CkSel == RADIO_XTAL_EXCLK_12MHZ)
//	if(gRadioCtrl.RadioData2Store.BandXtalSel & RADIO_XTAL_EXCLK_12MHZ)
//	if((gRadioCtrl.RadioData2Store.BandXtalSel & 0x80) == RADIO_XTAL_EXCLK_12MHZ)
	if((gRadioData.BandXtalSel & MASK_RADIO_XTAL) == RADIO_XTAL_EXCLK_12MHZ)
	{
		KT_Bus_Write(0x23, 0x1301); //reference clock = 12M; xtald = 1
	}


	for(i = 0; i < INIT_FAIL_TH; i++)
	{
		WaitMs(50);
		reg5 = KT_Bus_Read(0x12);			//Read power-up indicator
		if((reg5 & 0x8800) == 0x8800)
		{
			break;
		}
	}

	if(i == INIT_FAIL_TH)
	{
		DBG("INIT_FAIL_TH\n");
		return;
	}

	KT_Bus_Write(0x0a, 0x0000);      //Turn On AFC function
	KT_Bus_Write(0x02, 0x2207);      //Write Frequency Space to 100KHz
	if((KT_Bus_Read(0x1D) & 0x0080) == 0x0080)	//if bit7 == 1, KT0830EG run patch
	{
		KT_SetRegBit(0x22, 0xFFFF, 0x1000);
		KT_SetRegBit(0x21, 0xFFF8, 0x0003);
		KT_SetRegBit(0x09, 0x1FFF, 0x2000);
		KT_SetRegBit(0x0A, 0xFFFF, 0x0400);
		while((KT_Bus_Read(0x10) & 0x4000) != 0)			//Wait cali-done signal
		{
			WaitMs(50);
		}

		KT_Bus_Write(0x10, 0x5200);						//Recali sdmcode
		WaitMs(10);
		reg5 = KT_Bus_Read(0x11);
		if((reg5 & 0x000f) > 0x0A)
		{
			KT_Bus_Write(0x11, (reg5 & 0xfff0) | 0x000A);	//sdm_code patch
		}
	}

	KT_SetRegBit(0x23, 0xFFFF, 0x0008);
	KT_SetRegBit(0x04, 0xFFF0, 0x000F); //vol

	KT_FMAntTuneOn();
	WaitMs(50);

	DBG("END INIT\n");

}

bool KT0830ReadID(void)
{
	WaitMs(200);  //litao modified
	if(KT_Bus_Read(0x01) == 0x0440)
	{
		DBG("KT0830 ID : 0x0440\n");
		KT0830Init();
		return TRUE;
	}
	DBG("KT0830 ID ERR: %x\n", KT_Bus_Read(0x01));
	KT0830Mute(TRUE);
	return FALSE;
}

void KT0830OutputHIZ(bool HIZOnOff)   //litao add SDKV215
{
	if(HIZOnOff)
	{
#ifdef MUTE_WITH_HIZ
		KT_Bus_Write(0x08, 0xff70); 		 //Turn off audio driver,Audio Ouput High Impedance
#endif
	}
	else
	{
#ifdef MUTE_WITH_HIZ
		KT_Bus_Write(0x08, 0xFFF0);  		//Turn on audio driver,Audio Ouput Normal Operation
#endif
	}
}
void KT0830PowerOn(void)
{

}

void KT0830PowerDown(void)
{
	KT0830Mute(TRUE);
//	KT_Bus_Write(0x0f, 0x9a00);      //Write Standby bit to 1
// 	WaitMs(20);
}

/*
void KT0830SearchSet(uint16_t Freq)//, uint8_t Direction)
{
	KT0830SetFreq(Freq);
}
*/

//char KT0830E_GetAFC(unsigned int Frequency) // returned value's range is from -32 to 31, unit is KHz
int8_t KT0830EGetAFC(void) // returned value's range is from -32 to 31, unit is KHz
{
	return (int8_t)KT_Bus_Read(0x15);
}

uint8_t KT0830EFMGetSNR(void)
{
	return (uint8_t)KT_Bus_Read(0x1F);
}

bool KTFMGetST(void)
{
	uint16_t regx;

	regx = KT_Bus_Read(0x06);
	return ((regx & 0x7f00) < TST_TH);
}

uint8_t KT0830SearchRead(void) //0-->False Station 1-->Good Station //check AFC_DELTA only
{
	uint16_t	pFreq;//uint16_t* pFreq;
	uint16_t nextfreq;
	int8_t afc[3];						//AFC value for previous, current and next channels
	uint16_t freq[3];							//frequency values for previous, current and next channels
#ifdef SEEK_WITH_SNR
	uint8_t snr[3];							//SNR for previous, current and next channels
#endif
	uint8_t i, j;
	uint8_t snr2, snr3;
#if 1
	afc[0] = 0;
	afc[1] = 0;
	afc[2] = 0;				//initialize
	freq[0] = 0;
	freq[1] = 0;
	freq[2] = 0;			//initialize
#ifdef SEEK_WITH_SNR
	snr[0] = 0;
	snr[1] = 0;
	snr[2] = 0;			//initialize
#endif
#endif
	pFreq = gRadioData.CurrFreq;
	nextfreq = pFreq - CHANNEL_STEP;

	//Get AFC values of previous, current and next channels
	for(i = 0; i < 3; i++)
	{
		//get AFC values for the last station's information
		for(j = 0; j < 3; j++)
		{
			if(mem_freq[j] == nextfreq)
			{
				afc[i] = mem_afc[j];
				freq[i] = mem_freq[j];
#ifdef SEEK_WITH_SNR
				snr[i] = mem_snr[j];
#endif
			}
		}
		//get new AFC value if the station information is not investigated in the last run
		if(!freq[i])
		{
			KT0830SetFreq(nextfreq);
			WaitMs(50);
			afc[i] = KT0830EGetAFC();
#ifdef SEEK_WITH_SNR
			snr[i] = KT0830EFMGetSNR();
#endif
			freq[i] = nextfreq;
		}
		nextfreq += CHANNEL_STEP;
	}

	//Record AFC values
	mem_afc[0] = afc[0];
	mem_afc[1] = afc[1];
	mem_afc[2] = afc[2];
	mem_freq[0] = freq[0];
	mem_freq[1] = freq[1];
	mem_freq[2] = freq[2];
#ifdef SEEK_WITH_SNR
	mem_snr[0] = snr[0];
	mem_snr[1] = snr[1];
	mem_snr[2] = snr[2];
#endif

	//Determine the validation of current station
	if((afc[0] < afc[1]) && (afc[1] < afc[2]) && (afc[0] < -AFCTH_PREV)
	        && (afc[1] > -AFCTH) && (afc[1] < AFCTH) && (afc[2] > AFCTH_NEXT))
	{
		if((gRadioData.CurrFreq == 960) || (gRadioData.CurrFreq == 909) || (gRadioData.CurrFreq == 903) || (gRadioData.CurrFreq == 944)
		        || (gRadioData.CurrFreq == 985) || (gRadioData.CurrFreq == 1020) || (gRadioData.CurrFreq == 1067)
		        || (gRadioData.CurrFreq == 1080) || (gRadioData.CurrFreq  == 1040)) //gRadioCtrl.Freq
		{
			KT0830SetFreq(gRadioData.CurrFreq);
			WaitMs(200);
			if(!KTFMGetST())
			{
				return FALSE;
			}
		}
#ifdef SEEK_WITH_SNR
		KT0830SetFreq(gRadioData.CurrFreq);
		WaitMs(50);
		snr2 = KT0830EFMGetSNR();
		if((snr[1] >= SNR_TH) && (snr2 >= SNR_TH))
		{
			return TRUE;
		}
		if((snr[1] < SNR_TH) && (snr2 < SNR_TH))
		{
			return FALSE;
		}
		WaitMs(50);
		snr3 = KT0830EFMGetSNR();
		if(snr3 >= SNR_TH)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
#else
		return TRUE;
#endif
	}
	else
	{
		return FALSE;
	}
}

void KT0830VolSet(uint8_t vol)
{
	uint16_t u16reg;
	u16reg = KT_Bus_Read(0x04);
	vol &= 0x0F;
	u16reg &= 0xFFF0;
	u16reg |= vol;
	KT_Bus_Write(0x04, u16reg);
}


#endif	//#ifdef FUNC_RADIO_KT0830_EN
// end of file
