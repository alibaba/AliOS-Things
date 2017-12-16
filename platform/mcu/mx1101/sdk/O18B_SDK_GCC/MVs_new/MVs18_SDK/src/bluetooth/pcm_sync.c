/*
*****************************************************************************
*					Mountain View Silicon Tech. Inc.
*	Copyright 2013, Mountain View Silicon Tech. Inc., ShangHai, China
*					All rights reserved.
*
* Filename:			pcm_sync.c
* Description:		PCM MODULE DRIVER FOR BLUETOOTH
*
*  maintainer: Aissen
*
* Change History:
*        Justin	- 05/30/2013 - V0.1
*					- created
******************************************************************************/

#include <string.h>
#include "type.h"
#include "app_config.h"
#include "pcm_sync.h"
#include "pcm_fifo.h"
#include "pcm_transfer.h"
#include "i2s.h"
#include "audio_path.h"
#include "audio_adc.h"
#include "dac.h"
#include "clk.h"
#include "gpio.h"
#include "blue_aec.h"
#include "rst.h"
#include "timeout.h"
#include "debug.h"
#include "bt_app_func.h"

#define ADC_DATA_BUFFER_SIZE	(BLK_LEN*2)
#define I2S_DATA_BUFFER_SIZE	(BLK_LEN*2)
#define PCM_RECV_BUFFER_SIZE	(BLK_LEN*2)

uint8_t AdcDataBuffer[ADC_DATA_BUFFER_SIZE]__ATTRIBUTE__(AT(VMEM_ADDR)); //adc buffer
uint8_t I2sInDataBuffer[I2S_DATA_BUFFER_SIZE]__ATTRIBUTE__(AT(VMEM_ADDR + ADC_DATA_BUFFER_SIZE)); //i2s in data in buffer
uint8_t I2sOutDataBuffer[I2S_DATA_BUFFER_SIZE * 2]__ATTRIBUTE__(AT(VMEM_ADDR + ADC_DATA_BUFFER_SIZE + I2S_DATA_BUFFER_SIZE));//i2s out data in buffer
#ifdef BT_HF_RECORD_FUNC
uint16_t PcmRecvBuffer[PCM_RECV_BUFFER_SIZE];
uint16_t* PcmRecvBufferWPtr = PcmRecvBuffer;
uint16_t* PcmRecvBufferRPtr = PcmRecvBuffer;
#endif
BlueAECContext* gBlueAecContext = (BlueAECContext*)(VMEM_ADDR + (BLK_LEN * 2) * 4); // for aec

static bool PcmSyncInitFlag = FALSE;

//
// AecStatus
//0:disable phone and mv aec;
//1:disable phone ace,enable mv ace;
//2:enable phone aec, disable mv aec
//3:enable phone aec, enable mv aec
//
static uint8_t AecStatus = 3;  //default value, app-callback 函数内部enable开关可以打开
void PcmSyncAecSetStatus(uint8_t Status)
{
	AecStatus = Status;
}

uint8_t PcmSyncAecGetStatus(void)
{
	return  AecStatus;
}
//void AecDebug(void)
//{
//	DBG("aec  current aec status\n");
//    DBG("# AecStatus aec%d #\n", AecStatus);
//}
//
//void Aec0Debug(void)
//{
//    if((AecStatus == 2) && PcmSyncInitFlag)
//    {
//        DBG("change from aec2 to aec0. fail\n");
//    }
//    else
//    {
//        DBG("aec0  disable phone and mv aec\n");
//        AecStatus = 0;
//    }
//
//}
//
//void Aec1Debug(void)
//{
//    if((AecStatus == 2) && PcmSyncInitFlag)
//    {
//        DBG("change from aec2 to aec1. fail\n");
//    }
//    else
//    {
//    	DBG("aec1  disable phone ace, enable mv ace\n");
//        AecStatus = 1;
//    }
//}
//
//void Aec2Debug(void)
//{
//
//    if((!PcmSyncInitFlag) || (AecStatus == 2))
//    {
//    	DBG("aec2  enable phone aec, disable mv aec\n");
//        AecStatus = 2;
//    }
//    else
//    {
//        DBG("change from aec%d to aec2. fail\n", AecStatus);
//    }
//}
//
//SHELL_CMD(
//	aec, 0, AecDebug,
//	"current aec status\n",
//	"Usage:\n"
//	"aec\n"
//);
//
//SHELL_CMD(
//	aec0, 0, Aec0Debug,
//	"disable phone aec,disable mv aec\n",
//	"Usage:\n"
//	"aec0\n"
//);
//
//SHELL_CMD(
//	aec1, 0, Aec1Debug,
//	"disable phone aec,enable mv aec\n",
//	"Usage:\n"
//	"aec1\n"
//);
//
//SHELL_CMD(
//	aec2, 0, Aec2Debug,
//	"enable phone aec,disable mv aec\n",
//	"Usage:\n"
//	"aec2\n"
//);

