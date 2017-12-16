/**
 *******************************************************************************
 * @file    sys_vol.c
 * @author  Richard
 * @version V1.0.0
 * @date    07-01-2014
 * @brief   sys vol set here
 * @maintainer: Sam
 *******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, MVSILICON SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */
#include "type.h"
#include "app_config.h"
#include "sys_vol.h"
#include "dac.h"
#include "sound_remind.h"
#include "task_decoder.h"
#include "mixer.h"
#include "device_audio.h"
#include "audio_adc.h"
#include "clk.h"
#include "audio_path.h"
#include "eq.h"
#include "i2s.h"
#include "rst.h"
#include "gpio.h"
#include "bt_control_api.h"
#include "sys_app.h"
#include "bt_app_func.h"
#include "eq_params.h"
#include "pcm_fifo.h"
#include "pcm_transfer.h"
#include "beep.h"
#include "audio_decoder.h"

extern uint8_t EchoDelayStep;
extern uint8_t EchoDepthStep;

uint8_t gAudioAnaChannel = AUDIO_CH_NONE;

#ifdef FUNC_SPEC_TRUM_EN
uint8_t AudioSpecTrumStep = 0;
#endif

#ifdef FUNC_AUDIO_DRC_EFFECT_EN
bool gDRCEffectInited = FALSE;
#endif

#ifdef FUNC_AUDIO_VB_EFFECT_EN
bool gVbEffectInited = FALSE;
//#define WORKBUFSIZE sizeof(VB_BUFFER)
//uint8_t vbBuffer[WORKBUFSIZE];
#endif

#ifdef FUNC_AUDIO_3D_EFFECT_EN
bool	g3dEffectInited = FALSE;
#endif

#if defined(FUNC_AUDIO_VB_EFFECT_EN) || defined (FUNC_AUDIO_3D_EFFECT_EN) || defined (FUNC_AUDIO_DRC_EFFECT_EN)
static int32_t *AudioBasePcmAddr1 = (int32_t *)(VMEM_ADDR+ 28*1024);
static uint16_t AudioBaseLength1;
static uint8_t  AudioVb3dFirstData = TRUE;
static uint8_t  AudioVb3dDone = TRUE;

#endif

uint8_t			mixer_pcm_format = MIXER_FORMAT_STERO;

//HF音量表
#ifdef FUNC_BT_HF_EN
const uint16_t gHfVolArr[MAX_BTHF_VOLUME + 1] =
{
	0,		42,		107,	173,	
	332,	541,	880,	1035,	
	1218,	1433,	1686,	1984,
	2434,	2946,	3500,	4095
};
#endif

//音量表中数据表示的是音频通路数字部分的Gain值
//4095表示0dB,为0时表示Mute。允许正增益
//小于4095为负增益，大于4095为正增益
//两级音量之间的计算公式为 "20*log(Vol1/Vol2)"，单位dB

//decoder音量表
const uint16_t gDecVolArr[MAX_VOLUME + 1] =
{
	0,
	26,		31,		36,		42,		49,		58,		67,		78,
	91,		107,	125,	147,	173,	204,	240,	282,
	332,	391,	460,	541,	636,	748,	880,	1035,
	1218,	1433,	1686,	1984,	2434,	2946,	3500,	4095
};

//LINEIN、FM音量表
const uint16_t gAnaVolArr[MAX_VOLUME + 1] =
{
	0,
	42,		51,		60,		70,		82,		98,		114,	133,
	156,	184,	215,	254,	300,	354,	417,	491,
	579,	682,	803,	945,	1006,	1184,	1394,	1640,
	1930,	2271,	2672,	3146,	3860,	4664,	5542,	6484
};
#ifdef FUNC_MIC_EN
//MIC音量表，每一级相差1.5dB
const uint16_t gMicVolArr[MAX_MICIN_VOLUME + 1] =
{
	0, 		200, 	400,	876,	1040,	1235,	1467,	1742, 
	2070, 	2458,	2919,	3466,	4117, 	4890,	5808,	6898, 
	8192,
};
#endif

