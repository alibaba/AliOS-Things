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

#include <alibabacloud/rtc/model/DescribeRtcChannelListResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Rtc;
using namespace AlibabaCloud::Rtc::Model;

DescribeRtcChannelListResult::DescribeRtcChannelListResult() :
	ServiceResult()
{}

DescribeRtcChannelListResult::DescribeRtcChannelListResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeRtcChannelListResult::~DescribeRtcChannelListResult()
{}

void DescribeRtcChannelListResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allChannelListNode = value["ChannelList"]["ChannelListItem"];
	for (auto valueChannelListChannelListItem : allChannelListNode)
	{
		ChannelListItem channelListObject;
		if(!valueChannelListChannelListItem["ChannelId"].isNull())
			channelListObject.channelId = valueChannelListChannelListItem["ChannelId"].asString();
		if(!valueChannelListChannelListItem["StartTime"].isNull())
			channelListObject.startTime = valueChannelListChannelListItem["StartTime"].asString();
		if(!valueChannelListChannelListItem["EndTime"].isNull())
			channelListObject.endTime = valueChannelListChannelListItem["EndTime"].asString();
		if(!valueChannelListChannelListItem["TotalUserCnt"].isNull())
			channelListObject.totalUserCnt = std::stol(valueChannelListChannelListItem["TotalUserCnt"].asString());
		auto allCallArea = value["CallArea"]["CallArea"];
		for (auto value : allCallArea)
			channelListObject.callArea.push_back(value.asString());
		channelList_.push_back(channelListObject);
	}
	if(!value["PageSize"].isNull())
		pageSize_ = std::stol(value["PageSize"].asString());
	if(!value["PageNo"].isNull())
		pageNo_ = std::stol(value["PageNo"].asString());
	if(!value["TotalCnt"].isNull())
		totalCnt_ = std::stol(value["TotalCnt"].asString());

}

long DescribeRtcChannelListResult::getPageSize()const
{
	return pageSize_;
}

long DescribeRtcChannelListResult::getTotalCnt()const
{
	return totalCnt_;
}

long DescribeRtcChannelListResult::getPageNo()const
{
	return pageNo_;
}

std::vector<DescribeRtcChannelListResult::ChannelListItem> DescribeRtcChannelListResult::getChannelList()const
{
	return channelList_;
}

