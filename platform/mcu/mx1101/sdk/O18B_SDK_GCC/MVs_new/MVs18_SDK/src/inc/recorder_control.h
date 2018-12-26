/**
 *******************************************************************************
 * @file    record_control.h
 * @author  Alven
 * @version V1.0.0
 * @date    30-10-2013
 * @brief   record ctrl
 * @maintainer lilu
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


#ifndef __RECORDER_CONTROL_H__
#define __RECORDER_CONTROL_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "folder.h"
#include "fat_file.h"
#include "Host_stor.h"

#define RECORD_SOURCE_LINEIN  0
#define RECORD_SOURCE_FM         1

//#define WAV_FORMAT_SUPPORT
#define MP3_FORMAT_SUPPORT

#define SD_CARD                 (2)
#define USB_DISK                (1)

#define WAV_FORMAT              (0)
#define MP3_FORMAT              (1)

#define MAX_REC_FILE_NUM		200		 //suuport record max file num 

typedef struct _RECORD_CONTROL
{
	FOLDER      ParentFolder;
	FOLDER      RecordFolder;
	FAT_FILE    RecordFile;
	uint8_t        RecordFileName[11];
	uint16_t        RecordFileNum;
	uint8_t        RecordFileMap[MAX_REC_FILE_NUM];
	uint8_t        RecordFileSum;						//
	uint8_t        RecordDeviceID;
	uint32_t       RecordFileSize;
	uint8_t		EncoderType;
	uint16_t	EncoderSampleSize;//替代原宏定义ENCODER_SAMPLES_SIZE
} RECORD_CONTROL;



typedef enum _RECORD_ERR_CODE
{
    OPEN_FOLDER_ERR = -255,
    CREATE_FOLDER_ERR,
    CREATE_FILE_ERR,
    OPEN_FILE_ERR,
    WRITE_FILE_ERR,
    FILE_SEEK_ERR,
    DELETE_FILE_ERR,
    FLUSH_FILE_ERR,
    MP3_ENCODER_INIT_ERR,
	WAV_ENCODER_INIT_ERR,
    RECORD_NONE_ERR = 0,

} RECORD_ERR_CODE;
	
typedef enum _RECORD_STATE
{
    RECORD_STATE_INIT,
    RECORD_STATE_BROWSER,
    RECORD_STATE_REC,
    RECORD_STATE_REC_PAUSE,
    RECORD_STATE_PLAYINIT,
    RECORD_STATE_PLAYBACK,
    RECORD_STATE_PLAYBACK_PAUSE
} RECORD_STATE;

extern uint8_t     RecordState;
bool    RecorderControl(void);
bool     IsRecorderEnvironmentOk(void);

bool RecorderInit(uint8_t EncoderType);
void RecoderExit(uint8_t SongType);
void RecordPause(uint8_t SongType);
void RecordResume(uint8_t SongType);
bool EnterRecBackPlay(uint32_t ParentModuleId, DEV_ID DevId, bool IsNeedInitFs, uint16_t FolderId, uint16_t FileId);
bool RecBackPlayControl(void);

void MixerForRecoder(int16_t* PcmBuf1, int16_t* PcmBuf2, uint16_t SampleCnt, uint32_t PcmDataMode);

#ifdef BT_HF_RECORD_FUNC
void InitBtHfRecordBuffer(void);
bool EncodedDataToBuffer(uint8_t* EncodedData, uint32_t EncodedDataLen);
void EncodedDataBufferToFile(uint32_t Len);
#endif

extern uint32_t EncodedBufDataLen;

extern uint8_t RecordState;

#ifdef __cplusplus
}
#endif//__cplusplus

#endif

