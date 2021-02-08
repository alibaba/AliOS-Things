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

#include <alibabacloud/live/model/DescribeLiveStreamsControlHistoryResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveStreamsControlHistoryResult::DescribeLiveStreamsControlHistoryResult() :
	ServiceResult()
{}

DescribeLiveStreamsControlHistoryResult::DescribeLiveStreamsControlHistoryResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveStreamsControlHistoryResult::~DescribeLiveStreamsControlHistoryResult()
{}

void DescribeLiveStreamsControlHistoryResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allControlInfoNode = value["ControlInfo"]["LiveStreamControlInfo"];
	for (auto valueControlInfoLiveStreamControlInfo : allControlInfoNode)
	{
		LiveStreamControlInfo controlInfoObject;
		if(!valueControlInfoLiveStreamControlInfo["StreamName"].isNull())
			controlInfoObject.streamName = valueControlInfoLiveStreamControlInfo["StreamName"].asString();
		if(!valueControlInfoLiveStreamControlInfo["ClientIP"].isNull())
			controlInfoObject.clientIP = valueControlInfoLiveStreamControlInfo["ClientIP"].asString();
		if(!valueControlInfoLiveStreamControlInfo["Action"].isNull())
			controlInfoObject.action = valueControlInfoLiveStreamControlInfo["Action"].asString();
		if(!valueControlInfoLiveStreamControlInfo["TimeStamp"].isNull())
			controlInfoObject.timeStamp = valueControlInfoLiveStreamControlInfo["TimeStamp"].asString();
		controlInfo_.push_back(controlInfoObject);
	}

}

std::vector<DescribeLiveStreamsControlHistoryResult::LiveStreamControlInfo> DescribeLiveStreamsControlHistoryResult::getControlInfo()const
{
	return controlInfo_;
}

