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

#include <alibabacloud/iot/model/QueryConsumerGroupListResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

QueryConsumerGroupListResult::QueryConsumerGroupListResult() :
	ServiceResult()
{}

QueryConsumerGroupListResult::QueryConsumerGroupListResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QueryConsumerGroupListResult::~QueryConsumerGroupListResult()
{}

void QueryConsumerGroupListResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allDataNode = value["Data"]["ConsumerGroupDTO"];
	for (auto valueDataConsumerGroupDTO : allDataNode)
	{
		ConsumerGroupDTO dataObject;
		if(!valueDataConsumerGroupDTO["IotId"].isNull())
			dataObject.iotId = valueDataConsumerGroupDTO["IotId"].asString();
		if(!valueDataConsumerGroupDTO["GroupId"].isNull())
			dataObject.groupId = valueDataConsumerGroupDTO["GroupId"].asString();
		if(!valueDataConsumerGroupDTO["GroupName"].isNull())
			dataObject.groupName = valueDataConsumerGroupDTO["GroupName"].asString();
		if(!valueDataConsumerGroupDTO["Creator"].isNull())
			dataObject.creator = valueDataConsumerGroupDTO["Creator"].asString();
		if(!valueDataConsumerGroupDTO["CreateTime"].isNull())
			dataObject.createTime = valueDataConsumerGroupDTO["CreateTime"].asString();
		if(!valueDataConsumerGroupDTO["AuthType"].isNull())
			dataObject.authType = valueDataConsumerGroupDTO["AuthType"].asString();
		if(!valueDataConsumerGroupDTO["CloudCommodityType"].isNull())
			dataObject.cloudCommodityType = valueDataConsumerGroupDTO["CloudCommodityType"].asString();
		data_.push_back(dataObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["PageCount"].isNull())
		pageCount_ = std::stoi(value["PageCount"].asString());
	if(!value["Total"].isNull())
		total_ = std::stoi(value["Total"].asString());
	if(!value["CurrentPage"].isNull())
		currentPage_ = std::stoi(value["CurrentPage"].asString());
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();

}

int QueryConsumerGroupListResult::getPageSize()const
{
	return pageSize_;
}

int QueryConsumerGroupListResult::getPageCount()const
{
	return pageCount_;
}

int QueryConsumerGroupListResult::getCurrentPage()const
{
	return currentPage_;
}

int QueryConsumerGroupListResult::getTotal()const
{
	return total_;
}

std::vector<QueryConsumerGroupListResult::ConsumerGroupDTO> QueryConsumerGroupListResult::getData()const
{
	return data_;
}

std::string QueryConsumerGroupListResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string QueryConsumerGroupListResult::getCode()const
{
	return code_;
}

bool QueryConsumerGroupListResult::getSuccess()const
{
	return success_;
}

