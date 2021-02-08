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

#ifndef ALIBABACLOUD_LIVE_MODEL_SETLIVELAZYPULLSTREAMINFOCONFIGREQUEST_H_
#define ALIBABACLOUD_LIVE_MODEL_SETLIVELAZYPULLSTREAMINFOCONFIGREQUEST_H_

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
			class ALIBABACLOUD_LIVE_EXPORT SetLiveLazyPullStreamInfoConfigRequest : public RpcServiceRequest
			{

			public:
				SetLiveLazyPullStreamInfoConfigRequest();
				~SetLiveLazyPullStreamInfoConfigRequest();

				std::string getPullArgs()const;
				void setPullArgs(const std::string& pullArgs);
				std::string getAppName()const;
				void setAppName(const std::string& appName);
				std::string getLiveapiRequestFrom()const;
				void setLiveapiRequestFrom(const std::string& liveapiRequestFrom);
				std::string getPullAuthKey()const;
				void setPullAuthKey(const std::string& pullAuthKey);
				std::string getPullAuthType()const;
				void setPullAuthType(const std::string& pullAuthType);
				std::string getDomainName()const;
				void setDomainName(const std::string& domainName);
				std::string getPullDomainName()const;
				void setPullDomainName(const std::string& pullDomainName);
				long getOwnerId()const;
				void setOwnerId(long ownerId);
				std::string getPullAppName()const;
				void setPullAppName(const std::string& pullAppName);
				std::string getPullProtocol()const;
				void setPullProtocol(const std::string& pullProtocol);

            private:
				std::string pullArgs_;
				std::string appName_;
				std::string liveapiRequestFrom_;
				std::string pullAuthKey_;
				std::string pullAuthType_;
				std::string domainName_;
				std::string pullDomainName_;
				long ownerId_;
				std::string pullAppName_;
				std::string pullProtocol_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_LIVE_MODEL_SETLIVELAZYPULLSTREAMINFOCONFIGREQUEST_H_