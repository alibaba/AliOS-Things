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

#include <alibabacloud/cloudapi/model/DescribeVpcAccessesResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

DescribeVpcAccessesResult::DescribeVpcAccessesResult() :
	ServiceResult()
{}

DescribeVpcAccessesResult::DescribeVpcAccessesResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeVpcAccessesResult::~DescribeVpcAccessesResult()
{}

void DescribeVpcAccessesResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allVpcAccessAttributesNode = value["VpcAccessAttributes"]["VpcAccessAttribute"];
	for (auto valueVpcAccessAttributesVpcAccessAttribute : allVpcAccessAttributesNode)
	{
		VpcAccessAttribute vpcAccessAttributesObject;
		if(!valueVpcAccessAttributesVpcAccessAttribute["VpcId"].isNull())
			vpcAccessAttributesObject.vpcId = valueVpcAccessAttributesVpcAccessAttribute["VpcId"].asString();
		if(!valueVpcAccessAttributesVpcAccessAttribute["InstanceId"].isNull())
			vpcAccessAttributesObject.instanceId = valueVpcAccessAttributesVpcAccessAttribute["InstanceId"].asString();
		if(!valueVpcAccessAttributesVpcAccessAttribute["CreatedTime"].isNull())
			vpcAccessAttributesObject.createdTime = valueVpcAccessAttributesVpcAccessAttribute["CreatedTime"].asString();
		if(!valueVpcAccessAttributesVpcAccessAttribute["Port"].isNull())
			vpcAccessAttributesObject.port = std::stoi(valueVpcAccessAttributesVpcAccessAttribute["Port"].asString());
		if(!valueVpcAccessAttributesVpcAccessAttribute["RegionId"].isNull())
			vpcAccessAttributesObject.regionId = valueVpcAccessAttributesVpcAccessAttribute["RegionId"].asString();
		if(!valueVpcAccessAttributesVpcAccessAttribute["Name"].isNull())
			vpcAccessAttributesObject.name = valueVpcAccessAttributesVpcAccessAttribute["Name"].asString();
		vpcAccessAttributes_.push_back(vpcAccessAttributesObject);
	}
	if(!value["TotalCount"].isNull())
		totalCount_ = std::stoi(value["TotalCount"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["PageNumber"].isNull())
		pageNumber_ = std::stoi(value["PageNumber"].asString());

}

int DescribeVpcAccessesResult::getTotalCount()const
{
	return totalCount_;
}

int DescribeVpcAccessesResult::getPageSize()const
{
	return pageSize_;
}

int DescribeVpcAccessesResult::getPageNumber()const
{
	return pageNumber_;
}

std::vector<DescribeVpcAccessesResult::VpcAccessAttribute> DescribeVpcAccessesResult::getVpcAccessAttributes()const
{
	return vpcAccessAttributes_;
}

