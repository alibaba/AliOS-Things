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

#include <alibabacloud/config/model/ListDiscoveredResourcesResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Config;
using namespace AlibabaCloud::Config::Model;

ListDiscoveredResourcesResult::ListDiscoveredResourcesResult() :
	ServiceResult()
{}

ListDiscoveredResourcesResult::ListDiscoveredResourcesResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListDiscoveredResourcesResult::~ListDiscoveredResourcesResult()
{}

void ListDiscoveredResourcesResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto discoveredResourceProfilesNode = value["DiscoveredResourceProfiles"];
	if(!discoveredResourceProfilesNode["PageNumber"].isNull())
		discoveredResourceProfiles_.pageNumber = std::stoi(discoveredResourceProfilesNode["PageNumber"].asString());
	if(!discoveredResourceProfilesNode["PageSize"].isNull())
		discoveredResourceProfiles_.pageSize = std::stoi(discoveredResourceProfilesNode["PageSize"].asString());
	if(!discoveredResourceProfilesNode["TotalCount"].isNull())
		discoveredResourceProfiles_.totalCount = std::stoi(discoveredResourceProfilesNode["TotalCount"].asString());
	auto allDiscoveredResourceProfileListNode = discoveredResourceProfilesNode["DiscoveredResourceProfileList"]["DiscoveredResourceProfile"];
	for (auto discoveredResourceProfilesNodeDiscoveredResourceProfileListDiscoveredResourceProfile : allDiscoveredResourceProfileListNode)
	{
		DiscoveredResourceProfiles::DiscoveredResourceProfile discoveredResourceProfileObject;
		if(!discoveredResourceProfilesNodeDiscoveredResourceProfileListDiscoveredResourceProfile["AccountId"].isNull())
			discoveredResourceProfileObject.accountId = std::stol(discoveredResourceProfilesNodeDiscoveredResourceProfileListDiscoveredResourceProfile["AccountId"].asString());
		if(!discoveredResourceProfilesNodeDiscoveredResourceProfileListDiscoveredResourceProfile["Region"].isNull())
			discoveredResourceProfileObject.region = discoveredResourceProfilesNodeDiscoveredResourceProfileListDiscoveredResourceProfile["Region"].asString();
		if(!discoveredResourceProfilesNodeDiscoveredResourceProfileListDiscoveredResourceProfile["ResourceCreationTime"].isNull())
			discoveredResourceProfileObject.resourceCreationTime = std::stol(discoveredResourceProfilesNodeDiscoveredResourceProfileListDiscoveredResourceProfile["ResourceCreationTime"].asString());
		if(!discoveredResourceProfilesNodeDiscoveredResourceProfileListDiscoveredResourceProfile["ResourceDeleted"].isNull())
			discoveredResourceProfileObject.resourceDeleted = std::stoi(discoveredResourceProfilesNodeDiscoveredResourceProfileListDiscoveredResourceProfile["ResourceDeleted"].asString());
		if(!discoveredResourceProfilesNodeDiscoveredResourceProfileListDiscoveredResourceProfile["ResourceId"].isNull())
			discoveredResourceProfileObject.resourceId = discoveredResourceProfilesNodeDiscoveredResourceProfileListDiscoveredResourceProfile["ResourceId"].asString();
		if(!discoveredResourceProfilesNodeDiscoveredResourceProfileListDiscoveredResourceProfile["ResourceName"].isNull())
			discoveredResourceProfileObject.resourceName = discoveredResourceProfilesNodeDiscoveredResourceProfileListDiscoveredResourceProfile["ResourceName"].asString();
		if(!discoveredResourceProfilesNodeDiscoveredResourceProfileListDiscoveredResourceProfile["ResourceStatus"].isNull())
			discoveredResourceProfileObject.resourceStatus = discoveredResourceProfilesNodeDiscoveredResourceProfileListDiscoveredResourceProfile["ResourceStatus"].asString();
		if(!discoveredResourceProfilesNodeDiscoveredResourceProfileListDiscoveredResourceProfile["ResourceType"].isNull())
			discoveredResourceProfileObject.resourceType = discoveredResourceProfilesNodeDiscoveredResourceProfileListDiscoveredResourceProfile["ResourceType"].asString();
		if(!discoveredResourceProfilesNodeDiscoveredResourceProfileListDiscoveredResourceProfile["Tags"].isNull())
			discoveredResourceProfileObject.tags = discoveredResourceProfilesNodeDiscoveredResourceProfileListDiscoveredResourceProfile["Tags"].asString();
		discoveredResourceProfiles_.discoveredResourceProfileList.push_back(discoveredResourceProfileObject);
	}

}

ListDiscoveredResourcesResult::DiscoveredResourceProfiles ListDiscoveredResourcesResult::getDiscoveredResourceProfiles()const
{
	return discoveredResourceProfiles_;
}

