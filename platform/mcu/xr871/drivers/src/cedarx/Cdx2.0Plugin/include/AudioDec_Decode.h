#ifndef AUDIODEC_DECODE_H
#define AUDIODEC_DECODE_H
#include <pthread.h>
#include "unistd.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define cdx_mutex_lock(x)	pthread_mutex_lock(x)
#define cdx_mutex_unlock(x)  pthread_mutex_unlock(x)

typedef void AudioDecoderLib;

typedef struct CedarAudioCodec 
{
    const char *name;
    struct __AudioDEC_AC320 *(*init)(void);
    int	 (*exit)(struct __AudioDEC_AC320 *p);
    int   flag;
} CedarAudioCodec;


typedef struct InitCodecInfo 
{
	const char *handle;
    const char *name;
    const char *init;
    const char *exit;
    int   flag;
    struct __AudioDEC_AC320 *(*decInit)(void);
    int	 (*decExit)(struct __AudioDEC_AC320 *p);
} InitCodecInfo;

typedef struct RaFormatInofStruct
{
    unsigned int    ulSampleRate;
    unsigned int    ulActualRate;
    unsigned short  usBitsPerSample;
    unsigned short  usNumChannels;
    unsigned short  usAudioQuality;
    unsigned short  usFlavorIndex;
    unsigned int    ulBitsPerFrame;
    unsigned int    ulGranularity;
    unsigned int    ulOpaqueDataSize;
    unsigned char*  pOpaqueData;
} RaFormatInfoT;
/*********************************************************************************/

typedef enum INPUTBSFILLMODE
{
    BSFILL_MODE_UNKONE = 0,
    BSFILL_MODE_NORMAL,
    BSFILL_MODE_HDRWRAP,
    BSFILL_MODE_BSWRAP,
}INPUTBSFILLMODE;

typedef struct CedarInputBsManage
{
    INPUTBSFILLMODE mode;
    unsigned char   *pHoloStartAddr0;
    int             nHoloLen0;
    unsigned char   *pHoloStartAddr1;
    int             nHoloLen1;
}CedarInputBsManage;

typedef struct InputStreamBuf
{
    CedarInputBsManage InputBsManage;
    int                CedarAbsPackHdrLen;
    unsigned char      CedarAbsPackHdr[16];
}InputStreamBufT;

typedef struct AudioDecoderContextStructLib
{
    pthread_mutex_t   mutex_audiodec_thread;
    int               nAudioInfoFlags;
    InputStreamBufT   Streambuffer;    
    Ac320FileRead     DecFileInfo;
    com_internal      pInternal;
    
    CedarAudioCodec   *pCedarCodec;
	void              *libhandle;
    AudioDEC_AC320    *pCedarAudioDec;
#ifdef CEDARX_SUPPORT_SOUNDTOUCH
	SoundTouchAPI     *pSTouchAPI;
	STPrivData*       STpriv_data;
#endif
	const char        *handlename;
}AudioDecoderContextLib;


int ParseRequestAudioBitstreamBuffer(AudioDecoderLib* pDecoder,
               int              nRequireSize,
               unsigned char**  ppBuf,
               int*             pBufSize,
               unsigned char**  ppRingBuf,
               int*             pRingBufSize,
               int*             nOffset);
int ParseUpdateAudioBitstreamData(AudioDecoderLib* pDecoder,
               int     nFilledLen,
               int64_t nTimeStamp,
               int     nOffset);
int ParseAudioStreamDataSize(AudioDecoderLib* pDecoder);
void BitstreamQueryQuality(AudioDecoderLib* pDecoder, int* pValidPercent, int* vbv);
void ParseBitstreamSeekSync(AudioDecoderLib* pDecoder, int64_t nSeekTime, int nGetAudioInfoFlag);

int InitializeAudioDecodeLib(AudioDecoderLib*    pDecoder,
               AudioStreamInfo* pAudioStreamInfo,
               BsInFor *pBsInFor);
int DecodeAudioFrame(AudioDecoderLib* pDecoder,
               char*        ppBuf,
               int*          pBufSize);
int DestroyAudioDecodeLib(AudioDecoderLib* pDecoder);

void SetAudiolibRawParam(AudioDecoderLib* pDecoder, int commond);

AudioDecoderLib* CreateAudioDecodeLib(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif//AUDIODEC_DECODE_H
