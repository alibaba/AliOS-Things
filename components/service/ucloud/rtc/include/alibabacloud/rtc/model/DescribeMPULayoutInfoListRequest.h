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

#ifndef ALIBABACLOUD_RTC_MODEL_DESCRIBEMPULAYOUTINFOLISTREQUEST_H_
#define ALIBABACLOUD_RTC_MODEL_DESCRIBEMPULAYOUTINFOLISTREQUEST_H_

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
			class ALIBABACLOUD_RTC_EXPORT DescribeMPULayoutInfoListRequest : public RpcServiceRequest
			{

			public:
				DescribeMPULayoutInfoListRequest();
				~DescribeMPULayoutInfoListRequest();

				long getPageNum()const;
				void setPageNum(long pageNum);
				long getLayoutId()const;
				void setLayoutId(long layoutId);
				long getPageSize()const;
				void setPageSize(long pageSize);
				std::string getShowLog()const;
				void setShowLog(const std::string& showLog);
				long getOwnerId()const;
				void setOwnerId(long ownerId);
				std::string getAppId()const;
				void setAppId(const std::string& appId);
				std::string getName()const;
				void setName(const std::string& name);

            private:
				long pageNum_;
				long layoutId_;
				long pageSize_;
				std::string showLog_;
				long ownerId_;
				std::string appId_;
				std::string name_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_RTC_MODEL_DESCRIBEMPULAYOUTINFOLISTREQUEST_H_