//  maintainer: Sam
#include <string.h>
#include "type.h"
#include "app_config.h"
#include "i2c.h"
#include "clk.h"
#include "gpio.h"
#include "QN8035.h"
#include "dev_detect_driver.h"

#ifdef FUNC_RADIO_QN8035_EN

#define QN8025_CHIP_ADDR 	0x20

#define USING_INDUCTOR      0
#define INVERSE_IMR         1


#define R_TXRX_MASK    0x30
#define R_FMAM_MASK    0xc0

uint8_t	prevMode;

uint8_t	qnd_R16;
uint8_t	qnd_R17;
uint8_t	qnd_R46;
static void * FMI2cHandler = NULL;
bool QN8035WriteReg(uint16_t addr, uint8_t dat)
{
	uint8_t     tryCount = 8;

	while(--tryCount)
	{
		//if(I2C_WriteNByte(QN8025_CHIP_ADDR, addr,&dat, 1))
		if(I2cWriteNByte(FMI2cHandler, QN8025_CHIP_ADDR, addr, &dat, 1))
		{
			break;
		}
	}

	return 1;
}

uint8_t QNDReadReg(uint8_t regaddr)
{
	uint8_t	TempData = 0;
	uint8_t       tryCount = 8;

	while(--tryCount)
	{
		//if(I2C_ReadNByte(QN8025_CHIP_ADDR, regaddr, &TempData, 1))
		if(I2cReadNByte(FMI2cHandler, QN8025_CHIP_ADDR, regaddr, &TempData, 1))
		{
			break;
		}
	}

	return TempData;
}

void QNFSetRegBit(uint8_t regaddr, uint8_t bitMask, uint8_t dataval)
{
	uint8_t  temp;

	temp = QNDReadReg(regaddr);
	temp &= (~bitMask);
	temp |= dataval & bitMask;
	QN8035WriteReg((uint16_t)regaddr, temp);
}

//uint16_t FREQ2CHREG(uint16_t freq)
//{
//    return 	((freq << 1) - 1200);//(freq-6000)/5;
//}

uint8_t QNFSetCh(uint16_t freq)
{
	uint16_t f;
	uint8_t temp;

#if 1
	f = FREQ2CHREG(freq);
	//writing lower 8 bits of CCA channel start index
	QN8035WriteReg(CH_START, (uint8_t)f);
	//writing lower 8 bits of CCA channel stop index
	QN8035WriteReg(CH_STOP, (uint8_t)f);
	//writing lower 8 bits of channel index
	QN8035WriteReg(CH, (uint8_t)f);
	//writing higher bits of CCA channel start,stop and step index
	temp = (uint8_t)((f >> 8) & CH_CH);
	temp |= ((uint8_t)(f >> 6) & CH_CH_START);
	temp |= ((uint8_t)(f >> 4) & CH_CH_STOP);
	temp |= QND_STEP_CONSTANT;//(step << 6);
	QN8035WriteReg(CH_STEP, temp);
	return 1;

#else
	start = FREQ2CHREG(start);
	stop = FREQ2CHREG(stop);
	//writing lower 8 bits of CCA channel start index
	QN8035WriteReg(CH_START, (uint8_t)start);
	//writing lower 8 bits of CCA channel stop index
	QN8035WriteReg(CH_STOP, (uint8_t)stop);
	//writing lower 8 bits of channel index
	QN8035WriteReg(CH, (uint8_t)start);
	//writing higher bits of CCA channel start,stop and step index
	temp = (uint8_t)((start >> 8) & CH_CH);
	temp |= ((uint8_t)(start >> 6) & CH_CH_START);
	temp |= ((uint8_t)(stop >> 4) & CH_CH_STOP);
	temp |= (step << 6);
	QN8035WriteReg(CH_STEP, temp);
#endif
}

//void QNFRXInit(void)
//{
//}

//void QNFConfigScan(uint16_t start, uint16_t stop, uint8_t step)
//{
//	start =  stop =  step;
//}

