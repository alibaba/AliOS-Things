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

#ifndef ALIBABACLOUD_IOT_MODEL_QUERYCONSUMERGROUPLISTRESULT_H_
#define ALIBABACLOUD_IOT_MODEL_QUERYCONSUMERGROUPLISTRESULT_H_

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
			class ALIBABACLOUD_IOT_EXPORT QueryConsumerGroupListResult : public ServiceResult
			{
			public:
				struct ConsumerGroupDTO
				{
					std::string groupName;
					std::string iotId;
					std::string createTime;
					std::string cloudCommodityType;
					std::string creator;
					std::string authType;
					std::string groupId;
				};


				QueryConsumerGroupListResult();
				explicit QueryConsumerGroupListResult(const std::string &payload);
				~QueryConsumerGroupListResult();
				int getPageSize()const;
				int getPageCount()const;
				int getCurrentPage()const;
				int getTotal()const;
				std::vector<ConsumerGroupDTO> getData()const;
				std::string getErrorMessage()const;
				std::string getCode()const;
				bool getSuccess()const;

			protected:
				void parse(const std::string &payload);
			private:
				int pageSize_;
				int pageCount_;
				int currentPage_;
				int total_;
				std::vector<ConsumerGroupDTO> data_;
				std::string errorMessage_;
				std::string code_;
				bool success_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IOT_MODEL_QUERYCONSUMERGROUPLISTRESULT_H_