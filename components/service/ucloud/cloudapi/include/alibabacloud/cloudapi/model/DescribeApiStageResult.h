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

#ifndef ALIBABACLOUD_CLOUDAPI_MODEL_DESCRIBEAPISTAGERESULT_H_
#define ALIBABACLOUD_CLOUDAPI_MODEL_DESCRIBEAPISTAGERESULT_H_

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
			class ALIBABACLOUD_CLOUDAPI_EXPORT DescribeApiStageResult : public ServiceResult
			{
			public:
				struct VariableItem
				{
					struct StageRouteModel
					{
						struct RouteRuleItem
						{
							std::string conditionValue;
							long minValue;
							long maxValue;
							std::string resultValue;
						};
						std::string parameterType;
						std::string parameterCatalog;
						std::vector<RouteRuleItem> routeRules;
						std::string routeMatchSymbol;
						std::string serviceParameterName;
						std::string location;
					};
					std::string variableValue;
					std::string variableName;
					bool supportRoute;
					StageRouteModel stageRouteModel;
				};


				DescribeApiStageResult();
				explicit DescribeApiStageResult(const std::string &payload);
				~DescribeApiStageResult();
				std::vector<VariableItem> getVariables()const;
				std::string getDescription()const;
				std::string getCreatedTime()const;
				std::string getModifiedTime()const;
				std::string getStageName()const;
				std::string getStageId()const;
				std::string getGroupId()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::vector<VariableItem> variables_;
				std::string description_;
				std::string createdTime_;
				std::string modifiedTime_;
				std::string stageName_;
				std::string stageId_;
				std::string groupId_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CLOUDAPI_MODEL_DESCRIBEAPISTAGERESULT_H_