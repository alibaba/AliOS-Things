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

#include <alibabacloud/aliyuncvc/model/UpdateDeviceHeartBeatResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Aliyuncvc;
using namespace AlibabaCloud::Aliyuncvc::Model;

UpdateDeviceHeartBeatResult::UpdateDeviceHeartBeatResult() :
	ServiceResult()
{}

UpdateDeviceHeartBeatResult::UpdateDeviceHeartBeatResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

UpdateDeviceHeartBeatResult::~UpdateDeviceHeartBeatResult()
{}

void UpdateDeviceHeartBeatResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto deviceInfoNode = value["DeviceInfo"];
	if(!deviceInfoNode["ChannelType"].isNull())
		deviceInfo_.channelType = deviceInfoNode["ChannelType"].asString();
	if(!value["ErrorCode"].isNull())
		errorCode_ = std::stoi(value["ErrorCode"].asString());
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";

}

std::string UpdateDeviceHeartBeatResult::getMessage()const
{
	return message_;
}

UpdateDeviceHeartBeatResult::DeviceInfo UpdateDeviceHeartBeatResult::getDeviceInfo()const
{
	return deviceInfo_;
}

int UpdateDeviceHeartBeatResult::getErrorCode()const
{
	return errorCode_;
}

bool UpdateDeviceHeartBeatResult::getSuccess()const
{
	return success_;
}

