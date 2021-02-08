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

#ifndef ALIBABACLOUD_LIVE_MODEL_DESCRIBECASTERSCENEAUDIORESULT_H_
#define ALIBABACLOUD_LIVE_MODEL_DESCRIBECASTERSCENEAUDIORESULT_H_

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
			class ALIBABACLOUD_LIVE_EXPORT DescribeCasterSceneAudioResult : public ServiceResult
			{
			public:
				struct AudioLayer
				{
					std::string validChannel;
					float volumeRate;
					int fixedDelayDuration;
				};


				DescribeCasterSceneAudioResult();
				explicit DescribeCasterSceneAudioResult(const std::string &payload);
				~DescribeCasterSceneAudioResult();
				std::vector<std::string> getMixList()const;
				std::vector<AudioLayer> getAudioLayers()const;
				std::string getCasterId()const;
				int getFollowEnable()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::vector<std::string> mixList_;
				std::vector<AudioLayer> audioLayers_;
				std::string casterId_;
				int followEnable_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_LIVE_MODEL_DESCRIBECASTERSCENEAUDIORESULT_H_