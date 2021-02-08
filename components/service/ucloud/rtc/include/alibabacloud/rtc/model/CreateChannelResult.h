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

#ifndef ALIBABACLOUD_RTC_MODEL_CREATECHANNELRESULT_H_
#define ALIBABACLOUD_RTC_MODEL_CREATECHANNELRESULT_H_

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
			class ALIBABACLOUD_RTC_EXPORT CreateChannelResult : public ServiceResult
			{
			public:


				CreateChannelResult();
				explicit CreateChannelResult(const std::string &payload);
				~CreateChannelResult();
				std::string getNonce()const;
				std::string getChannelKey()const;
				int getTimestamp()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::string nonce_;
				std::string channelKey_;
				int timestamp_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_RTC_MODEL_CREATECHANNELRESULT_H_