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

#include <alibabacloud/iot/model/QueryDevicePropertiesDataResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

QueryDevicePropertiesDataResult::QueryDevicePropertiesDataResult() :
	ServiceResult()
{}

QueryDevicePropertiesDataResult::QueryDevicePropertiesDataResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QueryDevicePropertiesDataResult::~QueryDevicePropertiesDataResult()
{}

void QueryDevicePropertiesDataResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allPropertyDataInfosNode = value["PropertyDataInfos"]["PropertyDataInfo"];
	for (auto valuePropertyDataInfosPropertyDataInfo : allPropertyDataInfosNode)
	{
		PropertyDataInfo propertyDataInfosObject;
		if(!valuePropertyDataInfosPropertyDataInfo["Identifier"].isNull())
			propertyDataInfosObject.identifier = valuePropertyDataInfosPropertyDataInfo["Identifier"].asString();
		auto allListNode = allPropertyDataInfosNode["List"]["PropertyInfo"];
		for (auto allPropertyDataInfosNodeListPropertyInfo : allListNode)
		{
			PropertyDataInfo::PropertyInfo listObject;
			if(!allPropertyDataInfosNodeListPropertyInfo["Time"].isNull())
				listObject.time = std::stol(allPropertyDataInfosNodeListPropertyInfo["Time"].asString());
			if(!allPropertyDataInfosNodeListPropertyInfo["Value"].isNull())
				listObject.value = allPropertyDataInfosNodeListPropertyInfo["Value"].asString();
			propertyDataInfosObject.list.push_back(listObject);
		}
		propertyDataInfos_.push_back(propertyDataInfosObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();
	if(!value["NextValid"].isNull())
		nextValid_ = value["NextValid"].asString() == "true";
	if(!value["NextTime"].isNull())
		nextTime_ = std::stol(value["NextTime"].asString());

}

bool QueryDevicePropertiesDataResult::getNextValid()const
{
	return nextValid_;
}

long QueryDevicePropertiesDataResult::getNextTime()const
{
	return nextTime_;
}

std::vector<QueryDevicePropertiesDataResult::PropertyDataInfo> QueryDevicePropertiesDataResult::getPropertyDataInfos()const
{
	return propertyDataInfos_;
}

std::string QueryDevicePropertiesDataResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string QueryDevicePropertiesDataResult::getCode()const
{
	return code_;
}

bool QueryDevicePropertiesDataResult::getSuccess()const
{
	return success_;
}

