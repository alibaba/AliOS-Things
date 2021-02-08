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

#include <alibabacloud/imm/model/DescribeRegionsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

DescribeRegionsResult::DescribeRegionsResult() :
	ServiceResult()
{}

DescribeRegionsResult::DescribeRegionsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeRegionsResult::~DescribeRegionsResult()
{}

void DescribeRegionsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto regionsNode = value["Regions"];
	auto allRegionNode = regionsNode["Region"]["RegionItem"];
	for (auto regionsNodeRegionRegionItem : allRegionNode)
	{
		Regions::RegionItem regionItemObject;
		if(!regionsNodeRegionRegionItem["RegionId"].isNull())
			regionItemObject.regionId = regionsNodeRegionRegionItem["RegionId"].asString();
		auto allProjectTypes = value["ProjectTypes"]["ProjectTypes"];
		for (auto value : allProjectTypes)
			regionItemObject.projectTypes.push_back(value.asString());
		regions_.region.push_back(regionItemObject);
	}

}

DescribeRegionsResult::Regions DescribeRegionsResult::getRegions()const
{
	return regions_;
}

