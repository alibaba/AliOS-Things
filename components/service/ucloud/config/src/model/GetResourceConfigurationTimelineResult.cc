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

#include <alibabacloud/config/model/GetResourceConfigurationTimelineResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Config;
using namespace AlibabaCloud::Config::Model;

GetResourceConfigurationTimelineResult::GetResourceConfigurationTimelineResult() :
	ServiceResult()
{}

GetResourceConfigurationTimelineResult::GetResourceConfigurationTimelineResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetResourceConfigurationTimelineResult::~GetResourceConfigurationTimelineResult()
{}

void GetResourceConfigurationTimelineResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto resourceConfigurationTimelineNode = value["ResourceConfigurationTimeline"];
	if(!resourceConfigurationTimelineNode["NextToken"].isNull())
		resourceConfigurationTimeline_.nextToken = resourceConfigurationTimelineNode["NextToken"].asString();
	if(!resourceConfigurationTimelineNode["Limit"].isNull())
		resourceConfigurationTimeline_.limit = std::stoi(resourceConfigurationTimelineNode["Limit"].asString());
	if(!resourceConfigurationTimelineNode["TotalCount"].isNull())
		resourceConfigurationTimeline_.totalCount = std::stol(resourceConfigurationTimelineNode["TotalCount"].asString());
	auto allConfigurationListNode = resourceConfigurationTimelineNode["ConfigurationList"]["ConfigurationListItem"];
	for (auto resourceConfigurationTimelineNodeConfigurationListConfigurationListItem : allConfigurationListNode)
	{
		ResourceConfigurationTimeline::ConfigurationListItem configurationListItemObject;
		if(!resourceConfigurationTimelineNodeConfigurationListConfigurationListItem["AccountId"].isNull())
			configurationListItemObject.accountId = std::stol(resourceConfigurationTimelineNodeConfigurationListConfigurationListItem["AccountId"].asString());
		if(!resourceConfigurationTimelineNodeConfigurationListConfigurationListItem["AvailabilityZone"].isNull())
			configurationListItemObject.availabilityZone = resourceConfigurationTimelineNodeConfigurationListConfigurationListItem["AvailabilityZone"].asString();
		if(!resourceConfigurationTimelineNodeConfigurationListConfigurationListItem["CaptureTime"].isNull())
			configurationListItemObject.captureTime = resourceConfigurationTimelineNodeConfigurationListConfigurationListItem["CaptureTime"].asString();
		if(!resourceConfigurationTimelineNodeConfigurationListConfigurationListItem["ConfigurationDiff"].isNull())
			configurationListItemObject.configurationDiff = resourceConfigurationTimelineNodeConfigurationListConfigurationListItem["ConfigurationDiff"].asString();
		if(!resourceConfigurationTimelineNodeConfigurationListConfigurationListItem["Region"].isNull())
			configurationListItemObject.region = resourceConfigurationTimelineNodeConfigurationListConfigurationListItem["Region"].asString();
		if(!resourceConfigurationTimelineNodeConfigurationListConfigurationListItem["Relationship"].isNull())
			configurationListItemObject.relationship = resourceConfigurationTimelineNodeConfigurationListConfigurationListItem["Relationship"].asString();
		if(!resourceConfigurationTimelineNodeConfigurationListConfigurationListItem["RelationshipDiff"].isNull())
			configurationListItemObject.relationshipDiff = resourceConfigurationTimelineNodeConfigurationListConfigurationListItem["RelationshipDiff"].asString();
		if(!resourceConfigurationTimelineNodeConfigurationListConfigurationListItem["ResourceCreateTime"].isNull())
			configurationListItemObject.resourceCreateTime = resourceConfigurationTimelineNodeConfigurationListConfigurationListItem["ResourceCreateTime"].asString();
		if(!resourceConfigurationTimelineNodeConfigurationListConfigurationListItem["ResourceId"].isNull())
			configurationListItemObject.resourceId = resourceConfigurationTimelineNodeConfigurationListConfigurationListItem["ResourceId"].asString();
		if(!resourceConfigurationTimelineNodeConfigurationListConfigurationListItem["ResourceName"].isNull())
			configurationListItemObject.resourceName = resourceConfigurationTimelineNodeConfigurationListConfigurationListItem["ResourceName"].asString();
		if(!resourceConfigurationTimelineNodeConfigurationListConfigurationListItem["ResourceType"].isNull())
			configurationListItemObject.resourceType = resourceConfigurationTimelineNodeConfigurationListConfigurationListItem["ResourceType"].asString();
		if(!resourceConfigurationTimelineNodeConfigurationListConfigurationListItem["Tags"].isNull())
			configurationListItemObject.tags = resourceConfigurationTimelineNodeConfigurationListConfigurationListItem["Tags"].asString();
		if(!resourceConfigurationTimelineNodeConfigurationListConfigurationListItem["ResourceEventType"].isNull())
			configurationListItemObject.resourceEventType = resourceConfigurationTimelineNodeConfigurationListConfigurationListItem["ResourceEventType"].asString();
		resourceConfigurationTimeline_.configurationList.push_back(configurationListItemObject);
	}

}

GetResourceConfigurationTimelineResult::ResourceConfigurationTimeline GetResourceConfigurationTimelineResult::getResourceConfigurationTimeline()const
{
	return resourceConfigurationTimeline_;
}

