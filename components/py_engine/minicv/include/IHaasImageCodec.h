
/**
  * @file     	IHaasImageCodec.h
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
#ifndef IHAAS_IMAGE_CODEC_H
#define IHAAS_IMAGE_CODEC_H

#include "HaasCommonImage.h"
#include "HaasImageCodecDef.h"

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
