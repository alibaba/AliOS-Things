
/**
  * @file     	HaasImageUtils.h
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
#ifndef HAAS_IMAGE_UTILS_H
#define HAAS_IMAGE_UTILS_H

#include <stdint.h>
#include "HaasCommonImage.h"

class ImageUtils {
public:
	static uint32_t GetSize(PixelFmt_t format, uint32_t width, uint32_t height);
};

#endif
