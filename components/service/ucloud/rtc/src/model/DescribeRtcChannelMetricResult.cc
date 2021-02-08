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

#include <alibabacloud/rtc/model/DescribeRtcChannelMetricResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Rtc;
using namespace AlibabaCloud::Rtc::Model;

DescribeRtcChannelMetricResult::DescribeRtcChannelMetricResult() :
	ServiceResult()
{}

DescribeRtcChannelMetricResult::DescribeRtcChannelMetricResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeRtcChannelMetricResult::~DescribeRtcChannelMetricResult()
{}

void DescribeRtcChannelMetricResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto channelMetricInfoNode = value["ChannelMetricInfo"];
	auto channelMetricNode = channelMetricInfoNode["ChannelMetric"];
	if(!channelMetricNode["ChannelId"].isNull())
		channelMetricInfo_.channelMetric.channelId = channelMetricNode["ChannelId"].asString();
	if(!channelMetricNode["UserCount"].isNull())
		channelMetricInfo_.channelMetric.userCount = std::stoi(channelMetricNode["UserCount"].asString());
	if(!channelMetricNode["PubUserCount"].isNull())
		channelMetricInfo_.channelMetric.pubUserCount = std::stoi(channelMetricNode["PubUserCount"].asString());
	if(!channelMetricNode["SubUserCount"].isNull())
		channelMetricInfo_.channelMetric.subUserCount = std::stoi(channelMetricNode["SubUserCount"].asString());
	if(!channelMetricNode["StartTime"].isNull())
		channelMetricInfo_.channelMetric.startTime = channelMetricNode["StartTime"].asString();
	if(!channelMetricNode["EndTime"].isNull())
		channelMetricInfo_.channelMetric.endTime = channelMetricNode["EndTime"].asString();
	auto durationNode = channelMetricInfoNode["Duration"];
	auto pubDurationNode = durationNode["PubDuration"];
	if(!pubDurationNode["Audio"].isNull())
		channelMetricInfo_.duration.pubDuration.audio = std::stoi(pubDurationNode["Audio"].asString());
	if(!pubDurationNode["Video360"].isNull())
		channelMetricInfo_.duration.pubDuration.video360 = std::stoi(pubDurationNode["Video360"].asString());
	if(!pubDurationNode["Video720"].isNull())
		channelMetricInfo_.duration.pubDuration.video720 = std::stoi(pubDurationNode["Video720"].asString());
	if(!pubDurationNode["Video1080"].isNull())
		channelMetricInfo_.duration.pubDuration.video1080 = std::stoi(pubDurationNode["Video1080"].asString());
	if(!pubDurationNode["Content"].isNull())
		channelMetricInfo_.duration.pubDuration.content = std::stoi(pubDurationNode["Content"].asString());
	auto subDurationNode = durationNode["SubDuration"];
	if(!subDurationNode["Audio"].isNull())
		channelMetricInfo_.duration.subDuration.audio = std::stoi(subDurationNode["Audio"].asString());
	if(!subDurationNode["Video360"].isNull())
		channelMetricInfo_.duration.subDuration.video360 = std::stoi(subDurationNode["Video360"].asString());
	if(!subDurationNode["Video720"].isNull())
		channelMetricInfo_.duration.subDuration.video720 = std::stoi(subDurationNode["Video720"].asString());
	if(!subDurationNode["Video1080"].isNull())
		channelMetricInfo_.duration.subDuration.video1080 = std::stoi(subDurationNode["Video1080"].asString());
	if(!subDurationNode["Content"].isNull())
		channelMetricInfo_.duration.subDuration.content = std::stoi(subDurationNode["Content"].asString());

}

DescribeRtcChannelMetricResult::ChannelMetricInfo DescribeRtcChannelMetricResult::getChannelMetricInfo()const
{
	return channelMetricInfo_;
}

