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

#include <alibabacloud/iot/model/BatchQueryDeviceDetailResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

BatchQueryDeviceDetailResult::BatchQueryDeviceDetailResult() :
	ServiceResult()
{}

BatchQueryDeviceDetailResult::BatchQueryDeviceDetailResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

BatchQueryDeviceDetailResult::~BatchQueryDeviceDetailResult()
{}

void BatchQueryDeviceDetailResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allDataNode = value["Data"]["DataItem"];
	for (auto valueDataDataItem : allDataNode)
	{
		DataItem dataObject;
		if(!valueDataDataItem["ProductKey"].isNull())
			dataObject.productKey = valueDataDataItem["ProductKey"].asString();
		if(!valueDataDataItem["ProductName"].isNull())
			dataObject.productName = valueDataDataItem["ProductName"].asString();
		if(!valueDataDataItem["DeviceName"].isNull())
			dataObject.deviceName = valueDataDataItem["DeviceName"].asString();
		if(!valueDataDataItem["Nickname"].isNull())
			dataObject.nickname = valueDataDataItem["Nickname"].asString();
		if(!valueDataDataItem["DeviceSecret"].isNull())
			dataObject.deviceSecret = valueDataDataItem["DeviceSecret"].asString();
		if(!valueDataDataItem["IotId"].isNull())
			dataObject.iotId = valueDataDataItem["IotId"].asString();
		if(!valueDataDataItem["UtcCreate"].isNull())
			dataObject.utcCreate = valueDataDataItem["UtcCreate"].asString();
		if(!valueDataDataItem["GmtCreate"].isNull())
			dataObject.gmtCreate = valueDataDataItem["GmtCreate"].asString();
		if(!valueDataDataItem["UtcActive"].isNull())
			dataObject.utcActive = valueDataDataItem["UtcActive"].asString();
		if(!valueDataDataItem["GmtActive"].isNull())
			dataObject.gmtActive = valueDataDataItem["GmtActive"].asString();
		if(!valueDataDataItem["Status"].isNull())
			dataObject.status = valueDataDataItem["Status"].asString();
		if(!valueDataDataItem["FirmwareVersion"].isNull())
			dataObject.firmwareVersion = valueDataDataItem["FirmwareVersion"].asString();
		if(!valueDataDataItem["NodeType"].isNull())
			dataObject.nodeType = std::stoi(valueDataDataItem["NodeType"].asString());
		if(!valueDataDataItem["Region"].isNull())
			dataObject.region = valueDataDataItem["Region"].asString();
		data_.push_back(dataObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

std::vector<BatchQueryDeviceDetailResult::DataItem> BatchQueryDeviceDetailResult::getData()const
{
	return data_;
}

std::string BatchQueryDeviceDetailResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string BatchQueryDeviceDetailResult::getCode()const
{
	return code_;
}

bool BatchQueryDeviceDetailResult::getSuccess()const
{
	return success_;
}

