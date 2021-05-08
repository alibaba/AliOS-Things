
/**
  * @file     	HaasCommonVideo.h
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
#ifndef HAAS_COMMON_VIDEO_H
#define HAAS_COMMON_VIDEO_H

#include "HaasCommonImage.h"

/**@enum VideoCodecID_t
* @brief Video Codec ID Value \n
*/
typedef enum {
    VIDEO_CODEC_ID_NONE,
    VIDEO_CODEC_ID_H261,
    VIDEO_CODEC_ID_H263,
    VIDEO_CODEC_ID_MJPEG,
    VIDEO_CODEC_ID_LJPEG,
    VIDEO_CODEC_ID_H264,
    VIDEO_CODEC_ID_H265,
} VideoCodecID_t;

/**@struct VideoPacket_t
* @brief Video Packet information \n
*/
typedef struct {
    int64_t     pts;
    int64_t     dts;
    uint8_t		*data;
    int         size;
    int64_t     duration;
    int64_t     pos;
} VideoPacket_t;

/**@struct DecodeConfig_t
* @brief Decode Config information \n
*/
typedef struct {
    int          type;
    int         format;
    ImageSize_t *size;
    VideoPacket_t *data;
} DecodeConfig_t;


/**@struct ImageBuffer_t
* @brief Encode Config information \n
*/
typedef struct {
    int          type;
    int         format;
    ImageSize_t *size;
    ImageBuffer_t *data;
} EncodeConfig_t;

#endif