//设置系统主音量
void SetSysVol(void)
{
	uint16_t LeftVol;
	uint16_t RightVol;
	
	if(gSys.Volume > MAX_VOLUME)
	{
		gSys.Volume = MAX_VOLUME;
	}
	
	if((gSys.CurModuleID == MODULE_ID_BLUETOOTH) && (gSys.Volume >= MAX_VOLUME))
	{
		//if(IsAvrcpConnected())
		{
			;//BTPlayCtrlVolUp(); 			// 本地音量饱满后，可继续调节手机端音量
		}
	}
	
	if((gSys.CurModuleID == MODULE_ID_USB_AUDIO)
	|| (gSys.CurModuleID == MODULE_ID_USB_READER)
	|| (gSys.CurModuleID == MODULE_ID_USB_AUDIO_READER))
	{
		LeftVol = AudioLeftVolume * gDecVolArr[gSys.Volume] / AUDIO_MAX_VOLUME;
		RightVol = AudioRightVolume * gDecVolArr[gSys.Volume] / AUDIO_MAX_VOLUME;
		MixerConfigVolume(MIXER_SOURCE_USB, LeftVol, RightVol);  
	}
	else
	{
		MixerConfigVolume(MIXER_SOURCE_DEC, gDecVolArr[gSys.Volume], gDecVolArr[gSys.Volume]);
	}
	
	MixerConfigVolume(MIXER_SOURCE_ANA_MONO, gAnaVolArr[gSys.Volume], gAnaVolArr[gSys.Volume]);
	MixerConfigVolume(MIXER_SOURCE_ANA_STERO, gAnaVolArr[gSys.Volume], gAnaVolArr[gSys.Volume]); 

#ifdef FUNC_MIC_EN	
	if(gSys.MicVolume > MAX_MICIN_VOLUME)
	{
		gSys.MicVolume = MAX_MICIN_VOLUME;
	}
	MixerConfigVolume(MIXER_SOURCE_MIC, gMicVolArr[gSys.MicVolume], gMicVolArr[gSys.MicVolume]);
#endif
}

// 设置HF模式音量
#ifdef FUNC_BT_HF_EN
void SetBtHfVol(uint8_t vol)
{
	if(vol > MAX_BTHF_VOLUME)
	{
		vol = MAX_BTHF_VOLUME;
	}
	APP_DBG("[BT] HF Vol = %d\n", vol);
	gSys.HfVolume = vol;

	PhubAdcI2sInGainSet(gHfVolArr[gSys.HfVolume], gHfVolArr[gSys.HfVolume]);
}

uint8_t GetBtHfVol(void)
{
	return gSys.HfVolume;
}
#endif


//设置I2S输出引脚配置
void I2sPortSet(void)
{
    GpioI2sIoConfig(I2SIN_I2S_IO_PORT);     
#ifdef FUNC_I2S_MASTER_EN
    GpioMclkIoConfig(I2SIN_MCLK_IO_PORT);   
#else
    GpioMclkIoConfig(I2SIN_MCLK_IO_PORT+2); 
#endif
}


//设置I2S或者I2S和DAC同时输出通道
void AudioI2sOutSetChannel(uint32_t SampleRate, bool IsAndDacOut)
{
	static uint32_t PreSampleRate = 0;
	static uint32_t PreIsAndDacOut = FALSE; //第一次强制为0,但是会根据SampleRate赋值一次
	if((PreSampleRate == SampleRate) && (PreIsAndDacOut == IsAndDacOut))
	{
		return;
	}
	
    PcmFifoClear();
    PhubPathClose(ALL_PATH_CLOSE);   
	RstPhubModule();//reset 相关通路寄存器值，消除pop音，phub会复位寄存器，不要随便调用    
    RstMclk();      //reset 相关通路寄存器值，消除pop音
    DacAdcSampleRateSet(SampleRate, USB_MODE);
    I2sSampleRateSet(SampleRate, USB_MODE);
#ifdef FUNC_I2S_MASTER_EN
    I2sSetMasterMode(MDAC_DATA_FROM_DEC, USB_MODE, I2S_FORMAT_I2S);
#else
    I2sSetSlaveMode(MDAC_DATA_FROM_DEC, USB_MODE, I2S_FORMAT_I2S);
#endif
    WaitMs(50);
    if(IsAndDacOut)//i2s和dac同时输出
    {
        PhubPathSel(ADCIN2PMEM_PCMFIFO2DACOUTI2SOUT);
    }
    else           //只有I2S输出
    {
        PhubPathSel(ADCIN2PMEM_PCMFIFOBEEP2_IISOUT);
    } 
	
	PreSampleRate = SampleRate;
	PreIsAndDacOut = IsAndDacOut;
}

