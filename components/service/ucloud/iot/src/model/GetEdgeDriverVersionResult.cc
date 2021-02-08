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

#include <alibabacloud/iot/model/GetEdgeDriverVersionResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

GetEdgeDriverVersionResult::GetEdgeDriverVersionResult() :
	ServiceResult()
{}

GetEdgeDriverVersionResult::GetEdgeDriverVersionResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetEdgeDriverVersionResult::~GetEdgeDriverVersionResult()
{}

void GetEdgeDriverVersionResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["DriverId"].isNull())
		data_.driverId = dataNode["DriverId"].asString();
	if(!dataNode["DriverVersion"].isNull())
		data_.driverVersion = dataNode["DriverVersion"].asString();
	if(!dataNode["VersionState"].isNull())
		data_.versionState = dataNode["VersionState"].asString();
	if(!dataNode["EdgeVersion"].isNull())
		data_.edgeVersion = dataNode["EdgeVersion"].asString();
	if(!dataNode["Description"].isNull())
		data_.description = dataNode["Description"].asString();
	if(!dataNode["SourceConfig"].isNull())
		data_.sourceConfig = dataNode["SourceConfig"].asString();
	if(!dataNode["DriverConfig"].isNull())
		data_.driverConfig = dataNode["DriverConfig"].asString();
	if(!dataNode["ContainerConfig"].isNull())
		data_.containerConfig = dataNode["ContainerConfig"].asString();
	if(!dataNode["ConfigCheckRule"].isNull())
		data_.configCheckRule = dataNode["ConfigCheckRule"].asString();
	if(!dataNode["GmtCreateTimestamp"].isNull())
		data_.gmtCreateTimestamp = std::stol(dataNode["GmtCreateTimestamp"].asString());
	if(!dataNode["GmtModifiedTimestamp"].isNull())
		data_.gmtModifiedTimestamp = std::stol(dataNode["GmtModifiedTimestamp"].asString());
	if(!dataNode["Argument"].isNull())
		data_.argument = dataNode["Argument"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

GetEdgeDriverVersionResult::Data GetEdgeDriverVersionResult::getData()const
{
	return data_;
}

std::string GetEdgeDriverVersionResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string GetEdgeDriverVersionResult::getCode()const
{
	return code_;
}

bool GetEdgeDriverVersionResult::getSuccess()const
{
	return success_;
}

