
/**
  * @file     	IHaasUI.h
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
#ifndef IHAAS_UI_H
#define IHAAS_UI_H

#include "string.h"
#include "HaasCommonImage.h"
#include "HaasUIDef.h"

/**
* @brief Haas UI Control Class
* @author HaasAI Group
*/
class IHaasUI
{
public:
	virtual ~IHaasUI(){}

	/**
		* @brief		Get display device width
		* @note
		* @return
		*	 !0 successfully \n
		*	 -1 failed \n
		* @par Sample
		* @code
		*	IHaasUI * mHaasUI = NULL;
		*	mHaasUI = IHaasUIInstance();
		*	res_ res = 0;
		*   res = mHaasUI->GetDisplayWidth();
		* @endcode
		*/
	virtual int GetDisplayWidth() = 0;
	/**
		* @brief		Get display device height
		* @note
		* @return
		*	 !0 successfully \n
		*	 -1 failed \n
		* @par Sample
		* @code
		*	IHaasUI * mHaasUI = NULL;
		*	mHaasUI = IHaasUIInstance();
		*	res_ res = 0;
		*   res = mHaasUI->GetDisplayHeight();
		* @endcode
		*/
	virtual int GetDisplayHeight() = 0;

	/**
		* @brief		Get display device backlight value
		* @note
		* @return
		*	 !0 successfully \n
		*	 -1 failed \n
		* @par Sample
		* @code
		*	IHaasUI * mHaasUI = NULL;
		*	mHaasUI = IHaasUIInstance();
		*	res_ res = 0;
		*   res = mHaasUI->GetDisplayBacklight();
		* @endcode
		*/
	virtual int GetDisplayBacklight() = 0;
	/**
		* @brief		Set display device backlight value
		* @param[in]    value:backlight value
		* @note
		* @return
		*	 !0 successfully \n
		*	 -1 failed \n
		* @par Sample
		* @code
		*	IHaasUI * mHaasUI = NULL;
		*	mHaasUI = IHaasUIInstance();
		*	int res = 0;
        *	int32_t value = xx;
		*   res = mHaasUI->SetDisplayBacklight(value);
		* @endcode
		*/
	virtual int SetDisplayBacklight(int32_t value) = 0;

	/**
		* @brief		Get display device type
		* @note
		* @return
		*	 !0 successfully \n
		*	 -1 failed \n
		* @par Sample
		* @code
		*	IHaasUI * mHaasUI = NULL;
		*	mHaasUI = IHaasUIInstance();
		*	res_ res = 0;
		*   res = mHaasUI->GetDisplayType();
		* @endcode
		*/
	virtual int GetDisplayType() = 0;

	/**
		* @brief		Get display device freq
		* @note
		* @return
		*	 !0 successfully \n
		*	 -1 failed \n
		* @par Sample
		* @code
		*	IHaasUI * mHaasUI = NULL;
		*	mHaasUI = IHaasUIInstance();
		*	res_ res = 0;
		*   res = mHaasUI->GetDisplayFreq();
		* @endcode
		*/
	virtual int GetDisplayFreq() = 0;
	/**
		* @brief		Set display device freq
		* @param[in]    freq:display device freq
		* @note
		* @return
		*	 !0 successfully \n
		*	 -1 failed \n
		* @par Sample
		* @code
		*	IHaasUI * mHaasUI = NULL;
		*	mHaasUI = IHaasUIInstance();
		*	int res = 0;
        *	int32_t freq = xx;
		*   res = mHaasUI->SetDisplayFreq(freq);
		* @endcode
		*/
	virtual int SetDisplayFreq(int32_t freq) = 0;

