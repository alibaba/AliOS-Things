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

#include <alibabacloud/iot/model/QueryOTAJobResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

QueryOTAJobResult::QueryOTAJobResult() :
	ServiceResult()
{}

QueryOTAJobResult::QueryOTAJobResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QueryOTAJobResult::~QueryOTAJobResult()
{}

void QueryOTAJobResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["JobId"].isNull())
		data_.jobId = dataNode["JobId"].asString();
	if(!dataNode["UtcCreate"].isNull())
		data_.utcCreate = dataNode["UtcCreate"].asString();
	if(!dataNode["UtcModified"].isNull())
		data_.utcModified = dataNode["UtcModified"].asString();
	if(!dataNode["ProductKey"].isNull())
		data_.productKey = dataNode["ProductKey"].asString();
	if(!dataNode["FirmwareId"].isNull())
		data_.firmwareId = dataNode["FirmwareId"].asString();
	if(!dataNode["UtcStartTime"].isNull())
		data_.utcStartTime = dataNode["UtcStartTime"].asString();
	if(!dataNode["UtcEndTime"].isNull())
		data_.utcEndTime = dataNode["UtcEndTime"].asString();
	if(!dataNode["JobStatus"].isNull())
		data_.jobStatus = dataNode["JobStatus"].asString();
	if(!dataNode["JobType"].isNull())
		data_.jobType = dataNode["JobType"].asString();
	if(!dataNode["JobDesc"].isNull())
		data_.jobDesc = dataNode["JobDesc"].asString();
	if(!dataNode["Name"].isNull())
		data_.name = dataNode["Name"].asString();
	if(!dataNode["UtcScheduleTime"].isNull())
		data_.utcScheduleTime = dataNode["UtcScheduleTime"].asString();
	if(!dataNode["RetryInterval"].isNull())
		data_.retryInterval = std::stoi(dataNode["RetryInterval"].asString());
	if(!dataNode["RetryCount"].isNull())
		data_.retryCount = std::stoi(dataNode["RetryCount"].asString());
	if(!dataNode["TimeoutInMinutes"].isNull())
		data_.timeoutInMinutes = std::stoi(dataNode["TimeoutInMinutes"].asString());
	if(!dataNode["TargetSelection"].isNull())
		data_.targetSelection = dataNode["TargetSelection"].asString();
	if(!dataNode["SelectionType"].isNull())
		data_.selectionType = dataNode["SelectionType"].asString();
	if(!dataNode["GrayPercent"].isNull())
		data_.grayPercent = dataNode["GrayPercent"].asString();
	if(!dataNode["MaximumPerMinute"].isNull())
		data_.maximumPerMinute = std::stoi(dataNode["MaximumPerMinute"].asString());
	if(!dataNode["DestVersion"].isNull())
		data_.destVersion = dataNode["DestVersion"].asString();
		auto allSrcVersions = dataNode["SrcVersions"]["SrcVersion"];
		for (auto value : allSrcVersions)
			data_.srcVersions.push_back(value.asString());
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

QueryOTAJobResult::Data QueryOTAJobResult::getData()const
{
	return data_;
}

std::string QueryOTAJobResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string QueryOTAJobResult::getCode()const
{
	return code_;
}

bool QueryOTAJobResult::getSuccess()const
{
	return success_;
}

