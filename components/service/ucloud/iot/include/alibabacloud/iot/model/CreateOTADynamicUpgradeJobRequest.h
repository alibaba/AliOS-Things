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

#ifndef ALIBABACLOUD_IOT_MODEL_CREATEOTADYNAMICUPGRADEJOBREQUEST_H_
#define ALIBABACLOUD_IOT_MODEL_CREATEOTADYNAMICUPGRADEJOBREQUEST_H_

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
			class ALIBABACLOUD_IOT_EXPORT CreateOTADynamicUpgradeJobRequest : public RpcServiceRequest
			{

			public:
				CreateOTADynamicUpgradeJobRequest();
				~CreateOTADynamicUpgradeJobRequest();

				int getRetryCount()const;
				void setRetryCount(int retryCount);
				int getTimeoutInMinutes()const;
				void setTimeoutInMinutes(int timeoutInMinutes);
				std::string getAccessKeyId()const;
				void setAccessKeyId(const std::string& accessKeyId);
				std::string getIotInstanceId()const;
				void setIotInstanceId(const std::string& iotInstanceId);
				std::string getFirmwareId()const;
				void setFirmwareId(const std::string& firmwareId);
				std::string getProductKey()const;
				void setProductKey(const std::string& productKey);
				int getRetryInterval()const;
				void setRetryInterval(int retryInterval);
				std::vector<std::string> getSrcVersion()const;
				void setSrcVersion(const std::vector<std::string>& srcVersion);
				std::string getApiProduct()const;
				void setApiProduct(const std::string& apiProduct);
				std::string getApiRevision()const;
				void setApiRevision(const std::string& apiRevision);
				int getMaximumPerMinute()const;
				void setMaximumPerMinute(int maximumPerMinute);

            private:
				int retryCount_;
				int timeoutInMinutes_;
				std::string accessKeyId_;
				std::string iotInstanceId_;
				std::string firmwareId_;
				std::string productKey_;
				int retryInterval_;
				std::vector<std::string> srcVersion_;
				std::string apiProduct_;
				std::string apiRevision_;
				int maximumPerMinute_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IOT_MODEL_CREATEOTADYNAMICUPGRADEJOBREQUEST_H_