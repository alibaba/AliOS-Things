///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: sound_remind.h
//  maintainer: Halley
///////////////////////////////////////////////////////////////////////////////

#ifndef __SOUND_REMIND_H__
#define __SOUND_REMIND_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

typedef enum _SOUND_ID
{
	SOUND_NUM0,						//0
	SOUND_NUM1,						//1    
	SOUND_NUM2,						//2   
	SOUND_NUM3,						//3    
	SOUND_NUM4,						//4  
	SOUND_NUM5,						//5   
	SOUND_NUM6,						//6 
	SOUND_NUM7,						//7  
	SOUND_NUM8,						//8    
	SOUND_NUM9,						//9   
	SOUND_PLUS5,					//加5
	SOUND_SPEECH_OP,				//语音打开
	SOUND_SPEECH_CL,				//语音关闭	
	SOUND_SPEECH_CL_FORCE,			//语音强制关闭
	SOUND_NEXT_FOLD,				//下一目录
	SOUND_REP_1,					//单曲循环
	SOUND_REP_ALL,					//全部循环
	SOUND_START_REC,				//开始录音
	SOUND_SAVE_REC,					//录音保存
	SOUND_REC_PLAYBACK,				//录音播放
	SOUND_DELETE_REC,				//删除录音
	SOUND_LINEIN,					//音频输入
	SOUND_UDISC_PLAYBACK,			//u盘播放
	SOUND_CARD_PLAYBACK,			//卡播放
	SOUND_RADIO_MODE,				//收音模式
	SOUND_PREV_TRACK,				//上一曲
	SOUND_NEXT_TRACK,				//下一曲
	SOUND_PAUSE,					//暂停
	SOUND_STOP,						//停止
	SOUND_ECHO_REGULATE,			//混响调节
	SOUND_ECHO_PLUS,				//混响加
	SOUND_ECHO_REDUCE,				//混响减
	SOUND_DELAY_REGULATE,			//延时调节
	SOUND_DELAY_PLUS,				//延时加
	SOUND_DELAY_REDUCE,				//延时减
	SOUND_TREB_REGULATE,			//高音调节
	SOUND_TREB_PLUS,				//高音加
	SOUND_TREB_REDUCE,				//高音减
	SOUND_BASS_REGULATE,			//低音调节
	SOUND_BASS_PLUS,				//低音加
	SOUND_BASS_REDUCE,				//低音减
	SOUND_MIC_OPEN,					//话筒打开
	SOUND_MIC_CLOSE,				//话筒关闭
	SOUND_EQ_FLAT,					//自然
	SOUND_EQ_ROCK,					//摇滚
	SOUND_EQ_POP,					//流行
	SOUND_EQ_CLASSIC,				//古典
	SOUND_EQ_SOFT,					//柔和
	SOUND_EQ_JAZZ,					//爵士
	SOUND_SUBBASS,					//重低音
	SOUND_MENU,						//菜单
	SOUND_BT_MODE,					//蓝牙模式
	SOUND_BT_CONNECT,				//蓝牙连接
	SOUND_BT_DISCONNECT,			//蓝牙断开
	SOUND_POINT,					//点
	SOUND_LANG_MODE,				//中文模式/英文模式
	SOUND_USB_AUDIO_MODE,			//声卡模式
	SOUND_USB_AUDIO_READER_MODE,	//一线通模式
	SOUND_PLAY,	            		//播放
	SOUND_FF,	            		//快进
	SOUND_FB,	            		//快退
	SOUND_MHZ,	            		//兆赫兹
	SOUND_REP_FOLD,	        		//目录循环
	SOUND_INTRO,	            	//浏览播放
	SOUND_RANDOM,	        		//随机播放
	SOUND_AUTO_SCAN,	        	//自动搜台
	SOUND_PRE_STATION,	    		//上一电台
	SOUND_NXT_STATION,	    		//下一电台
	SOUND_PWR_OFF,	        		//已关机
	SOUND_GUITAR_VOL,	    		//吉他音量
	SOUND_SEARCHING,	        	//搜索中
	SOUND_MUTE_ON,	        		//静音打开
	SOUND_MUTE_OFF,	        		//静音关闭
	SOUND_MIC_VOL,	        		//话筒音量
	SOUND_MAIN_VOL,	        		//主音量
	SOUND_MIC_MODE_OPEN,	    	//话筒模式开
	SOUND_MIC_MODE_CLOSE,			//话筒模式关
	SOUND_MIC_FIRST_OPEN,			//MIC优先开
	SOUND_MIC_FIRST_CLOSE,			//MIC优先关
	SOUND_OPEN,	            		//打开
	SOUND_CLOSE,	            	//关闭
	SOUND_BAT_LOW_PWR,	    		//电池电量低，请充电
	SOUND_CHARGING,	        		//正在充电
	SOUND_BAT_FULL,	        		//电量已充满
	SOUND_WAIT_PAIR,	        	//等待配对设备
	SOUND_SHI,	            		//十
	SOUND_BAI,	            		//百
	SOUND_MIC_INSERT,	    		//话筒插入
	SOUND_MIC_PULLOUT,	    		//话筒拔出
	SOUND_50MS,	            		//50
	SOUND_100MS,	            	//100
	SOUND_150MS,	            	//150
	SOUND_200MS,	            	//200
	
	SOUND_BT_STATE_READY_RING,
	SOUND_BT_STATE_UNREADY_RING,

} SOUND_ID;


//提示音模块初始化
void SoundRemindInit(void);

//填充提示音数据缓冲区，有decoder task解码时调用
//return -1 -- stream over; 0 -- no fill; other value -- filled bytes
int32_t SoundRemindFillStream(void);

#ifdef FUNC_SOUND_REMIND
//阻塞播放提示音
//注意：该函数必须在main task中调用，否则会有重入问题
void SoundRemind(uint16_t SoundId);
#else
#define SoundRemind(SoundId)	//如果无提示音功能，则将该函数定义为空，保防止编译出错
#endif

// 是否在播放提示音
bool IsSoundRemindPlaying(void);

// 停止播放提示音(设置提示音停止标志)
void StopSoundRemind(void);

// 重置提示音停止标志
void ResetSoundRemindStopFlag(void);

uint32_t SoundRemindFillStreamCallback(void *buffer, uint32_t length);

extern bool InterruptSoundRemind;

extern bool gVbEffectInited;

#ifdef __cplusplus
}
#endif//__cplusplus

#endif