//设置STA输出通道
void AudioStaOutSetChannel(uint32_t SampleRate, bool IsAndDacOut)
{
	static uint32_t PreSampleRate = 0;
	if(PreSampleRate == SampleRate)
	{
		return;
	}
	
    PcmFifoClear();
    PhubPathClose(ALL_PATH_CLOSE);
	RstPhubModule();//reset 相关通路寄存器值，消除pop音，phub会复位寄存器，不要随便调用
    RstMclk();      //reset 相关通路寄存器值，消除pop音
    if(SampleRate < 32000) 
    {
        DacSoftMuteSet(TRUE, TRUE);
        AdcClose();        
        DacConfig(MDAC_DATA_FROM_DEC, NORMAL_MODE);
        CodecAdcAnaInit();
        CodecAdcFmGainConfig(18, 18);  
        CodecAdcChannelSel(ADC_CH_MDAC_LOOPBACK_L | ADC_CH_MDAC_LOOPBACK_R);
#ifdef FUNC_I2S_MASTER_EN
        I2sSetMasterMode(I2S_DATA_FROM_ADC, NORMAL_MODE, I2S_FORMAT_I2S);
#else
        I2sSetSlaveMode(I2S_DATA_FROM_ADC, NORMAL_MODE, I2S_FORMAT_I2S);
#endif
        I2sClassdSTASampleRateSet(SampleRate, NORMAL_MODE, 1);
        AdcToI2s(NORMAL_MODE);
        DacSoftMuteSet(FALSE, FALSE);
        WaitMs(50); 
        PhubPathSel(ADCIN2PMEM_PCMFIFO2DACOUT);
    }
    else
    {
        DacAdcSampleRateSet(SampleRate, NORMAL_MODE);
        AdcClose();
#ifdef FUNC_I2S_MASTER_EN
        I2sClassdMasterModeSet(I2S_DATA_FROM_DEC, I2S_FORMAT_I2S);
#else
        I2sClassdSlaveModeSet(I2S_DATA_FROM_DEC, I2S_FORMAT_I2S);
#endif
        I2sClassdSTASampleRateSet(SampleRate, NORMAL_MODE, 0); 
        AdcConfig(NORMAL_MODE);
        WaitMs(50); 
        if(IsAndDacOut)
        {
            PhubPathSel(ADCIN2PMEM_PCMFIFO2DACOUTI2SOUT);
        }
        else
        {
            PhubPathSel(ADCIN2PMEM_PCMFIFOBEEP2_IISOUT);
        }          
    }
	
	PreSampleRate = SampleRate;
}
//DAC输出设置
void AudioDacOutSetChannel(void)
{
    DacSoftMuteSet(TRUE, TRUE);
    PcmFifoClear();
    PhubPathClose(ALL_PATH_CLOSE);
	RstPhubModule();//reset 相关通路寄存器值，消除pop音，phub会复位寄存器，不要随便调用
	RstMclk();//reset 相关通路寄存器值，消除pop音
	PcmTxSetPcmDataMode(DATA_MODE_STEREO_LR);
	CodecDacChannelSel(DAC_CH_DECD_L | DAC_CH_DECD_R);
	DacSoftMuteSet(TRUE, TRUE);	
	DacConfig(MDAC_DATA_FROM_DEC, USB_MODE);
	DacAdcSampleRateSet(44100, USB_MODE);	 //九种采样
	WaitMs(100); 

	CodecDacInit(FALSE);            	//dac init
	
	PhubPathSel(ADCIN2PMEM_PCMFIFO2DACOUT);
	
	DacSoftMuteSet(FALSE, FALSE);
}

//模拟通路初始化
void AudioAnaInit(void)
{
	//MIC初始化
	CodecMicBiasEn(1);
	
	//FM输入初始化，C12[ADC_CH_FM2_R]	
	GpioClrRegBits(GPIO_C_PU, GPIOC12);		
	GpioSetRegBits(GPIO_C_PD, GPIOC12);
		
	GpioClrRegBits(GPIO_C_PU, GPIOC11);//C11[ADC_CH_FM1_R]		
	GpioSetRegBits(GPIO_C_PD, GPIOC11);
	
	//ADC初始化
	CodecAdcAnaInit();					//adc init
	AdcToPmem(USB_MODE, ADC_FIFO_ADDR - PMEM_ADDR, ADC_FIFO_LEN);
	AdcPmemWriteEn();
}

//设置输出通路初始化
void AudioOutputInit(void)
{
    
#ifdef OUTPUT_CHANNEL_DAC
    AudioDacOutSetChannel();
#elif defined (OUTPUT_CHANNEL_I2S)
    I2sPortSet();
    AudioI2sOutSetChannel(44100, 0);

#elif defined (OUTPUT_CHANNEL_DAC_I2S)
    I2sPortSet();
    AudioI2sOutSetChannel(44100, 1);

#elif defined (OUTPUT_CHANNEL_CLASSD)
    I2sPortSet();
    AdcDacClkModeSet(NORMAL_MODE);
    AudioStaOutSetChannel(44100, 0);
#else
    I2sPortSet();
    AdcDacClkModeSet(NORMAL_MODE);
    AudioStaOutSetChannel(44100, 1);
#endif    
}

