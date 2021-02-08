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

#ifndef ALIBABACLOUD_CLOUDAPI_MODEL_DESCRIBEAPIGROUPRESULT_H_
#define ALIBABACLOUD_CLOUDAPI_MODEL_DESCRIBEAPIGROUPRESULT_H_

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
			class ALIBABACLOUD_CLOUDAPI_EXPORT DescribeApiGroupResult : public ServiceResult
			{
			public:
				struct DomainItem
				{
					std::string domainCNAMEStatus;
					std::string domainRemark;
					std::string domainBindingStatus;
					std::string domainLegalStatus;
					std::string domainName;
					std::string certificateId;
					std::string domainWebSocketStatus;
					std::string bindStageName;
					std::string certificateName;
				};
				struct StageInfo
				{
					std::string description;
					std::string stageName;
					std::string stageId;
				};


				DescribeApiGroupResult();
				explicit DescribeApiGroupResult(const std::string &payload);
				~DescribeApiGroupResult();
				std::string getGroupName()const;
				std::string getStatus()const;
				std::string getDescription()const;
				std::string getCreatedTime()const;
				std::string getHttpsPolicy()const;
				std::string getInstanceId()const;
				std::string getCompatibleFlags()const;
				std::string getCustomTraceConfig()const;
				std::string getUserLogConfig()const;
				std::string getBillingStatus()const;
				int getTrafficLimit()const;
				std::string getGroupId()const;
				std::string getVpcDomain()const;
				std::string getSubDomain()const;
				std::vector<DomainItem> getCustomDomains()const;
				std::string getModifiedTime()const;
				std::string getRpcPattern()const;
				std::vector<StageInfo> getStageItems()const;
				std::string getIllegalStatus()const;
				std::string getRegionId()const;
				std::string getInstanceType()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::string groupName_;
				std::string status_;
				std::string description_;
				std::string createdTime_;
				std::string httpsPolicy_;
				std::string instanceId_;
				std::string compatibleFlags_;
				std::string customTraceConfig_;
				std::string userLogConfig_;
				std::string billingStatus_;
				int trafficLimit_;
				std::string groupId_;
				std::string vpcDomain_;
				std::string subDomain_;
				std::vector<DomainItem> customDomains_;
				std::string modifiedTime_;
				std::string rpcPattern_;
				std::vector<StageInfo> stageItems_;
				std::string illegalStatus_;
				std::string regionId_;
				std::string instanceType_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CLOUDAPI_MODEL_DESCRIBEAPIGROUPRESULT_H_