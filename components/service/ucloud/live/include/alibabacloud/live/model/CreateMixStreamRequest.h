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

#ifndef ALIBABACLOUD_LIVE_MODEL_CREATEMIXSTREAMREQUEST_H_
#define ALIBABACLOUD_LIVE_MODEL_CREATEMIXSTREAMREQUEST_H_

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
			class ALIBABACLOUD_LIVE_EXPORT CreateMixStreamRequest : public RpcServiceRequest
			{

			public:
				CreateMixStreamRequest();
				~CreateMixStreamRequest();

				std::string getOutputConfig()const;
				void setOutputConfig(const std::string& outputConfig);
				std::string getLayoutId()const;
				void setLayoutId(const std::string& layoutId);
				std::string getDomainName()const;
				void setDomainName(const std::string& domainName);
				std::string getInputStreamList()const;
				void setInputStreamList(const std::string& inputStreamList);
				long getOwnerId()const;
				void setOwnerId(long ownerId);
				std::string getCallbackConfig()const;
				void setCallbackConfig(const std::string& callbackConfig);

            private:
				std::string outputConfig_;
				std::string layoutId_;
				std::string domainName_;
				std::string inputStreamList_;
				long ownerId_;
				std::string callbackConfig_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_LIVE_MODEL_CREATEMIXSTREAMREQUEST_H_