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

#include <alibabacloud/config/model/DescribeDiscoveredResourceResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Config;
using namespace AlibabaCloud::Config::Model;

DescribeDiscoveredResourceResult::DescribeDiscoveredResourceResult() :
	ServiceResult()
{}

DescribeDiscoveredResourceResult::DescribeDiscoveredResourceResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeDiscoveredResourceResult::~DescribeDiscoveredResourceResult()
{}

void DescribeDiscoveredResourceResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto discoveredResourceDetailNode = value["DiscoveredResourceDetail"];
	if(!discoveredResourceDetailNode["AccountId"].isNull())
		discoveredResourceDetail_.accountId = std::stol(discoveredResourceDetailNode["AccountId"].asString());
	if(!discoveredResourceDetailNode["ResourceId"].isNull())
		discoveredResourceDetail_.resourceId = discoveredResourceDetailNode["ResourceId"].asString();
	if(!discoveredResourceDetailNode["ResourceType"].isNull())
		discoveredResourceDetail_.resourceType = discoveredResourceDetailNode["ResourceType"].asString();
	if(!discoveredResourceDetailNode["ResourceName"].isNull())
		discoveredResourceDetail_.resourceName = discoveredResourceDetailNode["ResourceName"].asString();
	if(!discoveredResourceDetailNode["Region"].isNull())
		discoveredResourceDetail_.region = discoveredResourceDetailNode["Region"].asString();
	if(!discoveredResourceDetailNode["AvailabilityZone"].isNull())
		discoveredResourceDetail_.availabilityZone = discoveredResourceDetailNode["AvailabilityZone"].asString();
	if(!discoveredResourceDetailNode["ResourceCreationTime"].isNull())
		discoveredResourceDetail_.resourceCreationTime = std::stol(discoveredResourceDetailNode["ResourceCreationTime"].asString());
	if(!discoveredResourceDetailNode["ResourceStatus"].isNull())
		discoveredResourceDetail_.resourceStatus = discoveredResourceDetailNode["ResourceStatus"].asString();
	if(!discoveredResourceDetailNode["ResourceDeleted"].isNull())
		discoveredResourceDetail_.resourceDeleted = std::stoi(discoveredResourceDetailNode["ResourceDeleted"].asString());
	if(!discoveredResourceDetailNode["Tags"].isNull())
		discoveredResourceDetail_.tags = discoveredResourceDetailNode["Tags"].asString();
	if(!discoveredResourceDetailNode["Configuration"].isNull())
		discoveredResourceDetail_.configuration = discoveredResourceDetailNode["Configuration"].asString();

}

DescribeDiscoveredResourceResult::DiscoveredResourceDetail DescribeDiscoveredResourceResult::getDiscoveredResourceDetail()const
{
	return discoveredResourceDetail_;
}

