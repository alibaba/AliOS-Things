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

#ifndef ALIBABACLOUD_LIVE_MODEL_ADDLIVEAPPSNAPSHOTCONFIGREQUEST_H_
#define ALIBABACLOUD_LIVE_MODEL_ADDLIVEAPPSNAPSHOTCONFIGREQUEST_H_

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
			class ALIBABACLOUD_LIVE_EXPORT AddLiveAppSnapshotConfigRequest : public RpcServiceRequest
			{

			public:
				AddLiveAppSnapshotConfigRequest();
				~AddLiveAppSnapshotConfigRequest();

				int getTimeInterval()const;
				void setTimeInterval(int timeInterval);
				std::string getOssEndpoint()const;
				void setOssEndpoint(const std::string& ossEndpoint);
				std::string getAppName()const;
				void setAppName(const std::string& appName);
				std::string getSecurityToken()const;
				void setSecurityToken(const std::string& securityToken);
				std::string getOverwriteOssObject()const;
				void setOverwriteOssObject(const std::string& overwriteOssObject);
				std::string getOssBucket()const;
				void setOssBucket(const std::string& ossBucket);
				std::string getDomainName()const;
				void setDomainName(const std::string& domainName);
				std::string getSequenceOssObject()const;
				void setSequenceOssObject(const std::string& sequenceOssObject);
				long getOwnerId()const;
				void setOwnerId(long ownerId);
				std::string getCallback()const;
				void setCallback(const std::string& callback);

            private:
				int timeInterval_;
				std::string ossEndpoint_;
				std::string appName_;
				std::string securityToken_;
				std::string overwriteOssObject_;
				std::string ossBucket_;
				std::string domainName_;
				std::string sequenceOssObject_;
				long ownerId_;
				std::string callback_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_LIVE_MODEL_ADDLIVEAPPSNAPSHOTCONFIGREQUEST_H_