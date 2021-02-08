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

#include <alibabacloud/cloudapi/model/DescribeAuthorizedApisResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

DescribeAuthorizedApisResult::DescribeAuthorizedApisResult() :
	ServiceResult()
{}

DescribeAuthorizedApisResult::DescribeAuthorizedApisResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeAuthorizedApisResult::~DescribeAuthorizedApisResult()
{}

void DescribeAuthorizedApisResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allAuthorizedApisNode = value["AuthorizedApis"]["AuthorizedApi"];
	for (auto valueAuthorizedApisAuthorizedApi : allAuthorizedApisNode)
	{
		AuthorizedApi authorizedApisObject;
		if(!valueAuthorizedApisAuthorizedApi["RegionId"].isNull())
			authorizedApisObject.regionId = valueAuthorizedApisAuthorizedApi["RegionId"].asString();
		if(!valueAuthorizedApisAuthorizedApi["GroupId"].isNull())
			authorizedApisObject.groupId = valueAuthorizedApisAuthorizedApi["GroupId"].asString();
		if(!valueAuthorizedApisAuthorizedApi["GroupName"].isNull())
			authorizedApisObject.groupName = valueAuthorizedApisAuthorizedApi["GroupName"].asString();
		if(!valueAuthorizedApisAuthorizedApi["StageName"].isNull())
			authorizedApisObject.stageName = valueAuthorizedApisAuthorizedApi["StageName"].asString();
		if(!valueAuthorizedApisAuthorizedApi["Operator"].isNull())
			authorizedApisObject._operator = valueAuthorizedApisAuthorizedApi["Operator"].asString();
		if(!valueAuthorizedApisAuthorizedApi["ApiId"].isNull())
			authorizedApisObject.apiId = valueAuthorizedApisAuthorizedApi["ApiId"].asString();
		if(!valueAuthorizedApisAuthorizedApi["ApiName"].isNull())
			authorizedApisObject.apiName = valueAuthorizedApisAuthorizedApi["ApiName"].asString();
		if(!valueAuthorizedApisAuthorizedApi["AuthorizationSource"].isNull())
			authorizedApisObject.authorizationSource = valueAuthorizedApisAuthorizedApi["AuthorizationSource"].asString();
		if(!valueAuthorizedApisAuthorizedApi["Description"].isNull())
			authorizedApisObject.description = valueAuthorizedApisAuthorizedApi["Description"].asString();
		if(!valueAuthorizedApisAuthorizedApi["AuthorizedTime"].isNull())
			authorizedApisObject.authorizedTime = valueAuthorizedApisAuthorizedApi["AuthorizedTime"].asString();
		if(!valueAuthorizedApisAuthorizedApi["AuthVaildTime"].isNull())
			authorizedApisObject.authVaildTime = valueAuthorizedApisAuthorizedApi["AuthVaildTime"].asString();
		authorizedApis_.push_back(authorizedApisObject);
	}
	if(!value["TotalCount"].isNull())
		totalCount_ = std::stoi(value["TotalCount"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["PageNumber"].isNull())
		pageNumber_ = std::stoi(value["PageNumber"].asString());

}

int DescribeAuthorizedApisResult::getTotalCount()const
{
	return totalCount_;
}

int DescribeAuthorizedApisResult::getPageSize()const
{
	return pageSize_;
}

int DescribeAuthorizedApisResult::getPageNumber()const
{
	return pageNumber_;
}

std::vector<DescribeAuthorizedApisResult::AuthorizedApi> DescribeAuthorizedApisResult::getAuthorizedApis()const
{
	return authorizedApis_;
}

