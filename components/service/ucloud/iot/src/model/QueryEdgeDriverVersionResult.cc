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

#include <alibabacloud/iot/model/QueryEdgeDriverVersionResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

QueryEdgeDriverVersionResult::QueryEdgeDriverVersionResult() :
	ServiceResult()
{}

QueryEdgeDriverVersionResult::QueryEdgeDriverVersionResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QueryEdgeDriverVersionResult::~QueryEdgeDriverVersionResult()
{}

void QueryEdgeDriverVersionResult::parse(const std::string &payload)
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
	auto allDriverVersionListNode = dataNode["DriverVersionList"]["DriverVersion"];
	for (auto dataNodeDriverVersionListDriverVersion : allDriverVersionListNode)
	{
		Data::DriverVersion driverVersionObject;
		if(!dataNodeDriverVersionListDriverVersion["DriverId"].isNull())
			driverVersionObject.driverId = dataNodeDriverVersionListDriverVersion["DriverId"].asString();
		if(!dataNodeDriverVersionListDriverVersion["DriverVersion"].isNull())
			driverVersionObject.driverVersion = dataNodeDriverVersionListDriverVersion["DriverVersion"].asString();
		if(!dataNodeDriverVersionListDriverVersion["VersionState"].isNull())
			driverVersionObject.versionState = dataNodeDriverVersionListDriverVersion["VersionState"].asString();
		if(!dataNodeDriverVersionListDriverVersion["EdgeVersion"].isNull())
			driverVersionObject.edgeVersion = dataNodeDriverVersionListDriverVersion["EdgeVersion"].asString();
		if(!dataNodeDriverVersionListDriverVersion["Description"].isNull())
			driverVersionObject.description = dataNodeDriverVersionListDriverVersion["Description"].asString();
		if(!dataNodeDriverVersionListDriverVersion["SourceConfig"].isNull())
			driverVersionObject.sourceConfig = dataNodeDriverVersionListDriverVersion["SourceConfig"].asString();
		if(!dataNodeDriverVersionListDriverVersion["DriverConfig"].isNull())
			driverVersionObject.driverConfig = dataNodeDriverVersionListDriverVersion["DriverConfig"].asString();
		if(!dataNodeDriverVersionListDriverVersion["ContainerConfig"].isNull())
			driverVersionObject.containerConfig = dataNodeDriverVersionListDriverVersion["ContainerConfig"].asString();
		if(!dataNodeDriverVersionListDriverVersion["ConfigCheckRule"].isNull())
			driverVersionObject.configCheckRule = dataNodeDriverVersionListDriverVersion["ConfigCheckRule"].asString();
		if(!dataNodeDriverVersionListDriverVersion["GmtCreateTimestamp"].isNull())
			driverVersionObject.gmtCreateTimestamp = std::stol(dataNodeDriverVersionListDriverVersion["GmtCreateTimestamp"].asString());
		if(!dataNodeDriverVersionListDriverVersion["GmtModifiedTimestamp"].isNull())
			driverVersionObject.gmtModifiedTimestamp = std::stol(dataNodeDriverVersionListDriverVersion["GmtModifiedTimestamp"].asString());
		if(!dataNodeDriverVersionListDriverVersion["Argument"].isNull())
			driverVersionObject.argument = dataNodeDriverVersionListDriverVersion["Argument"].asString();
		data_.driverVersionList.push_back(driverVersionObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

QueryEdgeDriverVersionResult::Data QueryEdgeDriverVersionResult::getData()const
{
	return data_;
}

std::string QueryEdgeDriverVersionResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string QueryEdgeDriverVersionResult::getCode()const
{
	return code_;
}

bool QueryEdgeDriverVersionResult::getSuccess()const
{
	return success_;
}

