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

#include <alibabacloud/live/model/DescribeLiveStreamsPublishListResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveStreamsPublishListResult::DescribeLiveStreamsPublishListResult() :
	ServiceResult()
{}

DescribeLiveStreamsPublishListResult::DescribeLiveStreamsPublishListResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveStreamsPublishListResult::~DescribeLiveStreamsPublishListResult()
{}

void DescribeLiveStreamsPublishListResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allPublishInfoNode = value["PublishInfo"]["LiveStreamPublishInfo"];
	for (auto valuePublishInfoLiveStreamPublishInfo : allPublishInfoNode)
	{
		LiveStreamPublishInfo publishInfoObject;
		if(!valuePublishInfoLiveStreamPublishInfo["DomainName"].isNull())
			publishInfoObject.domainName = valuePublishInfoLiveStreamPublishInfo["DomainName"].asString();
		if(!valuePublishInfoLiveStreamPublishInfo["AppName"].isNull())
			publishInfoObject.appName = valuePublishInfoLiveStreamPublishInfo["AppName"].asString();
		if(!valuePublishInfoLiveStreamPublishInfo["StreamName"].isNull())
			publishInfoObject.streamName = valuePublishInfoLiveStreamPublishInfo["StreamName"].asString();
		if(!valuePublishInfoLiveStreamPublishInfo["StreamUrl"].isNull())
			publishInfoObject.streamUrl = valuePublishInfoLiveStreamPublishInfo["StreamUrl"].asString();
		if(!valuePublishInfoLiveStreamPublishInfo["PublishTime"].isNull())
			publishInfoObject.publishTime = valuePublishInfoLiveStreamPublishInfo["PublishTime"].asString();
		if(!valuePublishInfoLiveStreamPublishInfo["StopTime"].isNull())
			publishInfoObject.stopTime = valuePublishInfoLiveStreamPublishInfo["StopTime"].asString();
		if(!valuePublishInfoLiveStreamPublishInfo["PublishUrl"].isNull())
			publishInfoObject.publishUrl = valuePublishInfoLiveStreamPublishInfo["PublishUrl"].asString();
		if(!valuePublishInfoLiveStreamPublishInfo["ClientAddr"].isNull())
			publishInfoObject.clientAddr = valuePublishInfoLiveStreamPublishInfo["ClientAddr"].asString();
		if(!valuePublishInfoLiveStreamPublishInfo["EdgeNodeAddr"].isNull())
			publishInfoObject.edgeNodeAddr = valuePublishInfoLiveStreamPublishInfo["EdgeNodeAddr"].asString();
		if(!valuePublishInfoLiveStreamPublishInfo["PublishDomain"].isNull())
			publishInfoObject.publishDomain = valuePublishInfoLiveStreamPublishInfo["PublishDomain"].asString();
		if(!valuePublishInfoLiveStreamPublishInfo["PublishType"].isNull())
			publishInfoObject.publishType = valuePublishInfoLiveStreamPublishInfo["PublishType"].asString();
		if(!valuePublishInfoLiveStreamPublishInfo["Transcoded"].isNull())
			publishInfoObject.transcoded = valuePublishInfoLiveStreamPublishInfo["Transcoded"].asString();
		if(!valuePublishInfoLiveStreamPublishInfo["TranscodeId"].isNull())
			publishInfoObject.transcodeId = valuePublishInfoLiveStreamPublishInfo["TranscodeId"].asString();
		publishInfo_.push_back(publishInfoObject);
	}
	if(!value["PageNum"].isNull())
		pageNum_ = std::stoi(value["PageNum"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["TotalNum"].isNull())
		totalNum_ = std::stoi(value["TotalNum"].asString());
	if(!value["TotalPage"].isNull())
		totalPage_ = std::stoi(value["TotalPage"].asString());

}

int DescribeLiveStreamsPublishListResult::getTotalNum()const
{
	return totalNum_;
}

int DescribeLiveStreamsPublishListResult::getPageNum()const
{
	return pageNum_;
}

int DescribeLiveStreamsPublishListResult::getPageSize()const
{
	return pageSize_;
}

int DescribeLiveStreamsPublishListResult::getTotalPage()const
{
	return totalPage_;
}

std::vector<DescribeLiveStreamsPublishListResult::LiveStreamPublishInfo> DescribeLiveStreamsPublishListResult::getPublishInfo()const
{
	return publishInfo_;
}

