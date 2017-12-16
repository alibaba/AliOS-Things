///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: task_decoder.c
///////////////////////////////////////////////////////////////////////////////
/*
zhouyi,2012.07.11, initial version
maintainer: Aissen
*/

#include "app_config.h"
#include "audio_decoder.h"
#include "mixer.h"
#include "fat_file.h"
#include "sys_app.h"
#include "sys_vol.h"
#include "timeout.h"
#include "debug.h"
#include "delay.h"
#include "sound_remind.h"
#include "micro_adjust.h"
#include "bt_stack_api.h"
#include "bt_app_func.h"
#include "dac.h"
#include "clk.h"
#include "uart.h"
#include "dev_detect_driver.h"


// 对外接口函数与本task之间通信的命令
#define DECODER_TASK_CTRL_NONE 		0	//空命令
#define DECODER_TASK_CTRL_PLAY		7	//播放
#define DECODER_TASK_CTRL_STOP		3	//停止本地播放、蓝牙播放、录音，但不退出任务
#define DECODER_TASK_CTRL_PAUSE		8	//暂停
#define DECODER_TASK_CTRL_RESUME	9	//由暂停恢复播放
#define DECODER_TASK_CTRL_FF		10	//快进
#define DECODER_TASK_CTRL_FB		11	//快退
#define DECODER_TASK_CTRL_SEEK		12	//指定时间播放
volatile static uint32_t  PlayerControlValue  = DECODER_TASK_CTRL_NONE;		//采用互斥机制
volatile static int32_t   PlayerControlParam  = 0; 							//互斥命令携带的参数信息
volatile static bool      PlayerControlPause  = FALSE; 						//互斥命令携带的参数信息

//decoder task状态
#define DECODER_STATE_STOP			0
#define DECODER_STATE_PLAY			1
#define DECODER_STATE_PAUSE			2
static uint8_t DecoderTaskState = DECODER_STATE_STOP;

//播放时间
static uint32_t TotalDecodedSamples = 0;	//统计已解码的samples，用于播放计时
static uint32_t DecoderPlayTime = 0;		//当前的播放时间，单位：ms
static uint8_t  IsFBToPreSong = 0;			//是否正处于快退到上一首歌曲的过程中
static bool  	IsDecoderInitOk = FALSE;	//Decoder初始化是否成功
//static uint32_t DecoderTicks = 0;

//当前文件
static void*    DecoderFileHandle = NULL;
static uint8_t  DecoderFileType = IO_TYPE_FILE;

#define DECODER_PLAY_START_DECODE	0
#define DECODER_PLAY_WAIT_XR_DONE	1
#define DECODER_PLAY_WAIT_MIX_DONE	2

static uint8_t DecoderPlayState = DECODER_PLAY_START_DECODE;

//#ifdef FUNC_SOUND_REMIND
//extern bool InterruptSoundRemind;
//#endif

#ifdef FUNC_FFFB_WITHOUT_SOUND
bool IsFFFBFlag = FALSE;
#endif
static uint32_t gCurDecoderType;

//#ifdef FUNC_AUDIO_DRC_EFFECT_EN
//extern bool	gDRCEffectInited;
//#endif

//#ifdef FUNC_AUDIO_3D_EFFECT_EN
//extern bool	g3dEffectInited;
//#endif

//#ifdef FUNC_AUDIO_VB_EFFECT_EN
//extern bool gVbEffectInited;
//#endif

//extern uint8_t mixer_pcm_format;

//底层在完成一帧数据的解码工作后回调本函数
void audio_decoder_interrupt_callback(int32_t intterupt_type)
{
	if(intterupt_type == 0)			//xr_done
	{
		OSQueueMsgSend(MSG_DECODER_XR_DONE, NULL, 0, MSGPRIO_LEVEL_MD, 0);
	}
}

static bool DecoderTaskWaitResp(uint16_t TimeOut)
{
	TIMER Timer;

	TimeOutSet(&Timer, TimeOut);
	while(PlayerControlValue != 0)
	{
		if(IsTimeOut(&Timer))
		{
			APP_DBG("start decoder task, but no response!\n");	
			return FALSE;
		} 	 
		OSRescheduleTimeout(5);
	}
	return TRUE;
}

//开始播放
//FileHandle: 文件指针
//FileType:   IO_TYPE_FILE--播放U盘/SD卡中的文件，IO_TYPE_MEMORY--播放提示音
//StartTime:  播放的开始时间 
bool DecoderTaskPlayStart(void* FileHandle, uint8_t FileType, uint32_t StartTime)
{	
	DBG("DecoderTaskPlayStart()\n");
	
	if(DecoderTaskState != DECODER_STATE_STOP)
	{
		return FALSE;
	}

	DecoderFileHandle = FileHandle;
	DecoderFileType = FileType;
	PlayerControlParam = StartTime;
	PlayerControlValue = DECODER_TASK_CTRL_PLAY;
	if(!DecoderTaskWaitResp(DECODER_TASK_WAIT_TIMEOUT))
	{
		return FALSE;
	}
	return TRUE;
}

//停止播放
void DecoderTaskStop(void)
{	
	DBG("DecoderTaskStop()\n");

	MixerMute(MIXER_SOURCE_DEC);
	WaitMs(200); //Modify by sam,2014-11-05.100有时候淡出不起作用。

	PlayerControlValue = DECODER_TASK_CTRL_STOP;
	DecoderTaskWaitResp(DECODER_TASK_WAIT_TIMEOUT);
}


