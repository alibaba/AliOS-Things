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

#ifndef ALIBABACLOUD_RTC_MODEL_DESCRIBECHANNELUSERSRESULT_H_
#define ALIBABACLOUD_RTC_MODEL_DESCRIBECHANNELUSERSRESULT_H_

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
			class ALIBABACLOUD_RTC_EXPORT DescribeChannelUsersResult : public ServiceResult
			{
			public:


				DescribeChannelUsersResult();
				explicit DescribeChannelUsersResult(const std::string &payload);
				~DescribeChannelUsersResult();
				std::vector<std::string> getInteractiveUserList()const;
				int getLiveUserNum()const;
				int getChannelProfile()const;
				int getInteractiveUserNum()const;
				std::vector<std::string> getUserList()const;
				std::vector<std::string> getLiveUserList()const;
				int getTimestamp()const;
				bool getIsChannelExist()const;
				int getCommTotalNum()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::vector<std::string> interactiveUserList_;
				int liveUserNum_;
				int channelProfile_;
				int interactiveUserNum_;
				std::vector<std::string> userList_;
				std::vector<std::string> liveUserList_;
				int timestamp_;
				bool isChannelExist_;
				int commTotalNum_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_RTC_MODEL_DESCRIBECHANNELUSERSRESULT_H_