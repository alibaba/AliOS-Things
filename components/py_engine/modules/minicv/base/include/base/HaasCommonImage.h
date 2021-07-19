
/**
  * @file     	HaasCommonImage.h
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
#ifndef HAAS_COMMON_IMAGE_H
#define HAAS_COMMON_IMAGE_H

//#include <cstdint>
#include <stdint.h>
#include <stdlib.h>
//#include <string>
#include <string.h>

/**@enum PixelFmt_t
* @brief Pixel Format Value \n
*/
typedef enum {
    PIXEL_FMT_NONE    = -1,
    PIXEL_FMT_YUV420P = 0,
    PIXEL_FMT_NV21    = 1,
    PIXEL_FMT_NV12    = 2,
    PIXEL_FMT_YUYV422 = 3,
    PIXEL_FMT_YUV422P = 4,
    PIXEL_FMT_RGB24   = 5,
    PIXEL_FMT_BGR24   = 6,
    PIXEL_FMT_ARGB    = 7,
    PIXEL_FMT_RGBA    = 8,
    PIXEL_FMT_ABGR    = 9,
    PIXEL_FMT_BGRA    = 10,
    PIXEL_FMT_RGB565  = 11,
} PixelFmt_t;

/**@struct ImagePos_t
* @brief image position \n
*/
typedef struct {
    uint32_t	x;
    uint32_t	y;
} ImagePos_t;

/**@struct Point_t
* @brief Point information \n
*/
typedef struct {
    uint32_t	x;
    uint32_t	y;
} Point_t;

/**@struct ImageSize_t
* @brief Image size infomation \n
*/
typedef struct {
    uint32_t	width; ///< image size width
    uint32_t	height; ///< image size height
} ImageSize_t;

/**@struct ImageBuffer_t
* @brief Image buffer information \n
*/
typedef struct {
    uint32_t		stride;
    uint32_t		width;
    uint32_t		height;
    uint32_t		size;
    PixelFmt_t	    format;
    uint32_t	    numPlane;
    uint8_t*		address[3];
    uint32_t		linesize[3];
} ImageBuffer_t;

#endif
