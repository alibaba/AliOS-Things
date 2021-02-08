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

#include <alibabacloud/live/model/DescribeLiveAudioAuditConfigResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveAudioAuditConfigResult::DescribeLiveAudioAuditConfigResult() :
	ServiceResult()
{}

DescribeLiveAudioAuditConfigResult::DescribeLiveAudioAuditConfigResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveAudioAuditConfigResult::~DescribeLiveAudioAuditConfigResult()
{}

void DescribeLiveAudioAuditConfigResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allLiveAudioAuditConfigListNode = value["LiveAudioAuditConfigList"]["LiveAudioAuditConfig"];
	for (auto valueLiveAudioAuditConfigListLiveAudioAuditConfig : allLiveAudioAuditConfigListNode)
	{
		LiveAudioAuditConfig liveAudioAuditConfigListObject;
		if(!valueLiveAudioAuditConfigListLiveAudioAuditConfig["DomainName"].isNull())
			liveAudioAuditConfigListObject.domainName = valueLiveAudioAuditConfigListLiveAudioAuditConfig["DomainName"].asString();
		if(!valueLiveAudioAuditConfigListLiveAudioAuditConfig["AppName"].isNull())
			liveAudioAuditConfigListObject.appName = valueLiveAudioAuditConfigListLiveAudioAuditConfig["AppName"].asString();
		if(!valueLiveAudioAuditConfigListLiveAudioAuditConfig["StreamName"].isNull())
			liveAudioAuditConfigListObject.streamName = valueLiveAudioAuditConfigListLiveAudioAuditConfig["StreamName"].asString();
		if(!valueLiveAudioAuditConfigListLiveAudioAuditConfig["BizType"].isNull())
			liveAudioAuditConfigListObject.bizType = valueLiveAudioAuditConfigListLiveAudioAuditConfig["BizType"].asString();
		auto allScenes = value["Scenes"]["scene"];
		for (auto value : allScenes)
			liveAudioAuditConfigListObject.scenes.push_back(value.asString());
		liveAudioAuditConfigList_.push_back(liveAudioAuditConfigListObject);
	}

}

std::vector<DescribeLiveAudioAuditConfigResult::LiveAudioAuditConfig> DescribeLiveAudioAuditConfigResult::getLiveAudioAuditConfigList()const
{
	return liveAudioAuditConfigList_;
}

