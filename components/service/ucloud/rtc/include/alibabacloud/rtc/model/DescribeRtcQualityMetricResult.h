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

#ifndef ALIBABACLOUD_RTC_MODEL_DESCRIBERTCQUALITYMETRICRESULT_H_
#define ALIBABACLOUD_RTC_MODEL_DESCRIBERTCQUALITYMETRICRESULT_H_

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
			class ALIBABACLOUD_RTC_EXPORT DescribeRtcQualityMetricResult : public ServiceResult
			{
			public:
				struct QualityMetric
				{
					struct SubInfo
					{
						struct User
						{
							std::string oS;
							std::string userId;
							std::string sDKVersion;
							std::string province;
						};
						struct DataItem
						{
							struct Fps
							{
								long small;
								long super;
								long large;
								long share;
							};
							struct Resolution
							{
								long small;
								long super;
								long large;
								long share;
							};
							struct BitRate
							{
								long small;
								long super;
								long large;
								long audio;
								long share;
							};
							struct Network
							{
								long rtt;
								float loss;
							};
							Network network;
							Fps fps;
							std::string time;
							BitRate bitRate;
							Resolution resolution;
						};
						struct EventsItem
						{
							std::string category;
							std::string description;
							long eventId;
							std::string time;
							std::string name;
							std::string result;
						};
						User user;
						std::vector<EventsItem> events;
						std::vector<DataItem> data;
					};
					struct PubInfo
					{
						struct User1
						{
							std::string oS;
							std::string userId;
							std::string sDKVersion;
							std::string province;
						};
						struct DataItem4
						{
							struct Fps5
							{
								long small;
								long super;
								long large;
								long share;
							};
							struct Resolution6
							{
								long small;
								long super;
								long large;
								long share;
							};
							struct BitRate7
							{
								long small;
								long super;
								long large;
								long audio;
								long share;
							};
							struct Network8
							{
								long rtt;
								float loss;
							};
							BitRate7 bitRate7;
							std::string time;
							Fps5 fps5;
							Resolution6 resolution6;
							Network8 network8;
						};
						struct EventsItem9
						{
							std::string category;
							std::string description;
							long eventId;
							std::string time;
							std::string name;
							std::string result;
						};
						std::vector<DataItem4> data2;
						User1 user1;
						std::vector<EventsItem9> events3;
					};
					SubInfo subInfo;
					PubInfo pubInfo;
				};


				DescribeRtcQualityMetricResult();
				explicit DescribeRtcQualityMetricResult(const std::string &payload);
				~DescribeRtcQualityMetricResult();
				QualityMetric getQualityMetric()const;

			protected:
				void parse(const std::string &payload);
			private:
				QualityMetric qualityMetric_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_RTC_MODEL_DESCRIBERTCQUALITYMETRICRESULT_H_