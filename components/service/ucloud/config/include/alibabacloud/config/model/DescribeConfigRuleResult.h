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

#ifndef ALIBABACLOUD_CONFIG_MODEL_DESCRIBECONFIGRULERESULT_H_
#define ALIBABACLOUD_CONFIG_MODEL_DESCRIBECONFIGRULERESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/config/ConfigExport.h>

namespace AlibabaCloud
{
	namespace Config
	{
		namespace Model
		{
			class ALIBABACLOUD_CONFIG_EXPORT DescribeConfigRuleResult : public ServiceResult
			{
			public:
				struct ConfigRule
				{
					struct CreateBy
					{
						std::string creatorId;
						std::string configRuleSceneName;
						std::string configRuleSceneId;
						std::string creatorType;
						std::string creatorName;
					};
					struct ConfigRuleEvaluationStatus
					{
						std::string lastErrorMessage;
						long lastFailedInvocationTimestamp;
						long firstActivatedTimestamp;
						long lastSuccessfulEvaluationTimestamp;
						bool firstEvaluationStarted;
						long lastFailedEvaluationTimestamp;
						std::string lastErrorCode;
						long lastSuccessfulInvocationTimestamp;
					};
					struct ManagedRule
					{
						struct SourceDetailsItem
						{
							std::string eventSource;
							std::string maximumExecutionFrequency;
							std::string messageType;
						};
						std::string managedRuleName;
						std::string optionalInputParameterDetails;
						std::string description;
						std::string identifier;
						std::string compulsoryInputParameterDetails;
						std::vector<std::string> labels;
						std::vector<SourceDetailsItem> sourceDetails;
						std::string helpUrl;
					};
					struct Source
					{
						struct SourceConditionsItem
						{
							std::string desiredValue;
							bool required;
							std::string _operator;
							std::string selectPath;
							std::string name;
							std::string tips;
						};
						struct SourceDetailsItem2
						{
							std::string eventSource;
							std::string maximumExecutionFrequency;
							std::string messageType;
						};
						std::string owner;
						std::string identifier;
						std::vector<SourceConditionsItem> sourceConditions;
						std::vector<SourceDetailsItem2> sourceDetails1;
					};
					struct Scope
					{
						std::string complianceResourceId;
						std::vector<std::string> complianceResourceTypes;
					};
					ManagedRule managedRule;
					std::string description;
					CreateBy createBy;
					ConfigRuleEvaluationStatus configRuleEvaluationStatus;
					Source source;
					std::string configRuleState;
					std::string maximumExecutionFrequency;
					std::string configRuleId;
					Scope scope;
					std::string configRuleArn;
					long modifiedTimestamp;
					std::string configRuleName;
					long createTimestamp;
					int riskLevel;
					std::string inputParameters;
				};


				DescribeConfigRuleResult();
				explicit DescribeConfigRuleResult(const std::string &payload);
				~DescribeConfigRuleResult();
				ConfigRule getConfigRule()const;

			protected:
				void parse(const std::string &payload);
			private:
				ConfigRule configRule_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CONFIG_MODEL_DESCRIBECONFIGRULERESULT_H_