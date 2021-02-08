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

#include <alibabacloud/iot/model/QueryDeviceDesiredPropertyResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

QueryDeviceDesiredPropertyResult::QueryDeviceDesiredPropertyResult() :
	ServiceResult()
{}

QueryDeviceDesiredPropertyResult::QueryDeviceDesiredPropertyResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QueryDeviceDesiredPropertyResult::~QueryDeviceDesiredPropertyResult()
{}

void QueryDeviceDesiredPropertyResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto allListNode = dataNode["List"]["DesiredPropertyInfo"];
	for (auto dataNodeListDesiredPropertyInfo : allListNode)
	{
		Data::DesiredPropertyInfo desiredPropertyInfoObject;
		if(!dataNodeListDesiredPropertyInfo["Unit"].isNull())
			desiredPropertyInfoObject.unit = dataNodeListDesiredPropertyInfo["Unit"].asString();
		if(!dataNodeListDesiredPropertyInfo["Identifier"].isNull())
			desiredPropertyInfoObject.identifier = dataNodeListDesiredPropertyInfo["Identifier"].asString();
		if(!dataNodeListDesiredPropertyInfo["DataType"].isNull())
			desiredPropertyInfoObject.dataType = dataNodeListDesiredPropertyInfo["DataType"].asString();
		if(!dataNodeListDesiredPropertyInfo["Time"].isNull())
			desiredPropertyInfoObject.time = dataNodeListDesiredPropertyInfo["Time"].asString();
		if(!dataNodeListDesiredPropertyInfo["Value"].isNull())
			desiredPropertyInfoObject.value = dataNodeListDesiredPropertyInfo["Value"].asString();
		if(!dataNodeListDesiredPropertyInfo["Name"].isNull())
			desiredPropertyInfoObject.name = dataNodeListDesiredPropertyInfo["Name"].asString();
		if(!dataNodeListDesiredPropertyInfo["Version"].isNull())
			desiredPropertyInfoObject.version = std::stol(dataNodeListDesiredPropertyInfo["Version"].asString());
		data_.list.push_back(desiredPropertyInfoObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

QueryDeviceDesiredPropertyResult::Data QueryDeviceDesiredPropertyResult::getData()const
{
	return data_;
}

std::string QueryDeviceDesiredPropertyResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string QueryDeviceDesiredPropertyResult::getCode()const
{
	return code_;
}

bool QueryDeviceDesiredPropertyResult::getSuccess()const
{
	return success_;
}

