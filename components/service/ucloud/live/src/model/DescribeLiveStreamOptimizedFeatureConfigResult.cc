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

#include <alibabacloud/live/model/DescribeLiveStreamOptimizedFeatureConfigResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveStreamOptimizedFeatureConfigResult::DescribeLiveStreamOptimizedFeatureConfigResult() :
	ServiceResult()
{}

DescribeLiveStreamOptimizedFeatureConfigResult::DescribeLiveStreamOptimizedFeatureConfigResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveStreamOptimizedFeatureConfigResult::~DescribeLiveStreamOptimizedFeatureConfigResult()
{}

void DescribeLiveStreamOptimizedFeatureConfigResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allLiveStreamOptimizedFeatureConfigListNode = value["LiveStreamOptimizedFeatureConfigList"]["LiveStreamOptimizedFeatureConfig"];
	for (auto valueLiveStreamOptimizedFeatureConfigListLiveStreamOptimizedFeatureConfig : allLiveStreamOptimizedFeatureConfigListNode)
	{
		LiveStreamOptimizedFeatureConfig liveStreamOptimizedFeatureConfigListObject;
		if(!valueLiveStreamOptimizedFeatureConfigListLiveStreamOptimizedFeatureConfig["DomainName"].isNull())
			liveStreamOptimizedFeatureConfigListObject.domainName = valueLiveStreamOptimizedFeatureConfigListLiveStreamOptimizedFeatureConfig["DomainName"].asString();
		if(!valueLiveStreamOptimizedFeatureConfigListLiveStreamOptimizedFeatureConfig["ConfigName"].isNull())
			liveStreamOptimizedFeatureConfigListObject.configName = valueLiveStreamOptimizedFeatureConfigListLiveStreamOptimizedFeatureConfig["ConfigName"].asString();
		if(!valueLiveStreamOptimizedFeatureConfigListLiveStreamOptimizedFeatureConfig["ConfigStatus"].isNull())
			liveStreamOptimizedFeatureConfigListObject.configStatus = valueLiveStreamOptimizedFeatureConfigListLiveStreamOptimizedFeatureConfig["ConfigStatus"].asString();
		if(!valueLiveStreamOptimizedFeatureConfigListLiveStreamOptimizedFeatureConfig["ConfigValue"].isNull())
			liveStreamOptimizedFeatureConfigListObject.configValue = valueLiveStreamOptimizedFeatureConfigListLiveStreamOptimizedFeatureConfig["ConfigValue"].asString();
		liveStreamOptimizedFeatureConfigList_.push_back(liveStreamOptimizedFeatureConfigListObject);
	}

}

std::vector<DescribeLiveStreamOptimizedFeatureConfigResult::LiveStreamOptimizedFeatureConfig> DescribeLiveStreamOptimizedFeatureConfigResult::getLiveStreamOptimizedFeatureConfigList()const
{
	return liveStreamOptimizedFeatureConfigList_;
}

