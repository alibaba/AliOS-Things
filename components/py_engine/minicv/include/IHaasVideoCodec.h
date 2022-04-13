
/**
  * @file     	IHaasVideoCodec.h
  * @author   	HaasAI Group
  * @version	V1.0.0
  * @date    	2021-01-10
  * @license  	GNU General Public License (GPL)
  * @brief
  * @attention
  *  This file is part of HaasAI.                                \n
  *  This program is free software; you can redistribute it and/or modify 		\n
  *  it under the terms of the GNU General Public License version 3 as 		    \n
  *  published by the Free Software Foundation.                               	\n
  *  You should have received a copy of the GNU General Public License   		\n
  *  along with HaasAI. If not, see <http://www.gnu.org/licenses/>.       			\n
  *  Unless required by applicable law or agreed to in writing, software       	\n
  *  distributed under the License is distributed on an "AS IS" BASIS,         	\n
  *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  	\n
  *  See the License for the specific language governing permissions and     	\n
  *  limitations under the License.   											\n
  *   																			\n
  * @htmlonly
  * <span style="font-weight: bold">History</span>
  * @endhtmlonly
  * Version|Author|Date|Describe
  * ------|----|------|--------
  * V1.0|HaasAI Group|2021-01-10|Create File
  * <h2><center>&copy;COPYRIGHT 2021 WELLCASA All Rights Reserved.</center></h2>
  */
#ifndef IHAAS_VIDEO_CODEC_H
#define IHAAS_VIDEO_CODEC_H

#include "HaasErrno.h"
#include "HaasCommonVideo.h"
#include "HaasCommonImage.h"
#include "HaasVideoCodecDef.h"

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
