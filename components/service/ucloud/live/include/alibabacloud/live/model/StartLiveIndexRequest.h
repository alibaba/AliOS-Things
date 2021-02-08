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

#ifndef ALIBABACLOUD_LIVE_MODEL_STARTLIVEINDEXREQUEST_H_
#define ALIBABACLOUD_LIVE_MODEL_STARTLIVEINDEXREQUEST_H_

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
			class ALIBABACLOUD_LIVE_EXPORT StartLiveIndexRequest : public RpcServiceRequest
			{

			public:
				StartLiveIndexRequest();
				~StartLiveIndexRequest();

				std::string getTokenId()const;
				void setTokenId(const std::string& tokenId);
				std::string getOssEndpoint()const;
				void setOssEndpoint(const std::string& ossEndpoint);
				std::string getAppName()const;
				void setAppName(const std::string& appName);
				std::string getOssRamRole()const;
				void setOssRamRole(const std::string& ossRamRole);
				std::string getStreamName()const;
				void setStreamName(const std::string& streamName);
				std::string getOssUserId()const;
				void setOssUserId(const std::string& ossUserId);
				std::string getOssBucket()const;
				void setOssBucket(const std::string& ossBucket);
				std::string getDomainName()const;
				void setDomainName(const std::string& domainName);
				std::string getInputUrl()const;
				void setInputUrl(const std::string& inputUrl);
				long getOwnerId()const;
				void setOwnerId(long ownerId);
				int getInterval()const;
				void setInterval(int interval);

            private:
				std::string tokenId_;
				std::string ossEndpoint_;
				std::string appName_;
				std::string ossRamRole_;
				std::string streamName_;
				std::string ossUserId_;
				std::string ossBucket_;
				std::string domainName_;
				std::string inputUrl_;
				long ownerId_;
				int interval_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_LIVE_MODEL_STARTLIVEINDEXREQUEST_H_