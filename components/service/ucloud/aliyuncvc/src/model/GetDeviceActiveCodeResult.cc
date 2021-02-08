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

#include <alibabacloud/aliyuncvc/model/GetDeviceActiveCodeResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Aliyuncvc;
using namespace AlibabaCloud::Aliyuncvc::Model;

GetDeviceActiveCodeResult::GetDeviceActiveCodeResult() :
	ServiceResult()
{}

GetDeviceActiveCodeResult::GetDeviceActiveCodeResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetDeviceActiveCodeResult::~GetDeviceActiveCodeResult()
{}

void GetDeviceActiveCodeResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allDevicesNode = value["Devices"]["data"];
	for (auto valueDevicesdata : allDevicesNode)
	{
		Data devicesObject;
		if(!valueDevicesdata["DeviceErrorCode"].isNull())
			devicesObject.deviceErrorCode = std::stoi(valueDevicesdata["DeviceErrorCode"].asString());
		if(!valueDevicesdata["DeviceMessage"].isNull())
			devicesObject.deviceMessage = valueDevicesdata["DeviceMessage"].asString();
		if(!valueDevicesdata["SN"].isNull())
			devicesObject.sN = valueDevicesdata["SN"].asString();
		if(!valueDevicesdata["ActiveCode"].isNull())
			devicesObject.activeCode = valueDevicesdata["ActiveCode"].asString();
		devices_.push_back(devicesObject);
	}
	if(!value["ErrorCode"].isNull())
		errorCode_ = std::stoi(value["ErrorCode"].asString());
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";

}

std::string GetDeviceActiveCodeResult::getMessage()const
{
	return message_;
}

int GetDeviceActiveCodeResult::getErrorCode()const
{
	return errorCode_;
}

std::vector<GetDeviceActiveCodeResult::Data> GetDeviceActiveCodeResult::getDevices()const
{
	return devices_;
}

bool GetDeviceActiveCodeResult::getSuccess()const
{
	return success_;
}

