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

#include <alibabacloud/rtc/model/DescribeRtcChannelsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Rtc;
using namespace AlibabaCloud::Rtc::Model;

DescribeRtcChannelsResult::DescribeRtcChannelsResult() :
	ServiceResult()
{}

DescribeRtcChannelsResult::DescribeRtcChannelsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeRtcChannelsResult::~DescribeRtcChannelsResult()
{}

void DescribeRtcChannelsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allChannelsNode = value["Channels"]["Channel"];
	for (auto valueChannelsChannel : allChannelsNode)
	{
		Channel channelsObject;
		if(!valueChannelsChannel["ChannelId"].isNull())
			channelsObject.channelId = valueChannelsChannel["ChannelId"].asString();
		if(!valueChannelsChannel["StartTime"].isNull())
			channelsObject.startTime = valueChannelsChannel["StartTime"].asString();
		if(!valueChannelsChannel["EndTime"].isNull())
			channelsObject.endTime = valueChannelsChannel["EndTime"].asString();
		if(!valueChannelsChannel["Finished"].isNull())
			channelsObject.finished = valueChannelsChannel["Finished"].asString() == "true";
		channels_.push_back(channelsObject);
	}
	if(!value["PageSize"].isNull())
		pageSize_ = std::stol(value["PageSize"].asString());
	if(!value["PageNo"].isNull())
		pageNo_ = std::stol(value["PageNo"].asString());
	if(!value["TotalCnt"].isNull())
		totalCnt_ = std::stol(value["TotalCnt"].asString());

}

long DescribeRtcChannelsResult::getPageSize()const
{
	return pageSize_;
}

long DescribeRtcChannelsResult::getTotalCnt()const
{
	return totalCnt_;
}

long DescribeRtcChannelsResult::getPageNo()const
{
	return pageNo_;
}

std::vector<DescribeRtcChannelsResult::Channel> DescribeRtcChannelsResult::getChannels()const
{
	return channels_;
}

