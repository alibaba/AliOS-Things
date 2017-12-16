/**
 *******************************************************************************
 * @file    task_audio_process.c
 * @author  Richard
 * @version V1.0.0
 * @date    15-11-2013
 * @brief   audio process task
 * @maintainer Halley
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
#include "clk.h"
#include "audio_adc.h"
#include "app_config.h"
#include "os.h"
#include "dac.h"
#include "mixer.h"
#include "echo.h"
#include "gpio.h"
#include "i2s.h"
#include "dev_detect_driver.h"
#include "sys_vol.h"
#include "sys_app.h"
#include "audio_encoder.h"
#include "bt_app_func.h"
#include "recorder_control.h"
#include "sound_remind.h"
#include "usb_audio_control.h"
#include "dc_blocker.h"
#include "silence_detector.h"

#ifdef FUNC_MIC_ECHO_EN
uint8_t	EchoDelayStep = DEFAULT_DELAY_STEP;	//回声延迟最长12000个采样点，44.1KHZ采样率下对应272ms
uint8_t	EchoDepthStep = DEFAULT_DEPTH_STEP;	//回声强度最大16384/32768，即0.5
EchoContext  ct;
#endif
#define MIC_BUF_LEN		192//128//256
int16_t AduioBuf[MIC_BUF_LEN * 2];		//缓存ADC两个声道的数据
//extern bool IsFillMemData;

#ifdef FUNC_DC_BLOCKER
DCBlocker dcb;
#ifdef FUNC_SILENCE_DETECTOR
SilenceDetectorContext sdct;
#endif
#endif

void AudioProcessTaskEntrance(void)
{
	uint16_t SampleCnt;
	uint16_t i;
#ifdef FUNC_MIC_ECHO_EN
	bool	 OldMicEnable = FALSE;
#endif
#ifdef FUNC_DC_BLOCKER
	uint8_t init_dcb_channel = 1;		// 初始化 DC blocker 声道数
	#ifdef FUNC_SILENCE_DETECTOR
	uint8_t	init_sd_channel = 1;
	int32_t	smoothed_pcm_amplitude;
	uint32_t silence_counter;
	#endif
#endif
	APP_DBG("Audio task Init...\n");
	AudioAnaSetChannel(AUDIO_CH_NONE);	

#ifdef FUNC_MIC_ECHO_EN
	//MIC ECHO初始化
	memset(&ct, 0x00, sizeof(ct));
	init_echo(&ct, 1);
#endif
	//先丢弃开始的一段数据，否则会有很强POP声
	for(i = 0; i < 1000; i++)
	{
		while(AdcPmemPcmRemainLenGet() < 44);
		AdcPcmDataRead((void*)AduioBuf, 44, 0x04);	
	}

#ifdef FUNC_DC_BLOCKER
	//先将dc blocker初始化为单声道
	init_dc_blocker(&dcb, init_dcb_channel);
	#ifdef FUNC_SILENCE_DETECTOR
	init_silence_detector(&sdct, init_sd_channel, 44100);
	silence_counter = 0;
	#endif
#endif

//	//将MIC水位填充到1024点左右
//	memset(AduioBuf, 0x00, sizeof(AduioBuf));
//	for(i = 0; i < 30; i++)
//	{
//		MixerSetData(MIXER_SOURCE_ADC, (void*)AduioBuf, MIC_BUF_LEN);
//		while(!MixerIsDone(MIXER_SOURCE_ADC));
//		if(MixerGetRemainSamples(MIXER_SOURCE_ADC) > 1536)
//		{
//			break;
//		}
//	}

	while(1)
	{
		if(gSys.CurModuleID == MODULE_ID_BT_HF)
		{
#ifdef BT_HF_RECORD_FUNC
			if(GetBtRecordState() == BT_RECORD_STATE_RECORDING)
			{
				uint8_t *	out_encoded_buf = NULL;
				uint32_t	out_encoded_buf_len = 0;

				if(audio_encoder_encode(&out_encoded_buf, &out_encoded_buf_len) == RT_SUCCESS)
				{
					EncodedDataToBuffer(out_encoded_buf, out_encoded_buf_len);
				}
				OSRescheduleTimeout(2); 
			}
			else
#endif
			{
				OSRescheduleTimeout(10);
			}
			continue;
		}

		if(gSys.CurModuleID == MODULE_ID_USB_AUDIO || 
			gSys.CurModuleID == MODULE_ID_USB_AUDIO_READER)
		{
			OSRescheduleTimeout(2); 
		}
		else
		{
			OSRescheduleTimeout(1); 
		}

        if(gSys.CurModuleID == MODULE_ID_I2SIN)
        {
            SampleCnt = I2sPmemPcmRemainLenGet();
        }
        else
        {
            SampleCnt = AdcPmemPcmRemainLenGet();
        }
        
		if(SampleCnt > MIC_BUF_LEN)
		{
			SampleCnt = MIC_BUF_LEN;
		}
		SampleCnt &= 0xFFFE;	//echo模块要求每次处理的点数必须是偶数

//		{
//			static uint16_t Cnt = 0;
//			if(Cnt++ > 100)
//			{
//				Cnt = 0;
//				DBG("%d\n", MixerGetRemainSamples(MIXER_SOURCE_MIC));	
//			}
//		}	
			
		//通道刚切换时会，会填充800多个sample的数据
		if(IsFillMemData == TRUE)
		{
			continue;
		}
		if((gSys.MicEnable) && (gSys.CurModuleID != MODULE_ID_I2SIN))	//如果当前有MIC，则MIC占用一路ADC，LINEIN、GUITAR、FM占用另一路ADC
		{
			if(MixerIsDone(MIXER_SOURCE_MIC) && MixerIsDone(MIXER_SOURCE_ANA_MONO))
			{
				//ADC的两个声道数据分开放，前SampleCnt个采样点是ADC一个声道，后SampleCnt个采样点是ADC另一个声道
				SampleCnt = AdcPcmDataRead((void*)AduioBuf, SampleCnt, 0x04);

#ifdef FUNC_DC_BLOCKER
				if(init_dcb_channel != 1)
				{
					init_dcb_channel = 1;
					memset(&dcb, 0, sizeof(DCBlocker));
					init_dc_blocker(&dcb, init_dcb_channel);
				}

				if(gSys.CurModuleID == MODULE_ID_LINEIN)
				{
					apply_dc_blocker(&dcb, (void*)AduioBuf, (void*)AduioBuf, SampleCnt);
				}
				
				#ifdef FUNC_SILENCE_DETECTOR
				if(init_sd_channel != 1)
				{
					init_sd_channel = 1;
					silence_counter = 0;
					memset(&sdct, 0, sizeof(SilenceDetectorContext));
					init_silence_detector(&sdct, init_sd_channel, 44100);
				}
				
				if(gSys.CurModuleID == MODULE_ID_LINEIN)
				{
					smoothed_pcm_amplitude = apply_silence_detector(&sdct, AduioBuf, SampleCnt);
					if(smoothed_pcm_amplitude < SILENCE_THRSHOLD)
					{
						silence_counter++; 
					}
					else
					{
						silence_counter = 0;
					}

					if(silence_counter > SILENCE_COUNT)
					{
						// go to sleep
					}
				}
				#endif
#endif
#ifdef FUNC_MIC_ECHO_EN
				if(!OldMicEnable)
				{
					memset(&ct, 0x00, sizeof(ct));
					init_echo(&ct, 1);
				}
				apply_echo(&ct, (void*)AduioBuf, (void*)AduioBuf, SampleCnt, EchoDepthStep * DEPTH_ONE_STEP, EchoDelayStep * DELAY_ONE_STEP * DacAdcSampleRateGet() / 44100); 
#endif
#ifdef FUNC_RECORD_EN
				MixerForRecoder((void*)AduioBuf, (void*)(AduioBuf + SampleCnt), SampleCnt, 0x04);	
#endif
				MixerSetData(MIXER_SOURCE_MIC, (void*)AduioBuf, SampleCnt);		//单声道MIC数据
#if defined(FUNC_USB_AUDIO_EN) || defined(FUNC_USB_READER_EN) || defined(FUNC_USB_AUDIO_READER_EN)
				DeviceAudioSetUpData((void*)AduioBuf, SampleCnt*2);
#endif

#ifdef FUNC_SPEC_TRUM_EN
				if((gSys.CurModuleID == MODULE_ID_LINEIN) 
					|| (gSys.CurModuleID == MODULE_ID_RADIO) 
					|| (gSys.CurModuleID == MODULE_ID_RECORDER))
				{
					SpecTrumProc((void*)(AduioBuf + SampleCnt), SampleCnt);
				}
#endif								
				MixerSetData(MIXER_SOURCE_ANA_MONO, (void*)(AduioBuf + SampleCnt), SampleCnt);	//单声道LINEIN、GUITAR、FM数据
			}
		}
		else			//如果当前无MIC，则LINEIN、FM, I2SIN为双声道输入
		{
			if(MixerIsDone(MIXER_SOURCE_ANA_STERO))
			{
				if(gSys.CurModuleID == MODULE_ID_I2SIN)
                {
                    SampleCnt = I2sPcmDataRead((void*)AduioBuf, SampleCnt, 0x03); 
                }
                else
                {
                    SampleCnt = AdcPcmDataRead((void*)AduioBuf, SampleCnt, 0x03);	
                }
                
#ifdef FUNC_DC_BLOCKER
				if(init_dcb_channel != 2)
				{
					init_dcb_channel = 2;
					memset(&dcb, 0, sizeof(DCBlocker));
					init_dc_blocker(&dcb, init_dcb_channel);
				}

				if(gSys.CurModuleID == MODULE_ID_LINEIN)
				{
					apply_dc_blocker(&dcb, (void*)AduioBuf, (void*)AduioBuf, SampleCnt);
				}

				#ifdef FUNC_SILENCE_DETECTOR
				if(init_sd_channel != 2)
				{
					init_sd_channel = 2;
					silence_counter = 0;
					memset(&sdct, 0, sizeof(SilenceDetectorContext));
					init_silence_detector(&sdct, init_sd_channel, 44100);
				}
				
				if(gSys.CurModuleID == MODULE_ID_LINEIN)
				{
					smoothed_pcm_amplitude = apply_silence_detector(&sdct, AduioBuf, SampleCnt);
					if(smoothed_pcm_amplitude < SILENCE_THRSHOLD)
					{
						silence_counter++; 
					}
					else
					{
						silence_counter = 0;
					}

					if(silence_counter > SILENCE_COUNT)
					{
						// go to sleep
					}
				}
				#endif
#endif

#ifdef FUNC_RECORD_EN
				MixerForRecoder((void*)AduioBuf, (void*)(AduioBuf + 1), SampleCnt, 0x03);
#endif
#ifdef FUNC_SPEC_TRUM_EN
                if((gSys.CurModuleID == MODULE_ID_LINEIN) 
					|| (gSys.CurModuleID == MODULE_ID_RADIO) 
					|| (gSys.CurModuleID == MODULE_ID_RECORDER))
            	{
					SpecTrumProc((void*)AduioBuf, SampleCnt);
            	}
#endif
#ifdef FUNC_AUDIO_DRC_EFFECT_EN   
                if(((gSys.CurModuleID == MODULE_ID_LINEIN) 
					|| (gSys.CurModuleID == MODULE_ID_RADIO) 
					|| (gSys.CurModuleID == MODULE_ID_RECORDER)
                    || (gSys.CurModuleID == MODULE_ID_I2SIN)) 
#ifdef FUNC_SOUND_REMIND
				&& (!IsSoundRemindPlaying())
#endif
				)                 
                {
                    AudioDRCPro(AduioBuf, 2, 44100, SampleCnt);
                }
#endif 
#ifdef FUNC_AUDIO_3D_EFFECT_EN   
                if(((gSys.CurModuleID == MODULE_ID_LINEIN) 
					|| (gSys.CurModuleID == MODULE_ID_RADIO) 
					|| (gSys.CurModuleID == MODULE_ID_RECORDER)
                    || (gSys.CurModuleID == MODULE_ID_I2SIN)) 
#ifdef FUNC_SOUND_REMIND
				&& (!IsSoundRemindPlaying())
#endif
				)                 
                {
                    Audio3dPro(AduioBuf, 2, 44100, SampleCnt);
                }
                
#endif 
#ifdef FUNC_AUDIO_VB_EFFECT_EN
                if(((gSys.CurModuleID == MODULE_ID_LINEIN) 
					|| (gSys.CurModuleID == MODULE_ID_RADIO) 
					|| (gSys.CurModuleID == MODULE_ID_RECORDER)
                    || (gSys.CurModuleID == MODULE_ID_I2SIN))
#ifdef FUNC_SOUND_REMIND
                    && (!IsSoundRemindPlaying())
#endif
				)
                {
                    AudioVbPro(AduioBuf, 2, 44100, SampleCnt);
                }
                
#endif                
				MixerSetData(MIXER_SOURCE_ANA_STERO, (void*)AduioBuf, SampleCnt);	//双声道LINEIN、GUITAR、FM数据
			}
		}

#ifdef FUNC_MIC_ECHO_EN
		OldMicEnable = gSys.MicEnable;
#endif
	}
}


//通道刚切换时会，往PCM部分填充800多个sample的数据
void PcmMemFillData(void)
{
	uint8_t i;
	//extern int16_t AduioBuf[];		//缓存ADC两个声道的数据
	memset((void*)AduioBuf, 0, (192*4));
	for(i=0; i<192; i++)
	{
		AduioBuf[i *2 +0] += 0;//0xFF60;
		AduioBuf[i *2 +1] += 0;//0xFF60;
	}
	if(gSys.MicEnable)
	{
		for(i=0; i<5; i++)
		{
			while(!MixerIsDone(MIXER_SOURCE_ANA_MONO));
			
			MixerSetData(MIXER_SOURCE_ANA_MONO, (void*)(AduioBuf + 192), 192);
			if(MixerGetRemainSamples(MIXER_SOURCE_ANA_MONO) > 880)
			{
				break;
			}
			while(!MixerIsDone(MIXER_SOURCE_MIC));
			
			MixerSetData(MIXER_SOURCE_MIC, (void*)AduioBuf, 192);
			if(MixerGetRemainSamples(MIXER_SOURCE_MIC) > 880)
			{
				break;
			}
		}
	}
	else
	{
		for(i=0; i<5; i++)
		{
			while(!MixerIsDone(MIXER_SOURCE_ANA_STERO));
			
			MixerSetData(MIXER_SOURCE_ANA_STERO, (void*)AduioBuf, 192);
			if(MixerGetRemainSamples(MIXER_SOURCE_ANA_STERO) > 880)
			{
				break;
			}
		}
	}
}
