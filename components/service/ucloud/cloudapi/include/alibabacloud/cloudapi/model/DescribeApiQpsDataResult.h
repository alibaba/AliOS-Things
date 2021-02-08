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

#ifndef ALIBABACLOUD_CLOUDAPI_MODEL_DESCRIBEAPIQPSDATARESULT_H_
#define ALIBABACLOUD_CLOUDAPI_MODEL_DESCRIBEAPIQPSDATARESULT_H_

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
			class ALIBABACLOUD_CLOUDAPI_EXPORT DescribeApiQpsDataResult : public ServiceResult
			{
			public:
				struct MonitorItem
				{
					std::string itemValue;
					std::string itemTime;
				};


				DescribeApiQpsDataResult();
				explicit DescribeApiQpsDataResult(const std::string &payload);
				~DescribeApiQpsDataResult();
				std::vector<MonitorItem> getCallFails()const;
				std::vector<MonitorItem> getCallSuccesses()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::vector<MonitorItem> callFails_;
				std::vector<MonitorItem> callSuccesses_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CLOUDAPI_MODEL_DESCRIBEAPIQPSDATARESULT_H_