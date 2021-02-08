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

#include <alibabacloud/ivision/model/DescribeFaceGroupsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Ivision;
using namespace AlibabaCloud::Ivision::Model;

DescribeFaceGroupsResult::DescribeFaceGroupsResult() :
	ServiceResult()
{}

DescribeFaceGroupsResult::DescribeFaceGroupsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeFaceGroupsResult::~DescribeFaceGroupsResult()
{}

void DescribeFaceGroupsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allGroupsNode = value["Groups"]["Group"];
	for (auto valueGroupsGroup : allGroupsNode)
	{
		Group groupsObject;
		if(!valueGroupsGroup["GroupId"].isNull())
			groupsObject.groupId = valueGroupsGroup["GroupId"].asString();
		if(!valueGroupsGroup["Name"].isNull())
			groupsObject.name = valueGroupsGroup["Name"].asString();
		if(!valueGroupsGroup["CreationTime"].isNull())
			groupsObject.creationTime = valueGroupsGroup["CreationTime"].asString();
		groups_.push_back(groupsObject);
	}
	if(!value["CurrentPage"].isNull())
		currentPage_ = std::stol(value["CurrentPage"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stol(value["PageSize"].asString());
	if(!value["NextPageToken"].isNull())
		nextPageToken_ = value["NextPageToken"].asString();
	if(!value["TotalNum"].isNull())
		totalNum_ = std::stol(value["TotalNum"].asString());

}

long DescribeFaceGroupsResult::getTotalNum()const
{
	return totalNum_;
}

long DescribeFaceGroupsResult::getPageSize()const
{
	return pageSize_;
}

long DescribeFaceGroupsResult::getCurrentPage()const
{
	return currentPage_;
}

std::vector<DescribeFaceGroupsResult::Group> DescribeFaceGroupsResult::getGroups()const
{
	return groups_;
}

std::string DescribeFaceGroupsResult::getNextPageToken()const
{
	return nextPageToken_;
}

