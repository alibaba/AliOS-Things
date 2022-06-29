
/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */
#ifndef IHAAS_VIDEO_CODEC_H
#define IHAAS_VIDEO_CODEC_H

#include "base/modules/core/include/HaasErrno.h"
#include "base/include/base/HaasCommonVideo.h"
#include "base/include/base/HaasCommonImage.h"
#include "base/include/base/HaasVideoCodecDef.h"

/**
* @brief Video Codec Control Class
* @author HaasAI Group
*/
class IHaasVideoCodec
{
public:
	virtual ~IHaasVideoCodec(){}

	/**
		* @brief		Open the video codec module
		* @note
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	IHaasVideoCodec * mHaasVideoCodec = NULL;
		*	mHaasVideoCodec = HaasVideoCodecInstance();
		*	res_ res = 0;
		*   res = mHaasVideoCodec->Open();
		* @endcode
		*/
	virtual int Open() = 0;
	/**
		* @brief		Close the video codec module
		* @note
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	IHaasVideoCodec * mHaasVideoCodec = NULL;
		*	mHaasVideoCodec = HaasVideoCodecInstance();
		*	res_ res = 0;
		*   res = mHaasVideoCodec->Close();
		* @endcode
		*/
	virtual int Close() = 0;

	/**
		* @brief		set Video decoder config information
		* @param[in]    config: obtain video decoder information
		* @note
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	IHaasVideoCodec * mHaasVideoCodec = NULL;
		*	mHaasVideoCodec = HaasVideoCodecInstance();
        *	DecodeConfig_t *config = xx;
		*	res_ res = 0;
		*   res = mHaasVideoCodec->DecodeConfig(config);
		* @endcode
		*/
	virtual int DecodeConfig(DecodeConfig_t* config) = 0;
	/**
		* @brief		start video codec
		* @note
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	IHaasVideoCodec * mHaasVideoCodec = NULL;
		*	mHaasVideoCodec = HaasVideoCodecInstance();
		*	res_ res = 0;
		*   res = mHaasVideoCodec->StartDecode();
		* @endcode
		*/
	virtual int StartDecode() = 0;
	/**
		* @brief		stop video codec
		* @note
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	IHaasVideoCodec * mHaasVideoCodec = NULL;
		*	mHaasVideoCodec = HaasVideoCodecInstance();
		*	res_ res = 0;
		*   res = mHaasVideoCodec->StopDecode();
		* @endcode
		*/
	virtual int StopDecode() = 0;
	/**
		* @brief		get video decode Image Data
		* @param[in]    data:obtain Video Image info
		* @note
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	IHaasVideoCodec * mHaasVideoCodec = NULL;
		*	mHaasVideoCodec = HaasVideoCodecInstance();
        *	ImageBuffer_t * image = NULL;
		*	res_ res = 0;
		*   res = mHaasVideoCodec->GetDecodeImageData(&image);
		* @endcode
		*/
	virtual int GetDecodeImageData(void *pkt, ImageBuffer_t** image) = 0;

	/**
		* @brief		set Video encode config information
		* @param[in]    config: obtain video encode information
		* @note
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	IHaasVideoCodec * mHaasVideoCodec = NULL;
		*	mHaasVideoCodec = HaasVideoCodecInstance();
        *	EncodeConfig_t *config = xx;
		*	res_ res = 0;
		*   res = mHaasVideoCodec->EncodeConfig(config);
		* @endcode
		*/
	virtual int EncodeConfig(EncodeConfig_t* config) = 0;
	/**
		* @brief		star video encode
		* @note
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	IHaasVideoCodec * mHaasVideoCodec = NULL;
		*	mHaasVideoCodec = HaasVideoCodecInstance();
		*	res_ res = 0;
		*   res = mHaasVideoCodec->StartEncode();
		* @endcode
		*/
	virtual int StartEncode() = 0;
	/**
		* @brief		stop video encode
		* @note
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	IHaasVideoCodec * mHaasVideoCodec = NULL;
		*	mHaasVideoCodec = HaasVideoCodecInstance();
		*	res_ res = 0;
		*   res = mHaasVideoCodec->StopEncode();
		* @endcode
		*/
	virtual int StopEncode() = 0;
	/**
		* @brief		get video encoder Data
		* @param[in]    data:obtain Video Packet info
		* @note
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	IHaasVideoCodec * mHaasVideoCodec = NULL;
		*	mHaasVideoCodec = HaasVideoCodecInstance();
        *	VideoPacket_t data;
		*	res_ res = 0;
		*   res = mHaasVideoCodec->GetEncodePacketData(&data);
		* @endcode
		*/
	virtual int GetEncodePacketData(VideoPacket_t **data) = 0;
};

extern "C" {
	/**
		* @brief		Get HaasVideoCodec Instance
		* @param[in]    type:VideoCodecType_t
		* @note	        NULL
		* @return
		*	 !0  successfully \n
		*	NULL failed \n
		* @par Sample
		* @code
		*	IHaasVideoCodec * mHaasVideoCodec = NULL;
		*	mHaasVideoCodec = IHaasVideoCodecInstance(type);
		* @endcode
		*/
IHaasVideoCodec* IHaasVideoCodecInstance(VideoCodecType_t type);
}
#endif
