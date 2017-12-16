/*
*****************************************************************************
*					Mountain View Silicon Tech. Inc.
*	Copyright 2013, Mountain View Silicon Tech. Inc., ShangHai, China
*					All rights reserved.
*
* Filename:			BK1080.h
* Description:		BK1080 FM driver header file.
* maintainer Sam
******************************************************************************
*/
#ifndef __BK1080_H__
#define __BK1080_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


/*
**********************************************************
*					GLOBAL VARIABLE
**********************************************************
*/

#define XTAL_CLOCK
#define REG_RW_I2C_MODE

#define BK1080_CHIP_ADDR	0x80

//Chip tag
//#define	BK1080_TEMPREG
//搜台过程中的起点和终点频率
#define RADIO_LOWER_BOUND		875
#define RADIO_UPPER_BOUND		1080

#define DEVIATION_LIMIT         250 //100 = 0x64
#define BK1080_MAX_VOLUME			15
#define BK1080_MIN_VOLUME	0
#define BK1080_INIT_VOLUME	15

//强度大于23时候表示搜到电台
#define BK1080_DEFAULT_RSSI_VALUE    10//23 

// @Note: BK1080协议与IIC有区别，读数据前不需要发Start和器件地址.
// 			所以BEKEN_I2c_Read不能调用I2C_ReadNBytes函数.
//bool BEKEN_I2c_Read(uint8_t reg, uint8_t* pBuf, uint8_t len);


bool BEKEN_I2C_Write(uint8_t reg, uint8_t* pBuf, uint8_t len);
bool BEKEN_I2C_Read(uint8_t reg, uint8_t* pBuf, uint8_t len);
uint16_t BK1080FreqToChan(uint16_t frequency);
void BK1080Mute(bool MuteFg);
void  BK1080SetFreq(uint16_t Freq);
bool BK1080Init(void);
bool BK1080ReadID(void);
bool BK1080PowerDown(void);
bool BK1080PowerOn(void);
bool BK1080SearchRead(uint16_t Freq);
//uint8_t BK1080RSSIRead(void);
void BK1080VolSet(uint8_t vol);
void BK1080SearchSet(uint16_t Freq);
void BK1080OutputHIZ(bool HIZOnOff);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif



