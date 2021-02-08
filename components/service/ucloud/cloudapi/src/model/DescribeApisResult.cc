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

#include <alibabacloud/cloudapi/model/DescribeApisResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

DescribeApisResult::DescribeApisResult() :
	ServiceResult()
{}

DescribeApisResult::DescribeApisResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeApisResult::~DescribeApisResult()
{}

void DescribeApisResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allApiSummarysNode = value["ApiSummarys"]["ApiSummary"];
	for (auto valueApiSummarysApiSummary : allApiSummarysNode)
	{
		ApiSummary apiSummarysObject;
		if(!valueApiSummarysApiSummary["RegionId"].isNull())
			apiSummarysObject.regionId = valueApiSummarysApiSummary["RegionId"].asString();
		if(!valueApiSummarysApiSummary["GroupId"].isNull())
			apiSummarysObject.groupId = valueApiSummarysApiSummary["GroupId"].asString();
		if(!valueApiSummarysApiSummary["GroupName"].isNull())
			apiSummarysObject.groupName = valueApiSummarysApiSummary["GroupName"].asString();
		if(!valueApiSummarysApiSummary["ApiId"].isNull())
			apiSummarysObject.apiId = valueApiSummarysApiSummary["ApiId"].asString();
		if(!valueApiSummarysApiSummary["ApiName"].isNull())
			apiSummarysObject.apiName = valueApiSummarysApiSummary["ApiName"].asString();
		if(!valueApiSummarysApiSummary["Visibility"].isNull())
			apiSummarysObject.visibility = valueApiSummarysApiSummary["Visibility"].asString();
		if(!valueApiSummarysApiSummary["Description"].isNull())
			apiSummarysObject.description = valueApiSummarysApiSummary["Description"].asString();
		if(!valueApiSummarysApiSummary["CreatedTime"].isNull())
			apiSummarysObject.createdTime = valueApiSummarysApiSummary["CreatedTime"].asString();
		if(!valueApiSummarysApiSummary["ModifiedTime"].isNull())
			apiSummarysObject.modifiedTime = valueApiSummarysApiSummary["ModifiedTime"].asString();
		apiSummarys_.push_back(apiSummarysObject);
	}
	if(!value["TotalCount"].isNull())
		totalCount_ = std::stoi(value["TotalCount"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["PageNumber"].isNull())
		pageNumber_ = std::stoi(value["PageNumber"].asString());

}

int DescribeApisResult::getTotalCount()const
{
	return totalCount_;
}

int DescribeApisResult::getPageSize()const
{
	return pageSize_;
}

int DescribeApisResult::getPageNumber()const
{
	return pageNumber_;
}

std::vector<DescribeApisResult::ApiSummary> DescribeApisResult::getApiSummarys()const
{
	return apiSummarys_;
}

