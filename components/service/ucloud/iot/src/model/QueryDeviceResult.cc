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

#include <alibabacloud/iot/model/QueryDeviceResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

QueryDeviceResult::QueryDeviceResult() :
	ServiceResult()
{}

QueryDeviceResult::QueryDeviceResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QueryDeviceResult::~QueryDeviceResult()
{}

void QueryDeviceResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allDataNode = value["Data"]["DeviceInfo"];
	for (auto valueDataDeviceInfo : allDataNode)
	{
		DeviceInfo dataObject;
		if(!valueDataDeviceInfo["DeviceId"].isNull())
			dataObject.deviceId = valueDataDeviceInfo["DeviceId"].asString();
		if(!valueDataDeviceInfo["DeviceSecret"].isNull())
			dataObject.deviceSecret = valueDataDeviceInfo["DeviceSecret"].asString();
		if(!valueDataDeviceInfo["ProductKey"].isNull())
			dataObject.productKey = valueDataDeviceInfo["ProductKey"].asString();
		if(!valueDataDeviceInfo["DeviceStatus"].isNull())
			dataObject.deviceStatus = valueDataDeviceInfo["DeviceStatus"].asString();
		if(!valueDataDeviceInfo["DeviceName"].isNull())
			dataObject.deviceName = valueDataDeviceInfo["DeviceName"].asString();
		if(!valueDataDeviceInfo["DeviceType"].isNull())
			dataObject.deviceType = valueDataDeviceInfo["DeviceType"].asString();
		if(!valueDataDeviceInfo["GmtCreate"].isNull())
			dataObject.gmtCreate = valueDataDeviceInfo["GmtCreate"].asString();
		if(!valueDataDeviceInfo["GmtModified"].isNull())
			dataObject.gmtModified = valueDataDeviceInfo["GmtModified"].asString();
		if(!valueDataDeviceInfo["UtcCreate"].isNull())
			dataObject.utcCreate = valueDataDeviceInfo["UtcCreate"].asString();
		if(!valueDataDeviceInfo["UtcModified"].isNull())
			dataObject.utcModified = valueDataDeviceInfo["UtcModified"].asString();
		if(!valueDataDeviceInfo["IotId"].isNull())
			dataObject.iotId = valueDataDeviceInfo["IotId"].asString();
		if(!valueDataDeviceInfo["Nickname"].isNull())
			dataObject.nickname = valueDataDeviceInfo["Nickname"].asString();
		data_.push_back(dataObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();
	if(!value["Total"].isNull())
		total_ = std::stoi(value["Total"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["PageCount"].isNull())
		pageCount_ = std::stoi(value["PageCount"].asString());
	if(!value["Page"].isNull())
		page_ = std::stoi(value["Page"].asString());

}

int QueryDeviceResult::getPageSize()const
{
	return pageSize_;
}

int QueryDeviceResult::getPageCount()const
{
	return pageCount_;
}

int QueryDeviceResult::getTotal()const
{
	return total_;
}

std::vector<QueryDeviceResult::DeviceInfo> QueryDeviceResult::getData()const
{
	return data_;
}

int QueryDeviceResult::getPage()const
{
	return page_;
}

std::string QueryDeviceResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string QueryDeviceResult::getCode()const
{
	return code_;
}

bool QueryDeviceResult::getSuccess()const
{
	return success_;
}

