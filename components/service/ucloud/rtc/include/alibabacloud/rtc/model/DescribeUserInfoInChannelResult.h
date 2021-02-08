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

#ifndef ALIBABACLOUD_RTC_MODEL_DESCRIBEUSERINFOINCHANNELRESULT_H_
#define ALIBABACLOUD_RTC_MODEL_DESCRIBEUSERINFOINCHANNELRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/rtc/RtcExport.h>

namespace AlibabaCloud
{
	namespace Rtc
	{
		namespace Model
		{
			class ALIBABACLOUD_RTC_EXPORT DescribeUserInfoInChannelResult : public ServiceResult
			{
			public:
				struct PropertyItem
				{
					int role;
					int join;
					std::string session;
				};


				DescribeUserInfoInChannelResult();
				explicit DescribeUserInfoInChannelResult(const std::string &payload);
				~DescribeUserInfoInChannelResult();
				bool getIsInChannel()const;
				std::vector<PropertyItem> getProperty()const;
				int getTimestamp()const;
				bool getIsChannelExist()const;

			protected:
				void parse(const std::string &payload);
			private:
				bool isInChannel_;
				std::vector<PropertyItem> property_;
				int timestamp_;
				bool isChannelExist_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_RTC_MODEL_DESCRIBEUSERINFOINCHANNELRESULT_H_