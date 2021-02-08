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

#ifndef ALIBABACLOUD_IOT_MODEL_UPDATESUBSCRIBERELATIONREQUEST_H_
#define ALIBABACLOUD_IOT_MODEL_UPDATESUBSCRIBERELATIONREQUEST_H_

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
			class ALIBABACLOUD_IOT_EXPORT UpdateSubscribeRelationRequest : public RpcServiceRequest
			{

			public:
				UpdateSubscribeRelationRequest();
				~UpdateSubscribeRelationRequest();

				bool getOtaEventFlag()const;
				void setOtaEventFlag(bool otaEventFlag);
				bool getDeviceTopoLifeCycleFlag()const;
				void setDeviceTopoLifeCycleFlag(bool deviceTopoLifeCycleFlag);
				std::string getType()const;
				void setType(const std::string& type);
				bool getDeviceLifeCycleFlag()const;
				void setDeviceLifeCycleFlag(bool deviceLifeCycleFlag);
				std::string getIotInstanceId()const;
				void setIotInstanceId(const std::string& iotInstanceId);
				bool getDeviceStatusChangeFlag()const;
				void setDeviceStatusChangeFlag(bool deviceStatusChangeFlag);
				std::vector<std::string> getConsumerGroupIds()const;
				void setConsumerGroupIds(const std::vector<std::string>& consumerGroupIds);
				std::string getProductKey()const;
				void setProductKey(const std::string& productKey);
				bool getThingHistoryFlag()const;
				void setThingHistoryFlag(bool thingHistoryFlag);
				bool getFoundDeviceListFlag()const;
				void setFoundDeviceListFlag(bool foundDeviceListFlag);
				std::string getApiProduct()const;
				void setApiProduct(const std::string& apiProduct);
				bool getDeviceDataFlag()const;
				void setDeviceDataFlag(bool deviceDataFlag);
				std::string getApiRevision()const;
				void setApiRevision(const std::string& apiRevision);
				std::string getMnsConfiguration()const;
				void setMnsConfiguration(const std::string& mnsConfiguration);

            private:
				bool otaEventFlag_;
				bool deviceTopoLifeCycleFlag_;
				std::string type_;
				bool deviceLifeCycleFlag_;
				std::string iotInstanceId_;
				bool deviceStatusChangeFlag_;
				std::vector<std::string> consumerGroupIds_;
				std::string productKey_;
				bool thingHistoryFlag_;
				bool foundDeviceListFlag_;
				std::string apiProduct_;
				bool deviceDataFlag_;
				std::string apiRevision_;
				std::string mnsConfiguration_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IOT_MODEL_UPDATESUBSCRIBERELATIONREQUEST_H_