//暂停
void DecoderTaskPause(void)
{	
	DBG("DecoderTaskPause()\n");

	if(DecoderTaskState != DECODER_STATE_PLAY)
	{
		return;
	}

	MixerMute(MIXER_SOURCE_DEC);
	WaitMs(50);
	PlayerControlValue = DECODER_TASK_CTRL_PAUSE;
	DecoderTaskWaitResp(DECODER_TASK_WAIT_TIMEOUT);
}

//由暂停恢复播放
void DecoderTaskResume(void)
{	
	DBG("DecoderTaskResume()\n");
	
#ifdef FUNC_FFFB_WITHOUT_SOUND	
	IsFFFBFlag = FALSE;
#endif		
	if(DecoderTaskState != DECODER_STATE_PAUSE)
	{
		return;
	}
	MixerUnmute(MIXER_SOURCE_DEC);
	PlayerControlValue = DECODER_TASK_CTRL_RESUME;
	DecoderTaskWaitResp(DECODER_TASK_WAIT_TIMEOUT);
}


//快进
void DecoderTaskFF(uint32_t StepTime)
{	
	DBG("DecoderTaskFF()\n");
	
	if(DecoderTaskState != DECODER_STATE_PLAY)
	{
		return;
	}

	MixerMute(MIXER_SOURCE_DEC);
	WaitMs(50);

	PlayerControlParam = StepTime;
	PlayerControlValue = DECODER_TASK_CTRL_FF;
	DecoderTaskWaitResp(DECODER_TASK_WAIT_TIMEOUT);

	WaitMs(50);
	MixerUnmute(MIXER_SOURCE_DEC);
}

//快退
void DecoderTaskFB(uint32_t StepTime)
{	
	DBG("DecoderTaskFB()\n");
	
	if(DecoderTaskState != DECODER_STATE_PLAY)
	{
		return;
	}

	MixerMute(MIXER_SOURCE_DEC);
	WaitMs(50);

	PlayerControlParam = StepTime;
	PlayerControlValue = DECODER_TASK_CTRL_FB;
	DecoderTaskWaitResp(DECODER_TASK_WAIT_TIMEOUT);

	WaitMs(50);
	MixerUnmute(MIXER_SOURCE_DEC);
}

//指定时间播放
void DecoderTaskSeek(uint32_t Time)
{	
	DBG("DecoderTaskSeek()\n");
	
	if(DecoderTaskState != DECODER_STATE_PLAY)
	{
		return;
	}

	PlayerControlParam = Time;
	PlayerControlValue = DECODER_TASK_CTRL_SEEK;
	DecoderTaskWaitResp(DECODER_TASK_WAIT_TIMEOUT);
}

//判断一首歌是否播放结束
bool DecoderTaskIsSongEnd(void)
{
	return (DecoderTaskState == DECODER_STATE_STOP);	
}

//判断一首歌是否快退到歌曲开头
bool DecoderTaskIsSongBegin(void)
{
	return IsFBToPreSong;	
}

//判断一首歌初始化是否成功
bool DecoderTaskIsInitializeOk(void)
{
	return IsDecoderInitOk;
}

//获取当前播放时间
uint32_t DecoderTaskGetPlayTime(void)
{
 	return DecoderPlayTime;
}

//阻塞播放提示音，播放完恢复原来的状态
bool DecoderTaskSyncPlay(void* FileHandle, uint8_t FileType)
{
//	extern const uint16_t gDecVolArr[];
	
	uint8_t  DecoderTaskState_Bak  = DecoderTaskState;
	uint32_t DecoderPlayTime_Bak   = DecoderPlayTime;
	uint8_t  IsFBToPreSong_Bak     = IsFBToPreSong;
	void*    DecoderFileHandle_Bak = DecoderFileHandle;
	uint8_t  DecoderFileType_Bak   = DecoderFileType;
	uint16_t DacSampleRate_Bak = DacAdcSampleRateGet();
	uint8_t	mixer_pcm_format_Bak = mixer_pcm_format;


	//停止原歌曲播放
	if(DecoderTaskState != DECODER_STATE_STOP)
	{
		DecoderTaskStop();
        MixerMute(MIXER_SOURCE_DEC);
        WaitMs(100);
	}

	//提示音使用固定音量
	MixerConfigVolume(MIXER_SOURCE_DEC, gDecVolArr[25], gDecVolArr[25]);
	MixerSetFadeSpeed(MIXER_SOURCE_DEC, 10, 10);

	//播放提示音
	DecoderTaskPlayStart(FileHandle, FileType, 0);

	while(!DecoderTaskIsSongEnd()
#ifdef FUNC_SOUND_REMIND
			&& !InterruptSoundRemind
#endif
			)
	{
		if(GetQuickResponseFlag())
		{
			SetQuickResponseFlag(TRUE);		// reconfig flag to TRUE
			DecoderTaskStop();
			break;
		}

		OSRescheduleTimeout(10);
	}
#ifdef FUNC_SOUND_REMIND
	if(InterruptSoundRemind)
	{
		DecoderTaskStop();
		InterruptSoundRemind = FALSE;
	}
#endif

 	IsFBToPreSong = IsFBToPreSong_Bak;

	MixerMute(MIXER_SOURCE_DEC);
	WaitMs(50);
	
	//恢复原歌曲播放
#ifndef FUNC_MIXER_SRC_EN
	AudioSampleRateSet(DacSampleRate_Bak);//恢复原来的采样率
#endif

	MixerConfigFormat(MIXER_SOURCE_DEC, DacSampleRate_Bak, mixer_pcm_format_Bak);
	MixerConfigVolume(MIXER_SOURCE_DEC, gDecVolArr[gSys.Volume], gDecVolArr[gSys.Volume]);
	MixerSetFadeSpeed(MIXER_SOURCE_DEC, DEC_FADEIN_TIME, 10);
	MixerUnmute(MIXER_SOURCE_DEC);
	
	if(DecoderTaskState_Bak != DECODER_STATE_STOP)
	{
		//如果原先是播放状态，则恢复原来的播放状态
		//如果原先是暂停状态，则恢复原来的暂停状态
		PlayerControlPause = (DecoderTaskState_Bak == DECODER_STATE_PAUSE);
		return DecoderTaskPlayStart(DecoderFileHandle_Bak, DecoderFileType_Bak, DecoderPlayTime_Bak);
	}
	return TRUE;
}