//设置通路采样率设置
void AudioSampleRateSet(uint32_t CurSamplerate)
{    
#ifdef OUTPUT_CHANNEL_DAC
    DacAdcSampleRateSet(CurSamplerate, USB_MODE);
    
#elif defined (OUTPUT_CHANNEL_I2S)
    I2sPortSet();
    I2sSampleRateSet(CurSamplerate, USB_MODE);

#elif defined (OUTPUT_CHANNEL_DAC_I2S)
    I2sPortSet();
    I2sSampleRateSet(CurSamplerate, USB_MODE);
    DacAdcSampleRateSet(CurSamplerate, USB_MODE);

#elif defined (OUTPUT_CHANNEL_CLASSD)
    I2sPortSet();
    AudioStaOutSetChannel(CurSamplerate, 0);
    
#else
    I2sPortSet();
    AudioStaOutSetChannel(CurSamplerate, 1);
#endif    
}

bool IsFillMemData = FALSE;
extern void PcmMemFillData(void);

//设置模拟通路
void AudioAnaSetChannel(uint8_t Channel)
{
}




//静音设置与取消
void AudioPlayerMute(bool MuteEnFlag)
{
	gSys.MuteFlag = MuteEnFlag;

	// 调用驱动层静音设置接口
	if(gSys.MuteFlag)
	{
		//DecoderTaskPause();
		//SoundRemind(SOUND_MUTE_ON);
		DacSoftMuteSet(TRUE, TRUE);
		//DecoderTaskResume();
	}
	else
	{
		//DecoderTaskPause();
		DacSoftMuteSet(FALSE, FALSE);
		//SoundRemind(SOUND_MUTE_OFF);
		//DecoderTaskResume();
	}

	// 刷新音量显示
	APP_DBG("Mute:%d\n", gSys.MuteFlag);
}


