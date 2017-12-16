/**
 *****************************************************************************
 * @file     device_audio.h
 * @author   Orson
 * @version  V1.0.0
 * @date     24-June-2013
 * @brief    device audio module driver interface
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#ifndef __DEVICE_AUDIO_H__
#define	__DEVICE_AUDIO_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


#define	USB_DAC_FREQ				44100			//USB声卡SPEAKER默认采样率
#define	USB_MIC_FREQ				44100			//USB声卡MIC默认采样率

//应用层可在USB声卡模式初始化时修改下面两个全局变量
//客户在开发调试时请注意：
//如果先以44KHZ采样率连接电脑，然后再次以48KHZ(或其他)采样率连接同一台电脑，电脑会无法播放出声音。
//如果遇到这种情况，有两种解决办法：
//1. 可以在电脑设备管理器中手工卸载该USB设备，然后重新插入该设备。
//2. 可以将程序中的gDevicePID修改为其他值。
extern uint32_t gDeviceSpeakerSampleRate;			//USB声卡SPEAKER采样率
extern uint32_t gDeviceMicSampleRate;				//USB声卡MIC采样率


extern uint16_t  gDeviceVID;
extern uint16_t  gDevicePID;
extern uint8_t* gDeviceString_Manu;					//max length: 32bytes
extern uint8_t* gDeviceString_Product;				//max length: 32bytes
extern uint8_t* gDeviceString_SerialNumber;			//max length: 32bytes

//USB声卡音量
extern uint16_t AudioLeftVolume;
extern uint16_t AudioRightVolume;

extern FPCALLBACK gFpDeviceAudioMuteEn;
extern FPCALLBACK gFpDeviceAudioMuteDis;
extern FPCALLBACK gFpDeviceAudioSetVolume;


//声卡Mic音量控制接口
extern uint16_t AudioMicLeftVolume;
extern uint16_t AudioMicRightVolume;
extern bool     AudioMicMuteFlag;

extern FPCALLBACK gFpDeviceAudioMicMuteEn;
extern FPCALLBACK gFpDeviceAudioMicMuteDis;
extern FPCALLBACK gFpDeviceAudioMicSetVolume;

//pc volume: 0 --- 999
#define AUDIO_MAX_VOLUME	999


//pc command
#define PC_CMD_NEXT			0x04
#define PC_CMD_PRE			0x08
#define PC_CMD_MUTE			0x10
#define PC_CMD_PLAY_PAUSE	0x40
#define PC_CMD_STOP			0x80


/**
 * @brief  USB声卡模式初始化
 * @param  NONE
 * @return NONE
 */
void DeviceAudioInit(void);

/**
 * @brief  USB声卡模式下，底层命令处理的总入口
 * @param  NONE
 * @return NONE
 */
void DeviceAudioProcess(void);

/**
 * @brief  USB声卡模式下，发送反向控制命令
 * @param  Cmd 反向控制命令
 * @return 1-成功，0-失败
 */
bool DeviceAudioSendPcCmd(uint8_t Cmd);

//设置USB设备工作模式
#define USB_DEVICE_AUDIO			1	//PC识别为单独的声卡
#define USB_DEVICE_READER			2	//PC识别为单独的读卡器
#define USB_DEVICE_AUDIO_READER		3	//PC同时识别出声卡和读卡器
/**
 * @brief  设置USB设备工作模式
 * @param  DeviceMode 工作模式
 * @param            USB_DEVICE_AUDIO: PC识别为单独的声卡
 * @param            USB_DEVICE_READER: PC识别为单独的读卡器
 * @param            USB_DEVICE_AUDIO_READER: PC同时识别出声卡和读卡器
 * @return NONE
 */
void DeviceAudioSetMode(uint8_t DeviceMode);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif
