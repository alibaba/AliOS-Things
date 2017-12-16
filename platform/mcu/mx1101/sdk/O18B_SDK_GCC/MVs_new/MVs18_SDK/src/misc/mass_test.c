///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2013, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: mass_test.c
///////////////////////////////////////////////////////////////////////////////
/*
zhouyi,2013.11.22, initial version
maintainer: Halley
*/
#include "type.h"
#include "app_config.h"
#include "audio_path.h"
#include "dac.h"
#include "clk.h"
#include "gpio.h"
#include "sw_uart.h"
#include "Pwm.h"
#include "audio_adc.h"
#include "bt_app_func.h"

////////////////////////////////////////////////////////////////////////////
//请注意实际测试设备与如下宏定义代码的pin一致，如果不是BankA 还需要调整下面代码中GPIO对应的控制寄存器参数。
#define INPUT_GPIOA_X     GPIOA19 /*SD Data---input test pin*/
#define OUTPUT_GPIOA_SDX1 GPIOA21 /*SD Cmd----output test pin*/
#define OUTPUT_GPIOA_SDX2 GPIOA20 /*SD Clk----output toggle pin*/

#define OUTPUT_GPIOA_DP   GPIOA22 /*USB DP----output toggle pin*/
#define OUTPUT_GPIOA_DM   GPIOA23 /*USB DM----output toggle pin*/
////////////////////////////////////////////////////////////////////////////

uint8_t MassTestFlag = 0;
void OtherMassTestFunc(void);

uint8_t Digital2Text(uint8_t Value /* & 0x0F*/)
{
	if(Value < 10)
	{
		return ('0' + Value);
	}

	return ('a' + (Value - 10));
}

//测试模式检测函数，本函数仅在main.c中调用一次
void DetectMassTestCondition(void)
{
	uint8_t i, I1 = 0, I2 = 0, Count = 0, *Addr;
#define MAX_DUR_IN_MS   50
#define MAX_COUNT       10

	MassTestFlag = 1;//set the default value

//    return; //for code debug
	//B7-->pwm for function test ONLY
//    PwmConfig(PWM_CH5_B7_B22, 12 * 1024 * 3, 6 * 1024 * 3);
//    PwmEnableChannel(PWM_CH5_B7_B22, PWM_IO_SEL0, PWM_MODE_OUT);

	//必需确认操作本GPIO不会对外接设备的初始状参数不良影响
	GpioClrRegOneBit(GPIO_A_OE, INPUT_GPIOA_X);
	GpioSetRegOneBit(GPIO_A_IE, INPUT_GPIOA_X);
	//no pull up no pull down
	GpioSetRegOneBit(GPIO_A_PU, INPUT_GPIOA_X);
	GpioClrRegOneBit(GPIO_A_PD, INPUT_GPIOA_X);

	//check the mass test input signal clock
	// fs = 1khz, so single MUST be less than 500Hz
	for(i = 0; i < MAX_DUR_IN_MS; i++)
	{
		I2 = (GpioGetReg(GPIO_A_IN) & INPUT_GPIOA_X) ? 1 : 0;

		DelayMs(1);

		if(I1 != I2)
		{
			I1 = I2;
			Count++;
			if(Count >= MAX_COUNT)
			{
				//end of detection, mass test signal found
				MassTestFlag = 1;
				return;
			}
		}
	}

	//normal run case
	MassTestFlag = 0;

	//output bd addr here through output1,SD cmd, ==> GPIOA21
	//SwUartTxInit(SWUART_GPIO_PORT_A, 21, 115200);

	//format: AT#BG "BD_ADDR" ===> AT#BG 1234567890A0
	//Addr = GetMassTestDevAddr();
	//SwUartSend("\nAT#BG ", 8);
	for(i = 5; i <= 5; i--)
	{
		//low
		I1 = Digital2Text(Addr[i] & 0x0F);
		//high
		I2 = Digital2Text((Addr[i] >> 4) & 0x0F);
		SwUartSend(&I2, 1);
		SwUartSend(&I1, 1);
	}
	SwUartSend("\n", 2);
}

bool GetMassTestFlag(void)
{
	return MassTestFlag ? 1 : 0;
}


