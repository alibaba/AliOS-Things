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

#include <alibabacloud/rtc/model/DescribeRtcChannelDetailResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Rtc;
using namespace AlibabaCloud::Rtc::Model;

DescribeRtcChannelDetailResult::DescribeRtcChannelDetailResult() :
	ServiceResult()
{}

DescribeRtcChannelDetailResult::DescribeRtcChannelDetailResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeRtcChannelDetailResult::~DescribeRtcChannelDetailResult()
{}

void DescribeRtcChannelDetailResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allChannelInfoNode = value["ChannelInfo"]["Detail"];
	for (auto valueChannelInfoDetail : allChannelInfoNode)
	{
		Detail channelInfoObject;
		if(!valueChannelInfoDetail["Sid"].isNull())
			channelInfoObject.sid = valueChannelInfoDetail["Sid"].asString();
		if(!valueChannelInfoDetail["Uid"].isNull())
			channelInfoObject.uid = valueChannelInfoDetail["Uid"].asString();
		if(!valueChannelInfoDetail["JoinTime"].isNull())
			channelInfoObject.joinTime = valueChannelInfoDetail["JoinTime"].asString();
		if(!valueChannelInfoDetail["LeaveTime"].isNull())
			channelInfoObject.leaveTime = valueChannelInfoDetail["LeaveTime"].asString();
		if(!valueChannelInfoDetail["SdkVersion"].isNull())
			channelInfoObject.sdkVersion = valueChannelInfoDetail["SdkVersion"].asString();
		if(!valueChannelInfoDetail["Platform"].isNull())
			channelInfoObject.platform = valueChannelInfoDetail["Platform"].asString();
		if(!valueChannelInfoDetail["OS"].isNull())
			channelInfoObject.oS = valueChannelInfoDetail["OS"].asString();
		if(!valueChannelInfoDetail["DeviceType"].isNull())
			channelInfoObject.deviceType = valueChannelInfoDetail["DeviceType"].asString();
		channelInfo_.push_back(channelInfoObject);
	}
	if(!value["ChannelId"].isNull())
		channelId_ = value["ChannelId"].asString();
	if(!value["PageSize"].isNull())
		pageSize_ = std::stol(value["PageSize"].asString());
	if(!value["PageNo"].isNull())
		pageNo_ = std::stol(value["PageNo"].asString());
	if(!value["TotalCnt"].isNull())
		totalCnt_ = std::stol(value["TotalCnt"].asString());

}

long DescribeRtcChannelDetailResult::getPageSize()const
{
	return pageSize_;
}

long DescribeRtcChannelDetailResult::getTotalCnt()const
{
	return totalCnt_;
}

std::vector<DescribeRtcChannelDetailResult::Detail> DescribeRtcChannelDetailResult::getChannelInfo()const
{
	return channelInfo_;
}

long DescribeRtcChannelDetailResult::getPageNo()const
{
	return pageNo_;
}

std::string DescribeRtcChannelDetailResult::getChannelId()const
{
	return channelId_;
}

