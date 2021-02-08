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

#include <alibabacloud/iot/model/BatchGetDeviceBindStatusResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

BatchGetDeviceBindStatusResult::BatchGetDeviceBindStatusResult() :
	ServiceResult()
{}

BatchGetDeviceBindStatusResult::BatchGetDeviceBindStatusResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

BatchGetDeviceBindStatusResult::~BatchGetDeviceBindStatusResult()
{}

void BatchGetDeviceBindStatusResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allDataNode = value["Data"]["DeviceStatus"];
	for (auto valueDataDeviceStatus : allDataNode)
	{
		DeviceStatus dataObject;
		if(!valueDataDeviceStatus["IotId"].isNull())
			dataObject.iotId = valueDataDeviceStatus["IotId"].asString();
		if(!valueDataDeviceStatus["BindStatus"].isNull())
			dataObject.bindStatus = std::stoi(valueDataDeviceStatus["BindStatus"].asString());
		data_.push_back(dataObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

std::vector<BatchGetDeviceBindStatusResult::DeviceStatus> BatchGetDeviceBindStatusResult::getData()const
{
	return data_;
}

std::string BatchGetDeviceBindStatusResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string BatchGetDeviceBindStatusResult::getCode()const
{
	return code_;
}

bool BatchGetDeviceBindStatusResult::getSuccess()const
{
	return success_;
}

