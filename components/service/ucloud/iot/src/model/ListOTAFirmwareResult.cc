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

#include <alibabacloud/iot/model/ListOTAFirmwareResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

ListOTAFirmwareResult::ListOTAFirmwareResult() :
	ServiceResult()
{}

ListOTAFirmwareResult::ListOTAFirmwareResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListOTAFirmwareResult::~ListOTAFirmwareResult()
{}

void ListOTAFirmwareResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allFirmwareInfoNode = value["FirmwareInfo"]["SimpleFirmwareInfo"];
	for (auto valueFirmwareInfoSimpleFirmwareInfo : allFirmwareInfoNode)
	{
		SimpleFirmwareInfo firmwareInfoObject;
		if(!valueFirmwareInfoSimpleFirmwareInfo["FirmwareName"].isNull())
			firmwareInfoObject.firmwareName = valueFirmwareInfoSimpleFirmwareInfo["FirmwareName"].asString();
		if(!valueFirmwareInfoSimpleFirmwareInfo["FirmwareId"].isNull())
			firmwareInfoObject.firmwareId = valueFirmwareInfoSimpleFirmwareInfo["FirmwareId"].asString();
		if(!valueFirmwareInfoSimpleFirmwareInfo["SrcVersion"].isNull())
			firmwareInfoObject.srcVersion = valueFirmwareInfoSimpleFirmwareInfo["SrcVersion"].asString();
		if(!valueFirmwareInfoSimpleFirmwareInfo["DestVersion"].isNull())
			firmwareInfoObject.destVersion = valueFirmwareInfoSimpleFirmwareInfo["DestVersion"].asString();
		if(!valueFirmwareInfoSimpleFirmwareInfo["UtcCreate"].isNull())
			firmwareInfoObject.utcCreate = valueFirmwareInfoSimpleFirmwareInfo["UtcCreate"].asString();
		if(!valueFirmwareInfoSimpleFirmwareInfo["UtcModified"].isNull())
			firmwareInfoObject.utcModified = valueFirmwareInfoSimpleFirmwareInfo["UtcModified"].asString();
		if(!valueFirmwareInfoSimpleFirmwareInfo["Status"].isNull())
			firmwareInfoObject.status = std::stoi(valueFirmwareInfoSimpleFirmwareInfo["Status"].asString());
		if(!valueFirmwareInfoSimpleFirmwareInfo["FirmwareDesc"].isNull())
			firmwareInfoObject.firmwareDesc = valueFirmwareInfoSimpleFirmwareInfo["FirmwareDesc"].asString();
		if(!valueFirmwareInfoSimpleFirmwareInfo["FirmwareSign"].isNull())
			firmwareInfoObject.firmwareSign = valueFirmwareInfoSimpleFirmwareInfo["FirmwareSign"].asString();
		if(!valueFirmwareInfoSimpleFirmwareInfo["FirmwareSize"].isNull())
			firmwareInfoObject.firmwareSize = std::stoi(valueFirmwareInfoSimpleFirmwareInfo["FirmwareSize"].asString());
		if(!valueFirmwareInfoSimpleFirmwareInfo["FirmwareUrl"].isNull())
			firmwareInfoObject.firmwareUrl = valueFirmwareInfoSimpleFirmwareInfo["FirmwareUrl"].asString();
		if(!valueFirmwareInfoSimpleFirmwareInfo["ProductKey"].isNull())
			firmwareInfoObject.productKey = valueFirmwareInfoSimpleFirmwareInfo["ProductKey"].asString();
		if(!valueFirmwareInfoSimpleFirmwareInfo["SignMethod"].isNull())
			firmwareInfoObject.signMethod = valueFirmwareInfoSimpleFirmwareInfo["SignMethod"].asString();
		if(!valueFirmwareInfoSimpleFirmwareInfo["ProductName"].isNull())
			firmwareInfoObject.productName = valueFirmwareInfoSimpleFirmwareInfo["ProductName"].asString();
		if(!valueFirmwareInfoSimpleFirmwareInfo["Type"].isNull())
			firmwareInfoObject.type = std::stoi(valueFirmwareInfoSimpleFirmwareInfo["Type"].asString());
		if(!valueFirmwareInfoSimpleFirmwareInfo["ModuleName"].isNull())
			firmwareInfoObject.moduleName = valueFirmwareInfoSimpleFirmwareInfo["ModuleName"].asString();
		firmwareInfo_.push_back(firmwareInfoObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();
	if(!value["Total"].isNull())
		total_ = std::stoi(value["Total"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["PageCount"].isNull())
		pageCount_ = std::stoi(value["PageCount"].asString());
	if(!value["CurrentPage"].isNull())
		currentPage_ = std::stoi(value["CurrentPage"].asString());

}

std::vector<ListOTAFirmwareResult::SimpleFirmwareInfo> ListOTAFirmwareResult::getFirmwareInfo()const
{
	return firmwareInfo_;
}

int ListOTAFirmwareResult::getPageSize()const
{
	return pageSize_;
}

int ListOTAFirmwareResult::getPageCount()const
{
	return pageCount_;
}

int ListOTAFirmwareResult::getCurrentPage()const
{
	return currentPage_;
}

int ListOTAFirmwareResult::getTotal()const
{
	return total_;
}

std::string ListOTAFirmwareResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string ListOTAFirmwareResult::getCode()const
{
	return code_;
}

bool ListOTAFirmwareResult::getSuccess()const
{
	return success_;
}

