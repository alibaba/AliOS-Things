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

#ifndef ALIBABACLOUD_LIVE_MODEL_ADDTRANCODESEIREQUEST_H_
#define ALIBABACLOUD_LIVE_MODEL_ADDTRANCODESEIREQUEST_H_

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
			class ALIBABACLOUD_LIVE_EXPORT AddTrancodeSEIRequest : public RpcServiceRequest
			{

			public:
				AddTrancodeSEIRequest();
				~AddTrancodeSEIRequest();

				std::string getPattern()const;
				void setPattern(const std::string& pattern);
				std::string getAppName()const;
				void setAppName(const std::string& appName);
				int getRepeat()const;
				void setRepeat(int repeat);
				std::string getText()const;
				void setText(const std::string& text);
				std::string getStreamName()const;
				void setStreamName(const std::string& streamName);
				std::string getDomainName()const;
				void setDomainName(const std::string& domainName);
				long getOwnerId()const;
				void setOwnerId(long ownerId);
				int getDelay()const;
				void setDelay(int delay);

            private:
				std::string pattern_;
				std::string appName_;
				int repeat_;
				std::string text_;
				std::string streamName_;
				std::string domainName_;
				long ownerId_;
				int delay_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_LIVE_MODEL_ADDTRANCODESEIREQUEST_H_