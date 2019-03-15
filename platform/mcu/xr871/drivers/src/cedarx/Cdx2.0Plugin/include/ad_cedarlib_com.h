#ifndef	__BsInfoType__
#define	__BsInfoType__
#include "adecoder.h"

typedef struct __AFRAME_INFO
{
    unsigned char    *startAddr;
    unsigned int    ptsValid;//0:锟斤拷前时锟斤拷锟斤拷锟叫э拷锟�锟斤拷锟斤拷前时锟斤拷锟斤拷锟叫�
    int64_t         pts;
    unsigned int    len;
    unsigned int    readlen;
}aframe_info_t;

typedef struct __ASTREAM_FIFO
{
    aframe_info_t 	*inFrames;
    unsigned int    rdIdx;//
    unsigned int    wtIdx;//
    int             ValidchunkCnt;//锟斤拷效锟斤拷packet锟斤拷目//0:锟斤拷前帧没锟斤拷锟斤拷荩锟�:锟斤拷前帧锟斤拷锟斤拷锟�    unsigned int    maxchunkNum;
    int64_t         NowPTSTime;
    int64_t         dLastValidPTS;
    unsigned int 	maxchunkNum;
}astream_fifo_t;


typedef struct __Ac320FileRead
{
    unsigned int    FileLength;             //锟侥硷拷锟杰筹拷锟斤拷
    unsigned int    FileReadingOpsition;    //锟侥硷拷锟铰达拷要锟斤拷锟斤拷位锟矫碉拷偏锟斤拷
    unsigned int    FileWritingOpsition;    //buffer锟铰达拷要写锟斤拷位锟矫碉拷偏锟斤拷
    unsigned int    need_offset;
    unsigned short  updataoffsetflag;
    unsigned short  BigENDFlag;             //1 big 0 little
    unsigned char   *bufStart;              //buffer锟阶碉拷址
    int             BufToTalLen;            //锟杰筹拷锟斤拷

    unsigned char   *bufReadingPtr;         //锟斤拷锟节讹拷锟斤拷指锟斤拷
    int             BufLen;                 //锟斤拷效锟斤拷莩锟斤拷锟�
    unsigned char   *bufWritingPtr;         //锟斤拷锟节讹拷锟斤拷指锟斤拷
    int             BufValideLen;           //锟斤拷锟洁长锟斤拷
    //unsigned int    ReadingOffset;
    astream_fifo_t 	frmFifo;
    //add for android
    void*           tmpGlobalAudioDecData;

}Ac320FileRead;

#define	AUD_TAG_INF_SIZE 512
typedef  struct com_internal_struct
{
    //0x88
    unsigned int             *pdecInfoSet;
    unsigned int             *pBackupBufferPoint;
    unsigned int             ulBitstreamAddPtr;
    unsigned int             ulDspVersionNum;
    unsigned int             ulPlayStartTime;
    unsigned int             ulPlayEndTime;
    unsigned int             ulBreakSentenceEnergy;
    unsigned int             ulBreakSentenceTime;
    unsigned int             ulFileSumData;
    unsigned int             ulForwardBackSilenceTime;
    unsigned int             ulForwardBackPlayTime;
    unsigned int             *SptrumoutAdd;
    unsigned int             *UserEqinputAdd;

    unsigned short           ulBackupLength;

    unsigned char            ulNormalPlay;               //=ulFadeInOutFinish
    unsigned char            ulDecComm;
    unsigned char            ulFadeinFlag;
    //0x8b
    unsigned char            ulFadeInOutFinish;
    //0x89
    unsigned int             ulTotalTimeMS;

    unsigned int             ulAverageBit;
    int /*unsigned short*/   ulSampleRate;               //?why not this data is fact data;
    //0x8a
    unsigned char            ulExitMean;
    unsigned int             ulDebugInfo;
    //0x60
    unsigned int             ulNowTimeMS;
    unsigned int             ulNowTimeMSAPoint;
    unsigned int             ulBitrate;
    unsigned char            ulMode;
    unsigned char            ulFFREVDoing;
    //0x70
    signed char              ulVpsSet;
    unsigned int/*char*/     ulFFREVStep;
    unsigned char            ulFFREVFlag;
    unsigned char            ulABSet;
    unsigned char            ulEQFlag;
    unsigned short           ulUserEq[10];
    unsigned char            ulspectrum;
    unsigned short           Hmspetrum;//1-8
//    unsigned short           ulspectrumval[8][32];	//reducted for saving memory
    unsigned char            ulChannelControl;
    unsigned char            ulVolumeSet;
    //0x61 a sentence end
    //0x62 dsp mips limit
    //0x63 dsp alive minimum time
    //0x40 decoder need new bitstream to decode
    unsigned int             ulReadBitstreamLen;
    int                      ulSkipPageLength;
    unsigned char            urFadeOutFlag;
    unsigned char            urFileFlag;
    unsigned int             urTrueReadLen;
    unsigned int             ulFilecurrentpage;
    //0x41
    unsigned char            ulIndex;
    unsigned int             ulDestinationAdd;
    //0x42
    unsigned char            ulIndex1;
    unsigned int             ulDestinationAdd1;
    //0x43
    unsigned int             bWithVideo;
    unsigned int             ulDestinationAdd2;
    //0x21
    unsigned int             *ulBufferAddress;
    unsigned int             ulBufferLength;

    unsigned short           NeedSetPlayTimeFlag;
    unsigned int             framecount;

 //   unsigned char            AudTagInfo[AUD_TAG_INF_SIZE];	//reducted for saving memory

} com_internal;

typedef struct __AudioDEC_AC320
{
    Ac320FileRead *FileReadInfo;
    BsInFor       *BsInformation;
    com_internal  *DecoderCommand;
    int           Decinitedflag;
    int           (*DecInit)(struct __AudioDEC_AC320 *p);
    int           (*DecFrame)(struct __AudioDEC_AC320 *p,char *OutBuffer,int *OutBuffLen);
    int           (*DecExit)(struct __AudioDEC_AC320 *p);

}AudioDEC_AC320;


#endif
