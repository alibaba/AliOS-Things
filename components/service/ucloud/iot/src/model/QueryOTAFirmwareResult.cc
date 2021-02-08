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

#include <alibabacloud/iot/model/QueryOTAFirmwareResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

QueryOTAFirmwareResult::QueryOTAFirmwareResult() :
	ServiceResult()
{}

QueryOTAFirmwareResult::QueryOTAFirmwareResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QueryOTAFirmwareResult::~QueryOTAFirmwareResult()
{}

void QueryOTAFirmwareResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto firmwareInfoNode = value["FirmwareInfo"];
	if(!firmwareInfoNode["FirmwareName"].isNull())
		firmwareInfo_.firmwareName = firmwareInfoNode["FirmwareName"].asString();
	if(!firmwareInfoNode["FirmwareId"].isNull())
		firmwareInfo_.firmwareId = firmwareInfoNode["FirmwareId"].asString();
	if(!firmwareInfoNode["SrcVersion"].isNull())
		firmwareInfo_.srcVersion = firmwareInfoNode["SrcVersion"].asString();
	if(!firmwareInfoNode["DestVersion"].isNull())
		firmwareInfo_.destVersion = firmwareInfoNode["DestVersion"].asString();
	if(!firmwareInfoNode["UtcCreate"].isNull())
		firmwareInfo_.utcCreate = firmwareInfoNode["UtcCreate"].asString();
	if(!firmwareInfoNode["UtcModified"].isNull())
		firmwareInfo_.utcModified = firmwareInfoNode["UtcModified"].asString();
	if(!firmwareInfoNode["Status"].isNull())
		firmwareInfo_.status = std::stoi(firmwareInfoNode["Status"].asString());
	if(!firmwareInfoNode["FirmwareDesc"].isNull())
		firmwareInfo_.firmwareDesc = firmwareInfoNode["FirmwareDesc"].asString();
	if(!firmwareInfoNode["FirmwareSign"].isNull())
		firmwareInfo_.firmwareSign = firmwareInfoNode["FirmwareSign"].asString();
	if(!firmwareInfoNode["FirmwareSize"].isNull())
		firmwareInfo_.firmwareSize = std::stoi(firmwareInfoNode["FirmwareSize"].asString());
	if(!firmwareInfoNode["FirmwareUrl"].isNull())
		firmwareInfo_.firmwareUrl = firmwareInfoNode["FirmwareUrl"].asString();
	if(!firmwareInfoNode["ProductKey"].isNull())
		firmwareInfo_.productKey = firmwareInfoNode["ProductKey"].asString();
	if(!firmwareInfoNode["SignMethod"].isNull())
		firmwareInfo_.signMethod = firmwareInfoNode["SignMethod"].asString();
	if(!firmwareInfoNode["ProductName"].isNull())
		firmwareInfo_.productName = firmwareInfoNode["ProductName"].asString();
	if(!firmwareInfoNode["Type"].isNull())
		firmwareInfo_.type = std::stoi(firmwareInfoNode["Type"].asString());
	if(!firmwareInfoNode["VerifyProgress"].isNull())
		firmwareInfo_.verifyProgress = std::stoi(firmwareInfoNode["VerifyProgress"].asString());
	if(!firmwareInfoNode["ModuleName"].isNull())
		firmwareInfo_.moduleName = firmwareInfoNode["ModuleName"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

QueryOTAFirmwareResult::FirmwareInfo QueryOTAFirmwareResult::getFirmwareInfo()const
{
	return firmwareInfo_;
}

std::string QueryOTAFirmwareResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string QueryOTAFirmwareResult::getCode()const
{
	return code_;
}

bool QueryOTAFirmwareResult::getSuccess()const
{
	return success_;
}

