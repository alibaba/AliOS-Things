
/**
  * @file     	IHaasML.h
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
#ifndef IHAAS_ML_H
#define IHAAS_ML_H
#include "HaasCommonImage.h"
#include "HaasMLDef.h"

/**
* @brief Machine Learning Control Class
* @author HaasAI Group
*/
class IHaasML
{
public:
	virtual ~IHaasML(){}

	/**
		* @brief        config http info(for ucloud AI)
		* @param[in]	key : OSS Access Key
		* @param[in]	secret : OSS Access secret
		* @param[in]	endpoint : The region id
		* @param[in]	bucket : OSS endpoint
		* @param[in]	url : for facebody compare AI model
		* @note.
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	IHaasML * mIHaasML = NULL;
		*	mIHaasML = IHaasMLInstance();
		*	res_ res = 0;
		*   res = mIHaasML->Config(NULL,NULL,NULL,NULL,NULL);
		* @endcode
		*/
	virtual int Config(char *key, char *secret, char *endpoint,
            char *bucket, char *url) {}
	/**
		* @brief		set input data(come frome HaasDataInput)
		* @param[in]	dataPath : The path of DataSource
		* @note	Notice that the dataPath is the full path.
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	IHaasML * mIHaasML = NULL;
		*	mIHaasML = IHaasMLInstance();
		*	char* dataPath = "/data/HaasAI/xxx"
		*	res_ res = 0;
		*   res = mIHaasML->SetInputData(dataPath.c_str());
		* @endcode
		*/
	virtual int SetInputData(const char* dataPath) = 0;
	/**
		* @brief		Load AI mode object
		* @param[in]	modePath : The path of AI Mode
		* @note	Notice that the modePath is the full path.
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	IHaasML * mIHaasML = NULL;
		*	mIHaasML = IHaasMLInstance();
		*	char* modePath = "/data/HaasAI/aimode"
		*	res_ res = 0;
		*   res = mIHaasML->LoadNet(modePath.c_str());
		* @endcode
		*/
	virtual int LoadNet(const char* modePath) = 0;
	/**
		* @brief		Start AI Predict
		* @note
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	IHaasML * mIHaasML = NULL;
		*	mIHaasML = IHaasMLInstance();
		*	res_ res = 0;
		*   res = mIHaasML->Predict();
		* @endcode
		*/
	virtual int Predict() = 0;
	/**
		* @brief		Get AI Predict Result
		* @param[out]	outResult : The result of Predict
		* @param[int]	len : The len of readBuffer
		* @note
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	IHaasML * mIHaasML = NULL;
		*	mIHaasML = IHaasMLInstance();
		*	int res = 0;
		*   res = mIHaasML->getTrainResponses();
		* @endcode
		*/
	virtual int GetPredictResponses(char* outResult, int len) = 0;
	/**
		* @brief		un load AI Predict mode obj
		* @note
		* @return
		*	 0 successfully \n
		*	-1 failed \n
		* @par Sample
		* @code
		*	IHaasML * mIHaasML = NULL;
		*	mIHaasML = IHaasMLInstance();
		*	res_ res = 0;
		*   res = mIHaasML->UnLoadNet();
		* @endcode
		*/
	virtual int UnLoadNet() = 0;
};

extern "C" {
	/**
		* @brief		Get HaasML Instance
		* @note	        NULL
		* @return
		*	 !0  successfully \n
		*	NULL failed \n
		* @par Sample
		* @code
		*	IHaasML * mHaasML = NULL;
		*	mHaasML = IHaasMLInstance();
		* @endcode
		*/
IHaasML* IHaasMLInstance(MLEngineType_t type);
}

#endif
