/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */
#ifndef IHAAS_IMAGE_CODEC_H
#define IHAAS_IMAGE_CODEC_H

#include "base/include/base/HaasCommonImage.h"
#include "base/include/base/HaasImageCodecDef.h"

/**
* @brief image Codec class
* @author HaasAI Group
*/
class IHaasImageCodec
{
public:
	virtual ~IHaasImageCodec(){}

	/**
		* @brief		Read A Image from Path
		* @param[out]	image : store the image infomation
		* @param[in]	filename : the path of image
		* @note	        filename is the image full path.
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	IHaasImageCodec * mIHaasImageCodec = NULL;
        *	ImageBuffer_t image;
		*	char* path = "/data/HaasAI/test.jpg";
		*	mIHaasImageCodec = IHaasImageCodecInstance();
		*	int res = 0;
		*   res = mIHaasImageCodec->ImgRead(&image, path.c_str());
		* @endcode
		*/
	virtual int ImgRead(ImageBuffer_t **image, char * filename) = 0;
	/**
		* @brief		Read Multi Image from Path
		* @param[out]	images : store the images infomation
		* @param[in]	filename : the path of image
		* @note	        filename is the image full path.
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	IHaasImageCodec * mIHaasImageCodec = NULL;
        *	ImageBuffer_t *image = NULL;
		*	char* path = "/data/HaasAI";
		*	mIHaasImageCodec = IHaasImageCodecInstance();
		*	int res = 0;
		*   res = mIHaasImageCodec->ImgReadMulti(&image, path.c_str());
		* @endcode
		*/
	virtual int ImgReadMulti(ImageBuffer_t **images, char * filename) = 0;
	/**
		* @brief		ImageBuffer_t A Image to Path
		* @param[in]	image : store the image infomation
		* @param[in]	filename : the path of image
		* @note	        filename is the image full path.
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	IHaasImageCodec * mIHaasImageCodec = NULL;
        *	ImageBuffer_t *image = xx;
		*	char* path = "/data/HaasAI/test.jpg";
		*	mIHaasImageCodec = IHaasImageCodecInstance();
		*	int res = 0;
		*   res = mIHaasImageCodec->ImgWrite(image, path.c_str());
		* @endcode
		*/
	virtual int ImgWrite(ImageBuffer_t *image, char * filename) = 0;
	/**
		* @brief		Write Images to Path
		* @param[in]	image : store the image infomation
		* @param[in]	filename : the path of image
		* @note	        filename is the image full path.
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	IHaasImageCodec * mIHaasImageCodec = NULL;
        *	ImageBuffer_t **image = xx;
		*	char* path = "/data/HaasAI";
		*	mIHaasImageCodec = IHaasImageCodecInstance();
		*	int res = 0;
		*   res = mIHaasImageCodec->ImgWriteMulti(image, path.c_str());
		* @endcode
		*/
	virtual int ImgWriteMulti(ImageBuffer_t **images, char * filename) = 0;
	/**
		* @brief		Decode image pixels to image Struct
		* @param[in]	addr : image pixels address
		* @param[out]	image : store the image infomation
		* @note	        filename is the image full path.
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	IHaasImageCodec * mIHaasImageCodec = NULL;
        *	ImageBuffer_t image;
        *	void *addr = xx;
		*	mIHaasImageCodec = IHaasImageCodecInstance();
		*	int res = 0;
		*   res = mIHaasImageCodec->ImgWriteMulti(addr, &image);
		* @endcode
		*/
	virtual int ImgDecode(void *addr, ImageBuffer_t **image) = 0;
	/**
		* @brief		Decode image to image Struct
		* @param[in]	filename : image path
		* @note	        filename is the image full path.
		* @return
		*	 !0 successfully \n
		*	NULL failed \n
		* @par Sample
		* @code
		*	IHaasImageCodec * mIHaasImageCodec = NULL;
		*	char* path = "/data/HaasAI/test.jpg";
        *	ImageBuffer_t *image = NULL;
		*	mIHaasImageCodec = IHaasImageCodecInstance();
		*   image = mIHaasImageCodec->ImgWriteMulti(path.c_str());
		* @endcode
		*/
    virtual ImageBuffer_t * ImgDecode(const char * filename) = 0;
	/**
		* @brief		Encode image to image Struct
		* @param[in]	addr : image pixels address
		* @param[out]	filename : image path
		* @note	        filename is the image full path.
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	IHaasImageCodec * mIHaasImageCodec = NULL;
        *	ImageBuffer_t image;
        *	void *addr = NULL;
		*	mIHaasImageCodec = IHaasImageCodecInstance();
		*   image = mIHaasImageCodec->ImgEncode(addr, &image);
		* @endcode
		*/
	virtual int ImgEncode(void *addr, ImageBuffer_t ** image) = 0;
	/**
		* @brief		check if has the encoder
		* @param[in]	filename : image path
		* @note	        filename is the image full path.
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	IHaasImageCodec * mIHaasImageCodec = NULL;
		*	mIHaasImageCodec = IHaasImageCodecInstance();
		*	char* path = "/data/HaasAI/test.jpg";
		*	int res = -1;
		*   res = mIHaasImageCodec->haveImageReader(path.c_str());
		* @endcode
		*/
	virtual int haveImageReader(char * filename) = 0;
	/**
		* @brief		check if has the decoder
		* @param[in]	filename : image path
		* @note	        filename is the image full path.
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	IHaasImageCodec * mIHaasImageCodec = NULL;
		*	mIHaasImageCodec = IHaasImageCodecInstance();
		*	char* path = "/data/HaasAI/test.jpg";
		*	int res = -1;
		*   res = mIHaasImageCodec->haveImageWriter(path.c_str());
		* @endcode
		*/
	virtual int haveImageWriter(char * filename) = 0;
};

extern "C" {
	/**
		* @brief		Get HaasImageCodec Instance
		* @param[in]	type : CodecImageType_t
		* @note
		* @return
		*	 !0 Open successfully \n
		*	NULL Open failed \n
		* @par Sample
		* @code
		*	IHaasImageCodec * mHaasImageCodec = NULL;
		*	mHaasImageCodec = IHaasImageCodecInstance();
		* @endcode
		*/
IHaasImageCodec* IHaasImageCodecInstance(CodecImageType_t type);
}

#endif
