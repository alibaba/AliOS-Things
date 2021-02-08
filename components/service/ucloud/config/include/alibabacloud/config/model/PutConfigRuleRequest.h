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

#ifndef ALIBABACLOUD_CONFIG_MODEL_PUTCONFIGRULEREQUEST_H_
#define ALIBABACLOUD_CONFIG_MODEL_PUTCONFIGRULEREQUEST_H_

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
			class ALIBABACLOUD_CONFIG_EXPORT PutConfigRuleRequest : public RpcServiceRequest
			{

			public:
				PutConfigRuleRequest();
				~PutConfigRuleRequest();

				std::string getConfigRuleId()const;
				void setConfigRuleId(const std::string& configRuleId);
				bool getMultiAccount()const;
				void setMultiAccount(bool multiAccount);
				std::string getClientToken()const;
				void setClientToken(const std::string& clientToken);
				std::string getDescription()const;
				void setDescription(const std::string& description);
				std::string getSourceIdentifier()const;
				void setSourceIdentifier(const std::string& sourceIdentifier);
				std::string getSourceMaximumExecutionFrequency()const;
				void setSourceMaximumExecutionFrequency(const std::string& sourceMaximumExecutionFrequency);
				std::string getScopeComplianceResourceTypes()const;
				void setScopeComplianceResourceTypes(const std::string& scopeComplianceResourceTypes);
				std::string getSourceDetailMessageType()const;
				void setSourceDetailMessageType(const std::string& sourceDetailMessageType);
				int getRiskLevel()const;
				void setRiskLevel(int riskLevel);
				std::string getSourceOwner()const;
				void setSourceOwner(const std::string& sourceOwner);
				std::string getInputParameters()const;
				void setInputParameters(const std::string& inputParameters);
				std::string getConfigRuleName()const;
				void setConfigRuleName(const std::string& configRuleName);
				std::string getScopeComplianceResourceId()const;
				void setScopeComplianceResourceId(const std::string& scopeComplianceResourceId);
				long getMemberId()const;
				void setMemberId(long memberId);

            private:
				std::string configRuleId_;
				bool multiAccount_;
				std::string clientToken_;
				std::string description_;
				std::string sourceIdentifier_;
				std::string sourceMaximumExecutionFrequency_;
				std::string scopeComplianceResourceTypes_;
				std::string sourceDetailMessageType_;
				int riskLevel_;
				std::string sourceOwner_;
				std::string inputParameters_;
				std::string configRuleName_;
				std::string scopeComplianceResourceId_;
				long memberId_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CONFIG_MODEL_PUTCONFIGRULEREQUEST_H_