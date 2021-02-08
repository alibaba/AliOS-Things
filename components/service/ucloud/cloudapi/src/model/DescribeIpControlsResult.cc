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

#include <alibabacloud/cloudapi/model/DescribeIpControlsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

DescribeIpControlsResult::DescribeIpControlsResult() :
	ServiceResult()
{}

DescribeIpControlsResult::DescribeIpControlsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeIpControlsResult::~DescribeIpControlsResult()
{}

void DescribeIpControlsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allIpControlInfosNode = value["IpControlInfos"]["IpControlInfo"];
	for (auto valueIpControlInfosIpControlInfo : allIpControlInfosNode)
	{
		IpControlInfo ipControlInfosObject;
		if(!valueIpControlInfosIpControlInfo["IpControlId"].isNull())
			ipControlInfosObject.ipControlId = valueIpControlInfosIpControlInfo["IpControlId"].asString();
		if(!valueIpControlInfosIpControlInfo["IpControlName"].isNull())
			ipControlInfosObject.ipControlName = valueIpControlInfosIpControlInfo["IpControlName"].asString();
		if(!valueIpControlInfosIpControlInfo["IpControlType"].isNull())
			ipControlInfosObject.ipControlType = valueIpControlInfosIpControlInfo["IpControlType"].asString();
		if(!valueIpControlInfosIpControlInfo["Description"].isNull())
			ipControlInfosObject.description = valueIpControlInfosIpControlInfo["Description"].asString();
		if(!valueIpControlInfosIpControlInfo["CreateTime"].isNull())
			ipControlInfosObject.createTime = valueIpControlInfosIpControlInfo["CreateTime"].asString();
		if(!valueIpControlInfosIpControlInfo["ModifiedTime"].isNull())
			ipControlInfosObject.modifiedTime = valueIpControlInfosIpControlInfo["ModifiedTime"].asString();
		if(!valueIpControlInfosIpControlInfo["RegionId"].isNull())
			ipControlInfosObject.regionId = valueIpControlInfosIpControlInfo["RegionId"].asString();
		ipControlInfos_.push_back(ipControlInfosObject);
	}
	if(!value["TotalCount"].isNull())
		totalCount_ = std::stoi(value["TotalCount"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["PageNumber"].isNull())
		pageNumber_ = std::stoi(value["PageNumber"].asString());

}

int DescribeIpControlsResult::getTotalCount()const
{
	return totalCount_;
}

int DescribeIpControlsResult::getPageSize()const
{
	return pageSize_;
}

int DescribeIpControlsResult::getPageNumber()const
{
	return pageNumber_;
}

std::vector<DescribeIpControlsResult::IpControlInfo> DescribeIpControlsResult::getIpControlInfos()const
{
	return ipControlInfos_;
}

