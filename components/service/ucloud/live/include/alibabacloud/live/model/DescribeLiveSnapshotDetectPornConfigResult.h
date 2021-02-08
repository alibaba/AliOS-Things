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

#ifndef ALIBABACLOUD_LIVE_MODEL_DESCRIBELIVESNAPSHOTDETECTPORNCONFIGRESULT_H_
#define ALIBABACLOUD_LIVE_MODEL_DESCRIBELIVESNAPSHOTDETECTPORNCONFIGRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/live/LiveExport.h>

namespace AlibabaCloud
{
	namespace Live
	{
		namespace Model
		{
			class ALIBABACLOUD_LIVE_EXPORT DescribeLiveSnapshotDetectPornConfigResult : public ServiceResult
			{
			public:
				struct LiveSnapshotDetectPornConfig
				{
					std::vector<std::string> scenes;
					std::string ossEndpoint;
					std::string domainName;
					std::string ossBucket;
					std::string ossObject;
					std::string appName;
					int interval;
				};


				DescribeLiveSnapshotDetectPornConfigResult();
				explicit DescribeLiveSnapshotDetectPornConfigResult(const std::string &payload);
				~DescribeLiveSnapshotDetectPornConfigResult();
				std::string getOrder()const;
				int getTotalNum()const;
				int getPageNum()const;
				int getPageSize()const;
				int getTotalPage()const;
				std::vector<LiveSnapshotDetectPornConfig> getLiveSnapshotDetectPornConfigList()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::string order_;
				int totalNum_;
				int pageNum_;
				int pageSize_;
				int totalPage_;
				std::vector<LiveSnapshotDetectPornConfig> liveSnapshotDetectPornConfigList_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_LIVE_MODEL_DESCRIBELIVESNAPSHOTDETECTPORNCONFIGRESULT_H_