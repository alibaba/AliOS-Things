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

#include <alibabacloud/iot/model/QueryDeviceServiceDataResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

QueryDeviceServiceDataResult::QueryDeviceServiceDataResult() :
	ServiceResult()
{}

QueryDeviceServiceDataResult::QueryDeviceServiceDataResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QueryDeviceServiceDataResult::~QueryDeviceServiceDataResult()
{}

void QueryDeviceServiceDataResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["NextTime"].isNull())
		data_.nextTime = std::stol(dataNode["NextTime"].asString());
	if(!dataNode["NextValid"].isNull())
		data_.nextValid = dataNode["NextValid"].asString() == "true";
	auto allListNode = dataNode["List"]["ServiceInfo"];
	for (auto dataNodeListServiceInfo : allListNode)
	{
		Data::ServiceInfo serviceInfoObject;
		if(!dataNodeListServiceInfo["Time"].isNull())
			serviceInfoObject.time = dataNodeListServiceInfo["Time"].asString();
		if(!dataNodeListServiceInfo["Identifier"].isNull())
			serviceInfoObject.identifier = dataNodeListServiceInfo["Identifier"].asString();
		if(!dataNodeListServiceInfo["Name"].isNull())
			serviceInfoObject.name = dataNodeListServiceInfo["Name"].asString();
		if(!dataNodeListServiceInfo["InputData"].isNull())
			serviceInfoObject.inputData = dataNodeListServiceInfo["InputData"].asString();
		if(!dataNodeListServiceInfo["OutputData"].isNull())
			serviceInfoObject.outputData = dataNodeListServiceInfo["OutputData"].asString();
		data_.list.push_back(serviceInfoObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

QueryDeviceServiceDataResult::Data QueryDeviceServiceDataResult::getData()const
{
	return data_;
}

std::string QueryDeviceServiceDataResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string QueryDeviceServiceDataResult::getCode()const
{
	return code_;
}

bool QueryDeviceServiceDataResult::getSuccess()const
{
	return success_;
}

