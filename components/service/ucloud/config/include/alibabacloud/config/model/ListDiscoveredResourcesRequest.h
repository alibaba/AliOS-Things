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

#ifndef ALIBABACLOUD_CONFIG_MODEL_LISTDISCOVEREDRESOURCESREQUEST_H_
#define ALIBABACLOUD_CONFIG_MODEL_LISTDISCOVEREDRESOURCESREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/config/ConfigExport.h>

namespace AlibabaCloud
{
	namespace Config
	{
		namespace Model
		{
			class ALIBABACLOUD_CONFIG_EXPORT ListDiscoveredResourcesRequest : public RpcServiceRequest
			{

			public:
				ListDiscoveredResourcesRequest();
				~ListDiscoveredResourcesRequest();

				int getResourceDeleted()const;
				void setResourceDeleted(int resourceDeleted);
				bool getMultiAccount()const;
				void setMultiAccount(bool multiAccount);
				std::string getRegions()const;
				void setRegions(const std::string& regions);
				int getPageNumber()const;
				void setPageNumber(int pageNumber);
				int getPageSize()const;
				void setPageSize(int pageSize);
				std::string getComplianceType()const;
				void setComplianceType(const std::string& complianceType);
				std::string getResourceId()const;
				void setResourceId(const std::string& resourceId);
				std::string getResourceTypes()const;
				void setResourceTypes(const std::string& resourceTypes);
				long getMemberId()const;
				void setMemberId(long memberId);

            private:
				int resourceDeleted_;
				bool multiAccount_;
				std::string regions_;
				int pageNumber_;
				int pageSize_;
				std::string complianceType_;
				std::string resourceId_;
				std::string resourceTypes_;
				long memberId_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CONFIG_MODEL_LISTDISCOVEREDRESOURCESREQUEST_H_