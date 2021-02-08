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

#include <alibabacloud/cloudapi/model/DescribeApisByAppResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

DescribeApisByAppResult::DescribeApisByAppResult() :
	ServiceResult()
{}

DescribeApisByAppResult::DescribeApisByAppResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeApisByAppResult::~DescribeApisByAppResult()
{}

void DescribeApisByAppResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allAppApiRelationInfosNode = value["AppApiRelationInfos"]["AppApiRelationInfo"];
	for (auto valueAppApiRelationInfosAppApiRelationInfo : allAppApiRelationInfosNode)
	{
		AppApiRelationInfo appApiRelationInfosObject;
		if(!valueAppApiRelationInfosAppApiRelationInfo["RegionId"].isNull())
			appApiRelationInfosObject.regionId = valueAppApiRelationInfosAppApiRelationInfo["RegionId"].asString();
		if(!valueAppApiRelationInfosAppApiRelationInfo["GroupId"].isNull())
			appApiRelationInfosObject.groupId = valueAppApiRelationInfosAppApiRelationInfo["GroupId"].asString();
		if(!valueAppApiRelationInfosAppApiRelationInfo["GroupName"].isNull())
			appApiRelationInfosObject.groupName = valueAppApiRelationInfosAppApiRelationInfo["GroupName"].asString();
		if(!valueAppApiRelationInfosAppApiRelationInfo["StageName"].isNull())
			appApiRelationInfosObject.stageName = valueAppApiRelationInfosAppApiRelationInfo["StageName"].asString();
		if(!valueAppApiRelationInfosAppApiRelationInfo["Operator"].isNull())
			appApiRelationInfosObject._operator = valueAppApiRelationInfosAppApiRelationInfo["Operator"].asString();
		if(!valueAppApiRelationInfosAppApiRelationInfo["ApiId"].isNull())
			appApiRelationInfosObject.apiId = valueAppApiRelationInfosAppApiRelationInfo["ApiId"].asString();
		if(!valueAppApiRelationInfosAppApiRelationInfo["ApiName"].isNull())
			appApiRelationInfosObject.apiName = valueAppApiRelationInfosAppApiRelationInfo["ApiName"].asString();
		if(!valueAppApiRelationInfosAppApiRelationInfo["AuthorizationSource"].isNull())
			appApiRelationInfosObject.authorizationSource = valueAppApiRelationInfosAppApiRelationInfo["AuthorizationSource"].asString();
		if(!valueAppApiRelationInfosAppApiRelationInfo["Description"].isNull())
			appApiRelationInfosObject.description = valueAppApiRelationInfosAppApiRelationInfo["Description"].asString();
		if(!valueAppApiRelationInfosAppApiRelationInfo["CreatedTime"].isNull())
			appApiRelationInfosObject.createdTime = valueAppApiRelationInfosAppApiRelationInfo["CreatedTime"].asString();
		appApiRelationInfos_.push_back(appApiRelationInfosObject);
	}
	if(!value["TotalCount"].isNull())
		totalCount_ = std::stoi(value["TotalCount"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["PageNumber"].isNull())
		pageNumber_ = std::stoi(value["PageNumber"].asString());

}

std::vector<DescribeApisByAppResult::AppApiRelationInfo> DescribeApisByAppResult::getAppApiRelationInfos()const
{
	return appApiRelationInfos_;
}

int DescribeApisByAppResult::getTotalCount()const
{
	return totalCount_;
}

int DescribeApisByAppResult::getPageSize()const
{
	return pageSize_;
}

int DescribeApisByAppResult::getPageNumber()const
{
	return pageNumber_;
}