//各模式下的通用消息处理
void CommonMsgProccess(uint16_t Msg)
{
	switch(Msg)
	{	
		case MSG_VOL_UP:	
			if(gSys.Volume < MAX_VOLUME)
			{
				gSys.Volume++;
			}
			
			if(gSys.MuteFlag)
			{
				gSys.MuteFlag = 0;
				AudioPlayerMute(FALSE);
			}
			APP_DBG("Vol+(%d)\n", gSys.Volume);
			SetSysVol();
			break;
				
		case MSG_VOL_DW:
			if(gSys.Volume > 0)
			{
				gSys.Volume--;
			}
            
            if(gSys.MuteFlag)
			{
				gSys.MuteFlag = 0;
				AudioPlayerMute(FALSE);
			}            
			APP_DBG("Vol-(%d)\n", gSys.Volume);
			SetSysVol();
			break;
		
		case MSG_EQ:
			AudioPlayerSwitchEQ();
#ifdef FUNC_TREB_BASS_EN
			gSys.EqStatus = 1;
#endif
			break;
		
		case MSG_MUTE:
			AudioPlayerMute(!gSys.MuteFlag);
			break;
		
#ifdef FUNC_AUDIO_DRC_EFFECT_EN
		case MSG_DRC:
			gSys.AudioDRCEffectFlag = !gSys.AudioDRCEffectFlag;
			DBG("DRC: %d\n", gSys.AudioDRCEffectFlag);
			if(gSys.AudioDRCEffectFlag)
			{
                gDRCEffectInited = FALSE;
				InitVb3dPcmPara();
			}
			break;
#endif
			
#ifdef FUNC_AUDIO_3D_EFFECT_EN
		case MSG_3D:
			gSys.Audio3dEffectFlag = !gSys.Audio3dEffectFlag;
			DBG("3D: %d\n", gSys.Audio3dEffectFlag);
			if(gSys.Audio3dEffectFlag)
			{
                g3dEffectInited = FALSE;
				InitVb3dPcmPara();
			}
			break;
#endif

#ifdef FUNC_AUDIO_VB_EFFECT_EN
        case MSG_VB:
            gSys.AudioVbEffectFlag = !gSys.AudioVbEffectFlag;
            APP_DBG("VB: %d\n",gSys.AudioVbEffectFlag);
            if(gSys.AudioVbEffectFlag)
            {
                gVbEffectInited = FALSE;
                InitVb3dPcmPara();
            }
            break;
#endif
			
#ifdef FUNC_SOUND_REMIND
		case MSG_REMIND:
			if(gSys.SoundRemindOn)
			{
				DBG("SoundRemindOn close!\n");
				SoundRemind(SOUND_SPEECH_CL);  
				gSys.SoundRemindOn = 0;
			}
			else
			{
				DBG("SoundRemindOn open!\n");
				gSys.SoundRemindOn = 1;
				SoundRemind(SOUND_SPEECH_OP);  
			}
			AudioSysInfoSetBreakPoint();
			break;

		case MSG_LANG:
			gSys.LanguageMode = (gSys.LanguageMode == LANG_ZH) ? LANG_EN : LANG_ZH;
			DBG("LANG: %d\n", gSys.LanguageMode);
			SoundRemind(SOUND_LANG_MODE);  
			AudioSysInfoSetBreakPoint();
			break;
#endif

		case MSG_MIC_VOL_UP:
		#ifdef FUNC_MIC_EN
			if(gSys.MicVolume < MAX_MICIN_VOLUME)
			{
				gSys.MicVolume++;
			}
			DBG("MIC_VOL_UP: %d\n", gSys.MicVolume);
			SetSysVol();
		#endif
			break;

		case MSG_MIC_VOL_DW:
		#ifdef FUNC_MIC_EN
			if(gSys.MicVolume > 0)
			{
				gSys.MicVolume--;
			}
			DBG("MIC_VOL_DW: %d\n", gSys.MicVolume);
			SetSysVol();
		#endif
			break;

		case MSG_ECHO_DELAY_UP:
		#ifdef FUNC_MIC_ECHO_EN
			if(EchoDelayStep < MAX_ECHO_DELAY_STEP)
			{
				EchoDelayStep++;
			}
			DBG("ECHO_DELAY_UP: %d\n", EchoDelayStep);
		#endif
			break;

		case MSG_ECHO_DELAY_DW:
		#ifdef FUNC_MIC_ECHO_EN
			if(EchoDelayStep > 0)
			{
				EchoDelayStep--;
			}
			DBG("ECHO_DELAY_DW: %d\n", EchoDelayStep);
		#endif
			break;

		case MSG_ECHO_DEPTH_UP:
		#ifdef FUNC_MIC_ECHO_EN
			if(EchoDepthStep < MAX_ECHO_DEPTH_STEP)
			{
				EchoDepthStep++;
			}			
			DBG("ECHO_DEPTH_UP: %d\n", EchoDepthStep);
		#endif
			break;

		case MSG_ECHO_DEPTH_DW:
		#ifdef FUNC_MIC_ECHO_EN
			if(EchoDepthStep > 0)
			{
				EchoDepthStep--;
			}
			DBG("ECHO_DEPTH_DW: %d\n", EchoDepthStep);
		#endif
			break;

		case MSG_TREB_UP:
		#ifdef FUNC_TREB_BASS_EN
			if(gSys.TrebVal < MAX_TREB_VAL)
			{
				gSys.TrebVal++;
				TrebBassSet(gSys.TrebVal, gSys.BassVal);
			}
			gSys.EqStatus = 0;
			DBG("TREB_UP: %d\n", gSys.TrebVal);
		#endif
			break;

		case MSG_TREB_DW:
		#ifdef FUNC_TREB_BASS_EN
			if(gSys.TrebVal > 0)
			{
				gSys.TrebVal--;
				TrebBassSet(gSys.TrebVal, gSys.BassVal);
			}
			gSys.EqStatus = 0;
			DBG("TREB_DW: %d\n", gSys.TrebVal);
		#endif
			break;

		case MSG_BASS_UP:
		#ifdef FUNC_TREB_BASS_EN
			if(gSys.BassVal < MAX_BASS_VAL)
			{
				gSys.BassVal++;
				TrebBassSet(gSys.TrebVal, gSys.BassVal);
			}
			gSys.EqStatus = 0;
			DBG("BASS_UP: %d\n", gSys.BassVal);
		#endif
			break;

		case MSG_BASS_DW:
		#ifdef FUNC_TREB_BASS_EN
			if(gSys.BassVal > 0)
			{
				gSys.BassVal--;
				TrebBassSet(gSys.TrebVal, gSys.BassVal);
			}
			gSys.EqStatus = 0;
			DBG("BASS_DW: %d\n", gSys.BassVal);
		#endif
			break;
				
#ifdef FUNC_AUDIO_MENU_EN
		case MSG_MENU:
			switch(gSys.AudioSetMode)
			{
				case AUDIO_SET_MODE_MAIN_VOL:	//主音量调节
					DBG("MAIN_VOL\n");
					break;
				
				case AUDIO_SET_MODE_MIC_VOL:	//MIC音量调节
					DBG("MIC_VOL\n");
					SoundRemind(SOUND_MIC_VOL);  
					break;

				case AUDIO_SET_MODE_ECHO_DEPTH:	//MIC混响调节
					DBG("ECHO_DEPTH\n");
					SoundRemind(SOUND_ECHO_REGULATE); 
					break;

				case AUDIO_SET_MODE_ECHO_DELAY:	//MIC延时调节
					DBG("ECHO_DELAY\n");
					//SoundRemind(SOUND_DELAY_REGULATE);
					break;

				case AUDIO_SET_MODE_TREB:		//高音调节
					DBG("TREB\n");
					SoundRemind(SOUND_TREB_REGULATE);  
					break;

				case AUDIO_SET_MODE_BASS:		//低音调节
					DBG("BASS\n");
					SoundRemind(SOUND_BASS_REGULATE);  
					break;
				
				default:
					break;
			}   
			break;
#endif				
		default:
			break;		
	}	
}


