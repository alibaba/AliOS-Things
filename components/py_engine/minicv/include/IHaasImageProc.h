
/**
  * @file     	IHaasImageProc.h
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
#ifndef IHAAS_IMAGE_PROC_H
#define IHAAS_IMAGE_PROC_H

#include "HaasCommonImage.h"
#include "HaasImageProcDef.h"

/**
* @brief image Proc class
* @author HaasAI Group
*/
class IHaasImageProc
{
    public:
        virtual ~IHaasImageProc(){}

	/**
		* @brief		Open image proc module
		* @note
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	IHaasImageProc * mIHaasImageProc = NULL;
		*	mIHaasImageProc = IHaasImageProcInstance();
		*	int res = 0;
		*   res = mIHaasImageProc->Open();
		* @endcode
		*/
        virtual int Open() = 0;
	/**
		* @brief		Close image proc module
		* @note
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	IHaasImageProc * mIHaasImageProc = NULL;
		*	mIHaasImageProc = IHaasImageProcInstance();
		*	int res = 0;
		*   res = mIHaasImageProc->Close();
		* @endcode
		*/
        virtual int Close() = 0;
	/**
		* @brief		convert the image format
		* @param[in]	src : source image
		* @param[in]	format : dest format
		* @param[out]	dst : dest image info
		* @note
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
        *	ImageBuffer_t *src = xxx;
        *	ImageBuffer_t *dst = NULL;
        *	PixelFmt_t format = xxx;
		*	IHaasImageProc * mIHaasImageProc = NULL;
		*	mIHaasImageProc = IHaasImageProcInstance();
		*	int res = 0;
		*   res = mIHaasImageProc->CvtColor(src, format, &dst);
		* @endcode
		*/
        virtual int CvtColor(const ImageBuffer_t* src, PixelFmt_t dst_format,
                ImageBuffer_t** dst) = 0;
	/**
		* @brief		resize the image size
		* @param[in]	src : source image
		* @param[in]	dst_size : dest size
		* @param[out]	dst : dest image info
		* @note
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
        *	ImageBuffer_t *src = xxx;
        *	ImageBuffer_t *dst = NULL;
        *	ImageSize_t size = xxx;
		*	IHaasImageProc * mIHaasImageProc = NULL;
		*	mIHaasImageProc = IHaasImageProcInstance();
		*	int res = 0;
		*   res = mIHaasImageProc->Resize(src, size, &dst);
		* @endcode
		*/
        virtual int Resize(const ImageBuffer_t* src, const ImageSize_t dst_size,
                ImageBuffer_t** dst) = 0;
	/**
		* @brief		copy the src img to tartge img
		* @param[in]	src : source image
		* @param[out]	dst : dest image info
		* @note
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
        *	ImageBuffer_t *src = xxx;
        *	ImageBuffer_t *dst = NULL;
		*	IHaasImageProc * mIHaasImageProc = NULL;
		*	mIHaasImageProc = IHaasImageProcInstance();
		*	int res = 0;
		*   res = mIHaasImageProc->ImgCopy(src, &dst);
		* @endcode
		*/
        virtual int ImgCopy(const ImageBuffer_t* src, ImageBuffer_t** dst) = 0;
	/**
		* @brief		draw a rectangle on tartge img
		* @param[in]	src : source image
		* @param[in]	left : rect left
		* @param[in]	top : rect top
		* @param[in]	right : rect right
		* @param[in]	bottom : rect bottom
		* @note
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
        *	ImageBuffer_t *src = xxx;
		*	IHaasImageProc * mIHaasImageProc = NULL;
		*	mIHaasImageProc = IHaasImageProcInstance();
		*	int res = 0;
        *	int32_t left = 10;
        *	int32_t top = 20;
        *	int32_t right = 50;
        *	int32_t bottom = 100;
		*   res = mIHaasImageProc->Rectangle(src, left, top, right, bottom);
		* @endcode
		*/
        virtual int Rectangle(const ImageBuffer_t* src, int32_t left, int32_t top,
                int32_t right, int32_t bottom) = 0;
	/**
		* @brief		draw a circle on tartge img
		* @param[in]	src : source image
		* @param[in]	center_x : circle center point x
		* @param[in]	center_y : circle center point r
		* @param[in]	radius : circle radius
		* @note
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
        *	ImageBuffer_t *src = xxx;
		*	IHaasImageProc * mIHaasImageProc = NULL;
		*	mIHaasImageProc = IHaasImageProcInstance();
		*	int res = 0;
        *	int32_t center_x = 10;
        *	int32_t center_y = 20;
        *	int32_t radius = 50;
		*   res = mIHaasImageProc->Circle(src, center_x, center_y, radius);
		* @endcode
		*/
        virtual int Circle(const ImageBuffer_t* src, int32_t center_x, int32_t center_y,
                int32_t radius) = 0;
	/**
		* @brief		draw a marker on tartge img
		* @param[in]	src : source image
		* @param[in]	pt : position of point
		* @param[in]	type : Marker type
		* @note
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
        *	ImageBuffer_t *src = xxx;
		*	IHaasImageProc * mIHaasImageProc = NULL;
		*	mIHaasImageProc = IHaasImageProcInstance();
		*	int res = 0;
        *	Point_t point;
        *	point.x = 10;
        *	point.y = 20;
        *	int32_t type = xx;
		*   res = mIHaasImageProc->DrawMarker(src, &point, type);
		* @endcode
		*/
        virtual int DrawMarker(const ImageBuffer_t* src, Point_t *pt, int32_t type) = 0;
	/**
		* @brief		Fill polly at target point
		* @param[in]	src : source image
		* @param[in]	pt : position of point
		* @param[in]	color : fill use color
		* @note
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
        *	ImageBuffer_t *src = xxx;
		*	IHaasImageProc * mIHaasImageProc = NULL;
		*	mIHaasImageProc = IHaasImageProcInstance();
		*	int res = 0;
        *	Point_t point;
        *	point.x = 10;
        *	point.y = 20;
        *	int32_t color = xx;
		*   res = mIHaasImageProc->FillPoly(src, &point, color);
		* @endcode
		*/
        virtual int FillPoly(const ImageBuffer_t* src, Point_t** pts, int32_t color) = 0;
	/**
		* @brief		Put Text at target image
		* @param[in]	src : source image
		* @param[in]	text : char*
		* @param[in]	left : text left position
		* @param[in]	top : text top position
		* @note
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
        *	ImageBuffer_t *src = xxx;
		*	IHaasImageProc * mIHaasImageProc = NULL;
		*	mIHaasImageProc = IHaasImageProcInstance();
		*	int res = 0;
        *	char* text = "for test"
        *	int32_t left = 10;
        *	int32_t top = 20;
		*   res = mIHaasImageProc->PutText(src, text, left, top);
		* @endcode
		*/
        virtual int PutText(const ImageBuffer_t* src, char* text, int32_t left,
                int32_t top) = 0;
	/**
		* @brief		Dilate Erode at target image
		* @param[in]	src : source image
		* @param[out]	dst : dst image
		* @param[in]	imode : set mode
		* @param[in]	iThresh : set iThresh
		* @note
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
        *	ImageBuffer_t *src = xxx;
        *	ImageBuffer_t dst;
		*	IHaasImageProc * mIHaasImageProc = NULL;
		*	mIHaasImageProc = IHaasImageProcInstance();
		*	int res = 0;
        *	int32_t iMode = xx;
        *	int32_t iThresh = xx;
		*   res = mIHaasImageProc->DilateErode(src, &dst, iMode, iThresh);
		* @endcode
		*/
        virtual int DilateErode(const ImageBuffer_t* src, ImageBuffer_t* dst,
                int32_t iMode, int32_t iThresh) = 0;
	/**
		* @brief		Add two image to target image
		* @param[in]	src : source image
		* @param[in]	added : added image
		* @param[out]	dst : dst image
		* @note
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
        *	ImageBuffer_t *src = xxx;
        *	ImageBuffer_t *added = yyy;
        *	ImageBuffer_t dst;
		*	IHaasImageProc * mIHaasImageProc = NULL;
		*	mIHaasImageProc = IHaasImageProcInstance();
		*	int res = 0;
		*   res = mIHaasImageProc->ImageAdd(src, added, &dst);
		* @endcode
		*/
        virtual void ImageAdd(const ImageBuffer_t* src, ImageBuffer_t* added,
                ImageBuffer_t* dst) = 0;
	/**
		* @brief		Minus two image to target image
		* @param[in]	src : source image
		* @param[in]	minused : minused image
		* @param[out]	dst : dst image
		* @note
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
        *	ImageBuffer_t *src = xxx;
        *	ImageBuffer_t *minused = yyy;
        *	ImageBuffer_t dst;
		*	IHaasImageProc * mIHaasImageProc = NULL;
		*	mIHaasImageProc = IHaasImageProcInstance();
		*	int res = 0;
		*   res = mIHaasImageProc->ImageMinus(src, minused, &dst);
		* @endcode
		*/
        virtual void ImageMinus(const ImageBuffer_t* src, ImageBuffer_t* minused,
                ImageBuffer_t* dst) = 0;
	/**
		* @brief		multiply two image to target image
		* @param[in]	src : source image
		* @param[in]	multiplied : added image
		* @param[out]	dst : dst image
		* @note
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
        *	ImageBuffer_t *src = xxx;
        *	ImageBuffer_t *multiplied = yyy;
        *	ImageBuffer_t dst;
		*	IHaasImageProc * mIHaasImageProc = NULL;
		*	mIHaasImageProc = IHaasImageProcInstance();
		*	int res = 0;
		*   res = mIHaasImageProc->ImageAdd(src, multiplied, &dst);
		* @endcode
		*/
        virtual void ImageElementMultiply(const ImageBuffer_t* src,
                ImageBuffer_t* multiplied, ImageBuffer_t* dst) = 0;
	/**
		* @brief		divie two image to target image
		* @param[in]	src : source image
		* @param[in]	divied : added image
		* @param[out]	dst : dst image
		* @note
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
        *	ImageBuffer_t *src = xxx;
        *	ImageBuffer_t *divied = yyy;
        *	ImageBuffer_t dst;
		*	IHaasImageProc * mIHaasImageProc = NULL;
		*	mIHaasImageProc = IHaasImageProcInstance();
		*	int res = 0;
		*   res = mIHaasImageProc->ImageElementDivide(src, divied, &dst);
		* @endcode
		*/
        virtual void ImageElementDivide(const ImageBuffer_t* src,
                ImageBuffer_t* divied, ImageBuffer_t* dst) = 0;
	/**
		* @brief		write source image to image file
		* @param[in]	src : source image
		* @param[in]	file_name : image store path
		* @note
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
        *	ImageBuffer_t *src = xxx;
        *	ImageBuffer_t dst;
		*	IHaasImageProc * mIHaasImageProc = NULL;
		*	mIHaasImageProc = IHaasImageProcInstance();
		*	int res = 0;
        *	std:string path = "/data/HaasAI/test.jpg";
		*   res = mIHaasImageProc->ImageAdd(src, path.c_str());
		* @endcode
		*/
        virtual int WriteImageToFile(const ImageBuffer_t* src, char* file_name) = 0;
};

extern "C" {
	/**
		* @brief		Get HaasImageProc Instance
		* @param[in]	type : ImageProcType_t
		* @note
		* @return
		*	 !0 Open successfully \n
		*	NULL Open failed \n
		* @par Sample
		* @code
		*	IHaasImageProc * mHaasImageProc = NULL;
		*	mHaasImageProc = IHaasImageProcInstance(type);
		* @endcode
		*/
    IHaasImageProc* IHaasImageProcInstance(ImageProcType_t type);
}

#endif
