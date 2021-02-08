/*
 * Copyright 2009-2017 Alibaba Cloud All rights reserved.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ALIBABACLOUD_IOT_MODEL_CREATEOTAFIRMWAREREQUEST_H_
#define ALIBABACLOUD_IOT_MODEL_CREATEOTAFIRMWAREREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/iot/IotExport.h>

namespace AlibabaCloud
{
	namespace Iot
	{
		namespace Model
		{
			class ALIBABACLOUD_IOT_EXPORT CreateOTAFirmwareRequest : public RpcServiceRequest
			{

			public:
				CreateOTAFirmwareRequest();
				~CreateOTAFirmwareRequest();

				std::string getSignMethod()const;
				void setSignMethod(const std::string& signMethod);
				int getType()const;
				void setType(int type);
				std::string getFirmwareUrl()const;
				void setFirmwareUrl(const std::string& firmwareUrl);
				std::string getAccessKeyId()const;
				void setAccessKeyId(const std::string& accessKeyId);
				std::string getIotInstanceId()const;
				void setIotInstanceId(const std::string& iotInstanceId);
				std::string getFirmwareDesc()const;
				void setFirmwareDesc(const std::string& firmwareDesc);
				std::string getModuleName()const;
				void setModuleName(const std::string& moduleName);
				std::string getFirmwareSign()const;
				void setFirmwareSign(const std::string& firmwareSign);
				int getFirmwareSize()const;
				void setFirmwareSize(int firmwareSize);
				std::string getFirmwareName()const;
				void setFirmwareName(const std::string& firmwareName);
				std::string getProductKey()const;
				void setProductKey(const std::string& productKey);
				std::string getSrcVersion()const;
				void setSrcVersion(const std::string& srcVersion);
				std::string getApiProduct()const;
				void setApiProduct(const std::string& apiProduct);
				std::string getApiRevision()const;
				void setApiRevision(const std::string& apiRevision);
				std::string getDestVersion()const;
				void setDestVersion(const std::string& destVersion);

            private:
				std::string signMethod_;
				int type_;
				std::string firmwareUrl_;
				std::string accessKeyId_;
				std::string iotInstanceId_;
				std::string firmwareDesc_;
				std::string moduleName_;
				std::string firmwareSign_;
				int firmwareSize_;
				std::string firmwareName_;
				std::string productKey_;
				std::string srcVersion_;
				std::string apiProduct_;
				std::string apiRevision_;
				std::string destVersion_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IOT_MODEL_CREATEOTAFIRMWAREREQUEST_H_