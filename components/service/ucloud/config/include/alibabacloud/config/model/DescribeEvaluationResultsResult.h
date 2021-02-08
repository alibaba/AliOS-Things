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

#ifndef ALIBABACLOUD_CONFIG_MODEL_DESCRIBEEVALUATIONRESULTSRESULT_H_
#define ALIBABACLOUD_CONFIG_MODEL_DESCRIBEEVALUATIONRESULTSRESULT_H_

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
			class ALIBABACLOUD_CONFIG_EXPORT DescribeEvaluationResultsResult : public ServiceResult
			{
			public:
				struct EvaluationResults
				{
					struct EvaluationResult
					{
						struct EvaluationResultIdentifier
						{
							struct EvaluationResultQualifier
							{
								std::string configRuleId;
								std::string configRuleArn;
								std::string resourceId;
								std::string configRuleName;
								std::string resourceType;
								std::string regionId;
							};
							EvaluationResultQualifier evaluationResultQualifier;
							long orderingTimestamp;
						};
						long configRuleInvokedTimestamp;
						std::string complianceType;
						long resultRecordedTimestamp;
						std::string invokingEventMessageType;
						EvaluationResultIdentifier evaluationResultIdentifier;
						int riskLevel;
						std::string annotation;
					};
					std::vector<EvaluationResult> evaluationResultList;
					long totalCount;
					int pageSize;
					int pageNumber;
				};


				DescribeEvaluationResultsResult();
				explicit DescribeEvaluationResultsResult(const std::string &payload);
				~DescribeEvaluationResultsResult();
				EvaluationResults getEvaluationResults()const;

			protected:
				void parse(const std::string &payload);
			private:
				EvaluationResults evaluationResults_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CONFIG_MODEL_DESCRIBEEVALUATIONRESULTSRESULT_H_