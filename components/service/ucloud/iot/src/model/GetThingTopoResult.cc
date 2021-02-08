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

#include <alibabacloud/iot/model/GetThingTopoResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

GetThingTopoResult::GetThingTopoResult() :
	ServiceResult()
{}

GetThingTopoResult::GetThingTopoResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetThingTopoResult::~GetThingTopoResult()
{}

void GetThingTopoResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["Total"].isNull())
		data_.total = std::stol(dataNode["Total"].asString());
	if(!dataNode["CurrentPage"].isNull())
		data_.currentPage = std::stoi(dataNode["CurrentPage"].asString());
	if(!dataNode["PageSize"].isNull())
		data_.pageSize = std::stoi(dataNode["PageSize"].asString());
	if(!dataNode["PageCount"].isNull())
		data_.pageCount = std::stol(dataNode["PageCount"].asString());
	auto allListNode = dataNode["List"]["deviceInfo"];
	for (auto dataNodeListdeviceInfo : allListNode)
	{
		Data::DeviceInfo deviceInfoObject;
		if(!dataNodeListdeviceInfo["IotId"].isNull())
			deviceInfoObject.iotId = dataNodeListdeviceInfo["IotId"].asString();
		if(!dataNodeListdeviceInfo["ProductKey"].isNull())
			deviceInfoObject.productKey = dataNodeListdeviceInfo["ProductKey"].asString();
		if(!dataNodeListdeviceInfo["DeviceName"].isNull())
			deviceInfoObject.deviceName = dataNodeListdeviceInfo["DeviceName"].asString();
		data_.list.push_back(deviceInfoObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

GetThingTopoResult::Data GetThingTopoResult::getData()const
{
	return data_;
}

std::string GetThingTopoResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string GetThingTopoResult::getCode()const
{
	return code_;
}

bool GetThingTopoResult::getSuccess()const
{
	return success_;
}

