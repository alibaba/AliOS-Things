/**
  * @file     	HaasDataInputDef.h
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
  *  along with HaasAI. If not, see <http://www.gnu.org/licenses/>.       	    \n
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
#ifndef HAAS_DATA_INPUT_DEF_H
#define HAAS_DATA_INPUT_DEF_H

#include "HaasCommonImage.h"

/**@enum CameraNumber_t
* @brief Camera Number \n
*/
typedef enum {
    DATA_INPUT_CAMERA_NUMBER_NONE          = -1,
    DATA_INPUT_CAMERA_NUMBER_FRONT         = 0,
    DATA_INPUT_CAMERA_NUMBER_BACK          = 1,
    DATA_INPUT_CAMERA_NUMBER_MAX           = 30,
} CameraNumber_t;

/**@enum DataInputType_t
* @brief Input Data Source Type \n
*/
typedef enum {
    DATA_INPUT_SOURCE_NONE          = -1,
    DATA_INPUT_SOURCE_IMAGE_BMP     = 0,
    DATA_INPUT_SOURCE_IMAGE_GIF     = 1,
    DATA_INPUT_SOURCE_IMAGE_PNG     = 2,
    DATA_INPUT_SOURCE_IMAGE_JPG     = 3,
    DATA_INPUT_SOURCE_VIDEO_FILE    = 4,
    DATA_INPUT_SOURCE_VIDEO_RTMP    = 5,
    DATA_INPUT_SOURCE_VIDEO_RTSP    = 6,
    DATA_INPUT_SOURCE_CAMERA_USB    = 7,
    DATA_INPUT_SOURCE_CAMERA_LOCAL  = 8,
    DATA_INPUT_SOURCE_CAMERA_IP     = 9,
    DATA_INPUT_SOURCE_MAX           = 30,
} DataInputType_t;
#endif
