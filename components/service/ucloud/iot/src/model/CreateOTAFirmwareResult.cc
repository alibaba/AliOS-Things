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

#include <alibabacloud/iot/model/CreateOTAFirmwareResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

CreateOTAFirmwareResult::CreateOTAFirmwareResult() :
	ServiceResult()
{}

CreateOTAFirmwareResult::CreateOTAFirmwareResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

CreateOTAFirmwareResult::~CreateOTAFirmwareResult()
{}

void CreateOTAFirmwareResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["FirmwareId"].isNull())
		data_.firmwareId = dataNode["FirmwareId"].asString();
	if(!dataNode["UtcCreate"].isNull())
		data_.utcCreate = dataNode["UtcCreate"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

CreateOTAFirmwareResult::Data CreateOTAFirmwareResult::getData()const
{
	return data_;
}

std::string CreateOTAFirmwareResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string CreateOTAFirmwareResult::getCode()const
{
	return code_;
}

bool CreateOTAFirmwareResult::getSuccess()const
{
	return success_;
}

