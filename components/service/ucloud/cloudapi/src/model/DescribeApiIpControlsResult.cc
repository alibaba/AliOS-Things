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

#include <alibabacloud/cloudapi/model/DescribeApiIpControlsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

DescribeApiIpControlsResult::DescribeApiIpControlsResult() :
	ServiceResult()
{}

DescribeApiIpControlsResult::DescribeApiIpControlsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeApiIpControlsResult::~DescribeApiIpControlsResult()
{}

void DescribeApiIpControlsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allApiIpControlsNode = value["ApiIpControls"]["ApiIpControlItem"];
	for (auto valueApiIpControlsApiIpControlItem : allApiIpControlsNode)
	{
		ApiIpControlItem apiIpControlsObject;
		if(!valueApiIpControlsApiIpControlItem["ApiId"].isNull())
			apiIpControlsObject.apiId = valueApiIpControlsApiIpControlItem["ApiId"].asString();
		if(!valueApiIpControlsApiIpControlItem["ApiName"].isNull())
			apiIpControlsObject.apiName = valueApiIpControlsApiIpControlItem["ApiName"].asString();
		if(!valueApiIpControlsApiIpControlItem["IpControlId"].isNull())
			apiIpControlsObject.ipControlId = valueApiIpControlsApiIpControlItem["IpControlId"].asString();
		if(!valueApiIpControlsApiIpControlItem["IpControlName"].isNull())
			apiIpControlsObject.ipControlName = valueApiIpControlsApiIpControlItem["IpControlName"].asString();
		if(!valueApiIpControlsApiIpControlItem["BoundTime"].isNull())
			apiIpControlsObject.boundTime = valueApiIpControlsApiIpControlItem["BoundTime"].asString();
		apiIpControls_.push_back(apiIpControlsObject);
	}
	if(!value["TotalCount"].isNull())
		totalCount_ = std::stoi(value["TotalCount"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["PageNumber"].isNull())
		pageNumber_ = std::stoi(value["PageNumber"].asString());

}

int DescribeApiIpControlsResult::getTotalCount()const
{
	return totalCount_;
}

int DescribeApiIpControlsResult::getPageSize()const
{
	return pageSize_;
}

int DescribeApiIpControlsResult::getPageNumber()const
{
	return pageNumber_;
}

std::vector<DescribeApiIpControlsResult::ApiIpControlItem> DescribeApiIpControlsResult::getApiIpControls()const
{
	return apiIpControls_;
}

