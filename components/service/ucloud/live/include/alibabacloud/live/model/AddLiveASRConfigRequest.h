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

#ifndef ALIBABACLOUD_LIVE_MODEL_ADDLIVEASRCONFIGREQUEST_H_
#define ALIBABACLOUD_LIVE_MODEL_ADDLIVEASRCONFIGREQUEST_H_

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
			class ALIBABACLOUD_LIVE_EXPORT AddLiveASRConfigRequest : public RpcServiceRequest
			{

			public:
				AddLiveASRConfigRequest();
				~AddLiveASRConfigRequest();

				std::string getAppName()const;
				void setAppName(const std::string& appName);
				std::string getMnsTopic()const;
				void setMnsTopic(const std::string& mnsTopic);
				std::string getStreamName()const;
				void setStreamName(const std::string& streamName);
				int getPeriod()const;
				void setPeriod(int period);
				std::string getDomainName()const;
				void setDomainName(const std::string& domainName);
				std::string getHttpCallbackURL()const;
				void setHttpCallbackURL(const std::string& httpCallbackURL);
				long getOwnerId()const;
				void setOwnerId(long ownerId);
				std::string getMnsRegion()const;
				void setMnsRegion(const std::string& mnsRegion);

            private:
				std::string appName_;
				std::string mnsTopic_;
				std::string streamName_;
				int period_;
				std::string domainName_;
				std::string httpCallbackURL_;
				long ownerId_;
				std::string mnsRegion_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_LIVE_MODEL_ADDLIVEASRCONFIGREQUEST_H_