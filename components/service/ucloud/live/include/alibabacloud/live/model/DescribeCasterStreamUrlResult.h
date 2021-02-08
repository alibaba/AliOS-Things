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

#ifndef ALIBABACLOUD_LIVE_MODEL_DESCRIBECASTERSTREAMURLRESULT_H_
#define ALIBABACLOUD_LIVE_MODEL_DESCRIBECASTERSTREAMURLRESULT_H_

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
			class ALIBABACLOUD_LIVE_EXPORT DescribeCasterStreamUrlResult : public ServiceResult
			{
			public:
				struct CasterStream
				{
					struct StreamInfo
					{
						std::string outputStreamUrl;
						std::string transcodeConfig;
						std::string videoFormat;
					};
					std::string sceneId;
					std::vector<CasterStream::StreamInfo> streamInfos;
					std::string streamUrl;
					std::string rtmpUrl;
					int outputType;
				};


				DescribeCasterStreamUrlResult();
				explicit DescribeCasterStreamUrlResult(const std::string &payload);
				~DescribeCasterStreamUrlResult();
				std::vector<CasterStream> getCasterStreams()const;
				int getTotal()const;
				std::string getCasterId()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::vector<CasterStream> casterStreams_;
				int total_;
				std::string casterId_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_LIVE_MODEL_DESCRIBECASTERSTREAMURLRESULT_H_