#ifdef FUNC_SPEC_TRUM_EN
//频谱采集
void SpecTrumProc(int16_t* Buffer, uint32_t Len)
{
	uint32_t i;
	int32_t TempSampleData;
	   
	for(i = 0; i < Len; i += 8)
	{
		if((gSys.CurModuleID == MODULE_ID_LINEIN) 
			|| (gSys.CurModuleID == MODULE_ID_RADIO) 
			|| (gSys.CurModuleID == MODULE_ID_RECORDER))
		{
			TempSampleData = (Buffer[i] << 1);
		}
		else
		{
			TempSampleData = Buffer[i];
		}
		if(TempSampleData < 1000)
		{
		       AudioSpecTrumStep = 0;	
		}
		else if(TempSampleData  < 6554)
		{
			AudioSpecTrumStep = 1;	
		}
		else if(TempSampleData < 13108)
		{
			AudioSpecTrumStep = 2;
		}
		else if(TempSampleData < 19622)
		{
			AudioSpecTrumStep = 3;
		}
		else if(TempSampleData < 26216)
		{
			AudioSpecTrumStep = 4;
		}
		else if(TempSampleData < 32770)
		{
			AudioSpecTrumStep = 5;
		}		
 	}		
}
#endif

#ifdef FUNC_AUDIO_DRC_EFFECT_EN
//DRC音效设置
void AudioDRCPro(int16_t *PcmDataAddr, int32_t CurNumChannel, int32_t CurSampleRate, int32_t PcmDataLen)
{
	if(gSys.AudioDRCEffectFlag)
	{
		if(!gDRCEffectInited)
		{
			//if(DRC_ERROR_OK != init_drc(&gSys.AudioDRCEffectHandle, CurNumChannel, CurSampleRate, AUDIO_EFFECT_PARAM_DRC_THRESHHOLD,
			////				0, AUDIO_EFFECT_PARAM_DRC_RATIO, AUDIO_EFFECT_PARAM_DRC_ATTACK_TIME,
			//				AUDIO_EFFECT_PARAM_DRC_RELEASE_TIME, AUDIO_EFFECT_PARAM_DRC_CROSSOVER_FREQ))
			{
				gDRCEffectInited = FALSE;
			}
			//else
			{
				gDRCEffectInited = TRUE;
			}
		}
		else
		{			
            //apply_drc(&gSys.AudioDRCEffectHandle, PcmDataAddr, PcmDataAddr, PcmDataLen);
		}
	}
}
#endif

#ifdef FUNC_AUDIO_3D_EFFECT_EN
//3D音效设置
void Audio3dPro(int16_t *PcmDataAddr, int32_t CurNumChannel, int32_t CurSampleRate, int32_t PcmDataLen)
{
	if(gSys.Audio3dEffectFlag)
	{
		if(!g3dEffectInited)
		{
			//if(THREE_D_ERROR_OK != init_3d(&gSys.Audio3dEffectHandle, CurNumChannel, CurSampleRate, AUDIO_EFFECT_PARAM_3D_BASS, AUDIO_EFFECT_PARAM_3D_MID_BOOST))
			{
				g3dEffectInited = FALSE;
			}
			//else
			{
				g3dEffectInited = TRUE;
			}
		}
		else
		{
			if(CurNumChannel == 2)
            {
                //apply_3d(&gSys.Audio3dEffectHandle, PcmDataAddr, PcmDataAddr, PcmDataLen, AUDIO_EFFECT_PARAM_3D_DEPTH, AUDIO_EFFECT_PARAM_3D_POSTGAIN, AUDIO_EFFECT_PARAM_3D_PREGAIN);
            }				
		}
	}
}
#endif

