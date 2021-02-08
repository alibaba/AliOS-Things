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

#include <alibabacloud/iot/model/BatchGetEdgeDriverResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

BatchGetEdgeDriverResult::BatchGetEdgeDriverResult() :
	ServiceResult()
{}

BatchGetEdgeDriverResult::BatchGetEdgeDriverResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

BatchGetEdgeDriverResult::~BatchGetEdgeDriverResult()
{}

void BatchGetEdgeDriverResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allDriverListNode = value["DriverList"]["Driver"];
	for (auto valueDriverListDriver : allDriverListNode)
	{
		Driver driverListObject;
		if(!valueDriverListDriver["DriverId"].isNull())
			driverListObject.driverId = valueDriverListDriver["DriverId"].asString();
		if(!valueDriverListDriver["DriverName"].isNull())
			driverListObject.driverName = valueDriverListDriver["DriverName"].asString();
		if(!valueDriverListDriver["DriverProtocol"].isNull())
			driverListObject.driverProtocol = valueDriverListDriver["DriverProtocol"].asString();
		if(!valueDriverListDriver["Runtime"].isNull())
			driverListObject.runtime = valueDriverListDriver["Runtime"].asString();
		if(!valueDriverListDriver["CpuArch"].isNull())
			driverListObject.cpuArch = valueDriverListDriver["CpuArch"].asString();
		if(!valueDriverListDriver["Type"].isNull())
			driverListObject.type = std::stoi(valueDriverListDriver["Type"].asString());
		if(!valueDriverListDriver["IsBuiltIn"].isNull())
			driverListObject.isBuiltIn = valueDriverListDriver["IsBuiltIn"].asString() == "true";
		if(!valueDriverListDriver["GmtCreateTimestamp"].isNull())
			driverListObject.gmtCreateTimestamp = std::stol(valueDriverListDriver["GmtCreateTimestamp"].asString());
		if(!valueDriverListDriver["GmtModifiedTimestamp"].isNull())
			driverListObject.gmtModifiedTimestamp = std::stol(valueDriverListDriver["GmtModifiedTimestamp"].asString());
		driverList_.push_back(driverListObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

std::vector<BatchGetEdgeDriverResult::Driver> BatchGetEdgeDriverResult::getDriverList()const
{
	return driverList_;
}

std::string BatchGetEdgeDriverResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string BatchGetEdgeDriverResult::getCode()const
{
	return code_;
}

bool BatchGetEdgeDriverResult::getSuccess()const
{
	return success_;
}