//初始化decoder，并返回当前解码类型
static DecoderType DecoderGetType(void)
{
	uint8_t* ExtFileName = ((FAT_FILE*)DecoderFileHandle)->ShortName;

	audio_decoder = NULL;
	
	//播放提示音
	if(DecoderFileType == IO_TYPE_MEMORY)
	{
//		uint8_t	fill_audio_stream_times = 10;

#ifdef FUNC_SOUND_REMIND
		mv_mread_callback_set(SoundRemindFillStreamCallback);
#endif
		if(RT_SUCCESS != audio_decoder_initialize((uint8_t*)DEC_MEM_ADDR, DecoderFileHandle, IO_TYPE_MEMORY, MP3_DECODER))
		{
            APP_DBG("mp3（格式错误）Error type: %d\r\n", audio_decoder_get_error_code());
            audio_decoder_close();
            audio_decoder = NULL;
            return UNKOWN_DECODER;
        }

		return MP3_DECODER;
	}

	//播放U盘/SD卡文件
	if(((ExtFileName[8] == 'M') && (ExtFileName[9] == 'P') && (ExtFileName[10] == '2'))
	|| ((ExtFileName[8] == 'M') && (ExtFileName[9] == 'P') && (ExtFileName[10] == '3'))
	|| ((ExtFileName[8] == 'W') && (ExtFileName[9] == 'M') && (ExtFileName[10] == 'V'))
	|| ((ExtFileName[8] == 'W') && (ExtFileName[9] == 'M') && (ExtFileName[10] == 'A'))
	|| ((ExtFileName[8] == 'A') && (ExtFileName[9] == 'S') && (ExtFileName[10] == 'F')))
	{
		if(audio_decoder_initialize((uint8_t*)DEC_MEM_ADDR, DecoderFileHandle, IO_TYPE_FILE, WAV_DECODER) == (int32_t)RT_SUCCESS)
		{
			audio_decoder_seek(0);
			return WAV_DECODER;
		}
            
		if(audio_decoder_initialize((uint8_t*)DEC_MEM_ADDR, DecoderFileHandle, IO_TYPE_FILE, WMA_DECODER) != (int32_t)RT_SUCCESS)
		{
			if(audio_decoder_get_error_code() != -256)
			{
				APP_DBG("wma（格式错误）Error type: %d\n", audio_decoder_get_error_code());
				APP_DBG("decoder size = %d\n", audio_decoder->decoder_size);
				audio_decoder_close();
				audio_decoder = NULL;
				return UNKOWN_DECODER;
			}
			if(audio_decoder_initialize((uint8_t*)DEC_MEM_ADDR, DecoderFileHandle, IO_TYPE_FILE, MP3_DECODER) != (int32_t)RT_SUCCESS)
			{
				APP_DBG("mp3（格式错误）Error type: %d\n", audio_decoder_get_error_code());
				audio_decoder_close();
				audio_decoder = NULL;
				return UNKOWN_DECODER;      //error found
			}
			return MP3_DECODER;
		}
		return WMA_DECODER;
	}
	
	if((ExtFileName[8] == 'W') && (ExtFileName[9] == 'A') && (ExtFileName[10] == 'V'))
	{
		APP_DBG("WAV_DECODER\n");
		return WAV_DECODER;
	}

	if((ExtFileName[8] == 'F') && (ExtFileName[9] == 'L') && (ExtFileName[10] == 'A'))
	{
		APP_DBG("FLAC_DECODER\n");
		return FLAC_DECODER;
	}

	if((ExtFileName[8] == 'S') && (ExtFileName[9] == 'B') && (ExtFileName[10] == 'C'))
	{
		return SBC_DECODER;
	}

	if(((ExtFileName[8] == 'A') && (ExtFileName[9] == 'A') && ExtFileName[10] == 'C') 
	|| ((ExtFileName[8] == 'M') && (ExtFileName[9] == 'P') && ExtFileName[10] == '4') 
	|| ((ExtFileName[8] == 'M') && (ExtFileName[9] == '4') && ExtFileName[10] == 'A'))
	{
		return AAC_DECODER;
	}
    
    if(ExtFileName[8] == 'A' && ExtFileName[9] == 'I' && ExtFileName[10] == 'F')
	{
		return AIF_DECODER;
	}
	return UNKOWN_DECODER;
}

// 音源格式是否支持，目前仅检测采样率
static bool IsSupportSourceType(uint32_t sample_rate, uint32_t decoder_type)
{
	bool		ret = FALSE;

	switch(sample_rate)
	{
		case 8000:
		case 11025:
		case 12000:
		case 16000:
		case 22050:
		case 24000:
		case 32000:
		case 44100:
		case 48000:
			ret = TRUE;
			break;

		default:
			ret = FALSE;
			break;
	}
	return ret;
}

