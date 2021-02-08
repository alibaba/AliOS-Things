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

#ifndef ALIBABACLOUD_LIVE_MODEL_DESCRIBELIVESTREAMRECORDCONTENTREQUEST_H_
#define ALIBABACLOUD_LIVE_MODEL_DESCRIBELIVESTREAMRECORDCONTENTREQUEST_H_

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
			class ALIBABACLOUD_LIVE_EXPORT DescribeLiveStreamRecordContentRequest : public RpcServiceRequest
			{

			public:
				DescribeLiveStreamRecordContentRequest();
				~DescribeLiveStreamRecordContentRequest();

				std::string getStartTime()const;
				void setStartTime(const std::string& startTime);
				std::string getAppName()const;
				void setAppName(const std::string& appName);
				std::string getSecurityToken()const;
				void setSecurityToken(const std::string& securityToken);
				std::string getStreamName()const;
				void setStreamName(const std::string& streamName);
				std::string getDomainName()const;
				void setDomainName(const std::string& domainName);
				std::string getEndTime()const;
				void setEndTime(const std::string& endTime);
				long getOwnerId()const;
				void setOwnerId(long ownerId);

            private:
				std::string startTime_;
				std::string appName_;
				std::string securityToken_;
				std::string streamName_;
				std::string domainName_;
				std::string endTime_;
				long ownerId_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_LIVE_MODEL_DESCRIBELIVESTREAMRECORDCONTENTREQUEST_H_