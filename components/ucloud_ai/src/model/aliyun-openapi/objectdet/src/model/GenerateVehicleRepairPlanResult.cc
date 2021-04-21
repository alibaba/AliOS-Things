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

#include <alibabacloud/objectdet/model/GenerateVehicleRepairPlanResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Objectdet;
using namespace AlibabaCloud::Objectdet::Model;

GenerateVehicleRepairPlanResult::GenerateVehicleRepairPlanResult() :
	ServiceResult()
{}

GenerateVehicleRepairPlanResult::GenerateVehicleRepairPlanResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GenerateVehicleRepairPlanResult::~GenerateVehicleRepairPlanResult()
{}

void GenerateVehicleRepairPlanResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["TaskId"].isNull())
		data_.taskId = dataNode["TaskId"].asString();
	if(!value["HttpCode"].isNull())
		httpCode_ = std::stoi(value["HttpCode"].asString());
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";

}

int GenerateVehicleRepairPlanResult::getHttpCode()const
{
	return httpCode_;
}

GenerateVehicleRepairPlanResult::Data GenerateVehicleRepairPlanResult::getData()const
{
	return data_;
}

std::string GenerateVehicleRepairPlanResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string GenerateVehicleRepairPlanResult::getCode()const
{
	return code_;
}

bool GenerateVehicleRepairPlanResult::getSuccess()const
{
	return success_;
}

