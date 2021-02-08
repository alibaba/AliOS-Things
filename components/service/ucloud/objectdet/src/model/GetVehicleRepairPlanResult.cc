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

#include <alibabacloud/objectdet/model/GetVehicleRepairPlanResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Objectdet;
using namespace AlibabaCloud::Objectdet::Model;

GetVehicleRepairPlanResult::GetVehicleRepairPlanResult() :
	ServiceResult()
{}

GetVehicleRepairPlanResult::GetVehicleRepairPlanResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetVehicleRepairPlanResult::~GetVehicleRepairPlanResult()
{}

void GetVehicleRepairPlanResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["FrameNo"].isNull())
		data_.frameNo = dataNode["FrameNo"].asString();
	auto allRepairPartsNode = dataNode["RepairParts"]["RepairItems"];
	for (auto dataNodeRepairPartsRepairItems : allRepairPartsNode)
	{
		Data::RepairItems repairItemsObject;
		if(!dataNodeRepairPartsRepairItems["GarageType"].isNull())
			repairItemsObject.garageType = dataNodeRepairPartsRepairItems["GarageType"].asString();
		if(!dataNodeRepairPartsRepairItems["OeMatch"].isNull())
			repairItemsObject.oeMatch = dataNodeRepairPartsRepairItems["OeMatch"].asString() == "true";
		if(!dataNodeRepairPartsRepairItems["OutStandardPartsId"].isNull())
			repairItemsObject.outStandardPartsId = dataNodeRepairPartsRepairItems["OutStandardPartsId"].asString();
		if(!dataNodeRepairPartsRepairItems["OutStandardPartsName"].isNull())
			repairItemsObject.outStandardPartsName = dataNodeRepairPartsRepairItems["OutStandardPartsName"].asString();
		if(!dataNodeRepairPartsRepairItems["PartNameMatch"].isNull())
			repairItemsObject.partNameMatch = dataNodeRepairPartsRepairItems["PartNameMatch"].asString() == "true";
		if(!dataNodeRepairPartsRepairItems["PartsStdCode"].isNull())
			repairItemsObject.partsStdCode = dataNodeRepairPartsRepairItems["PartsStdCode"].asString();
		if(!dataNodeRepairPartsRepairItems["PartsStdName"].isNull())
			repairItemsObject.partsStdName = dataNodeRepairPartsRepairItems["PartsStdName"].asString();
		if(!dataNodeRepairPartsRepairItems["RelationType"].isNull())
			repairItemsObject.relationType = dataNodeRepairPartsRepairItems["RelationType"].asString();
		if(!dataNodeRepairPartsRepairItems["RepairFee"].isNull())
			repairItemsObject.repairFee = dataNodeRepairPartsRepairItems["RepairFee"].asString();
		if(!dataNodeRepairPartsRepairItems["RepairType"].isNull())
			repairItemsObject.repairType = dataNodeRepairPartsRepairItems["RepairType"].asString();
		if(!dataNodeRepairPartsRepairItems["RepairTypeName"].isNull())
			repairItemsObject.repairTypeName = dataNodeRepairPartsRepairItems["RepairTypeName"].asString();
		data_.repairParts.push_back(repairItemsObject);
	}
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["HttpCode"].isNull())
		httpCode_ = std::stoi(value["HttpCode"].asString());

}

int GetVehicleRepairPlanResult::getHttpCode()const
{
	return httpCode_;
}

GetVehicleRepairPlanResult::Data GetVehicleRepairPlanResult::getData()const
{
	return data_;
}

std::string GetVehicleRepairPlanResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string GetVehicleRepairPlanResult::getCode()const
{
	return code_;
}

bool GetVehicleRepairPlanResult::getSuccess()const
{
	return success_;
}