#ifdef FUNC_AUDIO_VB_EFFECT_EN
#include "string.h"
//??????
void AudioVbPro(int16_t *PcmDataAddr, int32_t CurNumChannel, uint32_t CurSamplerate, int32_t PcmDataLen)
{
//    VB_BUFFER *vbBuffPtr = (VB_BUFFER *)&vbBuffer; 
//    int32_t Cnt = PcmDataLen / AUDIO_VB_MAX_N;
//    int32_t LeftLen = PcmDataLen - Cnt * AUDIO_VB_MAX_N;
    int16_t *PcmAddr = (int16_t *)PcmDataAddr;
//    uint16_t i = 0;
    
    if(PcmDataLen == 0)
    {
        return;
    }
    
    if(gSys.AudioVbEffectFlag)
    {
        if(!gVbEffectInited)
        {
           // init_vb(&gSys.AudioVbEffectHandle, CurNumChannel, CurSamplerate, AUDIO_EFFECT_PARAM_VB_CUTOFF_FREQ, AUDIO_EFFECT_PARAM_VB_LIMITER_EN);
            gVbEffectInited = TRUE;
        }
        /*
        while(i < Cnt)
        {
            apply_vb(&gSys.AudioVbEffectHandle,PcmAddr,  PcmAddr, AUDIO_VB_MAX_N,AUDIO_EFFECT_PARAM_VB_INTENSITY, AUDIO_EFFECT_PARAM_VB_PREGAIN, AUDIO_EFFECT_PARAM_VB_POSTGAIN);
            i++;
            PcmAddr = (int16_t *)(PcmDataAddr + i * (AUDIO_VB_MAX_N << (CurNumChannel-1)));
        }
        if(LeftLen > 0)
        {
            apply_vb(&gSys.AudioVbEffectHandle,PcmAddr,  PcmAddr, LeftLen,AUDIO_EFFECT_PARAM_VB_INTENSITY, AUDIO_EFFECT_PARAM_VB_PREGAIN, AUDIO_EFFECT_PARAM_VB_POSTGAIN);
        }
        */
        //apply_vb(&gSys.AudioVbEffectHandle,PcmAddr,  PcmAddr, PcmDataLen,AUDIO_EFFECT_PARAM_VB_INTENSITY, AUDIO_EFFECT_PARAM_VB_PREGAIN, AUDIO_EFFECT_PARAM_VB_POSTGAIN);
    }    
}
#endif

#if defined(FUNC_AUDIO_VB_EFFECT_EN) || defined(FUNC_AUDIO_3D_EFFECT_EN) || defined(FUNC_AUDIO_DRC_EFFECT_EN)
//???
void InitVb3dPcmPara(void)
{        
    AudioVb3dFirstData = TRUE;
    AudioVb3dDone = TRUE;
}
//????????
void DealWithVb3dPcmPara(uint32_t CurChannelNum, uint32_t CurSampleRate)
{
    if((AudioBasePcmAddr1 == NULL) || AudioVb3dDone|| AudioVb3dFirstData)
    {
        return;
    }
#ifdef FUNC_AUDIO_DRC_EFFECT_EN
    if(gSys.CurModuleID == MODULE_ID_BLUETOOTH 
#ifdef FUNC_SOUND_REMIND
   // && !IsSoundRemindPlaying()
#endif
    )
    AudioDRCPro((int16_t *)AudioBasePcmAddr1, CurChannelNum, CurSampleRate, AudioBaseLength1);
#endif
	
#ifdef FUNC_AUDIO_VB_EFFECT_EN
    AudioVbPro((int16_t *)AudioBasePcmAddr1, CurChannelNum, CurSampleRate, AudioBaseLength1);
#endif
    
#ifdef FUNC_AUDIO_3D_EFFECT_EN
    if(gSys.CurModuleID == MODULE_ID_BLUETOOTH 
#ifdef FUNC_SOUND_REMIND
   // && !IsSoundRemindPlaying()
#endif
    )
    {
        Audio3dPro((int16_t *)AudioBasePcmAddr1, CurChannelNum, CurSampleRate, AudioBaseLength1);           
    }       
#endif
    AudioVb3dDone = TRUE;   
}
//?????,?????
uint32_t SaveCurSendLast(int32_t *PcmIn, uint16_t PcmDataLen, uint32_t CurChannelNum)
{
	uint32_t i;
	uint16_t TmpLen;
	uint16_t LoopCount = PcmDataLen;

	if(AudioBasePcmAddr1 == NULL)
	{
		return PcmDataLen;
	}

	if(CurChannelNum == 1)
	{
		LoopCount >>= 1;				// 单声道数据为双声道数据的一半
	}

	if(AudioVb3dFirstData)
	{
		memset((void *)AudioBasePcmAddr1, 0, WMA_VB_FIFO_SIZE<<CurChannelNum);
		for(i=0; i<LoopCount; i++)
		{
			AudioBasePcmAddr1[i] = (AudioBasePcmAddr1[i] ^ PcmIn[i]);
			PcmIn[i] = (AudioBasePcmAddr1[i] ^ PcmIn[i]);
			AudioBasePcmAddr1[i] = (AudioBasePcmAddr1[i] ^ PcmIn[i]);
		}
		AudioBaseLength1 = PcmDataLen;
		AudioVb3dFirstData = FALSE;
		AudioVb3dDone = TRUE;
		return PcmDataLen;
	}

	if(CurChannelNum == 1)
	{
		LoopCount = (PcmDataLen > AudioBaseLength1 ? PcmDataLen/2 : AudioBaseLength1/2);
	}
	else
	{
		LoopCount = (PcmDataLen > AudioBaseLength1 ? PcmDataLen : AudioBaseLength1);
	}

	for(i=0; i<LoopCount; i++)
	{
		AudioBasePcmAddr1[i] = (AudioBasePcmAddr1[i] ^ PcmIn[i]);
		PcmIn[i] = (AudioBasePcmAddr1[i] ^ PcmIn[i]);
		AudioBasePcmAddr1[i] = (AudioBasePcmAddr1[i] ^ PcmIn[i]);
	}

	TmpLen = AudioBaseLength1;
	AudioBaseLength1 = PcmDataLen;
	PcmDataLen = TmpLen;

	AudioVb3dDone = FALSE;
	return PcmDataLen;
}
#endif
//


