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

#include <alibabacloud/live/model/DescribeLiveTagResourcesResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveTagResourcesResult::DescribeLiveTagResourcesResult() :
	ServiceResult()
{}

DescribeLiveTagResourcesResult::DescribeLiveTagResourcesResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveTagResourcesResult::~DescribeLiveTagResourcesResult()
{}

void DescribeLiveTagResourcesResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allTagResourcesNode = value["TagResources"]["TagResource"];
	for (auto valueTagResourcesTagResource : allTagResourcesNode)
	{
		TagResource tagResourcesObject;
		if(!valueTagResourcesTagResource["ResourceId"].isNull())
			tagResourcesObject.resourceId = valueTagResourcesTagResource["ResourceId"].asString();
		auto allTagNode = allTagResourcesNode["Tag"]["TagItem"];
		for (auto allTagResourcesNodeTagTagItem : allTagNode)
		{
			TagResource::TagItem tagObject;
			if(!allTagResourcesNodeTagTagItem["Key"].isNull())
				tagObject.key = allTagResourcesNodeTagTagItem["Key"].asString();
			if(!allTagResourcesNodeTagTagItem["Value"].isNull())
				tagObject.value = allTagResourcesNodeTagTagItem["Value"].asString();
			tagResourcesObject.tag.push_back(tagObject);
		}
		tagResources_.push_back(tagResourcesObject);
	}

}

std::vector<DescribeLiveTagResourcesResult::TagResource> DescribeLiveTagResourcesResult::getTagResources()const
{
	return tagResources_;
}

