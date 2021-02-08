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

#ifndef ALIBABACLOUD_CONFIG_MODEL_LISTCONFIGRULESREQUEST_H_
#define ALIBABACLOUD_CONFIG_MODEL_LISTCONFIGRULESREQUEST_H_

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
			class ALIBABACLOUD_CONFIG_EXPORT ListConfigRulesRequest : public RpcServiceRequest
			{

			public:
				ListConfigRulesRequest();
				~ListConfigRulesRequest();

				bool getMultiAccount()const;
				void setMultiAccount(bool multiAccount);
				std::string getMessageType()const;
				void setMessageType(const std::string& messageType);
				std::string getConfigRuleState()const;
				void setConfigRuleState(const std::string& configRuleState);
				int getPageNumber()const;
				void setPageNumber(int pageNumber);
				int getPageSize()const;
				void setPageSize(int pageSize);
				std::string getComplianceType()const;
				void setComplianceType(const std::string& complianceType);
				std::string getConfigRuleSceneId()const;
				void setConfigRuleSceneId(const std::string& configRuleSceneId);
				int getRiskLevel()const;
				void setRiskLevel(int riskLevel);
				long getMemberId()const;
				void setMemberId(long memberId);

            private:
				bool multiAccount_;
				std::string messageType_;
				std::string configRuleState_;
				int pageNumber_;
				int pageSize_;
				std::string complianceType_;
				std::string configRuleSceneId_;
				int riskLevel_;
				long memberId_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CONFIG_MODEL_LISTCONFIGRULESREQUEST_H_