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

#ifndef ALIBABACLOUD_IOT_MODEL_CREATEOTASTATICUPGRADEJOBREQUEST_H_
#define ALIBABACLOUD_IOT_MODEL_CREATEOTASTATICUPGRADEJOBREQUEST_H_

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
			class ALIBABACLOUD_IOT_EXPORT CreateOTAStaticUpgradeJobRequest : public RpcServiceRequest
			{

			public:
				CreateOTAStaticUpgradeJobRequest();
				~CreateOTAStaticUpgradeJobRequest();

				int getRetryCount()const;
				void setRetryCount(int retryCount);
				int getTimeoutInMinutes()const;
				void setTimeoutInMinutes(int timeoutInMinutes);
				std::string getAccessKeyId()const;
				void setAccessKeyId(const std::string& accessKeyId);
				std::string getIotInstanceId()const;
				void setIotInstanceId(const std::string& iotInstanceId);
				std::string getTargetSelection()const;
				void setTargetSelection(const std::string& targetSelection);
				std::string getGrayPercent()const;
				void setGrayPercent(const std::string& grayPercent);
				std::string getFirmwareId()const;
				void setFirmwareId(const std::string& firmwareId);
				std::string getProductKey()const;
				void setProductKey(const std::string& productKey);
				int getRetryInterval()const;
				void setRetryInterval(int retryInterval);
				std::vector<std::string> getSrcVersion()const;
				void setSrcVersion(const std::vector<std::string>& srcVersion);
				long getScheduleTime()const;
				void setScheduleTime(long scheduleTime);
				std::string getApiProduct()const;
				void setApiProduct(const std::string& apiProduct);
				std::string getApiRevision()const;
				void setApiRevision(const std::string& apiRevision);
				int getMaximumPerMinute()const;
				void setMaximumPerMinute(int maximumPerMinute);
				std::vector<std::string> getTargetDeviceName()const;
				void setTargetDeviceName(const std::vector<std::string>& targetDeviceName);

            private:
				int retryCount_;
				int timeoutInMinutes_;
				std::string accessKeyId_;
				std::string iotInstanceId_;
				std::string targetSelection_;
				std::string grayPercent_;
				std::string firmwareId_;
				std::string productKey_;
				int retryInterval_;
				std::vector<std::string> srcVersion_;
				long scheduleTime_;
				std::string apiProduct_;
				std::string apiRevision_;
				int maximumPerMinute_;
				std::vector<std::string> targetDeviceName_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IOT_MODEL_CREATEOTASTATICUPGRADEJOBREQUEST_H_