//歌曲播放初始化
static bool DecoderPlayStart(void)
{	
	//播放初始化
	uint32_t DecoderType;
	APP_DBG("DoPlayer...\n");

	TotalDecodedSamples = 0;
//	DecoderPlayTime = 0;
	IsFBToPreSong = FALSE;
	IsDecoderInitOk = FALSE;
	
    DecoderType = DecoderGetType();
    gCurDecoderType = DecoderType;
	if(DecoderType == UNKOWN_DECODER)
	{	
		return FALSE;
	}
	
	if(DecoderFileType == IO_TYPE_FILE)
	{
		FileSeek(DecoderFileHandle, 0, SEEK_SET);
		if(audio_decoder_initialize((uint8_t*)DEC_MEM_ADDR, DecoderFileHandle, IO_TYPE_FILE, DecoderType) != (int32_t)RT_SUCCESS)
		{
			audio_decoder = NULL;
			APP_DBG("（格式错误）Error Id = %x\n, DecoderType = %d\n", (int32_t)audio_decoder_get_error_code(), DecoderType);
			return FALSE;//error found
		}

		// 判断是否是支持的音源格式
		if(!IsSupportSourceType(audio_decoder->song_info->sampling_rate, DecoderType))
		{
			return FALSE;
		}
	}
	IsDecoderInitOk = TRUE;
	
	APP_DBG("decoder size = %d\n", audio_decoder->decoder_size);
	DBG("Fs = %d, Chl = %d\n", audio_decoder->song_info->sampling_rate, audio_decoder->song_info->num_channels);

	MixerConfigFormat(MIXER_SOURCE_DEC, audio_decoder->song_info->sampling_rate, (audio_decoder->song_info->num_channels == 1) ? MIXER_FORMAT_MONO : MIXER_FORMAT_STERO);
	if(DecoderFileType == IO_TYPE_FILE)
	{
		mixer_pcm_format = (audio_decoder->song_info->num_channels == 1) ? MIXER_FORMAT_MONO : MIXER_FORMAT_STERO;
	}


#ifndef FUNC_MIXER_SRC_EN
	AudioSampleRateSet(audio_decoder->song_info->sampling_rate);//设置采样率
#endif

    
	if(audio_decoder->bc->io_type != IO_TYPE_MEMORY)
	{
		audio_decoder_seek(DecoderPlayTime);
	}

	NVIC_EnableIRQ(DECODER_IRQn);

#ifdef FUNC_AUDIO_DRC_EFFECT_EN
	if(gSys.AudioDRCEffectFlag)
	{
        if(DRC_ERROR_OK != init_drc(&gSys.AudioDRCEffectHandle, audio_decoder->song_info->num_channels, audio_decoder->song_info->sampling_rate,
							AUDIO_EFFECT_PARAM_DRC_THRESHHOLD, 0, AUDIO_EFFECT_PARAM_DRC_RATIO, AUDIO_EFFECT_PARAM_DRC_ATTACK_TIME, 
							AUDIO_EFFECT_PARAM_DRC_RELEASE_TIME, AUDIO_EFFECT_PARAM_DRC_CROSSOVER_FREQ))
		{
            gDRCEffectInited = FALSE;
        }
        else
        {
        	gDRCEffectInited = TRUE;
        }
        InitVb3dPcmPara();
	}
#endif	
	
#ifdef FUNC_AUDIO_3D_EFFECT_EN
	if(gSys.Audio3dEffectFlag)
	{
        if(THREE_D_ERROR_OK != init_3d(&gSys.Audio3dEffectHandle, audio_decoder->song_info->num_channels, audio_decoder->song_info->sampling_rate
        								, AUDIO_EFFECT_PARAM_3D_BASS, AUDIO_EFFECT_PARAM_3D_MID_BOOST))
        {
            g3dEffectInited = FALSE;
        }
        else
        {
        	g3dEffectInited = TRUE;
        }
        InitVb3dPcmPara();
	}
#endif

#ifdef FUNC_AUDIO_VB_EFFECT_EN
    if(gSys.AudioVbEffectFlag)
    {
        if(VB_ERROR_OK != init_vb(&gSys.AudioVbEffectHandle, audio_decoder->song_info->num_channels, 
                                            audio_decoder->song_info->sampling_rate, AUDIO_EFFECT_PARAM_VB_CUTOFF_FREQ, AUDIO_EFFECT_PARAM_VB_LIMITER_EN))
        {
            gVbEffectInited = FALSE;
        } 
        else
        {
            gVbEffectInited = TRUE;
        }
        InitVb3dPcmPara();  
    }
#endif

	MixerUnmute(MIXER_SOURCE_DEC);
	MixerEnable(MIXER_SOURCE_DEC);

	return TRUE;
}

static void DecoderPlayStop(void)
{
	APP_DBG("end_of_play...\n");

	DecoderPlayTime = 0;

	if((DecoderTaskState == DECODER_STATE_PLAY) && (DecoderPlayState == DECODER_PLAY_WAIT_XR_DONE))
	{
		while(!audio_decoder_check_xr_done());
		audio_decoder_clear_xr_done();
	}
	DecoderPlayState = DECODER_PLAY_START_DECODE;

	mv_mread_callback_unset();
	
	//播放结束
//	MixerDisable(MIXER_SOURCE_DEC);

//	if(audio_decoder != NULL)
//	{
//		audio_decoder_close();
//	}
#ifdef FUNC_FFFB_WITHOUT_SOUND
	IsFFFBFlag = FALSE;
#endif
}


