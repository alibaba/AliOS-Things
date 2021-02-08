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

#ifndef ALIBABACLOUD_LIVE_MODEL_MODIFYCASTERLAYOUTREQUEST_H_
#define ALIBABACLOUD_LIVE_MODEL_MODIFYCASTERLAYOUTREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/live/LiveExport.h>

namespace AlibabaCloud
{
	namespace Live
	{
		namespace Model
		{
			class ALIBABACLOUD_LIVE_EXPORT ModifyCasterLayoutRequest : public RpcServiceRequest
			{
			public:
				struct AudioLayer
				{
					float volumeRate;
					std::string validChannel;
					int fixedDelayDuration;
				};
				struct VideoLayer
				{
					std::string fillMode;
					float heightNormalized;
					float widthNormalized;
					std::string positionRefer;
					std::vector<float> positionNormalized;
					int fixedDelayDuration;
				};

			public:
				ModifyCasterLayoutRequest();
				~ModifyCasterLayoutRequest();

				std::vector<std::string> getBlendList()const;
				void setBlendList(const std::vector<std::string>& blendList);
				std::string getLayoutId()const;
				void setLayoutId(const std::string& layoutId);
				std::string getCasterId()const;
				void setCasterId(const std::string& casterId);
				long getOwnerId()const;
				void setOwnerId(long ownerId);
				std::vector<AudioLayer> getAudioLayer()const;
				void setAudioLayer(const std::vector<AudioLayer>& audioLayer);
				std::vector<VideoLayer> getVideoLayer()const;
				void setVideoLayer(const std::vector<VideoLayer>& videoLayer);
				std::vector<std::string> getMixList()const;
				void setMixList(const std::vector<std::string>& mixList);

            private:
				std::vector<std::string> blendList_;
				std::string layoutId_;
				std::string casterId_;
				long ownerId_;
				std::vector<AudioLayer> audioLayer_;
				std::vector<VideoLayer> videoLayer_;
				std::vector<std::string> mixList_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_LIVE_MODEL_MODIFYCASTERLAYOUTREQUEST_H_