/*
*****************************************************************************
*					Mountain View Silicon Tech. Inc.
*	Copyright 2013, Mountain View Silicon Tech. Inc., ShangHai, China
*					All rights reserved.
*
* Filename:			pcm_sync.h
* Description:		PCM MODULE DRIVER FOR BLUETOOTH
*
*  maintainer: Aissen
*
* Change History:
*        Justin	- 05/30/2013 - V0.1
*					- created
******************************************************************************/

#ifndef __PCM_SYNC_H__
#define __PCM_SYNC_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#define MV_MCU_AS_PCMSYNC_CLK_MASTER  0
#define MV_MCU_AS_PCMSYNC_CLK_SLAVE   1

//
// AecStatus
#define PCMSYNC_AEC_NONE           0//:disable phone and mv aec;
#define PCMSYNC_AEC_MV             1//:disable phone ace,enable mv ace;
#define PCMSYNC_AEC_PHONE          2//:enable phone aec, disable mv aec
#define PCMSYNC_AEC_PHONE_MV       3//:enable phone ace,enable mv ace;
//
//Pcm Sync API functions prototype.
//

//
//Initialize I2s data in/out¡¢adc
//
void PcmSyncInit(uint8_t I2sClkType);

//
//Process Pcm Data
//
void PcmSyncDataProcess(void);

//
//Uninitialize I2s data in/out¡¢adc
//
void PcmSyncUnInit(void);

//
//Get PcmSyncInit Flag
//
bool PcmSyncGetInitFlag(void);

//
//Set  AecStatus
//
void PcmSyncAecSetStatus(uint8_t Status);

//
//Get  AecStatus
//
uint8_t PcmSyncAecGetStatus(void);


#ifdef BT_HF_RECORD_FUNC
void InitBtHfRecordPcm(void);
uint32_t GetAvailablePcmSyncDataSize(void);
uint32_t GetAvailablePcmBufSize(void);
bool WriteDataToPcmBuffer(void);
uint32_t ReadDataFromPcmBuffer(uint16_t* RecvBuf, uint32_t ReadLen);
#endif

#ifdef __cplusplus
}
#endif//__cplusplus

#endif//__PCM_SYNC_H__