static bool SongPlayDo(void)
{
	uint32_t Temp;
	static TIMER DecoderTimer;	//计算当前帧解码所花的时间
	uint8_t Retry = 3;

	while(Retry--)
	{
		if(DecoderPlayState == DECODER_PLAY_WAIT_MIX_DONE)			//等待MIX_DONE
		{
			if(!MixerIsDone(MIXER_SOURCE_DEC))
			{
				return TRUE;
			}
	
			//如果MIX_DONE，则本次立即进入DECODER_START_DECODE状态解码下一帧数据
			DecoderPlayState = DECODER_PLAY_START_DECODE;
		}
	
		if(DecoderPlayState == DECODER_PLAY_START_DECODE)			//启动解码一帧
		{
			if(audio_decoder->bc->io_type == IO_TYPE_MEMORY)
			{				
#ifdef FUNC_SOUND_REMIND
				if(SoundRemindFillStream() == -1 && audio_decoder_get_error_code() == -127)
				{
					return FALSE; // 流播放结束
				}
#endif
			}
	
		 	if(RT_YES != audio_decoder_can_continue())
			{
#ifdef FUNC_AUDIO_VB_EFFECT_EN
                if(gSys.AudioVbEffectFlag && (gCurDecoderType != FLAC_DECODER))
                {
                    DealWithVb3dPcmPara(audio_decoder->song_info->num_channels, audio_decoder->song_info->sampling_rate);
                    audio_decoder->song_info->pcm_data_length =  SaveCurSendLast((int32_t *)audio_decoder->song_info->pcm0_addr, 
                                                                                            audio_decoder->song_info->pcm_data_length, 
                                                                                            audio_decoder->song_info->num_channels);
                    MixerSetData(MIXER_SOURCE_DEC, audio_decoder->song_info->pcm0_addr, audio_decoder->song_info->pcm_data_length);

                }
#endif				
                return FALSE;	//歌曲播放到结尾
			}
			
			TimeOutSet(&DecoderTimer, 0);
			audio_decoder_clear_xr_done();
			if(RT_SUCCESS != audio_decoder_decode())
			{
				APP_DBG("audio_decoder_get_error_code(%d)\n", audio_decoder_get_error_code());
				return TRUE;
			}
			DecoderPlayState = DECODER_PLAY_WAIT_XR_DONE;
		}
		else if(DecoderPlayState == DECODER_PLAY_WAIT_XR_DONE)		//等待XR_DONE
		{
#ifdef  FUNC_AUDIO_VB_EFFECT_EN
            if(gSys.AudioVbEffectFlag && (gCurDecoderType != FLAC_DECODER))
            {
                DealWithVb3dPcmPara(audio_decoder->song_info->num_channels, audio_decoder->song_info->sampling_rate);
            } 
#endif
			if(is_audio_decoder_with_hardware())//wav不需要硬件解码
			{	
				if(!audio_decoder_check_xr_done())
				{
					return TRUE;
				}
				audio_decoder_clear_xr_done();
			}
	//		APP_DBG("XR: %d ms, %d\n", PastTimeGet(&DecoderTimer), audio_decoder->song_info->pcm_data_length);
	
			//U盘拔掉时，audio_decoder_decode()函数会阻塞260ms左右。
			//之后还会正确解出一帧数据，应丢弃本次PCM数据
			if(PastTimeGet(&DecoderTimer) > 100)
			{
				DBG("decoder error!\n");
				DecoderPlayState = DECODER_PLAY_WAIT_MIX_DONE;
				return TRUE;
			}
	
			//计算播放时间
			TotalDecodedSamples += audio_decoder->song_info->pcm_data_length;
			Temp = TotalDecodedSamples * 1000 / audio_decoder->song_info->sampling_rate;
			TotalDecodedSamples -= (Temp * audio_decoder->song_info->sampling_rate / 1000);
			DecoderPlayTime += Temp;
	
			//将decoder输出的PCM数据转换成统一的格式
			//单声道统一转换成DATA_MODE_MONO_S
			//双声道统一转换成DATA_MODE_STEREO_R_L
			audio_decoder_convert_pcm_data_layout();
            
#ifdef FUNC_AUDIO_DRC_EFFECT_EN
            AudioDRCPro((int16_t *)audio_decoder->song_info->pcm0_addr, audio_decoder->song_info->num_channels, 
                        audio_decoder->song_info->sampling_rate, audio_decoder->song_info->pcm_data_length);            
#endif   
			
#ifdef  FUNC_AUDIO_VB_EFFECT_EN
            if(gSys.AudioVbEffectFlag && (gCurDecoderType != FLAC_DECODER))
            {                  
                audio_decoder->song_info->pcm_data_length =  SaveCurSendLast((int32_t *)audio_decoder->song_info->pcm0_addr, 
                                                                            audio_decoder->song_info->pcm_data_length, 
                                                                            audio_decoder->song_info->num_channels);  
            }  
#endif 
			
#ifdef FUNC_SPEC_TRUM_EN
			SpecTrumProc((void*)audio_decoder->song_info->pcm0_addr, audio_decoder->song_info->pcm_data_length);
#endif
			
#ifdef FUNC_AUDIO_3D_EFFECT_EN
            Audio3dPro((int16_t *)audio_decoder->song_info->pcm0_addr, audio_decoder->song_info->num_channels, 
                        audio_decoder->song_info->sampling_rate, audio_decoder->song_info->pcm_data_length);            
#endif
			
            MixerSetData(MIXER_SOURCE_DEC, audio_decoder->song_info->pcm0_addr, audio_decoder->song_info->pcm_data_length);
			
			DecoderPlayState = DECODER_PLAY_WAIT_MIX_DONE;
		}
	}

	return TRUE;
}

/////////////////// BT Decoder ///////////////////
#ifdef FUNC_BT_EN

extern uint32_t gBtStatckReady;

typedef enum _SBC_DECODING_STATE{
	SBC_DECODING_START_DECODE = 0,
	SBC_DECODING_START_XR,
	SBC_DECODING_WAIT_XR_DONE,
	SBC_DECODING_START_MIX,
	SBC_DECODING_WAIT_MIX_DONE
} SBC_DECODING_STATE;

static bool			sbc_decoder_ready = FALSE;
static SBC_DECODING_STATE SbcDecodingState = SBC_DECODING_START_DECODE;

static void SetSbcDecodingState(SBC_DECODING_STATE state)
{
	SbcDecodingState = state;
}

static SBC_DECODING_STATE GetSbcDecodingState(void)
{
	return SbcDecodingState;
}

typedef enum _SBC_PROCESS_RET{
	SBC_PROCESS_RET_GENERAL = 0,
	SBC_PROCESS_RET_WAIT_XR_DONE,
	SBC_PROCESS_RET_NEED_MORE
}SBC_PROCESS_RET;

static uint8_t SbcDecoderProcess(void)
{	
	uint8_t Retry = 3;
    
    while(Retry--)
    {
        if(GetSbcDecodingState() == SBC_DECODING_WAIT_MIX_DONE)			//等待MIX_DONE
        {
            if(!MixerIsDone(MIXER_SOURCE_DEC))
            {
                return SBC_PROCESS_RET_GENERAL;
            }

            //如果MIX_DONE，则本次立即进入DECODER_START_DECODE状态解码下一帧数据
            SetSbcDecodingState(SBC_DECODING_START_DECODE);
        }
        
        if(GetSbcDecodingState() == SBC_DECODING_START_DECODE)			//启动解码一帧
        {
            audio_decoder_clear_xr_done();
            if(RT_SUCCESS != audio_decoder_decode())
            {
                if(audio_decoder_get_error_code() != SBC_ERROR_OK)
                {
                    if(audio_decoder_get_error_code() == SBC_ERROR_STREAM_EMPTY)
                    {
                        SetSbcDecDoneFlag();//inform bt stack that it can refill data now.
                        SetSbcDataState(SBC_DATA_STATE_NEED_MORE);
                        OSQueueMsgSend(MSG_NEED_MORE_SBC_DATA, NULL, 0, MSGPRIO_LEVEL_HI, 0);
                        return SBC_PROCESS_RET_NEED_MORE;
                    }
                    else
                    {
                        APP_DBG("[BT] TK : BT decoder error code : %d\n", audio_decoder_get_error_code());	//error
                    }
                }
                return SBC_PROCESS_RET_GENERAL;
            }

            if( 0
#ifdef FUNC_AUDIO_DRC_EFFECT_EN
			|| gSys.AudioDRCEffectFlag
#endif			
#ifdef FUNC_AUDIO_VB_EFFECT_EN
                || gSys.AudioVbEffectFlag
#endif
#ifdef FUNC_AUDIO_3D_EFFECT_EN
                ||  gSys.Audio3dEffectFlag
#endif
                )
            {
#if defined (FUNC_AUDIO_VB_EFFECT_EN) || defined (FUNC_AUDIO_3D_EFFECT_EN)  ||defined (FUNC_AUDIO_DRC_EFFECT_EN)
                DealWithVb3dPcmPara(audio_decoder->song_info->num_channels, audio_decoder->song_info->sampling_rate);
#endif
            }
            SetSbcDecodingState(SBC_DECODING_WAIT_XR_DONE);
        }
        else if(GetSbcDecodingState() == SBC_DECODING_WAIT_XR_DONE)		//等待XR_DONE
        {
            if(!audio_decoder_check_xr_done())
            {
                return SBC_PROCESS_RET_WAIT_XR_DONE;
            }
            audio_decoder_clear_xr_done();
            
            //将decoder输出的PCM数据转换成统一的格式
            //单声道统一转换成DATA_MODE_MONO_S
            //双声道统一转换成DATA_MODE_STEREO_R_L
            audio_decoder_convert_pcm_data_layout();
#ifdef FUNC_SOFT_ADJUST_EN        
            audio_decoder->song_info->pcm_data_length = BtSoftAdjustApply(4096, (int16_t *)audio_decoder->song_info->pcm0_addr, 
                                                                          audio_decoder->song_info->pcm_data_length,
                                                                          audio_decoder->song_info->num_channels);
#endif
                    
#ifdef FUNC_SPEC_TRUM_EN
            SpecTrumProc((void*)audio_decoder->song_info->pcm0_addr, audio_decoder->song_info->pcm_data_length);
#endif        

            if(0
#ifdef FUNC_AUDIO_DRC_EFFECT_EN
			|| gSys.AudioDRCEffectFlag
#endif			
#ifdef FUNC_AUDIO_VB_EFFECT_EN
                || gSys.AudioVbEffectFlag
#endif
#ifdef FUNC_AUDIO_3D_EFFECT_EN
                ||  gSys.Audio3dEffectFlag
#endif
            )
            {
#if defined(FUNC_AUDIO_VB_EFFECT_EN) || defined(FUNC_AUDIO_3D_EFFECT_EN) || defined (FUNC_AUDIO_DRC_EFFECT_EN)
                audio_decoder->song_info->pcm_data_length =  SaveCurSendLast((int32_t *)audio_decoder->song_info->pcm0_addr, 
                                                                            audio_decoder->song_info->pcm_data_length, 
                                                                            audio_decoder->song_info->num_channels);
#endif
            }

            MixerSetData(MIXER_SOURCE_BT, audio_decoder->song_info->pcm0_addr, audio_decoder->song_info->pcm_data_length);

            SetSbcDecodingState(SBC_DECODING_WAIT_MIX_DONE);
        }
    }   
	return SBC_PROCESS_RET_GENERAL;
}