	/**
		* @brief		Draw a point on screen
		* @param[in]    pt: the point position
		* @param[in]    color: point color
		* @note
		* @return
		*	 !0 successfully \n
		*	 -1 failed \n
		* @par Sample
		* @code
		*	IHaasUI * mHaasUI = NULL;
		*	mHaasUI = IHaasUIInstance();
		*	int res = 0;
        *   Point_t point;
        *   point.x = 10;
        *   point.y = 20;
        *	int32_t color = xx;
		*   res = mHaasUI->DrawPoint(&point, color);
		* @endcode
		*/
    virtual int DrawPoint(Point_t *pt, int32_t color) = 0;
	/**
		* @brief		Draw a line on screen
		* @param[in]    start: start point position
		* @param[in]    end: stop point position
		* @param[in]    color: point color
		* @note
		* @return
		*	 !0 successfully \n
		*	 -1 failed \n
		* @par Sample
		* @code
		*	IHaasUI * mHaasUI = NULL;
		*	mHaasUI = IHaasUIInstance();
		*	int res = 0;
        *   Point_t start;
        *   start.x = 10;
        *   start.y = 20;
        *   Point_t end;
        *   end.x = 10;
        *   end.y = 20;
        *	int32_t color = xx;
		*   res = mHaasUI->DrawLine(&start, &end, color);
		* @endcode
		*/
    virtual int DrawLine(Point_t *start, Point_t *end, int32_t color) = 0;
	/**
		* @brief		Draw a Rectangle on screen
		* @param[in]    left: Rectangle left
		* @param[in]    top: Rectangle top
		* @param[in]    right: Rectangle right
		* @param[in]    bottom: Rectangle bottom
		* @param[in]    color: point color
		* @note
		* @return
		*	 !0 successfully \n
		*	 -1 failed \n
		* @par Sample
		* @code
		*	IHaasUI * mHaasUI = NULL;
		*	mHaasUI = IHaasUIInstance();
		*	int res = 0;
        *	int32_t left = 10;
        *	int32_t top = 10;
        *	int32_t right = 50;
        *	int32_t bottom = 100;
        *	int32_t color = xx;
		*   res = mHaasUI->DrawRectangle(left, top, right, bottom, color);
		* @endcode
		*/
    virtual int DrawRectangle(int32_t left, int32_t top, int32_t right, int32_t bottom,
            int32_t color) = 0;
	/**
		* @brief		Draw a circle on screen
		* @param[in]    center: circle point
		* @param[in]    radius: circle radius
		* @param[in]    color: point color
		* @note
		* @return
		*	 !0 successfully \n
		*	 -1 failed \n
		* @par Sample
		* @code
		*	IHaasUI * mHaasUI = NULL;
		*	mHaasUI = IHaasUIInstance();
		*	int res = 0;
        *	Point_t point;
        *	point.x = 10;
        *	point.y = 50;
        *	int32_t radius = 100;
        *	int32_t color = xx;
		*   res = mHaasUI->DrawCircle(&point, radius, color);
		* @endcode
		*/
    virtual int DrawCircle(Point_t * center, int32_t radius, int32_t color) = 0;
	/**
		* @brief		Draw a String on screen
		* @param[in]    text: String value
		* @param[in]    size: String size
		* @param[in]    color: point color
		* @param[in]    left: Rectangle left
		* @param[in]    top: Rectangle top
		* @param[in]    right: Rectangle right
		* @param[in]    bottom: Rectangle bottom
		* @note
		* @return
		*	 !0 successfully \n
		*	 -1 failed \n
		* @par Sample
		* @code
		*	IHaasUI * mHaasUI = NULL;
		*	mHaasUI = IHaasUIInstance();
        *	char* text = "for test"
        *	int32_t size = 10;
        *	int32_t color = xx;
		*	int res = 0;
        *	int32_t left = 10;
        *	int32_t top = 10;
        *	int32_t right = 50;
        *	int32_t bottom = 100;
		*   res = mHaasUI->DrawText(text, size, color, left, top, right, bottom);
		* @endcode
		*/
    virtual void DrawText(char* text, int32_t size, int32_t color, int32_t left,
            int32_t top, int32_t right, int32_t bottom) = 0;
	/**
		* @brief		Draw Pixel on screen
		* @param[in]    pixles: data address
		* @param[in]    pt: pixles draw position
		* @param[in]    width: draw width
		* @param[in]    height: draw height
		* @note
		* @return
		*	 !0 successfully \n
		*	 -1 failed \n
		* @par Sample
		* @code
		*	IHaasUI * mHaasUI = NULL;
		*	mHaasUI = IHaasUIInstance();
		*	int res = 0;
        *	int32_t* pixels = address;
        *	Point_t point;
        *	point.x = 10;
        *	point.y = 50;
        *	int32_t width = 100;
        *	int32_t height = 200;
		*   res = mHaasUI->DrawCircle(pixels, &point, width, heigt);
		* @endcode
		*/
    virtual bool DrawPixels(int32_t* pixels, Point_t *pt, int32_t width, int32_t height) = 0;
	/**
		* @brief		Draw a image on screen
		* @param[in]    path: image path
		* @param[in]    pt: pixles draw position
		* @param[in]    width: draw width
		* @param[in]    height: draw height
		* @note
		* @return
		*	 !0 successfully \n
		*	 -1 failed \n
		* @par Sample
		* @code
		*	IHaasUI * mHaasUI = NULL;
		*	mHaasUI = IHaasUIInstance();
		*	int res = 0;
        *	char* path = "/data/HaasAI/test.jpg";
        *	Point_t point;
        *	point.x = 10;
        *	point.y = 50;
        *	int32_t width = 100;
        *	int32_t height = 200;
		*   res = mHaasUI->DrawCircle(path, &point, width, heigt);
		* @endcode
		*/
    virtual bool DrawImage(char * path, Point_t *pt, int32_t width, int32_t height) = 0;

	/**
		* @brief		update Displat Content
		* @note
		* @return
		*	 always successfully \n
		* @par Sample
		* @code
		*	IHaasUI * mHaasUI = NULL;
		*	mHaasUI = IHaasUIInstance();
		*	res_ res = 0;
		*   res = mHaasUI->UpdateDisplay();
		* @endcode
		*/
	virtual void UpdateDisplay() = 0;
};

extern "C" {
	/**
		* @brief		Get HaasUI Instance
		* @param[in]    type: UIFrameworkType_t
		* @note	        NULL
		* @return
		*	 !0  successfully \n
		*	NULL failed \n
		* @par Sample
		* @code
		*	IHaasUI * mHaasUI = NULL;
		*	mHaasUI = IHaasUIInstance();
		* @endcode
		*/
IHaasUI* IHaasUIInstance(UIFrameworkType_t type);
}
#endif