//void SetMassTestCfg_Mic(void)
//{
////  NVIC_EnableIRQ(DECODER_IRQn);
//    PhubPathClose(ALL_PATH_CLOSE);
//    if(!PhubPathSel(ADCIN2PMEM_PCMFIFO2DACOUT))
//    {
//        //APP_DBG("phub path sel err!\n");
//        return;
//    }
//
//    //config ADC
//    CodecAdcAnaInit();
//    CodecAdcChannelSel(ADC_CH_MIC_L | ADC_CH_MIC_R);
//
//    AdcPmemWriteDis();
//    AdcToPmem(USB_MODE, 0x3400, 0x800);
//    AdcPmemWriteEn();
//
//    CodecDacChannelSel(DAC_CH_DECD_L | DAC_CH_DECD_R); // select dac channel
//    DacAdcSampleRateSet(44100, USB_MODE);	 //九种采样率
//
////    CodecDacChannelSel(DAC_CH_DECD_L|DAC_CH_DECD_R); // select dac channel
////    DacConfig(MDAC_DATA_FROM_DEC, USB_MODE);
//}



//void SetMassTestCfg_BeepKey(void)
//{
////  NVIC_EnableIRQ(DECODER_IRQn);
//    PhubPathClose(ALL_PATH_CLOSE);
//    //if(!PhubPathSel(PCMFIFO_MIX_DACOUT))
//    if(!PhubPathSel(ADCIN2PMEM_PCMFIFO2DACOUT))
//    //if(!PhubPathSel(0x194))
//    {
//        //APP_DBG("phub path sel err!\n");
//        return;
//    }
//
//    CodecDacChannelSel(DAC_CH_DECD_L|DAC_CH_DECD_R); // select dac channel
//    DacConfig(MDAC_DATA_FROM_DEC, USB_MODE);
//
//    //CodecDacMuteSet(0, 0);
//    SetMassTestCfg_Mic();
//}

void SetMassTestCfg(void)
{
//  NVIC_EnableIRQ(DECODER_IRQn);
	PhubPathClose(ALL_PATH_CLOSE);
//    if(!PhubPathSel(PCMFIFO_MIX_DACOUT))
	if(!PhubPathSel(ADCIN2PMEM_PCMFIFO2DACOUT))
	{
		//APP_DBG("phub path sel err!\n");
		return;
	}

	CodecDacChannelSel(DAC_CH_DECD_L | DAC_CH_DECD_R); // select dac channel
	DacConfig(MDAC_DATA_FROM_DEC, USB_MODE);

	//SetMassTestCfg_Mic();return;
	//config ADC
	CodecAdcAnaInit();
	CodecAdcChannelSel(ADC_CH_MIC_L | ADC_CH_MIC_R);

	AdcPmemWriteDis();
	AdcToPmem(USB_MODE, 0x3400, 0x800);
	AdcPmemWriteEn();

	CodecDacChannelSel(DAC_CH_DECD_L | DAC_CH_DECD_R); // select dac channel
	DacAdcSampleRateSet(44100, USB_MODE);	 //九种采样率

	CodecMicBiasEn(1);

	CodecAdcMicGainConfig(0x0, 0x0);

	CodecDacMuteSet(0, 0);
}

void SetGPIOATogleCfg(uint32_t Mask)
{
	GpioSetRegOneBit(GPIO_A_OE, Mask);
	GpioClrRegOneBit(GPIO_A_IE, Mask);
	//no pull up no pull down
	GpioSetRegOneBit(GPIO_A_PU, Mask);
	GpioClrRegOneBit(GPIO_A_PD, Mask);
}

void SetGPIOAValue(uint32_t Mask, bool IsHigh)
{
	uint32_t T;

	T = GpioGetReg(GPIO_A_OUT);

	if(IsHigh)
	{
		GpioSetReg(GPIO_A_OUT, T | Mask);
	}
	else
	{
		GpioSetReg(GPIO_A_OUT, T & (~Mask));
	}
}

#define DELAY_ONE()  DelayMs(1)

//小于本宏定义幅度的MIC信号将被忽略，信号幅度至少需要超过本定义的50%
#define SIGNAL_AMP_THREADSHOLD  3276 /*0.1*/
//#define MIN_LEGAL_SINGNAL_HIT_COUNT  100

//检测信号幅度是否满足要求
//bool IsLegaleSignalAmp(int16_t* SignalOneChannle, uint32_t Num, int16_t Thread, uint32_t MiniHitCount)
//{
//    uint32_t i;
//    int16_t t;
//
//    for(i = 0; i < Num; i++)
//    {
//        t = SignalOneChannle[i];
//        if(t < 0) t = -t;
//
//        if(t >= Thread)MiniHitCount--;
//
//        if(MiniHitCount <= 0)return 1;
//    }
//
//    return 0;
//}

