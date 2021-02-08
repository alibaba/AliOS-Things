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

#include <alibabacloud/iot/model/QueryBatchRegisterDeviceStatusResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

QueryBatchRegisterDeviceStatusResult::QueryBatchRegisterDeviceStatusResult() :
	ServiceResult()
{}

QueryBatchRegisterDeviceStatusResult::QueryBatchRegisterDeviceStatusResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QueryBatchRegisterDeviceStatusResult::~QueryBatchRegisterDeviceStatusResult()
{}

void QueryBatchRegisterDeviceStatusResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["Status"].isNull())
		data_.status = dataNode["Status"].asString();
		auto allValidList = dataNode["ValidList"]["Name"];
		for (auto value : allValidList)
			data_.validList.push_back(value.asString());
		auto allInvalidList = dataNode["InvalidList"]["Name"];
		for (auto value : allInvalidList)
			data_.invalidList.push_back(value.asString());
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

QueryBatchRegisterDeviceStatusResult::Data QueryBatchRegisterDeviceStatusResult::getData()const
{
	return data_;
}

std::string QueryBatchRegisterDeviceStatusResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string QueryBatchRegisterDeviceStatusResult::getCode()const
{
	return code_;
}

bool QueryBatchRegisterDeviceStatusResult::getSuccess()const
{
	return success_;
}

