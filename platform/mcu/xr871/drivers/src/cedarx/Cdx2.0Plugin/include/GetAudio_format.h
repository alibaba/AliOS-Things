/******************************************************************************
* file:GetAudio_format.h 2008-12-24 10:47:12
*
*author lszhang
*
*
*
*******************************************************************************/
#ifndef _GETAUFIO_FORMAT_H_
#define _GETAUFIO_FORMAT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
typedef enum __AW_AUDIO_FORMAT
{
    SW_AUDIO_FORMAT_UNKNOWN = 0,            /* ï¿½Þ·ï¿½Ê¶ï¿½ï¿½ */
    
    SW_AUDIO_FORMAT_AAC,
    SW_AUDIO_FORMAT_AC3,
    SW_AUDIO_FORMAT_APE,
    SW_AUDIO_FORMAT_DTS,
    SW_AUDIO_FORMAT_FLAC,
    SW_AUDIO_FORMAT_MP3,
    SW_AUDIO_FORMAT_OGG,
    SW_AUDIO_FORMAT_RA,
    SW_AUDIO_FORMAT_WAV,    
    SW_AUDIO_FORMAT_WMA,
    SW_AUDIO_FORMAT_AMR,
    SW_AUDIO_FORMAT_ATRC,
    SW_AUDIO_FORMAT_MID,
        
    SW_AUDIO_FORMAT_ = -1
    
} __sw_audio_format_e;

typedef enum
{
    IMG_FORMAT_BMP =0,
    IMG_FORMAT_JPG,
    IMG_FORMAT_GIF,
    IMG_FORMAT_PNG,
    IMG_FORMAT_UNSUPPORT = -1
}__w_img_format_t;

typedef enum __A_AUDIO_FONTTYPE
{
    A_AUDIO_FONTTYPE_ISOIEC8859_1  = 0,       //ISO/IEC 8859-1
    A_AUDIO_FONTTYPE_UTF_16LE,//
    A_AUDIO_FONTTYPE_UTF_16BE,
    A_AUDIO_FONTTYPE_UTF_8,//
    A_AUDIO_FONTTYPE_ISOIEC8859_2,//
    A_AUDIO_FONTTYPE_ISOIEC8859_3,//
    A_AUDIO_FONTTYPE_ISOIEC8859_4,//
    A_AUDIO_FONTTYPE_ISOIEC8859_5,//
    A_AUDIO_FONTTYPE_ISOIEC8859_6,
    A_AUDIO_FONTTYPE_ISOIEC8859_7,
    A_AUDIO_FONTTYPE_ISOIEC8859_8,
    A_AUDIO_FONTTYPE_ISOIEC8859_9,
    A_AUDIO_FONTTYPE_ISOIEC8859_10,
    A_AUDIO_FONTTYPE_ISOIEC8859_11,
    A_AUDIO_FONTTYPE_ISOIEC8859_12,
    A_AUDIO_FONTTYPE_ISOIEC8859_13,
    A_AUDIO_FONTTYPE_ISOIEC8859_14,
    A_AUDIO_FONTTYPE_ISOIEC8859_15,
    A_AUDIO_FONTTYPE_ISOIEC8859_16,
    A_AUDIO_FONTTYPE_WINDOWS_1250,
    A_AUDIO_FONTTYPE_WINDOWS_1251,//
    A_AUDIO_FONTTYPE_WINDOWS_1252,
    A_AUDIO_FONTTYPE_WINDOWS_1253,
    A_AUDIO_FONTTYPE_WINDOWS_1254,
    A_AUDIO_FONTTYPE_WINDOWS_1255,
    A_AUDIO_FONTTYPE_WINDOWS_1256,
    A_AUDIO_FONTTYPE_WINDOWS_1257,
    A_AUDIO_FONTTYPE_WINDOWS_1258,
    A_AUDIO_FONTTYPE_KOI8_R,
    A_AUDIO_FONTTYPE_KOI8_U,
    A_AUDIO_FONTTYPE_GB2312,
    A_AUDIO_FONTTYPE_GBK,
    A_AUDIO_FONTTYPE_BIG5,
    
    
    A_AUDIO_FONTTYPE_ = -1
}__a_audio_fonttype_e;

typedef struct __ID3_IMAGE_INFO
{
    int               length;         //ï¿½ï¿½Ý³ï¿½ï¿½ï¿½
    int               FileLocation;   //ï¿½Ä¼ï¿½Æ«ï¿½ï¿½Î»ï¿½ï¿½
    __w_img_format_t  img_format;     //Í¼Æ¬ï¿½ï¿½Ê½
    int               pic_type;       //picture type;
    int               img_high;       //Ô¤ï¿½ï¿½ï¿½ï¿½Í¼Æ¬ï¿½ß¶ï¿½
    int               img_width;      //Ô¤ï¿½ï¿½ï¿½ï¿½Í¼Æ¬ï¿½ï¿½ï¿½
    int               otherdata;      //Ô¤ï¿½ï¿½

}__id3_image_info_t;


