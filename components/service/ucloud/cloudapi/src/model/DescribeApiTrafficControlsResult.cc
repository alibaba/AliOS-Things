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

#include <alibabacloud/cloudapi/model/DescribeApiTrafficControlsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

DescribeApiTrafficControlsResult::DescribeApiTrafficControlsResult() :
	ServiceResult()
{}

DescribeApiTrafficControlsResult::DescribeApiTrafficControlsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeApiTrafficControlsResult::~DescribeApiTrafficControlsResult()
{}

void DescribeApiTrafficControlsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allApiTrafficControlsNode = value["ApiTrafficControls"]["ApiTrafficControlItem"];
	for (auto valueApiTrafficControlsApiTrafficControlItem : allApiTrafficControlsNode)
	{
		ApiTrafficControlItem apiTrafficControlsObject;
		if(!valueApiTrafficControlsApiTrafficControlItem["ApiId"].isNull())
			apiTrafficControlsObject.apiId = valueApiTrafficControlsApiTrafficControlItem["ApiId"].asString();
		if(!valueApiTrafficControlsApiTrafficControlItem["ApiName"].isNull())
			apiTrafficControlsObject.apiName = valueApiTrafficControlsApiTrafficControlItem["ApiName"].asString();
		if(!valueApiTrafficControlsApiTrafficControlItem["TrafficControlId"].isNull())
			apiTrafficControlsObject.trafficControlId = valueApiTrafficControlsApiTrafficControlItem["TrafficControlId"].asString();
		if(!valueApiTrafficControlsApiTrafficControlItem["TrafficControlName"].isNull())
			apiTrafficControlsObject.trafficControlName = valueApiTrafficControlsApiTrafficControlItem["TrafficControlName"].asString();
		if(!valueApiTrafficControlsApiTrafficControlItem["BoundTime"].isNull())
			apiTrafficControlsObject.boundTime = valueApiTrafficControlsApiTrafficControlItem["BoundTime"].asString();
		apiTrafficControls_.push_back(apiTrafficControlsObject);
	}
	if(!value["TotalCount"].isNull())
		totalCount_ = std::stoi(value["TotalCount"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["PageNumber"].isNull())
		pageNumber_ = std::stoi(value["PageNumber"].asString());

}

int DescribeApiTrafficControlsResult::getTotalCount()const
{
	return totalCount_;
}

int DescribeApiTrafficControlsResult::getPageSize()const
{
	return pageSize_;
}

int DescribeApiTrafficControlsResult::getPageNumber()const
{
	return pageNumber_;
}

std::vector<DescribeApiTrafficControlsResult::ApiTrafficControlItem> DescribeApiTrafficControlsResult::getApiTrafficControls()const
{
	return apiTrafficControls_;
}

