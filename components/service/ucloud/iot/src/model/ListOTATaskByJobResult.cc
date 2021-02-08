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

#include <alibabacloud/iot/model/ListOTATaskByJobResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

ListOTATaskByJobResult::ListOTATaskByJobResult() :
	ServiceResult()
{}

ListOTATaskByJobResult::ListOTATaskByJobResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListOTATaskByJobResult::~ListOTATaskByJobResult()
{}

void ListOTATaskByJobResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allDataNode = value["Data"]["SimpleOTATaskInfo"];
	for (auto valueDataSimpleOTATaskInfo : allDataNode)
	{
		SimpleOTATaskInfo dataObject;
		if(!valueDataSimpleOTATaskInfo["TaskId"].isNull())
			dataObject.taskId = valueDataSimpleOTATaskInfo["TaskId"].asString();
		if(!valueDataSimpleOTATaskInfo["UtcModified"].isNull())
			dataObject.utcModified = valueDataSimpleOTATaskInfo["UtcModified"].asString();
		if(!valueDataSimpleOTATaskInfo["ProductKey"].isNull())
			dataObject.productKey = valueDataSimpleOTATaskInfo["ProductKey"].asString();
		if(!valueDataSimpleOTATaskInfo["FirmwareId"].isNull())
			dataObject.firmwareId = valueDataSimpleOTATaskInfo["FirmwareId"].asString();
		if(!valueDataSimpleOTATaskInfo["TaskStatus"].isNull())
			dataObject.taskStatus = valueDataSimpleOTATaskInfo["TaskStatus"].asString();
		if(!valueDataSimpleOTATaskInfo["JobId"].isNull())
			dataObject.jobId = valueDataSimpleOTATaskInfo["JobId"].asString();
		if(!valueDataSimpleOTATaskInfo["ProductName"].isNull())
			dataObject.productName = valueDataSimpleOTATaskInfo["ProductName"].asString();
		if(!valueDataSimpleOTATaskInfo["DeviceName"].isNull())
			dataObject.deviceName = valueDataSimpleOTATaskInfo["DeviceName"].asString();
		if(!valueDataSimpleOTATaskInfo["SrcVersion"].isNull())
			dataObject.srcVersion = valueDataSimpleOTATaskInfo["SrcVersion"].asString();
		if(!valueDataSimpleOTATaskInfo["DestVersion"].isNull())
			dataObject.destVersion = valueDataSimpleOTATaskInfo["DestVersion"].asString();
		if(!valueDataSimpleOTATaskInfo["TaskDesc"].isNull())
			dataObject.taskDesc = valueDataSimpleOTATaskInfo["TaskDesc"].asString();
		if(!valueDataSimpleOTATaskInfo["Progress"].isNull())
			dataObject.progress = valueDataSimpleOTATaskInfo["Progress"].asString();
		if(!valueDataSimpleOTATaskInfo["IotId"].isNull())
			dataObject.iotId = valueDataSimpleOTATaskInfo["IotId"].asString();
		if(!valueDataSimpleOTATaskInfo["UtcCreate"].isNull())
			dataObject.utcCreate = valueDataSimpleOTATaskInfo["UtcCreate"].asString();
		data_.push_back(dataObject);
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

int ListOTATaskByJobResult::getPageSize()const
{
	return pageSize_;
}

int ListOTATaskByJobResult::getPageCount()const
{
	return pageCount_;
}

int ListOTATaskByJobResult::getCurrentPage()const
{
	return currentPage_;
}

int ListOTATaskByJobResult::getTotal()const
{
	return total_;
}

std::vector<ListOTATaskByJobResult::SimpleOTATaskInfo> ListOTATaskByJobResult::getData()const
{
	return data_;
}

std::string ListOTATaskByJobResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string ListOTATaskByJobResult::getCode()const
{
	return code_;
}

bool ListOTATaskByJobResult::getSuccess()const
{
	return success_;
}

