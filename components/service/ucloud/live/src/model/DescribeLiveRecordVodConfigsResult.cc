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

#include <alibabacloud/live/model/DescribeLiveRecordVodConfigsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveRecordVodConfigsResult::DescribeLiveRecordVodConfigsResult() :
	ServiceResult()
{}

DescribeLiveRecordVodConfigsResult::DescribeLiveRecordVodConfigsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveRecordVodConfigsResult::~DescribeLiveRecordVodConfigsResult()
{}

void DescribeLiveRecordVodConfigsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allLiveRecordVodConfigsNode = value["LiveRecordVodConfigs"]["LiveRecordVodConfig"];
	for (auto valueLiveRecordVodConfigsLiveRecordVodConfig : allLiveRecordVodConfigsNode)
	{
		LiveRecordVodConfig liveRecordVodConfigsObject;
		if(!valueLiveRecordVodConfigsLiveRecordVodConfig["CreateTime"].isNull())
			liveRecordVodConfigsObject.createTime = valueLiveRecordVodConfigsLiveRecordVodConfig["CreateTime"].asString();
		if(!valueLiveRecordVodConfigsLiveRecordVodConfig["DomainName"].isNull())
			liveRecordVodConfigsObject.domainName = valueLiveRecordVodConfigsLiveRecordVodConfig["DomainName"].asString();
		if(!valueLiveRecordVodConfigsLiveRecordVodConfig["AppName"].isNull())
			liveRecordVodConfigsObject.appName = valueLiveRecordVodConfigsLiveRecordVodConfig["AppName"].asString();
		if(!valueLiveRecordVodConfigsLiveRecordVodConfig["StreamName"].isNull())
			liveRecordVodConfigsObject.streamName = valueLiveRecordVodConfigsLiveRecordVodConfig["StreamName"].asString();
		if(!valueLiveRecordVodConfigsLiveRecordVodConfig["VodTranscodeGroupId"].isNull())
			liveRecordVodConfigsObject.vodTranscodeGroupId = valueLiveRecordVodConfigsLiveRecordVodConfig["VodTranscodeGroupId"].asString();
		if(!valueLiveRecordVodConfigsLiveRecordVodConfig["CycleDuration"].isNull())
			liveRecordVodConfigsObject.cycleDuration = std::stoi(valueLiveRecordVodConfigsLiveRecordVodConfig["CycleDuration"].asString());
		if(!valueLiveRecordVodConfigsLiveRecordVodConfig["AutoCompose"].isNull())
			liveRecordVodConfigsObject.autoCompose = valueLiveRecordVodConfigsLiveRecordVodConfig["AutoCompose"].asString();
		if(!valueLiveRecordVodConfigsLiveRecordVodConfig["ComposeVodTranscodeGroupId"].isNull())
			liveRecordVodConfigsObject.composeVodTranscodeGroupId = valueLiveRecordVodConfigsLiveRecordVodConfig["ComposeVodTranscodeGroupId"].asString();
		liveRecordVodConfigs_.push_back(liveRecordVodConfigsObject);
	}
	if(!value["PageNum"].isNull())
		pageNum_ = std::stoi(value["PageNum"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["Total"].isNull())
		total_ = value["Total"].asString();

}

std::vector<DescribeLiveRecordVodConfigsResult::LiveRecordVodConfig> DescribeLiveRecordVodConfigsResult::getLiveRecordVodConfigs()const
{
	return liveRecordVodConfigs_;
}

int DescribeLiveRecordVodConfigsResult::getPageNum()const
{
	return pageNum_;
}

int DescribeLiveRecordVodConfigsResult::getPageSize()const
{
	return pageSize_;
}

std::string DescribeLiveRecordVodConfigsResult::getTotal()const
{
	return total_;
}

