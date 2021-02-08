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

#include <alibabacloud/live/model/DescribeCasterChannelsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeCasterChannelsResult::DescribeCasterChannelsResult() :
	ServiceResult()
{}

DescribeCasterChannelsResult::DescribeCasterChannelsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeCasterChannelsResult::~DescribeCasterChannelsResult()
{}

void DescribeCasterChannelsResult::parse(const std::string &payload)
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
		if(!valueChannelsChannel["ResourceId"].isNull())
			channelsObject.resourceId = valueChannelsChannel["ResourceId"].asString();
		if(!valueChannelsChannel["StreamUrl"].isNull())
			channelsObject.streamUrl = valueChannelsChannel["StreamUrl"].asString();
		if(!valueChannelsChannel["RtmpUrl"].isNull())
			channelsObject.rtmpUrl = valueChannelsChannel["RtmpUrl"].asString();
		channels_.push_back(channelsObject);
	}
	if(!value["Total"].isNull())
		total_ = std::stoi(value["Total"].asString());

}

int DescribeCasterChannelsResult::getTotal()const
{
	return total_;
}

std::vector<DescribeCasterChannelsResult::Channel> DescribeCasterChannelsResult::getChannels()const
{
	return channels_;
}

