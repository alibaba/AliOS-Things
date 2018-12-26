#ifndef XRECORD_H
#define XRECORD_H

#include "CdxKeyedVector.h"
#include "CaptureControl.h"


#define AUDIO_INPUT (1)
#define VIDEO_INPUT (0)

typedef struct RecoderContext XRecord;

typedef enum XRECODER_AUDIO_ENCODE_TYPE
{
    XRECODER_AUDIO_ENCODE_AMR_TYPE,
	XRECODER_AUDIO_ENCODE_PCM_TYPE,
} XRECODER_AUDIO_ENCODE_TYPE;

typedef enum
{
    XRECODER_STATE_INIT,
    XRECODER_STATE_PREPARED,
    XRECODER_STATE_STARTED,
    XRECODER_STATE_STOPED,
    XRECODER_STATE_DESTROYED,
} XRECODER_STATE;

XRecord* XRecordCreate();

int XRecordSetDataDstUrl(XRecord* p,
                            const char* pUrl,
                            void* arg,
                            const CdxKeyedVectorT* pHeaders);

int XRecordDestroy(XRecord* p);

void XRecordSetAudioCap(XRecord* p, const CaptureCtrl* audioSrc);

int XRecordSetAudioEncodeType(XRecord* p, XRECODER_AUDIO_ENCODE_TYPE type, AudioEncodeConfig *config);

int XRecordPrepare(XRecord* p);

int XRecordStart(XRecord* p);

int XRecordStop(XRecord* p);

#endif /* XRECORD_H */