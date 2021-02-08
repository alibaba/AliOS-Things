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

#ifndef ALIBABACLOUD_IOT_MODEL_LISTRULERESULT_H_
#define ALIBABACLOUD_IOT_MODEL_LISTRULERESULT_H_

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
			class ALIBABACLOUD_IOT_EXPORT ListRuleResult : public ServiceResult
			{
			public:
				struct RuleInfo
				{
					std::string status;
					std::string modified;
					std::string utcCreated;
					long createUserId;
					std::string name;
					std::string created;
					std::string select;
					std::string shortTopic;
					std::string dataType;
					std::string ruleDesc;
					std::string utcModified;
					long id;
					std::string topic;
					std::string where;
					std::string productKey;
				};


				ListRuleResult();
				explicit ListRuleResult(const std::string &payload);
				~ListRuleResult();
				int getPageSize()const;
				int getTotal()const;
				std::vector<RuleInfo> getData()const;
				int getPage()const;
				std::string getErrorMessage()const;
				std::string getCode()const;
				bool getSuccess()const;

			protected:
				void parse(const std::string &payload);
			private:
				int pageSize_;
				int total_;
				std::vector<RuleInfo> data_;
				int page_;
				std::string errorMessage_;
				std::string code_;
				bool success_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IOT_MODEL_LISTRULERESULT_H_