/**
 * @brief  set all audio channel EQ
 * @param  NONE
 * @return NONE
 */
void AudioPlayerSwitchEQ(void)
{
	uint32_t SampleRateTmp = DacAdcSampleRateGet();
	
	gSys.Eq = (gSys.Eq + 1) % EQ_STYLE_SUM;
	APP_DBG("SwitchEQ: %d\n", gSys.Eq); 
/*需要根据eq_params.h中定义的来修改此处
	switch(gSys.Eq)
   	{
	   	case EQ_STYLE_FLAT:
			APP_DBG("EQ_STYLE_FLAT\n");
			SoundRemind(SOUND_EQ_FLAT);  
			break;

		case EQ_STYLE_CLASSICAL:
			APP_DBG("EQ_STYLE_CLASSICAL\n");
			SoundRemind(SOUND_EQ_CLASSIC);  
			break;

		case EQ_STYLE_POP:
			APP_DBG("EQ_STYLE_POP\n");
			SoundRemind(SOUND_EQ_POP);  
			break;

		case EQ_STYLE_ROCK:
			APP_DBG("EQ_STYLE_ROCK\n");
			SoundRemind(SOUND_EQ_ROCK);  
			break;

		case EQ_STYLE_JAZZ:
			APP_DBG("EQ_STYLE_JAZZ\n");
			SoundRemind(SOUND_EQ_JAZZ);  
			break;

		default:
			break;
   	}
*/
	if(SampleRateTmp == 0)
	{
		SampleRateTmp = 44100;
	}

    EqStyleSelect(SampleRateTmp, gSys.Eq);
}



/////////////////////////以下为treb/bass相关函数//////////////////////////////////////////////////////////////////////////////////////////
EqFilterParams BassTreb_0db[2] =
{
	{ 0x0001, 0x0082, 0x02cc, 0x0400 }, // Parameters for filter 0
	{ 0x0002, 0x1388, 0x02cc, 0x0400 }, // Parameters for filter 4
};

const uint16_t TrebBassTab[] =
{
	0x02ff,		//-10db
	0x032d,		//-8db
	0x035d,		//-6db
	0x0390,		//-4db
	0x03c6,		//-2db
	0x0400,		//0db
	0x043c,		// 2db
	0x047c,		// 4db
	0x048c,		// 5db
	0x049c,		// 6db
	0x04ac,		// 7db
	//0x0509,		// 8db
	//0x0555		// 10db
};

/**
 * @brief  Treb  Bass gain set
 * @param  TrebBassMode: 0 :treb mode;1:bass mode
 * @param  TrebVol: treb volume(0~10)
 * @param  BassVol: bass volume(0~10)
 * @return NONE
 */
void TrebBassSet(uint8_t TrebVol, uint8_t BassVol)
{    
	uint8_t  i;	  
	uint16_t TrebSqrtA = BassTreb_0db[1].sqrtA;
	uint16_t BassSqrtA = BassTreb_0db[0].sqrtA;

    for(i = 1; i <= 10; i++)
    {
        BassTreb_0db[1].sqrtA = TrebSqrtA + (TrebBassTab[TrebVol] - TrebSqrtA)*i/10;
        BassTreb_0db[0].sqrtA = BassSqrtA + (TrebBassTab[BassVol] - BassSqrtA)*i/10;
        EqStyleConfigure(44100, (EqFilterParams*)BassTreb_0db, 2, 0x10000);
        WaitUs(150);
    } 
}

