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

#include <alibabacloud/cloudapi/model/DescribeIpControlPolicyItemsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

DescribeIpControlPolicyItemsResult::DescribeIpControlPolicyItemsResult() :
	ServiceResult()
{}

DescribeIpControlPolicyItemsResult::DescribeIpControlPolicyItemsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeIpControlPolicyItemsResult::~DescribeIpControlPolicyItemsResult()
{}

void DescribeIpControlPolicyItemsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allIpControlPolicyItemsNode = value["IpControlPolicyItems"]["IpControlPolicyItem"];
	for (auto valueIpControlPolicyItemsIpControlPolicyItem : allIpControlPolicyItemsNode)
	{
		IpControlPolicyItem ipControlPolicyItemsObject;
		if(!valueIpControlPolicyItemsIpControlPolicyItem["AppId"].isNull())
			ipControlPolicyItemsObject.appId = valueIpControlPolicyItemsIpControlPolicyItem["AppId"].asString();
		if(!valueIpControlPolicyItemsIpControlPolicyItem["CidrIp"].isNull())
			ipControlPolicyItemsObject.cidrIp = valueIpControlPolicyItemsIpControlPolicyItem["CidrIp"].asString();
		if(!valueIpControlPolicyItemsIpControlPolicyItem["PolicyItemId"].isNull())
			ipControlPolicyItemsObject.policyItemId = valueIpControlPolicyItemsIpControlPolicyItem["PolicyItemId"].asString();
		if(!valueIpControlPolicyItemsIpControlPolicyItem["CreateTime"].isNull())
			ipControlPolicyItemsObject.createTime = valueIpControlPolicyItemsIpControlPolicyItem["CreateTime"].asString();
		if(!valueIpControlPolicyItemsIpControlPolicyItem["ModifiedTime"].isNull())
			ipControlPolicyItemsObject.modifiedTime = valueIpControlPolicyItemsIpControlPolicyItem["ModifiedTime"].asString();
		ipControlPolicyItems_.push_back(ipControlPolicyItemsObject);
	}
	if(!value["TotalCount"].isNull())
		totalCount_ = std::stoi(value["TotalCount"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["PageNumber"].isNull())
		pageNumber_ = std::stoi(value["PageNumber"].asString());

}

int DescribeIpControlPolicyItemsResult::getTotalCount()const
{
	return totalCount_;
}

int DescribeIpControlPolicyItemsResult::getPageSize()const
{
	return pageSize_;
}

std::vector<DescribeIpControlPolicyItemsResult::IpControlPolicyItem> DescribeIpControlPolicyItemsResult::getIpControlPolicyItems()const
{
	return ipControlPolicyItems_;
}

int DescribeIpControlPolicyItemsResult::getPageNumber()const
{
	return pageNumber_;
}

