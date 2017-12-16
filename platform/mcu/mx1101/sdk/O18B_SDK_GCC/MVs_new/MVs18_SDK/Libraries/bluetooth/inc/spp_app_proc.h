//  maintainer: Halley
//spp_app_proc.h

#ifndef __SPP_APP_PROC_H__
#define __SPP_APP_PROC_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


#define SLAVE_NEXT_MODE				0X0101
#define SLAVE_SELECT_MODE			0X0102
#define SLAVE_GET_MODE              0X0103
#define SLAVE_GET_DEVICE_LINK       0X0104
#define SLAVE_SELECT_MODE_PENDING	0X0105
#define SLAVE_GET_VALID_FOLDER_SUM  0X0201
#define SLAVE_GET_FILE_SUM          0X0202
#define SLAVE_GET_FOLDER_INFO       0X0203
#define SLAVE_GET_FILE_INFO         0X0204
#define SLAVE_PLAY                  0X0301
#define SLAVE_PAUSE                 0X0302
#define SLAVE_STOP                  0X0303
#define SLAVE_NEXT                  0X0304
#define SLAVE_PRE                   0X0305
#define SLAVE_PLAY_PAUSE            0X0306
#define SLAVE_FF                    0X0307
#define SLAVE_FB                    0X0308
#define SLAVE_STOP_FF_FB            0X0309
#define SLAVE_SEEK_PLAY_TIME        0X030A
#define SLAVE_NEXT_FOLDER        	0X030B
#define SLAVE_PRE_FOLDER        	0X030C
#define SLAVE_SELECT_SONG        	0X030D
#define SLAVE_GET_PLAY_STATUS       0X0320
#define SLAVE_GET_SONG_INFO         0X0321
#define SLAVE_SET_REPEAT_MODE       0X0322
#define SLAVE_GET_REPEAT_MODE       0X0323
#define SLAVE_ENABLE_FOLDER         0X0324
#define SLAVE_DISABLE_FOLDER        0X0325
#define SLAVE_GET_TITLE             0X0330
#define SLAVE_GET_ARTIST            0X0331
#define SLAVE_GET_ALBUM             0X0332
#define SLAVE_GET_COMMENT           0X0333
#define SLAVE_GET_YEAR              0X0334
#define SLAVE_VOLADD                0X0401
#define SLAVE_VOLSUB                0X0402
#define SLAVE_SET_VOL               0X0403
#define SLAVE_GET_VOL               0X0404
#define SLAVE_SET_EQ                0X0405
#define SLAVE_GET_EQ                0X0406
#define SLAVE_MUTE                  0X0407
#define SLAVE_UNMUTE                0X0408
//#define SLAVE_SET_LINEIN_CHANNEL    0X0409
#define SLAVE_SET_LINEIN_GAIN       0X0409
#define SLAVE_ENABLE_MIC            0X040A
#define SLAVE_DISABLE_MIC           0X040B
#define SLAVE_SET_TIME              0X0501
#define SLAVE_GET_TIME              0X0502
#define SLAVE_SET_ALARM_TIME        0X0503
#define SLAVE_GET_ALARM_TIME        0X0504
#define SLAVE_ENABLE_ALARM			0x0505
#define SLAVE_DISABLE_ALARM			0x0506
#define SLAVE_GET_ALARM_STATUS      0X0507
#define SLAVE_GET_ALARM_FLAG		0x0508
#define SLAVE_GET_MONTH_DAYS        0X0510
#define SLAVE_SOLAR_TO_LUNAR        0X0511
#define SLAVE_GET_HEAVENLY_STEM     0X0512
#define SLAVE_GET_EARTHLY_BRANCH    0X0513
#define SLAVE_SET_GPIO_REG          0X0601
#define SLAVE_GET_GPIO_REG          0X0602
#define SLAVE_SET_GPIO_REG_BIT      0X0603
#define SLAVE_CLR_GPIO_REG_BIT      0X0604
#define SLAVE_ENABLE_ADC_CHANNEL    0X0701
#define SLAVE_DISABLE_ADC_CHANNEL   0X0702
#define SLAVE_GET_ADC_CHANNEL_VALUE 0X0703
#define SLAVE_GET_VIN_VOLTAGE       0X0704
#define SLAVE_PWM_CONFIG_CHANNEL    0X0801
#define SLAVE_ENABLE_PWM_CHANNEL    0X0802
#define SLAVE_DISABLE_PWM_CHANNEL   0X0803
#define SLAVE_SET_WAKEUP_MODE       0X0901
#define SLAVE_GOTO_DEEP_SLEEP       0X0902
#define SLAVE_SET_RADIO_PARAM        0X0A01
#define SLAVE_SET_RADIO_FREQ        0X0A02
#define SLAVE_NEXT_STEP        0X0A03
#define SLAVE_PREV_STEP        0X0A04
#define SLAVE_NEXT_STATION        0X0A05
#define SLAVE_PREV_STATION        0X0A06
#define SLAVE_AUTO_SCAN        0X0A07
#define SLAVE_NEXT_SEEK        0X0A08
#define SLAVE_PREV_SEEK        0X0A09
#define SLAVE_SAVE_STATION        0X0A0A
#define SLAVE_SET_MONO_MODE        0X0A0B
#define SLAVE_GET_CUR_STATUS       0X0A0C
#define SLAVE_GET_STORED_STATION      0X0A0D

