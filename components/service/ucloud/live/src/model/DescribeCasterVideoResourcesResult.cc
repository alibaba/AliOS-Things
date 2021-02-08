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

#include <alibabacloud/live/model/DescribeCasterVideoResourcesResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeCasterVideoResourcesResult::DescribeCasterVideoResourcesResult() :
	ServiceResult()
{}

DescribeCasterVideoResourcesResult::DescribeCasterVideoResourcesResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeCasterVideoResourcesResult::~DescribeCasterVideoResourcesResult()
{}

void DescribeCasterVideoResourcesResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allVideoResourcesNode = value["VideoResources"]["VideoResource"];
	for (auto valueVideoResourcesVideoResource : allVideoResourcesNode)
	{
		VideoResource videoResourcesObject;
		if(!valueVideoResourcesVideoResource["MaterialId"].isNull())
			videoResourcesObject.materialId = valueVideoResourcesVideoResource["MaterialId"].asString();
		if(!valueVideoResourcesVideoResource["ResourceId"].isNull())
			videoResourcesObject.resourceId = valueVideoResourcesVideoResource["ResourceId"].asString();
		if(!valueVideoResourcesVideoResource["ResourceName"].isNull())
			videoResourcesObject.resourceName = valueVideoResourcesVideoResource["ResourceName"].asString();
		if(!valueVideoResourcesVideoResource["LocationId"].isNull())
			videoResourcesObject.locationId = valueVideoResourcesVideoResource["LocationId"].asString();
		if(!valueVideoResourcesVideoResource["LiveStreamUrl"].isNull())
			videoResourcesObject.liveStreamUrl = valueVideoResourcesVideoResource["LiveStreamUrl"].asString();
		if(!valueVideoResourcesVideoResource["RepeatNum"].isNull())
			videoResourcesObject.repeatNum = std::stoi(valueVideoResourcesVideoResource["RepeatNum"].asString());
		if(!valueVideoResourcesVideoResource["VodUrl"].isNull())
			videoResourcesObject.vodUrl = valueVideoResourcesVideoResource["VodUrl"].asString();
		if(!valueVideoResourcesVideoResource["BeginOffset"].isNull())
			videoResourcesObject.beginOffset = std::stoi(valueVideoResourcesVideoResource["BeginOffset"].asString());
		if(!valueVideoResourcesVideoResource["EndOffset"].isNull())
			videoResourcesObject.endOffset = std::stoi(valueVideoResourcesVideoResource["EndOffset"].asString());
		if(!valueVideoResourcesVideoResource["PtsCallbackInterval"].isNull())
			videoResourcesObject.ptsCallbackInterval = std::stoi(valueVideoResourcesVideoResource["PtsCallbackInterval"].asString());
		videoResources_.push_back(videoResourcesObject);
	}
	if(!value["Total"].isNull())
		total_ = std::stoi(value["Total"].asString());

}

int DescribeCasterVideoResourcesResult::getTotal()const
{
	return total_;
}

std::vector<DescribeCasterVideoResourcesResult::VideoResource> DescribeCasterVideoResourcesResult::getVideoResources()const
{
	return videoResources_;
}

