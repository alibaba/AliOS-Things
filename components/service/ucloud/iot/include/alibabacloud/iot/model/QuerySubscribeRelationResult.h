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

#ifndef ALIBABACLOUD_IOT_MODEL_QUERYSUBSCRIBERELATIONRESULT_H_
#define ALIBABACLOUD_IOT_MODEL_QUERYSUBSCRIBERELATIONRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/iot/IotExport.h>

namespace AlibabaCloud
{
	namespace Iot
	{
		namespace Model
		{
			class ALIBABACLOUD_IOT_EXPORT QuerySubscribeRelationResult : public ServiceResult
			{
			public:


				QuerySubscribeRelationResult();
				explicit QuerySubscribeRelationResult(const std::string &payload);
				~QuerySubscribeRelationResult();
				bool getDeviceLifeCycleFlag()const;
				bool getDeviceDataFlag()const;
				bool getDeviceTopoLifeCycleFlag()const;
				bool getDeviceStatusChangeFlag()const;
				std::vector<std::string> getConsumerGroupIds()const;
				std::string getCode()const;
				bool getSuccess()const;
				bool getThingHistoryFlag()const;
				std::string getType()const;
				bool getFoundDeviceListFlag()const;
				bool getOtaEventFlag()const;
				std::string getErrorMessage()const;
				std::string getProductKey()const;
				std::string getMnsConfiguration()const;

			protected:
				void parse(const std::string &payload);
			private:
				bool deviceLifeCycleFlag_;
				bool deviceDataFlag_;
				bool deviceTopoLifeCycleFlag_;
				bool deviceStatusChangeFlag_;
				std::vector<std::string> consumerGroupIds_;
				std::string code_;
				bool success_;
				bool thingHistoryFlag_;
				std::string type_;
				bool foundDeviceListFlag_;
				bool otaEventFlag_;
				std::string errorMessage_;
				std::string productKey_;
				std::string mnsConfiguration_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IOT_MODEL_QUERYSUBSCRIBERELATIONRESULT_H_