static void SbcDecoderStart(void)
{
	int32_t 			ret = 0;
	SongInfo *		CurSongInfo;
	
	APP_DBG("[BT] TK : SbcDecoderStart\n");

	mv_mread_callback_unset();
	
	ret = audio_decoder_initialize((uint8_t*)DEC_MEM_ADDR, GetSbcStreamHandle(), IO_TYPE_MEMORY, SBC_DECODER);
	
	if(ret != RT_SUCCESS)
	{
		// to add some judgement in future !!!
		SetSbcDecDoneFlag();
		APP_DBG("audio_decoder_initialize error code:%d!\n", audio_decoder_get_error_code());
		return;
	}

	APP_DBG("BT audio decoder sample rate = %d\n", audio_decoder->song_info->sampling_rate);
	
	NVIC_EnableIRQ(DECODER_IRQn);
	SetSbcPlayCfgFlag(TRUE);				//inform bt stack that decoder is ready now.

	CurSongInfo = audio_decoder_get_song_info(); //decoder api function
    
	MixerConfigFormat(MIXER_SOURCE_BT, 
					  CurSongInfo->sampling_rate, 
					  (CurSongInfo->num_channels == 1) ? MIXER_FORMAT_MONO : MIXER_FORMAT_STERO);
	mixer_pcm_format = (CurSongInfo->num_channels == 1) ? MIXER_FORMAT_MONO : MIXER_FORMAT_STERO;
#ifdef FUNC_SOFT_ADJUST_EN
    SoftAdjustInit(CurSongInfo->num_channels);   
#endif
    
#ifdef FUNC_AUDIO_DRC_EFFECT_EN
	if(gSys.AudioDRCEffectFlag)
	{
		if(DRC_ERROR_OK != init_drc(&gSys.AudioDRCEffectHandle, (CurSongInfo->num_channels == 1) ? MIXER_FORMAT_MONO : MIXER_FORMAT_STERO, CurSongInfo->sampling_rate, 
							AUDIO_EFFECT_PARAM_DRC_THRESHHOLD, 0, AUDIO_EFFECT_PARAM_DRC_RATIO, AUDIO_EFFECT_PARAM_DRC_ATTACK_TIME, 
							AUDIO_EFFECT_PARAM_DRC_RELEASE_TIME, AUDIO_EFFECT_PARAM_DRC_CROSSOVER_FREQ))
        {
            APP_DBG("Init DRC effect fail, close\n");
            gDRCEffectInited = FALSE;
        }
        else
        {
        	gDRCEffectInited = TRUE;
        }
        InitVb3dPcmPara();
	}
#endif
#ifdef FUNC_AUDIO_3D_EFFECT_EN
	if(gSys.Audio3dEffectFlag)
	{
		if(THREE_D_ERROR_OK != init_3d(&gSys.Audio3dEffectHandle, (CurSongInfo->num_channels == 1) ? MIXER_FORMAT_MONO : MIXER_FORMAT_STERO, CurSongInfo->sampling_rate
										, AUDIO_EFFECT_PARAM_3D_BASS, AUDIO_EFFECT_PARAM_3D_MID_BOOST))
        {
            //gSys.Audio3dEffectFlag = 0;
            APP_DBG("Init 3d effect fail, close\n");
            g3dEffectInited = FALSE;
        }
        else
        {
        	g3dEffectInited = TRUE;
        }
        InitVb3dPcmPara();
	}
#endif

#ifdef FUNC_AUDIO_VB_EFFECT_EN
    if(gSys.AudioVbEffectFlag)
    {
        if(VB_ERROR_OK != init_vb(&gSys.AudioVbEffectHandle, (CurSongInfo->num_channels == 1) ? MIXER_FORMAT_MONO : MIXER_FORMAT_STERO, 
                                            CurSongInfo->sampling_rate, AUDIO_EFFECT_PARAM_VB_CUTOFF_FREQ, AUDIO_EFFECT_PARAM_VB_LIMITER_EN))
        {
            gVbEffectInited = FALSE;
        } 
        else
        {
            gVbEffectInited = TRUE;
        }
        InitVb3dPcmPara();
    }
#endif    
    
#ifndef FUNC_MIXER_SRC_EN
	AudioSampleRateSet(CurSongInfo->sampling_rate);
#endif

	SetSysVol();
	MixerUnmute(MIXER_SOURCE_BT);
	MixerEnable(MIXER_SOURCE_BT);


	MsgAddSet(MSG_DECODER_INT_CLASS);		//注册DECODER消息集
	
	SetSbcDecodingState(SBC_DECODING_START_DECODE);
	
	sbc_decoder_ready = TRUE;
}

static void SbcDecoderStop(void)
{
	APP_DBG("[BT] TK : SbcDecoderStop\n");

#if (!defined (OUTPUT_CHANNEL_CLASSD)) && (!defined (OUTPUT_CHANNEL_DAC_CLASSD))//usb 模式下微调
    DacSampRateAdjust(TRUE, 0);
#endif

	SetSbcPlayCfgFlag(FALSE);
	SetSbcDataState(SBC_DATA_STATE_NONE);

//	MsgDelSet(MSG_DECODER_INT_CLASS);
	MsgClearClass(MSG_DECODER_INT_CLASS);
	sbc_decoder_ready = FALSE;
}

