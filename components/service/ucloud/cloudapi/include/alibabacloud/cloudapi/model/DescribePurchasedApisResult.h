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

#ifndef ALIBABACLOUD_CLOUDAPI_MODEL_DESCRIBEPURCHASEDAPISRESULT_H_
#define ALIBABACLOUD_CLOUDAPI_MODEL_DESCRIBEPURCHASEDAPISRESULT_H_

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
			class ALIBABACLOUD_CLOUDAPI_EXPORT DescribePurchasedApisResult : public ServiceResult
			{
			public:
				struct PurchasedApi
				{
					std::string groupName;
					std::string description;
					std::string apiName;
					std::string stageName;
					std::string regionId;
					std::string apiId;
					std::string groupId;
					std::string purchasedTime;
				};


				DescribePurchasedApisResult();
				explicit DescribePurchasedApisResult(const std::string &payload);
				~DescribePurchasedApisResult();
				int getTotalCount()const;
				int getPageSize()const;
				int getPageNumber()const;
				std::vector<PurchasedApi> getPurchasedApis()const;

			protected:
				void parse(const std::string &payload);
			private:
				int totalCount_;
				int pageSize_;
				int pageNumber_;
				std::vector<PurchasedApi> purchasedApis_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CLOUDAPI_MODEL_DESCRIBEPURCHASEDAPISRESULT_H_