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

#ifndef ALIBABACLOUD_LIVE_MODEL_SETLIVESTREAMDELAYCONFIGREQUEST_H_
#define ALIBABACLOUD_LIVE_MODEL_SETLIVESTREAMDELAYCONFIGREQUEST_H_

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
			class ALIBABACLOUD_LIVE_EXPORT SetLiveStreamDelayConfigRequest : public RpcServiceRequest
			{

			public:
				SetLiveStreamDelayConfigRequest();
				~SetLiveStreamDelayConfigRequest();

				std::string getFlvLevel()const;
				void setFlvLevel(const std::string& flvLevel);
				std::string getHlsLevel()const;
				void setHlsLevel(const std::string& hlsLevel);
				int getRtmpDelay()const;
				void setRtmpDelay(int rtmpDelay);
				std::string getDomainName()const;
				void setDomainName(const std::string& domainName);
				long getOwnerId()const;
				void setOwnerId(long ownerId);
				int getFlvDelay()const;
				void setFlvDelay(int flvDelay);
				std::string getRtmpLevel()const;
				void setRtmpLevel(const std::string& rtmpLevel);
				int getHlsDelay()const;
				void setHlsDelay(int hlsDelay);

            private:
				std::string flvLevel_;
				std::string hlsLevel_;
				int rtmpDelay_;
				std::string domainName_;
				long ownerId_;
				int flvDelay_;
				std::string rtmpLevel_;
				int hlsDelay_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_LIVE_MODEL_SETLIVESTREAMDELAYCONFIGREQUEST_H_