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

#include <alibabacloud/cloudapi/model/DescribeAuthorizedAppsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

DescribeAuthorizedAppsResult::DescribeAuthorizedAppsResult() :
	ServiceResult()
{}

DescribeAuthorizedAppsResult::DescribeAuthorizedAppsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeAuthorizedAppsResult::~DescribeAuthorizedAppsResult()
{}

void DescribeAuthorizedAppsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allAuthorizedAppsNode = value["AuthorizedApps"]["AuthorizedApp"];
	for (auto valueAuthorizedAppsAuthorizedApp : allAuthorizedAppsNode)
	{
		AuthorizedApp authorizedAppsObject;
		if(!valueAuthorizedAppsAuthorizedApp["StageName"].isNull())
			authorizedAppsObject.stageName = valueAuthorizedAppsAuthorizedApp["StageName"].asString();
		if(!valueAuthorizedAppsAuthorizedApp["AppId"].isNull())
			authorizedAppsObject.appId = std::stol(valueAuthorizedAppsAuthorizedApp["AppId"].asString());
		if(!valueAuthorizedAppsAuthorizedApp["AppName"].isNull())
			authorizedAppsObject.appName = valueAuthorizedAppsAuthorizedApp["AppName"].asString();
		if(!valueAuthorizedAppsAuthorizedApp["Operator"].isNull())
			authorizedAppsObject._operator = valueAuthorizedAppsAuthorizedApp["Operator"].asString();
		if(!valueAuthorizedAppsAuthorizedApp["AuthorizationSource"].isNull())
			authorizedAppsObject.authorizationSource = valueAuthorizedAppsAuthorizedApp["AuthorizationSource"].asString();
		if(!valueAuthorizedAppsAuthorizedApp["Description"].isNull())
			authorizedAppsObject.description = valueAuthorizedAppsAuthorizedApp["Description"].asString();
		if(!valueAuthorizedAppsAuthorizedApp["AuthorizedTime"].isNull())
			authorizedAppsObject.authorizedTime = valueAuthorizedAppsAuthorizedApp["AuthorizedTime"].asString();
		if(!valueAuthorizedAppsAuthorizedApp["AuthVaildTime"].isNull())
			authorizedAppsObject.authVaildTime = valueAuthorizedAppsAuthorizedApp["AuthVaildTime"].asString();
		authorizedApps_.push_back(authorizedAppsObject);
	}
	if(!value["TotalCount"].isNull())
		totalCount_ = std::stoi(value["TotalCount"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["PageNumber"].isNull())
		pageNumber_ = std::stoi(value["PageNumber"].asString());

}

int DescribeAuthorizedAppsResult::getTotalCount()const
{
	return totalCount_;
}

int DescribeAuthorizedAppsResult::getPageSize()const
{
	return pageSize_;
}

std::vector<DescribeAuthorizedAppsResult::AuthorizedApp> DescribeAuthorizedAppsResult::getAuthorizedApps()const
{
	return authorizedApps_;
}

int DescribeAuthorizedAppsResult::getPageNumber()const
{
	return pageNumber_;
}

