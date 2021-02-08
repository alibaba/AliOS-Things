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

#include <alibabacloud/live/model/DescribeLiveAudioAuditNotifyConfigResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveAudioAuditNotifyConfigResult::DescribeLiveAudioAuditNotifyConfigResult() :
	ServiceResult()
{}

DescribeLiveAudioAuditNotifyConfigResult::DescribeLiveAudioAuditNotifyConfigResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveAudioAuditNotifyConfigResult::~DescribeLiveAudioAuditNotifyConfigResult()
{}

void DescribeLiveAudioAuditNotifyConfigResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allLiveAudioAuditNotifyConfigListNode = value["LiveAudioAuditNotifyConfigList"]["LiveAudioAuditNotifyConfig"];
	for (auto valueLiveAudioAuditNotifyConfigListLiveAudioAuditNotifyConfig : allLiveAudioAuditNotifyConfigListNode)
	{
		LiveAudioAuditNotifyConfig liveAudioAuditNotifyConfigListObject;
		if(!valueLiveAudioAuditNotifyConfigListLiveAudioAuditNotifyConfig["DomainName"].isNull())
			liveAudioAuditNotifyConfigListObject.domainName = valueLiveAudioAuditNotifyConfigListLiveAudioAuditNotifyConfig["DomainName"].asString();
		if(!valueLiveAudioAuditNotifyConfigListLiveAudioAuditNotifyConfig["Callback"].isNull())
			liveAudioAuditNotifyConfigListObject.callback = valueLiveAudioAuditNotifyConfigListLiveAudioAuditNotifyConfig["Callback"].asString();
		if(!valueLiveAudioAuditNotifyConfigListLiveAudioAuditNotifyConfig["CallbackTemplate"].isNull())
			liveAudioAuditNotifyConfigListObject.callbackTemplate = valueLiveAudioAuditNotifyConfigListLiveAudioAuditNotifyConfig["CallbackTemplate"].asString();
		liveAudioAuditNotifyConfigList_.push_back(liveAudioAuditNotifyConfigListObject);
	}

}

std::vector<DescribeLiveAudioAuditNotifyConfigResult::LiveAudioAuditNotifyConfig> DescribeLiveAudioAuditNotifyConfigResult::getLiveAudioAuditNotifyConfigList()const
{
	return liveAudioAuditNotifyConfigList_;
}

