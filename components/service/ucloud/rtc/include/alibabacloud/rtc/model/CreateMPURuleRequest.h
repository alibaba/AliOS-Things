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

#ifndef ALIBABACLOUD_RTC_MODEL_CREATEMPURULEREQUEST_H_
#define ALIBABACLOUD_RTC_MODEL_CREATEMPURULEREQUEST_H_

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
			class ALIBABACLOUD_RTC_EXPORT CreateMPURuleRequest : public RpcServiceRequest
			{

			public:
				CreateMPURuleRequest();
				~CreateMPURuleRequest();

				int getBackgroundColor()const;
				void setBackgroundColor(int backgroundColor);
				int getCropMode()const;
				void setCropMode(int cropMode);
				std::string getChannelPrefix()const;
				void setChannelPrefix(const std::string& channelPrefix);
				std::string getTaskProfile()const;
				void setTaskProfile(const std::string& taskProfile);
				std::vector<long> getLayoutIds()const;
				void setLayoutIds(const std::vector<long>& layoutIds);
				std::string getShowLog()const;
				void setShowLog(const std::string& showLog);
				std::string getPlayDomain()const;
				void setPlayDomain(const std::string& playDomain);
				long getOwnerId()const;
				void setOwnerId(long ownerId);
				std::string getAppId()const;
				void setAppId(const std::string& appId);
				int getMediaEncode()const;
				void setMediaEncode(int mediaEncode);
				std::string getCallBack()const;
				void setCallBack(const std::string& callBack);

            private:
				int backgroundColor_;
				int cropMode_;
				std::string channelPrefix_;
				std::string taskProfile_;
				std::vector<long> layoutIds_;
				std::string showLog_;
				std::string playDomain_;
				long ownerId_;
				std::string appId_;
				int mediaEncode_;
				std::string callBack_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_RTC_MODEL_CREATEMPURULEREQUEST_H_