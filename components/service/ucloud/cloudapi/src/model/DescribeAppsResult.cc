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

#include <alibabacloud/cloudapi/model/DescribeAppsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

DescribeAppsResult::DescribeAppsResult() :
	ServiceResult()
{}

DescribeAppsResult::DescribeAppsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeAppsResult::~DescribeAppsResult()
{}

void DescribeAppsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allAppsNode = value["Apps"]["AppItem"];
	for (auto valueAppsAppItem : allAppsNode)
	{
		AppItem appsObject;
		if(!valueAppsAppItem["AppId"].isNull())
			appsObject.appId = std::stol(valueAppsAppItem["AppId"].asString());
		if(!valueAppsAppItem["AppName"].isNull())
			appsObject.appName = valueAppsAppItem["AppName"].asString();
		if(!valueAppsAppItem["Description"].isNull())
			appsObject.description = valueAppsAppItem["Description"].asString();
		apps_.push_back(appsObject);
	}
	if(!value["TotalCount"].isNull())
		totalCount_ = std::stoi(value["TotalCount"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["PageNumber"].isNull())
		pageNumber_ = std::stoi(value["PageNumber"].asString());

}

int DescribeAppsResult::getTotalCount()const
{
	return totalCount_;
}

int DescribeAppsResult::getPageSize()const
{
	return pageSize_;
}

int DescribeAppsResult::getPageNumber()const
{
	return pageNumber_;
}

std::vector<DescribeAppsResult::AppItem> DescribeAppsResult::getApps()const
{
	return apps_;
}

