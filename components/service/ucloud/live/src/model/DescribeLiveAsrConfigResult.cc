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

#include <alibabacloud/live/model/DescribeLiveAsrConfigResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveAsrConfigResult::DescribeLiveAsrConfigResult() :
	ServiceResult()
{}

DescribeLiveAsrConfigResult::DescribeLiveAsrConfigResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveAsrConfigResult::~DescribeLiveAsrConfigResult()
{}

void DescribeLiveAsrConfigResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allLiveAsrConfigNode = value["LiveAsrConfig"]["LiveAsrConfigList"];
	for (auto valueLiveAsrConfigLiveAsrConfigList : allLiveAsrConfigNode)
	{
		LiveAsrConfigList liveAsrConfigObject;
		if(!valueLiveAsrConfigLiveAsrConfigList["DomainName"].isNull())
			liveAsrConfigObject.domainName = std::stoi(valueLiveAsrConfigLiveAsrConfigList["DomainName"].asString());
		if(!valueLiveAsrConfigLiveAsrConfigList["AppName"].isNull())
			liveAsrConfigObject.appName = valueLiveAsrConfigLiveAsrConfigList["AppName"].asString();
		if(!valueLiveAsrConfigLiveAsrConfigList["StreamName"].isNull())
			liveAsrConfigObject.streamName = valueLiveAsrConfigLiveAsrConfigList["StreamName"].asString();
		if(!valueLiveAsrConfigLiveAsrConfigList["Period"].isNull())
			liveAsrConfigObject.period = std::stoi(valueLiveAsrConfigLiveAsrConfigList["Period"].asString());
		if(!valueLiveAsrConfigLiveAsrConfigList["MnsTopic"].isNull())
			liveAsrConfigObject.mnsTopic = valueLiveAsrConfigLiveAsrConfigList["MnsTopic"].asString();
		if(!valueLiveAsrConfigLiveAsrConfigList["MnsRegion"].isNull())
			liveAsrConfigObject.mnsRegion = valueLiveAsrConfigLiveAsrConfigList["MnsRegion"].asString();
		if(!valueLiveAsrConfigLiveAsrConfigList["HttpCallbackURL"].isNull())
			liveAsrConfigObject.httpCallbackURL = valueLiveAsrConfigLiveAsrConfigList["HttpCallbackURL"].asString();
		liveAsrConfig_.push_back(liveAsrConfigObject);
	}

}

std::vector<DescribeLiveAsrConfigResult::LiveAsrConfigList> DescribeLiveAsrConfigResult::getLiveAsrConfig()const
{
	return liveAsrConfig_;
}

