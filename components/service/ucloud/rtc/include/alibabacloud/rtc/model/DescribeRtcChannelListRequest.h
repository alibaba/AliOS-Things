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

#ifndef ALIBABACLOUD_RTC_MODEL_DESCRIBERTCCHANNELLISTREQUEST_H_
#define ALIBABACLOUD_RTC_MODEL_DESCRIBERTCCHANNELLISTREQUEST_H_

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
			class ALIBABACLOUD_RTC_EXPORT DescribeRtcChannelListRequest : public RpcServiceRequest
			{

			public:
				DescribeRtcChannelListRequest();
				~DescribeRtcChannelListRequest();

				std::string getSortType()const;
				void setSortType(const std::string& sortType);
				std::string getUserId()const;
				void setUserId(const std::string& userId);
				std::string getServiceArea()const;
				void setServiceArea(const std::string& serviceArea);
				long getPageSize()const;
				void setPageSize(long pageSize);
				std::string getShowLog()const;
				void setShowLog(const std::string& showLog);
				long getOwnerId()const;
				void setOwnerId(long ownerId);
				long getPageNo()const;
				void setPageNo(long pageNo);
				std::string getAppId()const;
				void setAppId(const std::string& appId);
				std::string getChannelId()const;
				void setChannelId(const std::string& channelId);
				std::string getTimePoint()const;
				void setTimePoint(const std::string& timePoint);

            private:
				std::string sortType_;
				std::string userId_;
				std::string serviceArea_;
				long pageSize_;
				std::string showLog_;
				long ownerId_;
				long pageNo_;
				std::string appId_;
				std::string channelId_;
				std::string timePoint_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_RTC_MODEL_DESCRIBERTCCHANNELLISTREQUEST_H_