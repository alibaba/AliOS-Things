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

#include <alibabacloud/iot/model/GetGatewayBySubDeviceResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

GetGatewayBySubDeviceResult::GetGatewayBySubDeviceResult() :
	ServiceResult()
{}

GetGatewayBySubDeviceResult::GetGatewayBySubDeviceResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetGatewayBySubDeviceResult::~GetGatewayBySubDeviceResult()
{}

void GetGatewayBySubDeviceResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["ProductKey"].isNull())
		data_.productKey = dataNode["ProductKey"].asString();
	if(!dataNode["ProductName"].isNull())
		data_.productName = dataNode["ProductName"].asString();
	if(!dataNode["DeviceSecret"].isNull())
		data_.deviceSecret = dataNode["DeviceSecret"].asString();
	if(!dataNode["DeviceName"].isNull())
		data_.deviceName = dataNode["DeviceName"].asString();
	if(!dataNode["FirmwareVersion"].isNull())
		data_.firmwareVersion = dataNode["FirmwareVersion"].asString();
	if(!dataNode["GmtCreate"].isNull())
		data_.gmtCreate = dataNode["GmtCreate"].asString();
	if(!dataNode["UtcCreate"].isNull())
		data_.utcCreate = dataNode["UtcCreate"].asString();
	if(!dataNode["GmtActive"].isNull())
		data_.gmtActive = dataNode["GmtActive"].asString();
	if(!dataNode["UtcActive"].isNull())
		data_.utcActive = dataNode["UtcActive"].asString();
	if(!dataNode["GmtOnline"].isNull())
		data_.gmtOnline = dataNode["GmtOnline"].asString();
	if(!dataNode["UtcOnline"].isNull())
		data_.utcOnline = dataNode["UtcOnline"].asString();
	if(!dataNode["Status"].isNull())
		data_.status = dataNode["Status"].asString();
	if(!dataNode["IpAddress"].isNull())
		data_.ipAddress = dataNode["IpAddress"].asString();
	if(!dataNode["NodeType"].isNull())
		data_.nodeType = dataNode["NodeType"].asString();
	if(!dataNode["region"].isNull())
		data_.region = dataNode["region"].asString();
	if(!dataNode["iotId"].isNull())
		data_.iotId = dataNode["iotId"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

GetGatewayBySubDeviceResult::Data GetGatewayBySubDeviceResult::getData()const
{
	return data_;
}

std::string GetGatewayBySubDeviceResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string GetGatewayBySubDeviceResult::getCode()const
{
	return code_;
}

bool GetGatewayBySubDeviceResult::getSuccess()const
{
	return success_;
}

