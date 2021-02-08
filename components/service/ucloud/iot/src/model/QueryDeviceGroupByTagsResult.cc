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

#include <alibabacloud/iot/model/QueryDeviceGroupByTagsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

QueryDeviceGroupByTagsResult::QueryDeviceGroupByTagsResult() :
	ServiceResult()
{}

QueryDeviceGroupByTagsResult::QueryDeviceGroupByTagsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QueryDeviceGroupByTagsResult::~QueryDeviceGroupByTagsResult()
{}

void QueryDeviceGroupByTagsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allDataNode = value["Data"]["DeviceGroup"];
	for (auto valueDataDeviceGroup : allDataNode)
	{
		DeviceGroup dataObject;
		if(!valueDataDeviceGroup["GroupId"].isNull())
			dataObject.groupId = valueDataDeviceGroup["GroupId"].asString();
		if(!valueDataDeviceGroup["GroupName"].isNull())
			dataObject.groupName = valueDataDeviceGroup["GroupName"].asString();
		data_.push_back(dataObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();
	if(!value["Page"].isNull())
		page_ = std::stoi(value["Page"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["PageCount"].isNull())
		pageCount_ = std::stoi(value["PageCount"].asString());
	if(!value["Total"].isNull())
		total_ = std::stoi(value["Total"].asString());

}

int QueryDeviceGroupByTagsResult::getPageSize()const
{
	return pageSize_;
}

int QueryDeviceGroupByTagsResult::getPageCount()const
{
	return pageCount_;
}

int QueryDeviceGroupByTagsResult::getTotal()const
{
	return total_;
}

std::vector<QueryDeviceGroupByTagsResult::DeviceGroup> QueryDeviceGroupByTagsResult::getData()const
{
	return data_;
}

int QueryDeviceGroupByTagsResult::getPage()const
{
	return page_;
}

std::string QueryDeviceGroupByTagsResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string QueryDeviceGroupByTagsResult::getCode()const
{
	return code_;
}

bool QueryDeviceGroupByTagsResult::getSuccess()const
{
	return success_;
}