void QN8035VolumeSet(void)
{
	QN8035WriteReg((uint16_t)0x14, (QNDReadReg(0x14) & 0xC0) | 0x07);
}

void QN8035Mute(bool MUTEFLAG)
{
	if(MUTEFLAG)
	{
		QN8035WriteReg(0x4A, 0x30);
	}
	else
	{
		WaitMs(200);
		QN8035WriteReg(0x4A, 0x10);
	}
}


void QNChipInitialization(void)
{
	QN8035WriteReg(0x00, 0x81);
	WaitMs(10);
	/*********User sets chip working clock **********/
	//Following is where change the input clock wave type,as sine-wave or square-wave.
	//default set is 32.768KHZ square-wave input.
	//QN8035WriteReg(0x01, QND_SINE_WAVE_CLOCK);//XTAL_INJ，晶体选择
	//QN8035WriteReg(XTAL_DIV0, 0x6E);
	//QN8035WriteReg(XTAL_DIV1, 0x01);
	//QN8035WriteReg(XTAL_DIV2, 0x54);
	QN8035WriteReg(0x01, QND_DIGITAL_CLOCK); //XTAL_INJ，晶体选择



	QN8035WriteReg(XTAL_DIV0, 0x01);
	QN8035WriteReg(XTAL_DIV1, 0x08);
	QN8035WriteReg(XTAL_DIV2, 0x5C);
	//WaitMs(10);

	QN8035WriteReg(0x54, 0x47);//mod PLL setting
	//select SNR as filter3,SM step is 2db
	QN8035WriteReg(0x19, 0xc4);
	QN8035WriteReg(0x40, 0x70);	//set SNR as SM,SNC,HCC MPX
	QN8035WriteReg(0x33, 0x9e);//set HCC and SM Hystersis 5db
	QN8035WriteReg(0x2d, 0xd6);//notch filter threshold adjusting
	QN8035WriteReg(0x43, 0x10);//notch filter threshold enable
	QN8035WriteReg(0x47, 0x39);
	//QND_WriteReg(0x57, 0x21);//only for qn8035B test
	//enter receiver mode directly
	QN8035WriteReg(0x00, 0x11);
	//Enable the channel condition filter3 adaptation,Let ccfilter3 adjust freely
	QN8035WriteReg(0x1d, 0xa9);
	QN8035WriteReg(0x4f, 0x40);//dsiable auto tuning
	QN8035WriteReg(0x34, SMSTART_VAL); ///set SMSTART
	QN8035WriteReg(0x35, SNCSTART_VAL); ///set SNCSTART
	QN8035WriteReg(0x36, HCCSTART_VAL); ///set HCCSTART
	QN8035Mute(TRUE);
}


