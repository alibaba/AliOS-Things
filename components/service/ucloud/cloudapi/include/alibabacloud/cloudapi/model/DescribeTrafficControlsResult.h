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

#ifndef ALIBABACLOUD_CLOUDAPI_MODEL_DESCRIBETRAFFICCONTROLSRESULT_H_
#define ALIBABACLOUD_CLOUDAPI_MODEL_DESCRIBETRAFFICCONTROLSRESULT_H_

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
			class ALIBABACLOUD_CLOUDAPI_EXPORT DescribeTrafficControlsResult : public ServiceResult
			{
			public:
				struct TrafficControl
				{
					struct SpecialPolicy
					{
						struct Special
						{
							std::string specialKey;
							int trafficValue;
						};
						std::vector<SpecialPolicy::Special> specials;
						std::string specialType;
					};
					std::string description;
					std::string createdTime;
					std::string trafficControlUnit;
					std::string modifiedTime;
					std::string trafficControlName;
					int appDefault;
					int apiDefault;
					int userDefault;
					std::vector<TrafficControl::SpecialPolicy> specialPolicies;
					std::string trafficControlId;
				};


				DescribeTrafficControlsResult();
				explicit DescribeTrafficControlsResult(const std::string &payload);
				~DescribeTrafficControlsResult();
				int getTotalCount()const;
				int getPageSize()const;
				int getPageNumber()const;
				std::vector<TrafficControl> getTrafficControls()const;

			protected:
				void parse(const std::string &payload);
			private:
				int totalCount_;
				int pageSize_;
				int pageNumber_;
				std::vector<TrafficControl> trafficControls_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CLOUDAPI_MODEL_DESCRIBETRAFFICCONTROLSRESULT_H_