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

#ifndef ALIBABACLOUD_IOT_MODEL_QUERYCONSUMERGROUPSTATUSRESULT_H_
#define ALIBABACLOUD_IOT_MODEL_QUERYCONSUMERGROUPSTATUSRESULT_H_

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
			class ALIBABACLOUD_IOT_EXPORT QueryConsumerGroupStatusResult : public ServiceResult
			{
			public:
				struct ConsumerGroupClientConnectionInfo
				{
					std::string clientId;
					long onlineTime;
					std::string clientIpPort;
				};


				QueryConsumerGroupStatusResult();
				explicit QueryConsumerGroupStatusResult(const std::string &payload);
				~QueryConsumerGroupStatusResult();
				std::vector<ConsumerGroupClientConnectionInfo> getClientConnectionStatusList()const;
				std::string getLastConsumerTime()const;
				std::string getErrorMessage()const;
				std::string getCode()const;
				bool getSuccess()const;
				int getAccumulationCount()const;
				int getConsumerSpeed()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::vector<ConsumerGroupClientConnectionInfo> clientConnectionStatusList_;
				std::string lastConsumerTime_;
				std::string errorMessage_;
				std::string code_;
				bool success_;
				int accumulationCount_;
				int consumerSpeed_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IOT_MODEL_QUERYCONSUMERGROUPSTATUSRESULT_H_