void QN8035SetFreq(uint16_t Frep)
{
	uint8_t reg;
#if 0
	Qn8035_Mute(TRUE);
	//increase reference PLL charge pump current.
	QN8035WriteReg(REG_REF, 0x7a);

	/********** QNF_RXInit ****************/
	QN8035WriteReg(0x1b, 0x70); //Let NFILT adjust freely
	//QNF_SetRegBit(0x2C, 0x3F, 0x12);  ///When SNR<ccth31, ccfilt3 will work
	//setting the threshold of Filter3 will be worked.
	QN8035WriteReg(0x2c, 0x52);
	//QNF_SetRegBit(0x1D, 0x40, 0x00);  ///Let ccfilter3 adjust freely
	//QNF_SetRegBit(0x41, 0x0F, 0x0A);  ///Set a hcc index to trig ccfilter3's adjust
	QN8035WriteReg(0x45, 0x50);       ///Set aud_thrd will affect ccfilter3's tap number
	//QNF_SetRegBit(0x40, 0x70, 0x70);  ///snc/hcc/sm snr_rssi_sel; snc_start=0x40; hcc_start=0x30; sm_start=0x20
	QN8035WriteReg(0x40, 0x70);
	QN8035WriteReg(0x41, 0xca);
	QN8035WriteReg(0x34, SMSTART_VAL); ///set SMSTART
	QN8035WriteReg(0x35, SNCSTART_VAL); ///set SNCSTART
	QN8035WriteReg(0x36, HCCSTART_VAL); ///set HCCSTART
	/********** End of QNF_RXInit ****************/

	QNF_SetCh(Frep);
	//enable CCA mode with user write into frequency
	QN8035WriteReg(0x00, 0x13);
	//Auto tuning
	QN8035WriteReg(0x4f, 0x80);
	reg = QND_ReadReg(0x4f);
	reg >>= 1;
	QN8035WriteReg(0x4f, reg);
	///avoid the "POP" noise.
	WaitMs(CH_SETUP_DELAY_TIME);
	///decrease reference PLL charge pump current.
	QN8035WriteReg(REG_REF, 0x70);

#else
	QN8035WriteReg(REG_REF, 0x7a);

	/********** QNF_RXInit ****************/
	QN8035WriteReg(0x1b, 0x70);	//Let NFILT adjust freely
	QN8035WriteReg(0x2c, 0x52);
	QN8035WriteReg(0x45, 0x50);	//Set aud_thrd will affect ccfilter3's tap number
	QN8035WriteReg(0x41, 0xca);
	/********** End of QNF_RXInit ****************/
#if INVERSE_IMR
	reg = QNDReadReg(CCA)&~0x40;
	if((Frep == 934) || (Frep == 939) || (Frep == 953) || (Frep == 998) || (Frep == 1048))
	{
		reg |= 0x40;	// inverse IMR.
	}
	else
	{
		reg &= ~0x40;
	}
	QN8035WriteReg(CCA, reg);
#endif
	QN8035Mute(TRUE);
//	QNF_SetCh(Frep, Frep, 1);
	QNFSetCh(Frep);
	//enable CCA mode with user write into frequency
	QN8035WriteReg(0x00, 0x13);
#if USING_INDUCTOR
	//Auto tuning
	QN8035WriteReg(0x4f, 0x80);
	reg = QNDReadReg(0x4f);
	reg >>= 1;
	QN8035WriteReg(0x4f, reg);
#endif
	///avoid the "POP" noise.
	WaitMs(CH_SETUP_DELAY_TIME);
	///decrease reference PLL charge pump current.
	QN8035WriteReg(REG_REF, 0x70);
	//QN8035Mute(FALSE);  //litao modified,自动搜台中有"哒哒"声。
#endif
}

bool QN8035Init(void)
{
	DBG("QN8035 Init\n");
	if(FMI2cHandler)
	{
		//GpioClk32kIoConfig(1);
		QNChipInitialization();
		QN8035SetFreq(QN8035_DEF_FREQ);//??
		QN8035VolumeSet();
	}
	else
	{
		APP_DBG("I2cMasterCreate Fail!\n");
		return FALSE;
	}
	return TRUE;
}

void QN8035OutputHIZ(bool HIZOnOff)   //litao add SDKV215 [QNxxx没有高阻抗设置寄存器，通过验证QNxxx没法实现输出高阻抗]
{
	if(HIZOnOff)
	{
		prevMode = QNDReadReg(SYSTEM1);
		QNFSetRegBit(SYSTEM1, R_TXRX_MASK, STNBY); 	// power down
	}
	else
	{
		QNFSetRegBit(SYSTEM1, R_TXRX_MASK, ~STNBY);  //power up
	}
	if(HIZOnOff == FALSE)
	{
		WaitMs(200);    // wait for power stable
	}
}

