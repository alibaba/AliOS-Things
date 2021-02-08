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

#include <alibabacloud/cloudapi/model/DescribeApiGroupsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

DescribeApiGroupsResult::DescribeApiGroupsResult() :
	ServiceResult()
{}

DescribeApiGroupsResult::DescribeApiGroupsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeApiGroupsResult::~DescribeApiGroupsResult()
{}

void DescribeApiGroupsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allApiGroupAttributesNode = value["ApiGroupAttributes"]["ApiGroupAttribute"];
	for (auto valueApiGroupAttributesApiGroupAttribute : allApiGroupAttributesNode)
	{
		ApiGroupAttribute apiGroupAttributesObject;
		if(!valueApiGroupAttributesApiGroupAttribute["GroupId"].isNull())
			apiGroupAttributesObject.groupId = valueApiGroupAttributesApiGroupAttribute["GroupId"].asString();
		if(!valueApiGroupAttributesApiGroupAttribute["GroupName"].isNull())
			apiGroupAttributesObject.groupName = valueApiGroupAttributesApiGroupAttribute["GroupName"].asString();
		if(!valueApiGroupAttributesApiGroupAttribute["SubDomain"].isNull())
			apiGroupAttributesObject.subDomain = valueApiGroupAttributesApiGroupAttribute["SubDomain"].asString();
		if(!valueApiGroupAttributesApiGroupAttribute["Description"].isNull())
			apiGroupAttributesObject.description = valueApiGroupAttributesApiGroupAttribute["Description"].asString();
		if(!valueApiGroupAttributesApiGroupAttribute["CreatedTime"].isNull())
			apiGroupAttributesObject.createdTime = valueApiGroupAttributesApiGroupAttribute["CreatedTime"].asString();
		if(!valueApiGroupAttributesApiGroupAttribute["ModifiedTime"].isNull())
			apiGroupAttributesObject.modifiedTime = valueApiGroupAttributesApiGroupAttribute["ModifiedTime"].asString();
		if(!valueApiGroupAttributesApiGroupAttribute["RegionId"].isNull())
			apiGroupAttributesObject.regionId = valueApiGroupAttributesApiGroupAttribute["RegionId"].asString();
		if(!valueApiGroupAttributesApiGroupAttribute["TrafficLimit"].isNull())
			apiGroupAttributesObject.trafficLimit = std::stoi(valueApiGroupAttributesApiGroupAttribute["TrafficLimit"].asString());
		if(!valueApiGroupAttributesApiGroupAttribute["BillingStatus"].isNull())
			apiGroupAttributesObject.billingStatus = valueApiGroupAttributesApiGroupAttribute["BillingStatus"].asString();
		if(!valueApiGroupAttributesApiGroupAttribute["IllegalStatus"].isNull())
			apiGroupAttributesObject.illegalStatus = valueApiGroupAttributesApiGroupAttribute["IllegalStatus"].asString();
		if(!valueApiGroupAttributesApiGroupAttribute["InstanceId"].isNull())
			apiGroupAttributesObject.instanceId = valueApiGroupAttributesApiGroupAttribute["InstanceId"].asString();
		if(!valueApiGroupAttributesApiGroupAttribute["InstanceType"].isNull())
			apiGroupAttributesObject.instanceType = valueApiGroupAttributesApiGroupAttribute["InstanceType"].asString();
		if(!valueApiGroupAttributesApiGroupAttribute["HttpsPolicy"].isNull())
			apiGroupAttributesObject.httpsPolicy = valueApiGroupAttributesApiGroupAttribute["HttpsPolicy"].asString();
		apiGroupAttributes_.push_back(apiGroupAttributesObject);
	}
	if(!value["TotalCount"].isNull())
		totalCount_ = std::stoi(value["TotalCount"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["PageNumber"].isNull())
		pageNumber_ = std::stoi(value["PageNumber"].asString());

}

int DescribeApiGroupsResult::getTotalCount()const
{
	return totalCount_;
}

int DescribeApiGroupsResult::getPageSize()const
{
	return pageSize_;
}

int DescribeApiGroupsResult::getPageNumber()const
{
	return pageNumber_;
}

std::vector<DescribeApiGroupsResult::ApiGroupAttribute> DescribeApiGroupsResult::getApiGroupAttributes()const
{
	return apiGroupAttributes_;
}

