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

#ifndef ALIBABACLOUD_LIVE_MODEL_DESCRIBELIVESTREAMTRANSCODEINFORESULT_H_
#define ALIBABACLOUD_LIVE_MODEL_DESCRIBELIVESTREAMTRANSCODEINFORESULT_H_

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
			class ALIBABACLOUD_LIVE_EXPORT DescribeLiveStreamTranscodeInfoResult : public ServiceResult
			{
			public:
				struct DomainTranscodeInfo
				{
					struct CustomTranscodeParameters
					{
						int fPS;
						std::string gop;
						std::string templateType;
						int audioBitrate;
						std::string bframes;
						std::string audioProfile;
						std::string rtsFlag;
						int height;
						std::string videoProfile;
						int audioChannelNum;
						int width;
						std::string audioCodec;
						int audioRate;
						int videoBitrate;
					};
					std::string transcodeTemplate;
					CustomTranscodeParameters customTranscodeParameters;
					std::string transcodeApp;
					std::string transcodeName;
				};


				DescribeLiveStreamTranscodeInfoResult();
				explicit DescribeLiveStreamTranscodeInfoResult(const std::string &payload);
				~DescribeLiveStreamTranscodeInfoResult();
				std::vector<DomainTranscodeInfo> getDomainTranscodeList()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::vector<DomainTranscodeInfo> domainTranscodeList_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_LIVE_MODEL_DESCRIBELIVESTREAMTRANSCODEINFORESULT_H_