typedef struct __AUDIO_FILE_INFO
{
    unsigned int                    ulSampleRate;   // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ sample rate
    unsigned int                    ulBitRate;      // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ bit rateï¿½ï¿½Î»ï¿½ï¿½BPS
    unsigned int                    ulChannels;     // ï¿½ï¿½ï¿½ï¿½ï¿½ channel
    unsigned int                    ulDuration;     // ï¿½ï¿½ï¿½ï¿½Ê±ï¿½ï¿½ duration ï¿½ï¿½Î»ï¿½ï¿½ms
    unsigned int                    ulBitsSample;   // ï¿½ï¿½ï¿½ï¿½ï¿½Î»ï¿½ï¿½ sample 8/16/24/32
    unsigned int                    ulCharEncode;   // 0:GB2312.1:UNICODE
    
    int                             ulAudio_name_sz;        // ï¿½ï¿½Æµï¿½ï¿½Ê½Ëµï¿½ï¿½
    signed char                     *ulAudio_name;          // mp3 /RealAudio Cook.sipo. / aac-lc....
    __a_audio_fonttype_e            ulAudio_nameCharEncode; // 
    
    int                             ulGenre_sz;             // ï¿½ï¿½ï¿½ï¿½
    signed char                     *ulGenre;               // pop soft...
    __a_audio_fonttype_e            ulGenreCharEncode;
    
    int                             ultitle_sz;             // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
    signed char                     *ultitle;
    __a_audio_fonttype_e            ultitleCharEncode;
    
    int                             ulauthor_sz;            // ï¿½Ý³ï¿½ï¿½ï¿½
    signed char                     *ulauthor;
    __a_audio_fonttype_e            ulauthorCharEncode;
    
    __sw_audio_format_e             ulFormat;
    int                             ulAlbum_sz;             // ×¨ï¿½ï¿½
    signed char                     *ulAlbum;
    __a_audio_fonttype_e            ulAlbumCharEncode;
    
    int                             ulYear_sz;              // ï¿½ï¿½Æ·ï¿½ï¿½ï¿½
    signed char                     *ulYear;
    __a_audio_fonttype_e            ulYearCharEncode;
    
    int                             ulAPic_sz;             // attached picture
    __id3_image_info_t              *ulAPic;
    __a_audio_fonttype_e            ulAPicCharEncode;
    
    int                             ulUslt_sz;             // ï¿½ï¿½Í¬ï¿½ï¿½ï¿½Ä¸ï¿½ï¿½/ï¿½Ä±ï¿½ ï¿½ï¿½ï¿½ï¿½
    signed char*                    ulUslt;                // int ulFileLocationï¿½ï¿½
    __a_audio_fonttype_e            ulUsltCharEncode;
    
    int                             ulSylt_sz;             // Í¬ï¿½ï¿½ï¿½Ä¸ï¿½ï¿½/ï¿½Ä±ï¿½
    signed char*                    ulSylt;                // int ulFileLocationï¿½ï¿½
    __a_audio_fonttype_e            ulSyltCharEncode;
    
    int                             ulbufflag;				//0:ï¿½ï¿½ï¿½Ä¼ï¿½ï¿½ï¿½ï¿½Ð·ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½bufï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ý½ï¿½ï¿½Ð·ï¿½ï¿½ï¿½
    int                             ulbuf_length;			//buf length;
    char*                           ulbuf;					//ï¿½ï¿½ï¿½ï¿½bufï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½.
    int                             data[3];	
    int64_t                         offset;		 //audio data ÔÚÎÄ¼þÖÐµÄÆ«ÒÆÁ¿£¬id3Ò²ÊôÓÚaudio dataÕâÈý¸ö±äÁ¿Ôö¼ÓÊÇÎªÁË½âÂëÒ»¸öÎÄ¼þÖÐÄ³¸ö²¿·ÖÊÇaudioÓÃ
    int64_t                         length;		 //audio data lenth£»ÓÐÐ§audioÊý¾ÝµÄ³¤¶È¡£¿ÉÒÔ¸³Öµ0£¬ÒâË¼ÊÇÕû¸ö¶¼ÊÇÓÐÐ§Êý¾Ý
    int64_t                         readlen;		 //µ±Ç°¶ÁÈ¡¶àÉÙ¸öÓÐÐ§µÄaudio Êý¾Ý£¬Ïàµ±ÓÚÎÄ¼þµÄÆ«ÒÆÎ»ÖÃ¼õÈ¥audio dataÔÚÎÄ¼þÖÐµÄÆ«ÒÆÁ¿¡£
    
    signed char                     *ulBSINFO;              // temporary buffer of read data
    int                             InforBufLeftLength;
    signed char                     *InforBuf;              // ï¿½ï¿½ï¿½Ú´æ´¢ ulAudio_name  ultitle ulauthorï¿½ï¿½ï¿½ï¿½Ï¢ï¿½ï¿½buffer
    
}audio_file_info_t;


int GetAudioFormat(const char *pFilename,int *A_Audio_Format);//return 1 :succed 0 :fail
int GetAudioInfo(const char *pFilename, audio_file_info_t *AIF);  //return 1 :succed 0 :fail
/*
function :GetAudioFormatData
ËµÃ÷£º¸ù¾ÝÎÄ¼þÇ°¼¸¸ö×Ö½Ú¿ìËÙÅÐ¶ÏÎÄ¼þÀàÐÍ¡£
²ÎÊýËµÃ÷£ºptr £ºÎÄ¼þÊý¾ÝÊ×µØÖ·¡£
			buflen£ºÊý¾Ý³¤¶È¡£
			A_Audio_Format£º·µ»ØÎÄ¼þÎÄ¼þ¸ñÊ½
·µ»ØÖµ£º1 :succed 0 :fail
*/
//simple format from first 16bytes
int GetAudioFormatData(unsigned char *ptr,int buflen,int *A_Audio_Format);//return 1 :succed 0 :fail

int GetAudioDataInfo(const char *pFilename, audio_file_info_t *AIF,signed char* buf,int datalen);  //return 1 :succed 0 :fail
int GetAudioFileInfo(FILE *Bitstream, audio_file_info_t *AIF);  //return 1 :succed 0 :fail
int GetAudioFileInfoOffset(FILE *Bitstream,int64_t offset,int64_t length,audio_file_info_t *AIF);  //return 1 :succed 0 :fail 
#ifdef __cplusplus
}
#endif

#endif //_GETAUFIO_FORMAT_H_

