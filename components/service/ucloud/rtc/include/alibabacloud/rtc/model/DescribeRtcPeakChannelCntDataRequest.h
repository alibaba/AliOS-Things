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

#ifndef ALIBABACLOUD_RTC_MODEL_DESCRIBERTCPEAKCHANNELCNTDATAREQUEST_H_
#define ALIBABACLOUD_RTC_MODEL_DESCRIBERTCPEAKCHANNELCNTDATAREQUEST_H_

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
			class ALIBABACLOUD_RTC_EXPORT DescribeRtcPeakChannelCntDataRequest : public RpcServiceRequest
			{

			public:
				DescribeRtcPeakChannelCntDataRequest();
				~DescribeRtcPeakChannelCntDataRequest();

				std::string getStartTime()const;
				void setStartTime(const std::string& startTime);
				std::string getServiceArea()const;
				void setServiceArea(const std::string& serviceArea);
				std::string getShowLog()const;
				void setShowLog(const std::string& showLog);
				std::string getEndTime()const;
				void setEndTime(const std::string& endTime);
				long getOwnerId()const;
				void setOwnerId(long ownerId);
				std::string getAppId()const;
				void setAppId(const std::string& appId);
				std::string getInterval()const;
				void setInterval(const std::string& interval);

            private:
				std::string startTime_;
				std::string serviceArea_;
				std::string showLog_;
				std::string endTime_;
				long ownerId_;
				std::string appId_;
				std::string interval_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_RTC_MODEL_DESCRIBERTCPEAKCHANNELCNTDATAREQUEST_H_