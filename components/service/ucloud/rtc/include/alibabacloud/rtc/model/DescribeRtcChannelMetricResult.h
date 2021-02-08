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

#ifndef ALIBABACLOUD_RTC_MODEL_DESCRIBERTCCHANNELMETRICRESULT_H_
#define ALIBABACLOUD_RTC_MODEL_DESCRIBERTCCHANNELMETRICRESULT_H_

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
			class ALIBABACLOUD_RTC_EXPORT DescribeRtcChannelMetricResult : public ServiceResult
			{
			public:
				struct ChannelMetricInfo
				{
					struct ChannelMetric
					{
						std::string endTime;
						int userCount;
						std::string startTime;
						int subUserCount;
						std::string channelId;
						int pubUserCount;
					};
					struct Duration
					{
						struct PubDuration
						{
							int video720;
							int video1080;
							int content;
							int video360;
							int audio;
						};
						struct SubDuration
						{
							int video720;
							int video1080;
							int content;
							int video360;
							int audio;
						};
						SubDuration subDuration;
						PubDuration pubDuration;
					};
					ChannelMetric channelMetric;
					Duration duration;
				};


				DescribeRtcChannelMetricResult();
				explicit DescribeRtcChannelMetricResult(const std::string &payload);
				~DescribeRtcChannelMetricResult();
				ChannelMetricInfo getChannelMetricInfo()const;

			protected:
				void parse(const std::string &payload);
			private:
				ChannelMetricInfo channelMetricInfo_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_RTC_MODEL_DESCRIBERTCCHANNELMETRICRESULT_H_