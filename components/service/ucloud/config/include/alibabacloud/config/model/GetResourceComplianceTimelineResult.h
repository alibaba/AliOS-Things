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

#ifndef ALIBABACLOUD_CONFIG_MODEL_GETRESOURCECOMPLIANCETIMELINERESULT_H_
#define ALIBABACLOUD_CONFIG_MODEL_GETRESOURCECOMPLIANCETIMELINERESULT_H_

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
			class ALIBABACLOUD_CONFIG_EXPORT GetResourceComplianceTimelineResult : public ServiceResult
			{
			public:
				struct ResourceComplianceTimeline
				{
					struct ComplianceListItem
					{
						std::string accountId;
						long captureTime;
						std::string configurationDiff;
						std::string configuration;
						std::string resourceId;
						std::string resourceName;
						std::string availabilityZone;
						std::string region;
						std::string resourceStatus;
						std::string resourceType;
						long resourceCreateTime;
						std::string tags;
					};
					long totalCount;
					std::vector<ComplianceListItem> complianceList;
					std::string nextToken;
					int limit;
				};


				GetResourceComplianceTimelineResult();
				explicit GetResourceComplianceTimelineResult(const std::string &payload);
				~GetResourceComplianceTimelineResult();
				ResourceComplianceTimeline getResourceComplianceTimeline()const;

			protected:
				void parse(const std::string &payload);
			private:
				ResourceComplianceTimeline resourceComplianceTimeline_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CONFIG_MODEL_GETRESOURCECOMPLIANCETIMELINERESULT_H_