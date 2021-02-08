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

#ifndef ALIBABACLOUD_LIVE_MODEL_ADDCASTERVIDEORESOURCEREQUEST_H_
#define ALIBABACLOUD_LIVE_MODEL_ADDCASTERVIDEORESOURCEREQUEST_H_

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
			class ALIBABACLOUD_LIVE_EXPORT AddCasterVideoResourceRequest : public RpcServiceRequest
			{

			public:
				AddCasterVideoResourceRequest();
				~AddCasterVideoResourceRequest();

				int getEndOffset()const;
				void setEndOffset(int endOffset);
				std::string getMaterialId()const;
				void setMaterialId(const std::string& materialId);
				std::string getVodUrl()const;
				void setVodUrl(const std::string& vodUrl);
				std::string getStreamId()const;
				void setStreamId(const std::string& streamId);
				std::string getCasterId()const;
				void setCasterId(const std::string& casterId);
				long getOwnerId()const;
				void setOwnerId(long ownerId);
				int getBeginOffset()const;
				void setBeginOffset(int beginOffset);
				std::string getLiveStreamUrl()const;
				void setLiveStreamUrl(const std::string& liveStreamUrl);
				std::string getLocationId()const;
				void setLocationId(const std::string& locationId);
				int getPtsCallbackInterval()const;
				void setPtsCallbackInterval(int ptsCallbackInterval);
				std::string getResourceName()const;
				void setResourceName(const std::string& resourceName);
				int getRepeatNum()const;
				void setRepeatNum(int repeatNum);

            private:
				int endOffset_;
				std::string materialId_;
				std::string vodUrl_;
				std::string streamId_;
				std::string casterId_;
				long ownerId_;
				int beginOffset_;
				std::string liveStreamUrl_;
				std::string locationId_;
				int ptsCallbackInterval_;
				std::string resourceName_;
				int repeatNum_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_LIVE_MODEL_ADDCASTERVIDEORESOURCEREQUEST_H_