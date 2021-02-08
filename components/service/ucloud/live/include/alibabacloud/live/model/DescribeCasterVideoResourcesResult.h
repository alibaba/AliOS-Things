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

#ifndef ALIBABACLOUD_LIVE_MODEL_DESCRIBECASTERVIDEORESOURCESRESULT_H_
#define ALIBABACLOUD_LIVE_MODEL_DESCRIBECASTERVIDEORESOURCESRESULT_H_

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
			class ALIBABACLOUD_LIVE_EXPORT DescribeCasterVideoResourcesResult : public ServiceResult
			{
			public:
				struct VideoResource
				{
					int endOffset;
					int beginOffset;
					std::string materialId;
					std::string resourceId;
					std::string resourceName;
					std::string locationId;
					int repeatNum;
					std::string liveStreamUrl;
					int ptsCallbackInterval;
					std::string vodUrl;
				};


				DescribeCasterVideoResourcesResult();
				explicit DescribeCasterVideoResourcesResult(const std::string &payload);
				~DescribeCasterVideoResourcesResult();
				int getTotal()const;
				std::vector<VideoResource> getVideoResources()const;

			protected:
				void parse(const std::string &payload);
			private:
				int total_;
				std::vector<VideoResource> videoResources_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_LIVE_MODEL_DESCRIBECASTERVIDEORESOURCESRESULT_H_