bool QN8035PowerOn(void)
{
	if(!FMI2cHandler)
	{
		FMI2cHandler = I2cMasterCreate(PORT_C, 3, PORT_C, 4);
	}
	if(FMI2cHandler)
	{
		if(QN8035ReadID())
		{
			QNFSetRegBit(SYSTEM1, R_TXRX_MASK, ~STNBY);  //power up
			WaitMs(200); // wait for power stable
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		APP_DBG("I2cMasterCreate Fail!\n");
		return FALSE;
	}
//	return TRUE;
}

bool QN8035PowerDown(void)
{
	QN8035Mute(TRUE);
//	prevMode = QND_ReadReg(SYSTEM1);
//	QNF_SetRegBit(SYSTEM1, R_TXRX_MASK, STNBY);
	return TRUE;
}

/*
void QNDSetSysMode(uint16_t mode)
{
	mode = mode;
}
*/

void QN8035RXSetTH(void)
{
#if 0
	QN8035WriteReg(0x4f, 0x00);//enable auto tunnging in CCA mode
	///increase reference PLL charge pump current.
	QN8035WriteReg(REG_REF, 0x7a);
	//NFILT program is enabled
	QN8035WriteReg(0x1b, 0x78);
	//using Filter3
	QN8035WriteReg(CCA1, 0x75);
	//setting CCA IF counter error range value(256).
	QN8035WriteReg(CCA_CNT2, 0x05);
#if PILOT_CCA
	QN8035WriteReg(PLT1, 0x00);
#endif
	QN8035WriteReg(CCA, QND_ReadReg(CCA) & 0xc0 | 25);
#if PILOT_CCA
	QN8035WriteReg(CCA_SNR_TH_1, 8 + 0); //setting SNR threshold for CCA
#else
	QN8035WriteReg(CCA_SNR_TH_1, 9 + 0); //setting SNR threshold for CCA
#endif

#else
	uint8_t rssi_th, cca_snr_th_1, cca_snr_th_2 = 0x05;

//	rssi_th = (qnd_PreNoiseFloor-28) + 3;
	///increase reference PLL charge pump current.
	QN8035WriteReg(REG_REF, 0x7a);
	//NFILT program is enabled
	QN8035WriteReg(0x1b, 0x78);
	//using Filter3
	QN8035WriteReg(CCA1, 0x75);
	//setting CCA IF counter error range value(768).
	QN8035WriteReg(CCA_CNT2, 0x03);
#if PILOT_CCA
	QN8035WriteReg(PLT1, 0x00);
#endif
	//selection the time of CCA FSM wait SNR calculator to settle:20ms
	//0x00:	    20ms(default)
	//0x40:	    40ms
	//0x80:	    60ms
	//0xC0:	    100m	
	cca_snr_th_1 = 0x40;//0xC0; //litao modified SDKV215 [自动搜台无法搜到台]
	//selection the time of CCA FSM wait RF front end and AGC to settle:20ms
	//0x00:     10ms
	//0x40:     20ms(default)
	//0x80:     40ms
	//0xC0:     60ms	
	cca_snr_th_2 |= 0x80;//0xC0;   //litao modified SDKV215
	QN8035WriteReg(CCA_SNR_TH_2, cca_snr_th_2);

	rssi_th = 0;
	QN8035WriteReg(CCA, QNDReadReg(CCA) & 0xc0 | rssi_th);
#if PILOT_CCA
	QN8035WriteReg(CCA_SNR_TH_1, cca_snr_th_1 | 10); //setting SNR threshold for CCA
#else
	QN8035WriteReg(CCA_SNR_TH_1, cca_snr_th_1 | 9); //setting SNR threshold for CCA
#endif

#endif
}


void QN8035SearchSet(uint16_t freq)
{
	QN8035Mute(TRUE);
	QN8035RXSetTH();
	QNFSetCh(freq);

#if USING_INDUCTOR
	//Auto tuning
	QN8035WriteReg(0x00, 0x11);
	QN8035WriteReg(0x4f, 0x80);
	regValue1 = QNDReadReg(0x4f);
	regValue1 = (regValue1 >> 1);
	QN8035WriteReg(0x4f, regValue1);
#endif

	QN8035WriteReg(0x00, 0x12);
}

bool QN8035SearchRead(uint16_t Freq)
{
	uint8_t regValue;
	uint8_t snr;
	static	uint8_t	timeOut = 0;
	uint8_t readCnt, stereoCount;
	regValue = QNDReadReg(SYSTEM1);
	if((!(regValue & CHSC)) || (timeOut > 5))
	{
		timeOut = 0;
		//regValue = QND_ReadReg(STATUS1) & RXCCA_FAIL;
		regValue = (QNDReadReg(STATUS1) & RXCCA_FAIL ? 0 : 1);
		if(regValue)
		{
			DBG("Search  Freq %d\n", Freq);
#if PILOT_CCA
			WaitMs(60);
			snr = QNDReadReg(SNR);
			DBG("snr %d\n", (uint16_t)snr);

			//if((Freq != 960) && (Freq  != 1080))
			{
				//if(snr > 30)
				if(snr> 15)
				{
					DBG("Search Freq %d\n", Freq);
					DBG("SNR > 25 %d\n", Freq);
					return TRUE;
				}
			}

			stereoCount = 0;
			for(readCnt = 10; readCnt > 0; readCnt--)
			{
				WaitMs(2);//QND_Delay(2);
				stereoCount += ((QNDReadReg(STATUS1) & ST_MO_RX) ? 0 : 1);
				if(stereoCount > 3)
				{

					DBG(" Freq ok  okok %d\n", Freq);
					return TRUE;//RADIO_SEEK_VALIDSTATION;
				}
			}
			return   FALSE;
#endif
//			return TRUE;

		}
		return FALSE;
	}
	timeOut++;
	return FALSE;//RADIO_SEEK_FALSE;

}

bool QN8035ReadID(void)
{
	if((QNDReadReg(0x06) & 0xFC) == 0x84)
	{
		DBG("QN8035 ID : 0x84\n");
		//QN8035Init();
		return TRUE;
	}
	else
	{
		DBG("QN8035 ID ERR : %d\n", (QNDReadReg(0x06) & 0xFC));
		return FALSE;
	}
//	QN8035Mute(TRUE);
}

//
//bool QN8035ReadID(void)
//{
//	uint8_t i = 0;
//
//	for(i = 0; i < 5; i++)
//	{
//		WaitMs(5);
//		if((QNDReadReg(0x06) & 0xFC) == 0x84)
//		{
//			DBG("QN8035 ID : 0x84\n");
//			QN8035Init();
//			return TRUE;
//		}
///*
//		else if((QNDReadReg(0x06) & 0xF8) == 0x80)
//		{
////QN8025 QN8035	驱动不一样的
//			DBG("QN8025 ID : 0x84\n");
//			QN8025_Init();
//			return TRUE;
//		}
//*/
//		else
//		{
//			//DBG("QN8035 ID ERR : %BU\n",(QNDReadReg(0x06) & 0xFC));
//			//DBG("QN8035 ID ERR : %2BX\n",(QNDReadReg(0x06) & 0xFC));
//			DBG("QN8035 ID ERR :: %2BX\n",(QNDReadReg(0x06)));
//			//return FALSE;
//			//return TRUE;//just debug
//		}
//	}
//	return FALSE;
//}

/*
The QN8035 integrates an analog volume controller and a digital volume controller to set audio output gain.
The digital gain step is 1dB, and the analog gain step is 6dB.
The total gain range is -47 dB to 0 dB.
Refer to Reg14h for more descriptions.
*/
void QN8035VolSet(uint8_t vol)
{
	uint8_t u8reg;

	u8reg = QNDReadReg(0x14);
	//vol &= 0x3F;
	//u8reg &= 0xC0;
	vol &= 0x07;//we just use the 6dB
	u8reg &= 0xF8;
	u8reg |= vol;
	QN8035WriteReg(0x14, u8reg);
}

#endif	//#ifdef FUNC_RADIO_QN8035_EN