//检测信号频率，使用toggle的方法计数
bool IsLegaleSignalToggle(int16_t* SignalOneChannle, uint32_t Num, int16_t Thread)
{
	uint32_t i, Count = 0;

	SignalOneChannle[0] = 0;
	if(SignalOneChannle[0] > 0)
	{
		SignalOneChannle[0] = 1;
	}

	for(i = 1; i < Num; i++)
	{
		if(SignalOneChannle[i - 1])
		{
			if(SignalOneChannle[i] <= -Thread)
			{
				SignalOneChannle[i] = 0;
			}
			else
			{
				SignalOneChannle[i] = 1;
			}
		}
		else
		{
			if(SignalOneChannle[i] <= Thread)
			{
				SignalOneChannle[i] = 0;
			}
			else
			{
				SignalOneChannle[i] = 1;
			}
		}

		if(SignalOneChannle[i] != SignalOneChannle[i - 1])
		{
			Count++;    //get the toggle frequency
		}
	}

	//get the toggle frequency
	if((Count > 6) && (Count < 20))
	{
		return 1;
	}

	return 0;
}

int32_t SpiFlashConstGetInfo(uint32_t IdxId, uint32_t* DatSz, uint32_t* Offset);

//本函数为实际最终执行的测试代码，函数不会返回
//USB,SD pins toggle, never return
void OtherMassTestFunc(void)
{
	int16_t* AdcMem;
	uint32_t Size, Offset, toggle;

	//enable beep and micfunction if any.
	SetMassTestCfg();

	//disable SDIO ---> GPIO
	GpioSdIoConfig(0xFF);

	//set toggle i/o ==> output
	SetGPIOATogleCfg(OUTPUT_GPIOA_SDX1);
	SetGPIOATogleCfg(OUTPUT_GPIOA_SDX2);

	SetGPIOATogleCfg(OUTPUT_GPIOA_DP);
	SetGPIOATogleCfg(OUTPUT_GPIOA_DM);

	//check MIC if any based on Flash Constdata info "TALK"
	if(0 == SpiFlashConstGetInfo(*(uint32_t*)"TALK", &Size, &Offset)) //found Mic Syntax
	{
		uint8_t ContinuedHitCount;
		toggle = 0;
		ContinuedHitCount = 0;

		//mic path test
		//AdcMem = (int16_t*)OSMalloc(1024, MMM_ALLOC_TASK);
		AdcMem = (int16_t*)(0x4000 + (0x20018000)); //pmem space
		if(AdcMem)

			while(1)
			{
				uint32_t t;
				t = AdcPmemPcmRemainLenGet();
				if(t > 0x100) //half @44.1khz
				{
					AdcPcmDataRead((uint8_t*)AdcMem, 0x100, 0x04);
					{
						//            for(i = 0; i < 0x100; i++)
						//            {
						//                APP_DBG("%d, ", AdcMem[i]);
						//            }
						//            APP_DBG("%d\n\n", t);

						if(IsLegaleSignalToggle(AdcMem, 0x100, SIGNAL_AMP_THREADSHOLD))//found signal
						{
							ContinuedHitCount++;
							APP_DBG("ok(%d)\n", ContinuedHitCount);
							if(ContinuedHitCount > 70)/*double check, about 400ms*/
							{
								break; //pass
							}
						}
						else
						{
							ContinuedHitCount = 0;//recount,noise signal found
						}
						//            for(i = 0; i < 0x100; i++)
						//            {
						//                APP_DBG("%d, ", AdcMem[i]);
						//            }
						//            APP_DBG("%d\n\n", t);
					}
				}

				//toggle output pin signal indicating from now pls open the mic input sound source.
				DELAY_ONE();
				SetGPIOAValue(OUTPUT_GPIOA_SDX1, toggle);
				toggle = !toggle;
			}
	}

	//end of testing,toggle all pins, input pin is excluded.
	while(1)//fs = 500Hz
	{
		DELAY_ONE();
		SetGPIOAValue(OUTPUT_GPIOA_SDX1, 1);
		SetGPIOAValue(OUTPUT_GPIOA_SDX2, 0);

		SetGPIOAValue(OUTPUT_GPIOA_DP, 1);
		SetGPIOAValue(OUTPUT_GPIOA_DM, 0);

		DELAY_ONE();
		SetGPIOAValue(OUTPUT_GPIOA_SDX1, 0);
		SetGPIOAValue(OUTPUT_GPIOA_SDX2, 1);

		SetGPIOAValue(OUTPUT_GPIOA_DP, 0);
		SetGPIOAValue(OUTPUT_GPIOA_DM, 1);
	}
}

