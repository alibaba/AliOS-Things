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

#include <alibabacloud/iot/model/QueryEdgeDriverResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

QueryEdgeDriverResult::QueryEdgeDriverResult() :
	ServiceResult()
{}

QueryEdgeDriverResult::QueryEdgeDriverResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QueryEdgeDriverResult::~QueryEdgeDriverResult()
{}

void QueryEdgeDriverResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["Total"].isNull())
		data_.total = std::stoi(dataNode["Total"].asString());
	if(!dataNode["PageSize"].isNull())
		data_.pageSize = std::stoi(dataNode["PageSize"].asString());
	if(!dataNode["CurrentPage"].isNull())
		data_.currentPage = std::stoi(dataNode["CurrentPage"].asString());
	auto allDriverListNode = dataNode["DriverList"]["Driver"];
	for (auto dataNodeDriverListDriver : allDriverListNode)
	{
		Data::Driver driverObject;
		if(!dataNodeDriverListDriver["DriverId"].isNull())
			driverObject.driverId = dataNodeDriverListDriver["DriverId"].asString();
		if(!dataNodeDriverListDriver["DriverName"].isNull())
			driverObject.driverName = dataNodeDriverListDriver["DriverName"].asString();
		if(!dataNodeDriverListDriver["DriverProtocol"].isNull())
			driverObject.driverProtocol = dataNodeDriverListDriver["DriverProtocol"].asString();
		if(!dataNodeDriverListDriver["Runtime"].isNull())
			driverObject.runtime = dataNodeDriverListDriver["Runtime"].asString();
		if(!dataNodeDriverListDriver["CpuArch"].isNull())
			driverObject.cpuArch = dataNodeDriverListDriver["CpuArch"].asString();
		if(!dataNodeDriverListDriver["Type"].isNull())
			driverObject.type = std::stoi(dataNodeDriverListDriver["Type"].asString());
		if(!dataNodeDriverListDriver["IsBuiltIn"].isNull())
			driverObject.isBuiltIn = dataNodeDriverListDriver["IsBuiltIn"].asString() == "true";
		if(!dataNodeDriverListDriver["GmtCreateTimestamp"].isNull())
			driverObject.gmtCreateTimestamp = std::stol(dataNodeDriverListDriver["GmtCreateTimestamp"].asString());
		if(!dataNodeDriverListDriver["GmtModifiedTimestamp"].isNull())
			driverObject.gmtModifiedTimestamp = std::stol(dataNodeDriverListDriver["GmtModifiedTimestamp"].asString());
		data_.driverList.push_back(driverObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

QueryEdgeDriverResult::Data QueryEdgeDriverResult::getData()const
{
	return data_;
}

std::string QueryEdgeDriverResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string QueryEdgeDriverResult::getCode()const
{
	return code_;
}

bool QueryEdgeDriverResult::getSuccess()const
{
	return success_;
}

