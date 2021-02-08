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

#ifndef ALIBABACLOUD_CLOUDAPI_MODEL_DESCRIBEPURCHASEDAPIGROUPRESULT_H_
#define ALIBABACLOUD_CLOUDAPI_MODEL_DESCRIBEPURCHASEDAPIGROUPRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/cloudapi/CloudAPIExport.h>

namespace AlibabaCloud
{
	namespace CloudAPI
	{
		namespace Model
		{
			class ALIBABACLOUD_CLOUDAPI_EXPORT DescribePurchasedApiGroupResult : public ServiceResult
			{
			public:
				struct DomainItem
				{
					std::string domainName;
				};


				DescribePurchasedApiGroupResult();
				explicit DescribePurchasedApiGroupResult(const std::string &payload);
				~DescribePurchasedApiGroupResult();
				std::vector<DomainItem> getDomains()const;
				std::string getGroupName()const;
				std::string getStatus()const;
				std::string getDescription()const;
				std::string getRegionId()const;
				std::string getGroupId()const;
				std::string getPurchasedTime()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::vector<DomainItem> domains_;
				std::string groupName_;
				std::string status_;
				std::string description_;
				std::string regionId_;
				std::string groupId_;
				std::string purchasedTime_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CLOUDAPI_MODEL_DESCRIBEPURCHASEDAPIGROUPRESULT_H_