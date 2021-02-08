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

#ifndef ALIBABACLOUD_LIVE_MODEL_DELETELIVEREALTIMELOGDELIVERYREQUEST_H_
#define ALIBABACLOUD_LIVE_MODEL_DELETELIVEREALTIMELOGDELIVERYREQUEST_H_

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
			class ALIBABACLOUD_LIVE_EXPORT DeleteLiveRealtimeLogDeliveryRequest : public RpcServiceRequest
			{

			public:
				DeleteLiveRealtimeLogDeliveryRequest();
				~DeleteLiveRealtimeLogDeliveryRequest();

				std::string getProject()const;
				void setProject(const std::string& project);
				std::string getDomainName()const;
				void setDomainName(const std::string& domainName);
				long getOwnerId()const;
				void setOwnerId(long ownerId);
				std::string getRegion()const;
				void setRegion(const std::string& region);
				std::string getLogstore()const;
				void setLogstore(const std::string& logstore);

            private:
				std::string project_;
				std::string domainName_;
				long ownerId_;
				std::string region_;
				std::string logstore_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_LIVE_MODEL_DELETELIVEREALTIMELOGDELIVERYREQUEST_H_