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

#ifndef ALIBABACLOUD_LIVE_MODEL_ADDCASTEREPISODEGROUPREQUEST_H_
#define ALIBABACLOUD_LIVE_MODEL_ADDCASTEREPISODEGROUPREQUEST_H_

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
			class ALIBABACLOUD_LIVE_EXPORT AddCasterEpisodeGroupRequest : public RpcServiceRequest
			{
			public:
				struct Item
				{
					std::string itemName;
					std::string vodUrl;
				};

			public:
				AddCasterEpisodeGroupRequest();
				~AddCasterEpisodeGroupRequest();

				std::string getClientToken()const;
				void setClientToken(const std::string& clientToken);
				std::string getStartTime()const;
				void setStartTime(const std::string& startTime);
				std::string getSideOutputUrl()const;
				void setSideOutputUrl(const std::string& sideOutputUrl);
				std::vector<Item> getItem()const;
				void setItem(const std::vector<Item>& item);
				std::string getDomainName()const;
				void setDomainName(const std::string& domainName);
				long getOwnerId()const;
				void setOwnerId(long ownerId);
				int getRepeatNum()const;
				void setRepeatNum(int repeatNum);
				std::string getCallbackUrl()const;
				void setCallbackUrl(const std::string& callbackUrl);

            private:
				std::string clientToken_;
				std::string startTime_;
				std::string sideOutputUrl_;
				std::vector<Item> item_;
				std::string domainName_;
				long ownerId_;
				int repeatNum_;
				std::string callbackUrl_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_LIVE_MODEL_ADDCASTEREPISODEGROUPREQUEST_H_