#define SLAVE_ENABLE_VIRTUAL_BASS   0X7001
#define SLAVE_DISABLE_VIRTUAL_BASS  0X7002






#define BYTE0(val)			(((uint8_t*)(&val))[0])
#define BYTE1(val)			(((uint8_t*)(&val))[1])
#define BYTE2(val)			(((uint8_t*)(&val))[2])
#define BYTE3(val)			(((uint8_t*)(&val))[3])

#define WORD0(val)			(((uint16_t*)(&val))[0])
#define WORD1(val)			(((uint16_t*)(&val))[1])



//////////////////////////////////////////////////
/*Size = -1 means flush all*/
void  SppFlushRecBuf(int16_t Size);        //清除指定大小接收缓冲区数据，-1表示清除全部
uint16_t  SppSendData(uint8_t* Buf, uint16_t Size); //发送数据，最大当前可发送数据大小为SppGetSendBufFreeSize()
int16_t SppGetSendBufFreeSize(void);       //获取当前可发送数据量大小
uint16_t  SppReadData(uint8_t* Buf, uint16_t Len);  //读取接收缓存区指定数量数据，returns actually read data size
uint16_t  SppGetRecBufDataSize(void);        //获取当前接收缓冲区数据量大小

int32_t SppGetWRingFreeSpace(void);
#define SppGetSendBufFreeSize() SppGetWRingFreeSpace()


/////////////////////////////////////////////////////////////////////////////
//  Following four work parameters(2-uint8_t* and 2-uint16_t*) are defined in stack
//uint8_t *
uint8_t *SppGetWorkParameterByte1(void);
uint8_t *SppGetWorkParameterByte2(void);
#define SppGetWorkParAppFuncID   SppGetWorkParameterByte1
#define SppGetWorkParProcStatus  SppGetWorkParameterByte2
//uint16_t *
uint16_t *SppGetWorkParameterWord1(void);
uint16_t *SppGetWorkParameterWord2(void);
#define SppGetWorkParWorkBufFillSize SppGetWorkParameterWord1
#define SppGetWorkParAppWantDataSize SppGetWorkParameterWord2

/////////////////////////////////////////////////////////////////////////////
//  Following one work buffer are defined in stack
uint8_t *SppGetWorkBuf(void);  //work buffer pointer
uint16_t SppGetWorkBufLen(void);//work buffer size(capability)

#ifdef __cplusplus
}
#endif//__cplusplus

#endif