#endif
//decoder task entrance
void DecoderTaskEntrance(void)
{
	PlayerControlValue = DECODER_TASK_CTRL_NONE;		// 采用互斥机制
	PlayerControlParam = 0; 							// 互斥命令携带的参数信息

	DBG("enter decoder task，wait gui task init...\n");
	audio_decoder = NULL;

	// 注册消息
	MsgAddSet(MSG_DECODER_INT_CLASS);

	DecoderTaskState = DECODER_STATE_STOP;
	PlayerControlValue = 0;

	while(1)
	{
#ifdef FUNC_BT_EN
		if((gSys.CurModuleID == MODULE_ID_BLUETOOTH) 
			&& (gBtStatckReady != 0) 
			&& IsBtAudioPlay()
#ifdef FUNC_SOUND_REMIND
			&& !IsSoundRemindPlaying()
#endif
			)
		{
			if(!sbc_decoder_ready && GetHasSbcDataFlag())
			{
				SbcDecoderStart();
			}

			if(sbc_decoder_ready && GetHasSbcDataFlag())
			{
                
#ifndef OUTPUT_CHANNEL_CLASSD
    #ifndef OUTPUT_CHANNEL_DAC_CLASSD
        #ifndef FUNC_SOFT_ADJUST_EN
                DacAdjust(); //如果没有开启软件微调，则使用usb 模式下硬件微调
        #endif
    #endif
#endif

				switch(SbcDecoderProcess())
				{
					case SBC_PROCESS_RET_NEED_MORE:
						while(GetSbcDataState() != SBC_DATA_READY)
						{
							OSQueueMsgRecv(NULL, NULL, 10);
							if(GetSbcDataState() == SBC_DATA_STATE_NONE)
								break;
						}
						break;
					case SBC_PROCESS_RET_GENERAL:
					case SBC_PROCESS_RET_WAIT_XR_DONE:
					default:
						if((BuartIOctl(BUART_IOCTL_RXFIFO_DATLEN_GET, 0) > 800) || (GetRcvListItemNum() >= 2))
						{
							OSQueueMsgSend(MSG_BT_STACK_RUN, NULL, 0, MSGPRIO_LEVEL_HI, 0);
						}
						OSQueueMsgRecv(NULL, NULL, 1);
						break;
				}
			}
			else
			{
				OSRescheduleTimeout(2);
			}
		}
		else
#endif
		{
			MsgRecv((DecoderTaskState == DECODER_STATE_PLAY) ? 1 : 10);

			//手动切歌
			switch(PlayerControlValue)
			{
				case DECODER_TASK_CTRL_STOP:
					DecoderPlayStop(); 
					DecoderTaskState = DECODER_STATE_STOP;
					PlayerControlValue = 0;
					break;

				case DECODER_TASK_CTRL_PLAY:
					if(DecoderTaskState == DECODER_STATE_STOP)
					{
						DecoderPlayTime = PlayerControlParam;
						if(DecoderPlayStart())
						{
						 	if(PlayerControlPause)
							{
								DecoderTaskState = DECODER_STATE_PAUSE;
							}
							else
							{
								DecoderTaskState = DECODER_STATE_PLAY;
							}
						}
					}
					PlayerControlPause = FALSE;	//默认初始化后自动播放
					PlayerControlValue = 0;
					break;

				case DECODER_TASK_CTRL_PAUSE:
					if(DecoderTaskState == DECODER_STATE_PLAY)
					{
						DecoderTaskState = DECODER_STATE_PAUSE;
					}
					PlayerControlValue = 0;
					break;

				case DECODER_TASK_CTRL_RESUME:
					if(DecoderTaskState == DECODER_STATE_PAUSE)
					{
						DecoderTaskState = DECODER_STATE_PLAY;
					}
					PlayerControlValue = 0;
					break;

				case DECODER_TASK_CTRL_FF:
					if(DecoderTaskState == DECODER_STATE_PLAY)
					{		
						if(DecoderPlayTime >= audio_decoder->song_info->duration)
						{
							DecoderPlayStop(); 
							DecoderTaskState = DECODER_STATE_STOP;
							break;
						}
						else
						{
							DecoderPlayTime += PlayerControlParam;
							TotalDecodedSamples = 0;
							APP_DBG("FF:%dms\n", DecoderPlayTime);
							audio_decoder_seek(DecoderPlayTime);
#ifdef FUNC_FFFB_WITHOUT_SOUND
							IsFFFBFlag = TRUE;
#endif
						}
					}
					PlayerControlValue = 0;
					break;

				case DECODER_TASK_CTRL_FB:
					if(DecoderTaskState == DECODER_STATE_PLAY)
					{	
						if(DecoderPlayTime <= PlayerControlParam)
						{
							DecoderPlayTime = 0;
							TotalDecodedSamples = 0;
							IsFBToPreSong = TRUE;
							DecoderPlayStop(); 
							DecoderTaskState = DECODER_STATE_STOP;
						}
						else
						{
							DecoderPlayTime -= PlayerControlParam;
							TotalDecodedSamples = 0;
							APP_DBG("FF:%dms\n", DecoderPlayTime);
							audio_decoder_seek(DecoderPlayTime);
#ifdef FUNC_FFFB_WITHOUT_SOUND
							IsFFFBFlag = TRUE;
#endif
						}						
					}
					PlayerControlValue = 0;
					break;

				default:
					PlayerControlValue = 0;
					break;
			}

			//播放状态下解码
			if(DecoderTaskState == DECODER_STATE_PLAY)
			{
				if(
#ifdef FUNC_FFFB_WITHOUT_SOUND
				!IsFFFBFlag && 
#endif
				!SongPlayDo())
				{
					DecoderPlayStop(); 
					DecoderTaskState = DECODER_STATE_STOP;
				}
	//			DecoderTicks = OSSysTickGet(); // 实时解码时、刷新ticks，用于判断当前解码是否阻塞中
			}
		}
#ifdef FUNC_BT_EN
		if(gBtStatckReady != 0)
		{
			if(GetA2dpState() == BT_A2DP_STATE_STREAMING)
			{
				if(!((gSys.CurModuleID == MODULE_ID_BLUETOOTH) && IsBtAudioPlay()))
				{
					SetSbcDecDoneFlag();
				}
			}

			if(!IsBtAudioPlay() && sbc_decoder_ready)
			{
				SbcDecoderStop();
			}
		}
#endif
	}
}
