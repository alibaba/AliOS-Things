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

#include <alibabacloud/iot/model/QueryEdgeInstanceDeviceResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

QueryEdgeInstanceDeviceResult::QueryEdgeInstanceDeviceResult() :
	ServiceResult()
{}

QueryEdgeInstanceDeviceResult::QueryEdgeInstanceDeviceResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QueryEdgeInstanceDeviceResult::~QueryEdgeInstanceDeviceResult()
{}

void QueryEdgeInstanceDeviceResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["Total"].isNull())
		data_.total = std::stoi(dataNode["Total"].asString());
	if(!dataNode["PageSize"].isNull())
		data_.pageSize = std::stoi(dataNode["PageSize"].asString());
	if(!dataNode["CurrentPage"].isNull())
		data_.currentPage = std::stoi(dataNode["CurrentPage"].asString());
	auto allDeviceListNode = dataNode["DeviceList"]["Device"];
	for (auto dataNodeDeviceListDevice : allDeviceListNode)
	{
		Data::Device deviceObject;
		if(!dataNodeDeviceListDevice["IotId"].isNull())
			deviceObject.iotId = dataNodeDeviceListDevice["IotId"].asString();
		if(!dataNodeDeviceListDevice["ProductKey"].isNull())
			deviceObject.productKey = dataNodeDeviceListDevice["ProductKey"].asString();
		if(!dataNodeDeviceListDevice["DeviceName"].isNull())
			deviceObject.deviceName = dataNodeDeviceListDevice["DeviceName"].asString();
		if(!dataNodeDeviceListDevice["DriverId"].isNull())
			deviceObject.driverId = dataNodeDeviceListDevice["DriverId"].asString();
		data_.deviceList.push_back(deviceObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

QueryEdgeInstanceDeviceResult::Data QueryEdgeInstanceDeviceResult::getData()const
{
	return data_;
}

std::string QueryEdgeInstanceDeviceResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string QueryEdgeInstanceDeviceResult::getCode()const
{
	return code_;
}

bool QueryEdgeInstanceDeviceResult::getSuccess()const
{
	return success_;
}

