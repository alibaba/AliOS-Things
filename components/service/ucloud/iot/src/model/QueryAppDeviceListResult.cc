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

#include <alibabacloud/iot/model/QueryAppDeviceListResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

QueryAppDeviceListResult::QueryAppDeviceListResult() :
	ServiceResult()
{}

QueryAppDeviceListResult::QueryAppDeviceListResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QueryAppDeviceListResult::~QueryAppDeviceListResult()
{}

void QueryAppDeviceListResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allDataNode = value["Data"]["DataItem"];
	for (auto valueDataDataItem : allDataNode)
	{
		DataItem dataObject;
		if(!valueDataDataItem["ProductName"].isNull())
			dataObject.productName = valueDataDataItem["ProductName"].asString();
		if(!valueDataDataItem["ProductKey"].isNull())
			dataObject.productKey = valueDataDataItem["ProductKey"].asString();
		if(!valueDataDataItem["DeviceName"].isNull())
			dataObject.deviceName = valueDataDataItem["DeviceName"].asString();
		if(!valueDataDataItem["NodeType"].isNull())
			dataObject.nodeType = std::stoi(valueDataDataItem["NodeType"].asString());
		if(!valueDataDataItem["Status"].isNull())
			dataObject.status = valueDataDataItem["Status"].asString();
		if(!valueDataDataItem["ActiveTime"].isNull())
			dataObject.activeTime = valueDataDataItem["ActiveTime"].asString();
		if(!valueDataDataItem["LastOnlineTime"].isNull())
			dataObject.lastOnlineTime = valueDataDataItem["LastOnlineTime"].asString();
		if(!valueDataDataItem["CreateTime"].isNull())
			dataObject.createTime = valueDataDataItem["CreateTime"].asString();
		if(!valueDataDataItem["ChildDeviceCount"].isNull())
			dataObject.childDeviceCount = std::stol(valueDataDataItem["ChildDeviceCount"].asString());
		if(!valueDataDataItem["UtcActiveTime"].isNull())
			dataObject.utcActiveTime = valueDataDataItem["UtcActiveTime"].asString();
		if(!valueDataDataItem["UtcLastOnlineTime"].isNull())
			dataObject.utcLastOnlineTime = valueDataDataItem["UtcLastOnlineTime"].asString();
		if(!valueDataDataItem["UtcCreateTime"].isNull())
			dataObject.utcCreateTime = valueDataDataItem["UtcCreateTime"].asString();
		data_.push_back(dataObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["Page"].isNull())
		page_ = std::stoi(value["Page"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["PageCount"].isNull())
		pageCount_ = std::stoi(value["PageCount"].asString());
	if(!value["Total"].isNull())
		total_ = std::stoi(value["Total"].asString());

}

int QueryAppDeviceListResult::getPageSize()const
{
	return pageSize_;
}

int QueryAppDeviceListResult::getPageCount()const
{
	return pageCount_;
}

int QueryAppDeviceListResult::getTotal()const
{
	return total_;
}

std::vector<QueryAppDeviceListResult::DataItem> QueryAppDeviceListResult::getData()const
{
	return data_;
}

int QueryAppDeviceListResult::getPage()const
{
	return page_;
}

std::string QueryAppDeviceListResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string QueryAppDeviceListResult::getCode()const
{
	return code_;
}

bool QueryAppDeviceListResult::getSuccess()const
{
	return success_;
}

