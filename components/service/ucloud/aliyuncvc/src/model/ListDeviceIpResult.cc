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

#include <alibabacloud/aliyuncvc/model/ListDeviceIpResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Aliyuncvc;
using namespace AlibabaCloud::Aliyuncvc::Model;

ListDeviceIpResult::ListDeviceIpResult() :
	ServiceResult()
{}

ListDeviceIpResult::ListDeviceIpResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListDeviceIpResult::~ListDeviceIpResult()
{}

void ListDeviceIpResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allDevicesNode = value["Devices"]["DevicesItem"];
	for (auto valueDevicesDevicesItem : allDevicesNode)
	{
		DevicesItem devicesObject;
		if(!valueDevicesDevicesItem["ScreenCode"].isNull())
			devicesObject.screenCode = valueDevicesDevicesItem["ScreenCode"].asString();
		if(!valueDevicesDevicesItem["Ip"].isNull())
			devicesObject.ip = valueDevicesDevicesItem["Ip"].asString();
		if(!valueDevicesDevicesItem["Mac"].isNull())
			devicesObject.mac = valueDevicesDevicesItem["Mac"].asString();
		if(!valueDevicesDevicesItem["SsId"].isNull())
			devicesObject.ssId = valueDevicesDevicesItem["SsId"].asString();
		devices_.push_back(devicesObject);
	}
	if(!value["ErrorCode"].isNull())
		errorCode_ = std::stoi(value["ErrorCode"].asString());
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";

}

std::string ListDeviceIpResult::getMessage()const
{
	return message_;
}

int ListDeviceIpResult::getErrorCode()const
{
	return errorCode_;
}

std::vector<ListDeviceIpResult::DevicesItem> ListDeviceIpResult::getDevices()const
{
	return devices_;
}

bool ListDeviceIpResult::getSuccess()const
{
	return success_;
}

