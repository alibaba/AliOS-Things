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

#ifndef ALIBABACLOUD_LIVE_MODEL_UPDATEBOARDCALLBACKREQUEST_H_
#define ALIBABACLOUD_LIVE_MODEL_UPDATEBOARDCALLBACKREQUEST_H_

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
			class ALIBABACLOUD_LIVE_EXPORT UpdateBoardCallbackRequest : public RpcServiceRequest
			{

			public:
				UpdateBoardCallbackRequest();
				~UpdateBoardCallbackRequest();

				std::string getAuthKey()const;
				void setAuthKey(const std::string& authKey);
				int getCallbackEnable()const;
				void setCallbackEnable(int callbackEnable);
				std::string getCallbackEvents()const;
				void setCallbackEvents(const std::string& callbackEvents);
				long getOwnerId()const;
				void setOwnerId(long ownerId);
				std::string getCallbackUri()const;
				void setCallbackUri(const std::string& callbackUri);
				std::string getAppId()const;
				void setAppId(const std::string& appId);
				std::string getAuthSwitch()const;
				void setAuthSwitch(const std::string& authSwitch);

            private:
				std::string authKey_;
				int callbackEnable_;
				std::string callbackEvents_;
				long ownerId_;
				std::string callbackUri_;
				std::string appId_;
				std::string authSwitch_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_LIVE_MODEL_UPDATEBOARDCALLBACKREQUEST_H_