/////////Example: set phone call mic out digital gain, added by zhouyi @ 20140515/////////////
static int32_t I2SMicOutVolumGain = 0x100; /*x100 => 1.0*/

/////---API: One can change the gain online----///////////////////////////
void SetI2SMicOutVolumGain(int32_t Gain/*0x100 => 1.0*/)
{
    I2SMicOutVolumGain = Gain;
    
    if(I2SMicOutVolumGain > 32767)
    {
        I2SMicOutVolumGain = 32767;
    }
}

void DoI2SMicOutDigitalGain(int16_t *DataInOut, int32_t Samples, int32_t Gain)
{
    int32_t i;
    
    for(i = 0 ; i < Samples ; i ++)
    {
        DataInOut[i] = __ssat( (((int32_t)DataInOut[i]) * Gain) >> 8, 16);
    }
}

void PcmSyncPlayInit(void)
{
	memset(((uint8_t*)I2sInDataBuffer), 0, BLK_LEN * 6);

	NVIC_DisableIRQ(DECODER_IRQn);
	// Transfer PCM data to FIFO
	PcmTxInitWithDefaultPcmFifoSize(DATA_MODE_MONO_LOW);

	PcmFifoInit(1024 * 11, 2048);

	PcmTxTransferData((void*)I2sOutDataBuffer, (void*)I2sOutDataBuffer, BLK_LEN);
}

void PcmSyncInit(uint8_t I2sClkType)
{
	if(!PcmSyncInitFlag)
	{
		CodecDacMuteSet(TRUE, TRUE);
//		GpioPcmSyncIoConfig(1);        //0:B24/25/26/27
		RstDecModule();
		PhubPathClose(ALL_PATH_CLOSE);
		
		if(!PhubPathSel(ADCIN2PMEM_IISIN2DACOUT_PCMFIFO2IISOUT)) // config bluetooth mic path
		{
			DBG("phub path sel err!\n");
			return;
		}
//		PhubPathClose((PHUB_PATH_SEL)0x7FD); //add by sam @2014-08-07，I2S In path Disable
//		RstPhubModule();//reset 相关通路寄存器值，消除pop音，phub会复位寄存器，不要随便调用
//		RstMclk();//reset 相关通路寄存器值，消除pop音		
		//config ADC
		CodecAdcAnaInit();
		CodecAdcChannelSel(ADC_CH_MIC_L | ADC_CH_MIC_R);

		AdcPmemWriteDis();
		AdcToPmem(USB_MODE, 0x3400, 0x800);
		AdcPmemWriteEn();

		CodecAdcMicGainConfig(0,0); //模拟端调到最大 默认是20，0是最大
		DacVolumeSet(3800, 3800);
		AdcVolumeSet(0xfff, 0xfff);

		//config I2s to dac
		I2sWritePmemDis();
		I2sInToPmemConfig(0x3C00, 0x800);
		I2sWritePmemEn();

		if(MV_MCU_AS_PCMSYNC_CLK_MASTER == I2sClkType)
		{
			I2sSampleRateSet(8000, USB_MODE);
			I2sBluetoothMasterModeSet(I2S_DATA_FROM_DEC, USB_MODE, I2S_DSP_MODEA);
		}
		else
		{
			I2sSampleRateSet(8000, USB_MODE);
			I2sBluetoothSlaveModeSet(I2S_DATA_FROM_DEC, USB_MODE, I2S_DSP_MODEA);
		}
		
		DacSoftMuteSet(TRUE, TRUE);
		CodecDacChannelSel(DAC_CH_DECD_L | DAC_CH_DECD_R); // select dac channel
		
		DacSoftMuteSet(TRUE, TRUE); //add by sam @2014-08-07，再次Mute，CodecDacChannelSel()函数会有Unmute操作
		DacAdcSampleRateSet(8000, USB_MODE);	 //九种采样率
		DacConfig(MDAC_DATA_FROM_I2S, USB_MODE);
		WaitMs(500);
		
//		if(!PhubPathSel(ADCIN2PMEM_IISIN2DACOUT_PCMFIFO2IISOUT))//add by sam @2014-08-07，I2S In path enable
//		{
//			DBG(("phub path sel err!\n"));
//			return;
//		}

//		CodecDacMuteSet(FALSE, FALSE);
		PcmSyncPlayInit();

		blue_aec_init(gBlueAecContext);

		DacSoftMuteSet(FALSE, FALSE);
	}

	PcmSyncInitFlag = TRUE;
}

//
//Get PcmSyncInit Flag
//
bool PcmSyncGetInitFlag(void)
{
	return PcmSyncInitFlag;
}

