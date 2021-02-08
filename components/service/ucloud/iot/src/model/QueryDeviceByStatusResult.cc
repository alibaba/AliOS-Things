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

#include <alibabacloud/iot/model/QueryDeviceByStatusResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

QueryDeviceByStatusResult::QueryDeviceByStatusResult() :
	ServiceResult()
{}

QueryDeviceByStatusResult::QueryDeviceByStatusResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QueryDeviceByStatusResult::~QueryDeviceByStatusResult()
{}

void QueryDeviceByStatusResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allDataNode = value["Data"]["SimpleDeviceInfo"];
	for (auto valueDataSimpleDeviceInfo : allDataNode)
	{
		SimpleDeviceInfo dataObject;
		if(!valueDataSimpleDeviceInfo["ProductKey"].isNull())
			dataObject.productKey = valueDataSimpleDeviceInfo["ProductKey"].asString();
		if(!valueDataSimpleDeviceInfo["DeviceName"].isNull())
			dataObject.deviceName = valueDataSimpleDeviceInfo["DeviceName"].asString();
		if(!valueDataSimpleDeviceInfo["Status"].isNull())
			dataObject.status = valueDataSimpleDeviceInfo["Status"].asString();
		if(!valueDataSimpleDeviceInfo["IotId"].isNull())
			dataObject.iotId = valueDataSimpleDeviceInfo["IotId"].asString();
		if(!valueDataSimpleDeviceInfo["Nickname"].isNull())
			dataObject.nickname = valueDataSimpleDeviceInfo["Nickname"].asString();
		if(!valueDataSimpleDeviceInfo["GmtCreate"].isNull())
			dataObject.gmtCreate = valueDataSimpleDeviceInfo["GmtCreate"].asString();
		if(!valueDataSimpleDeviceInfo["GmtModified"].isNull())
			dataObject.gmtModified = valueDataSimpleDeviceInfo["GmtModified"].asString();
		if(!valueDataSimpleDeviceInfo["UtcCreate"].isNull())
			dataObject.utcCreate = valueDataSimpleDeviceInfo["UtcCreate"].asString();
		if(!valueDataSimpleDeviceInfo["UtcModified"].isNull())
			dataObject.utcModified = valueDataSimpleDeviceInfo["UtcModified"].asString();
		if(!valueDataSimpleDeviceInfo["DeviceSecret"].isNull())
			dataObject.deviceSecret = valueDataSimpleDeviceInfo["DeviceSecret"].asString();
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

int QueryDeviceByStatusResult::getPageSize()const
{
	return pageSize_;
}

int QueryDeviceByStatusResult::getPageCount()const
{
	return pageCount_;
}

int QueryDeviceByStatusResult::getTotal()const
{
	return total_;
}

std::vector<QueryDeviceByStatusResult::SimpleDeviceInfo> QueryDeviceByStatusResult::getData()const
{
	return data_;
}

int QueryDeviceByStatusResult::getPage()const
{
	return page_;
}

std::string QueryDeviceByStatusResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string QueryDeviceByStatusResult::getCode()const
{
	return code_;
}

bool QueryDeviceByStatusResult::getSuccess()const
{
	return success_;
}

