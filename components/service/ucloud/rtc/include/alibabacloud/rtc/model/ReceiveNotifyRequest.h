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

#ifndef ALIBABACLOUD_RTC_MODEL_RECEIVENOTIFYREQUEST_H_
#define ALIBABACLOUD_RTC_MODEL_RECEIVENOTIFYREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/rtc/RtcExport.h>

namespace AlibabaCloud
{
	namespace Rtc
	{
		namespace Model
		{
			class ALIBABACLOUD_RTC_EXPORT ReceiveNotifyRequest : public RpcServiceRequest
			{

			public:
				ReceiveNotifyRequest();
				~ReceiveNotifyRequest();

				std::string getTraceId()const;
				void setTraceId(const std::string& traceId);
				std::string getContent()const;
				void setContent(const std::string& content);
				std::string getEvent()const;
				void setEvent(const std::string& event);
				std::string getShowLog()const;
				void setShowLog(const std::string& showLog);
				long getOwnerId()const;
				void setOwnerId(long ownerId);
				std::string getContentType()const;
				void setContentType(const std::string& contentType);
				std::string getBizId()const;
				void setBizId(const std::string& bizId);

            private:
				std::string traceId_;
				std::string content_;
				std::string event_;
				std::string showLog_;
				long ownerId_;
				std::string contentType_;
				std::string bizId_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_RTC_MODEL_RECEIVENOTIFYREQUEST_H_