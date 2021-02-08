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

#include <alibabacloud/cloudapi/model/DescribeDeployedApisResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

DescribeDeployedApisResult::DescribeDeployedApisResult() :
	ServiceResult()
{}

DescribeDeployedApisResult::DescribeDeployedApisResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeDeployedApisResult::~DescribeDeployedApisResult()
{}

void DescribeDeployedApisResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allDeployedApisNode = value["DeployedApis"]["DeployedApiItem"];
	for (auto valueDeployedApisDeployedApiItem : allDeployedApisNode)
	{
		DeployedApiItem deployedApisObject;
		if(!valueDeployedApisDeployedApiItem["RegionId"].isNull())
			deployedApisObject.regionId = valueDeployedApisDeployedApiItem["RegionId"].asString();
		if(!valueDeployedApisDeployedApiItem["ApiId"].isNull())
			deployedApisObject.apiId = valueDeployedApisDeployedApiItem["ApiId"].asString();
		if(!valueDeployedApisDeployedApiItem["ApiName"].isNull())
			deployedApisObject.apiName = valueDeployedApisDeployedApiItem["ApiName"].asString();
		if(!valueDeployedApisDeployedApiItem["GroupId"].isNull())
			deployedApisObject.groupId = valueDeployedApisDeployedApiItem["GroupId"].asString();
		if(!valueDeployedApisDeployedApiItem["GroupName"].isNull())
			deployedApisObject.groupName = valueDeployedApisDeployedApiItem["GroupName"].asString();
		if(!valueDeployedApisDeployedApiItem["StageName"].isNull())
			deployedApisObject.stageName = valueDeployedApisDeployedApiItem["StageName"].asString();
		if(!valueDeployedApisDeployedApiItem["Visibility"].isNull())
			deployedApisObject.visibility = valueDeployedApisDeployedApiItem["Visibility"].asString();
		if(!valueDeployedApisDeployedApiItem["Description"].isNull())
			deployedApisObject.description = valueDeployedApisDeployedApiItem["Description"].asString();
		if(!valueDeployedApisDeployedApiItem["DeployedTime"].isNull())
			deployedApisObject.deployedTime = valueDeployedApisDeployedApiItem["DeployedTime"].asString();
		deployedApis_.push_back(deployedApisObject);
	}
	if(!value["TotalCount"].isNull())
		totalCount_ = std::stoi(value["TotalCount"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["PageNumber"].isNull())
		pageNumber_ = std::stoi(value["PageNumber"].asString());

}

int DescribeDeployedApisResult::getTotalCount()const
{
	return totalCount_;
}

int DescribeDeployedApisResult::getPageSize()const
{
	return pageSize_;
}

int DescribeDeployedApisResult::getPageNumber()const
{
	return pageNumber_;
}

std::vector<DescribeDeployedApisResult::DeployedApiItem> DescribeDeployedApisResult::getDeployedApis()const
{
	return deployedApis_;
}

