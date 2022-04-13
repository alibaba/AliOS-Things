/**
  * @file     	IHaasDataInput.h
  * @author   	HaasAI Group
  * @version	V1.0.0
  * @date    	2021-01-10
  * @license  	GNU General Public License (GPL)
  * @brief   	Data input, for example:image.video file,camera ...
  * @attention
  *  This file is part of HaasAI.                                \n
  *  This program is free software; you can redistribute it and/or modify 		\n
  *  it under the terms of the GNU General Public License version 3 as 		    \n
  *  published by the Free Software Foundation.                               	\n
  *  You should have received a copy of the GNU General Public License   		\n
  *  along with HaasAI.. If not, see <http://www.gnu.org/licenses/>.       			\n
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

#ifndef IHAAS_DATA_INPUT_H
#define IHAAS_DATA_INPUT_H

#include "HaasDataInputDef.h"
/**
* @brief All Data input Class
* @author HaasAI Group
*/
class IHaasDataInput
{
public:
	virtual ~IHaasDataInput(){}
	/**
		* @brief		Open the dataSouce
		* @param[in]	filename : The Name of DataSource
		* @note	Notice that the filename is the full path.
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	IHaasDataInput * mDataInput = NULL;
		*	mDataInput = IHaasDataInputInstance();
		*	char* filename = "/data/HaasAI/testvideo.jpg"
		*	res_ res = 0;
		*   res = mDataInput->Open(filename);
		* @endcode
		*/
	virtual int Open(char *filename) = 0;
	/**
		* @brief		Open the Camera
		* @param[in]	cameraNum : the number of Camera
		* @note	Notice that 0 = Front Camera;1 = Back Camera.
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	IHaasDataInput * mDataInput = NULL;
		*	mDataInput = IHaasDataInputInstance();
		*	int cameraindex = 0
		*	res_ res = 0;
		*   res = mDataInput->Open(cameraindex);
		* @endcode
		*/
	virtual int Open(int cameraNum) = 0;
	/**
		* @brief		Close the Camera
		* @note         NULL
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	IHaasDataInput * mDataInput = NULL;
		*	mDataInput = IHaasDataInputInstance();
		*	res_ res = 0;
		*   res = mDataInput->Close();
		* @endcode
		*/
	virtual int Close() = 0;
	/**
		* @brief		Check the data Ready state
		* @note         NULL
		* @return
		*	 0 Ready \n
		*	-1 Not Ready \n
		* @par Sample
		* @code
		*	IHaasDataInput * mDataInput = NULL;
		*	mDataInput = IHaasDataInputInstance();
		*	bool res = false;
		*   res = mDataInput->CheckDataReady();
		* @endcode
		*/
	virtual bool CheckDataReady() = 0;
	/**
		* @brief		Request Source Data
		* @param[in]	image : image Struct include data information
		* @param[in]	timeout : set the timoutout value
		* @note	        NULL
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	ImageBuffer_t *image = NULL;
		*	IHaasDataInput * mDataInput = NULL;
		*	mDataInput = IHaasDataInputInstance();
		*	res_ res = 0;
		*   res = mDataInput->RequestData(&image, 1000);
		* @endcode
		*/
	virtual int RequestData(ImageBuffer_t** image, int32_t timeout) = 0;
	/**
		* @brief		Release Source Data
		* @param[in]	image : image Struct include data information
		* @note	        NULL
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	IHaasDataInput * mDataInput = NULL;
		*	mDataInput = IHaasDataInputInstance();
		*	res_ res = 0;
		*   res = mDataInput->ReleaseData(image, 1000);
		* @endcode
		*/
	virtual int ReleaseData(ImageBuffer_t* image) = 0;

};

extern "C" {
	/**
		* @brief		Get HaasDataInput Instance
		* @note	        NULL
		* @return
		*	 !0  successfully \n
		*	NULL failed \n
		* @par Sample
		* @code
		*	IHaasDataInput * mDataInput = NULL;
		*	mDataInput = IHaasDataInputInstance();
		* @endcode
		*/
IHaasDataInput* IHaasDataInputInstance(DataInputType_t type);
}

#endif
