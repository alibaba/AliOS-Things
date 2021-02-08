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

#ifndef ALIBABACLOUD_LIVE_MODEL_ADDLIVERECORDVODCONFIGREQUEST_H_
#define ALIBABACLOUD_LIVE_MODEL_ADDLIVERECORDVODCONFIGREQUEST_H_

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
			class ALIBABACLOUD_LIVE_EXPORT AddLiveRecordVodConfigRequest : public RpcServiceRequest
			{

			public:
				AddLiveRecordVodConfigRequest();
				~AddLiveRecordVodConfigRequest();

				std::string getAutoCompose()const;
				void setAutoCompose(const std::string& autoCompose);
				std::string getComposeVodTranscodeGroupId()const;
				void setComposeVodTranscodeGroupId(const std::string& composeVodTranscodeGroupId);
				std::string getStorageLocation()const;
				void setStorageLocation(const std::string& storageLocation);
				std::string getAppName()const;
				void setAppName(const std::string& appName);
				std::string getStreamName()const;
				void setStreamName(const std::string& streamName);
				std::string getVodTranscodeGroupId()const;
				void setVodTranscodeGroupId(const std::string& vodTranscodeGroupId);
				std::string getDomainName()const;
				void setDomainName(const std::string& domainName);
				int getCycleDuration()const;
				void setCycleDuration(int cycleDuration);
				long getOwnerId()const;
				void setOwnerId(long ownerId);

            private:
				std::string autoCompose_;
				std::string composeVodTranscodeGroupId_;
				std::string storageLocation_;
				std::string appName_;
				std::string streamName_;
				std::string vodTranscodeGroupId_;
				std::string domainName_;
				int cycleDuration_;
				long ownerId_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_LIVE_MODEL_ADDLIVERECORDVODCONFIGREQUEST_H_