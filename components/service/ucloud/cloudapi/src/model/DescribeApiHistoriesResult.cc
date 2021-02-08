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

#include <alibabacloud/cloudapi/model/DescribeApiHistoriesResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

DescribeApiHistoriesResult::DescribeApiHistoriesResult() :
	ServiceResult()
{}

DescribeApiHistoriesResult::DescribeApiHistoriesResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeApiHistoriesResult::~DescribeApiHistoriesResult()
{}

void DescribeApiHistoriesResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allApiHisItemsNode = value["ApiHisItems"]["ApiHisItem"];
	for (auto valueApiHisItemsApiHisItem : allApiHisItemsNode)
	{
		ApiHisItem apiHisItemsObject;
		if(!valueApiHisItemsApiHisItem["RegionId"].isNull())
			apiHisItemsObject.regionId = valueApiHisItemsApiHisItem["RegionId"].asString();
		if(!valueApiHisItemsApiHisItem["ApiId"].isNull())
			apiHisItemsObject.apiId = valueApiHisItemsApiHisItem["ApiId"].asString();
		if(!valueApiHisItemsApiHisItem["ApiName"].isNull())
			apiHisItemsObject.apiName = valueApiHisItemsApiHisItem["ApiName"].asString();
		if(!valueApiHisItemsApiHisItem["GroupId"].isNull())
			apiHisItemsObject.groupId = valueApiHisItemsApiHisItem["GroupId"].asString();
		if(!valueApiHisItemsApiHisItem["GroupName"].isNull())
			apiHisItemsObject.groupName = valueApiHisItemsApiHisItem["GroupName"].asString();
		if(!valueApiHisItemsApiHisItem["StageName"].isNull())
			apiHisItemsObject.stageName = valueApiHisItemsApiHisItem["StageName"].asString();
		if(!valueApiHisItemsApiHisItem["HistoryVersion"].isNull())
			apiHisItemsObject.historyVersion = valueApiHisItemsApiHisItem["HistoryVersion"].asString();
		if(!valueApiHisItemsApiHisItem["Status"].isNull())
			apiHisItemsObject.status = valueApiHisItemsApiHisItem["Status"].asString();
		if(!valueApiHisItemsApiHisItem["Description"].isNull())
			apiHisItemsObject.description = valueApiHisItemsApiHisItem["Description"].asString();
		if(!valueApiHisItemsApiHisItem["DeployedTime"].isNull())
			apiHisItemsObject.deployedTime = valueApiHisItemsApiHisItem["DeployedTime"].asString();
		apiHisItems_.push_back(apiHisItemsObject);
	}
	if(!value["TotalCount"].isNull())
		totalCount_ = std::stoi(value["TotalCount"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["PageNumber"].isNull())
		pageNumber_ = std::stoi(value["PageNumber"].asString());

}

int DescribeApiHistoriesResult::getTotalCount()const
{
	return totalCount_;
}

int DescribeApiHistoriesResult::getPageSize()const
{
	return pageSize_;
}

int DescribeApiHistoriesResult::getPageNumber()const
{
	return pageNumber_;
}

std::vector<DescribeApiHistoriesResult::ApiHisItem> DescribeApiHistoriesResult::getApiHisItems()const
{
	return apiHisItems_;
}