//
//Process Pcm Data
//
void PcmSyncDataProcess(void)
{
	if(PcmSyncInitFlag)
	{

		if(PcmTxIsTransferDone())
		{
			uint32_t Size = 0;
			uint32_t i = 0, pos = 0;
			TIMER timer;
			TimeOutSet(&timer, 20);

			PcmTxDoneSignalClear();

			while((Size < BLK_LEN) && (!IsTimeOut(&timer)))
			{
				Size += AdcPcmDataRead(AdcDataBuffer + Size * 2, BLK_LEN - Size, 1);
			}

			Size = 0;
			TimeOutSet(&timer, 20);
			while((Size < BLK_LEN) && (!IsTimeOut(&timer)))
			{
				Size += I2sPcmDataRead(I2sInDataBuffer + Size * 2, BLK_LEN - Size, 1);
			}

			if((AecStatus == 3) || (AecStatus == 1))
			{
				blue_aec_run(gBlueAecContext, (int16_t*)I2sInDataBuffer, (int16_t*)AdcDataBuffer, (int16_t*)I2sOutDataBuffer);
			}
			else
			{
				memcpy(I2sOutDataBuffer, AdcDataBuffer, BLK_LEN * 2);
			}

#ifdef BT_HF_RECORD_FUNC
			if(GetBtRecordState() == BT_RECORD_STATE_RECORDING)
			{
				WriteDataToPcmBuffer();
			}
#endif

			DoI2SMicOutDigitalGain((int16_t*)(I2sOutDataBuffer), BLK_LEN, I2SMicOutVolumGain);
			
			pos = I2S_DATA_BUFFER_SIZE - 2;
			
			for(i = I2S_DATA_BUFFER_SIZE * 2 - 4; i > 0; i -= 4, pos -= 2)
			{
				*((uint16_t*)&I2sOutDataBuffer[i]) = *((uint16_t*)&I2sOutDataBuffer[pos]);
			}
			
			PcmTxTransferData((void*)I2sOutDataBuffer, (void*)I2sOutDataBuffer, BLK_LEN);
		}

	}
}

//
//Uninitialize I2s data in/out、adc
//
void PcmSyncUnInit(void)
{
	if(PcmSyncInitFlag)
	{
//		CodecDacMuteSet(TRUE, TRUE);
//		CodecAdcChannelDisconnect(ADC_CH_MIC_L | ADC_CH_MIC_R);
		CodecAdcAnaDeInit();
		AdcClose();
		DacVolumeSet(4000, 4000);
		AdcVolumeSet(4000, 4000);
        AdcPmemWriteDis();
        I2sWritePmemDis();

		NVIC_EnableIRQ(DECODER_IRQn);

		PcmSyncInitFlag = FALSE;
	}

}

#ifdef BT_HF_RECORD_FUNC
void InitBtHfRecordPcm(void)
{
	memset(PcmRecvBuffer, 0, PCM_RECV_BUFFER_SIZE);
	PcmRecvBufferWPtr = PcmRecvBuffer;
	PcmRecvBufferRPtr = PcmRecvBuffer;
}

uint32_t GetAvailablePcmSyncDataSize(void)
{
	if(PcmRecvBufferWPtr >= PcmRecvBufferRPtr)
	{
		return (PcmRecvBufferWPtr - PcmRecvBufferRPtr);
	}
	else
	{
		return (PCM_RECV_BUFFER_SIZE - (PcmRecvBufferRPtr - PcmRecvBufferWPtr));
	}
}

uint32_t GetAvailablePcmBufSize(void)
{
	if(PcmRecvBufferWPtr >= PcmRecvBufferRPtr)
	{
		return PCM_RECV_BUFFER_SIZE - (PcmRecvBufferWPtr - PcmRecvBufferRPtr);
	}
	else
	{
		return (PcmRecvBufferRPtr - PcmRecvBufferWPtr);
	}
}

bool WriteDataToPcmBuffer(void)
{
	uint32_t	i;

	if(GetAvailablePcmBufSize() >= (I2S_DATA_BUFFER_SIZE >> 1))
	{
		for(i = 0; i < BLK_LEN; i++)
		{
			*(PcmRecvBufferWPtr++) = __SSAT((int32_t)((int16_t*)I2sInDataBuffer)[i] + (int32_t)((int16_t*)I2sOutDataBuffer)[i], 16);
			if(PcmRecvBufferWPtr > PcmRecvBuffer + PCM_RECV_BUFFER_SIZE)
			{
				PcmRecvBufferWPtr = PcmRecvBuffer;
			}
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

uint32_t ReadDataFromPcmBuffer(uint16_t* RecvBuf, uint32_t ReadLen)
{
	uint32_t		CpyLen = ReadLen;
	uint32_t		i;
	uint32_t		AvailableDataSize = 0;
	
	if(RecvBuf == NULL || ReadLen == 0)
		return 0;

	AvailableDataSize = GetAvailablePcmSyncDataSize();
	if(AvailableDataSize < ReadLen)
	{
		CpyLen = AvailableDataSize;
	}

	for(i = 0; i < CpyLen; i++)
	{
		*(RecvBuf++) = *(PcmRecvBufferRPtr++);
		if(PcmRecvBufferRPtr > PcmRecvBuffer + PCM_RECV_BUFFER_SIZE)
			PcmRecvBufferRPtr = PcmRecvBuffer;
	}

	return CpyLen;
}
#endif
