/*
*****************************************************************************
*					Mountain View Silicon Tech. Inc.
*	Copyright 2009, Mountain View Silicon Tech. Inc., ShangHai, China
*					All rights reserved.
*
* Filename:
* Description:
* maintainer Sam
******************************************************************************
*/
#ifndef __KT830E_H__
#define __KT830E_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus



/*
**********************************************************
*					GLOBAL VARIABLE
**********************************************************
*/
#define INIT_FAIL_TH 	         30////3
#define CHANNEL_STEP              1 //10
#define AFCTH_PREV                  10
#define AFCTH                            15
#define AFCTH_NEXT                 10
#define TST_TH                         0x2800
#define SNR_TH                         0x30

#define MUTE_WITH_HIZ			    // Turn on only if audio outpus is required to be HiZ in mute state
//#define LONG_ANTENNA				// Turn on only if the length of antenna exceeds 2m
//#define SEPERATION_ENHANCE			// Turn on only if stereo seperation needs to be enhanced.
#define SEEK_WITH_SNR				// Turn on only if SNR double check is required in SEEK process

//#define IND_MATCH  95				// Unit is nH
//#define CEXT_MATCH 11000				// Unit if fF



bool KT_Bus_Write(uint8_t Register_Address, uint16_t Word_Data);
uint16_t KT_Bus_Read(uint8_t Register_Address);
void KT_SetRegBit(uint8_t regaddr, uint16_t bitMask, uint16_t dataval);
void KT_FMAntTuneOn(void);
void KT0830Mute(bool MuteFg);
void KT0830SetFreq(uint16_t Freq);
void KT0830Init(void);
bool KT0830ReadID(void);
void KT0830PowerOn(void);
void KT0830PowerDown(void);
//void KT0830SearchSet(uint16_t Freq);
int8_t KT0830EGetAFC(void);
uint8_t KT0830EFMGetSNR(void);
bool KTFMGetST(void);
uint8_t KT0830SearchRead(void);
void KT0830VolSet(uint8_t vol);
void KT0830SearchSet(uint16_t Freq);
void KT0830OutputHIZ(bool HIZOnOff);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif
