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

#ifndef ALIBABACLOUD_LIVE_MODEL_DESCRIBECASTERLAYOUTSRESULT_H_
#define ALIBABACLOUD_LIVE_MODEL_DESCRIBECASTERLAYOUTSRESULT_H_

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
			class ALIBABACLOUD_LIVE_EXPORT DescribeCasterLayoutsResult : public ServiceResult
			{
			public:
				struct Layout
				{
					struct VideoLayer
					{
						std::vector<std::string> positionNormalizeds;
						std::string fillMode;
						float heightNormalized;
						std::string positionRefer;
						int fixedDelayDuration;
						float widthNormalized;
					};
					struct AudioLayer
					{
						std::string validChannel;
						float volumeRate;
						int fixedDelayDuration;
					};
					std::vector<std::string> mixList;
					std::vector<Layout::AudioLayer> audioLayers;
					std::vector<Layout::VideoLayer> videoLayers;
					std::string layoutId;
					std::vector<std::string> blendList;
				};


				DescribeCasterLayoutsResult();
				explicit DescribeCasterLayoutsResult(const std::string &payload);
				~DescribeCasterLayoutsResult();
				std::vector<Layout> getLayouts()const;
				int getTotal()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::vector<Layout> layouts_;
				int total_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_LIVE_MODEL_DESCRIBECASTERLAYOUTSRESULT_H_