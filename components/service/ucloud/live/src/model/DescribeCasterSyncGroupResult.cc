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

#include <alibabacloud/live/model/DescribeCasterSyncGroupResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeCasterSyncGroupResult::DescribeCasterSyncGroupResult() :
	ServiceResult()
{}

DescribeCasterSyncGroupResult::DescribeCasterSyncGroupResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeCasterSyncGroupResult::~DescribeCasterSyncGroupResult()
{}

void DescribeCasterSyncGroupResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allSyncGroupsNode = value["SyncGroups"]["SyncGroup"];
	for (auto valueSyncGroupsSyncGroup : allSyncGroupsNode)
	{
		SyncGroup syncGroupsObject;
		if(!valueSyncGroupsSyncGroup["Mode"].isNull())
			syncGroupsObject.mode = std::stoi(valueSyncGroupsSyncGroup["Mode"].asString());
		if(!valueSyncGroupsSyncGroup["HostResourceId"].isNull())
			syncGroupsObject.hostResourceId = valueSyncGroupsSyncGroup["HostResourceId"].asString();
		auto allResourceIds = value["ResourceIds"]["ResourceId"];
		for (auto value : allResourceIds)
			syncGroupsObject.resourceIds.push_back(value.asString());
		syncGroups_.push_back(syncGroupsObject);
	}
	if(!value["CasterId"].isNull())
		casterId_ = value["CasterId"].asString();

}

std::vector<DescribeCasterSyncGroupResult::SyncGroup> DescribeCasterSyncGroupResult::getSyncGroups()const
{
	return syncGroups_;
}

std::string DescribeCasterSyncGroupResult::getCasterId()const
{
	return casterId_;
}

