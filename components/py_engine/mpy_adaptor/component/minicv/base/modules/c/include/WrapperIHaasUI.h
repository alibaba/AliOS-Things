
/**
  * @file     	WrapperIHaasUI.h
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
#ifndef WRAPPER_IHAAS_UI_H
#define WRAPPER_IHAAS_UI_H

#include "HaasCommonImage.h"
#include "HaasUIDef.h"
#ifdef __cplusplus
extern "C" {
#endif

    void* UICreateInstance(UIFrameworkType_t type);
    void UIDestoryInstance(void* instance);
	int UIGetDisplayWidth(void* instance);
	int UIGetDisplayHeight(void* instance);
	int UIGetDisplayBacklight(void* instance);
	int UISetDisplayBacklight(void* instance, int32_t value);
	int UIGetDisplayType(void* instance);
	int UIGetDisplayFreq(void* instance);
	int UISetDisplayFreq(void* instance, int32_t freq);
    int UIDrawPoint(void* instance, Point_t *pt, int32_t color);
    int UIDrawLine(void* instance, Point_t *start, Point_t *end, int32_t color);
    int UIDrawRectangle(void* instance, int32_t left, int32_t top, int32_t right, int32_t bottom,
            int32_t color);
    int UIDrawCircle(void* instance, Point_t * center, int32_t radius, int32_t color);
    void UIDrawText(void* instance, char* text, int32_t size, int32_t color, int32_t left,
            int32_t top, int32_t right, int32_t bottom);
    bool UIDrawPixels(void* instance, int32_t* pixels, Point_t *pt, int32_t width, int32_t height);
    bool UIDrawImage(void* instance, char * path, Point_t *pt, int32_t width, int32_t height);
	void UIUpdateDisplay(void* instance);

#ifdef __cplusplus
};
#endif

#endif
