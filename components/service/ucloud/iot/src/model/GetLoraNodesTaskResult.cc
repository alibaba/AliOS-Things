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

#include <alibabacloud/iot/model/GetLoraNodesTaskResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

GetLoraNodesTaskResult::GetLoraNodesTaskResult() :
	ServiceResult()
{}

GetLoraNodesTaskResult::GetLoraNodesTaskResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetLoraNodesTaskResult::~GetLoraNodesTaskResult()
{}

void GetLoraNodesTaskResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allSuccessDevEuis = value["SuccessDevEuis"]["SuccessDevEui"];
	for (const auto &item : allSuccessDevEuis)
		successDevEuis_.push_back(item.asString());
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();
	if(!value["TaskId"].isNull())
		taskId_ = value["TaskId"].asString();
	if(!value["TaskState"].isNull())
		taskState_ = value["TaskState"].asString();
	if(!value["TotalCount"].isNull())
		totalCount_ = std::stol(value["TotalCount"].asString());
	if(!value["SuccessCount"].isNull())
		successCount_ = std::stol(value["SuccessCount"].asString());

}

std::vector<std::string> GetLoraNodesTaskResult::getSuccessDevEuis()const
{
	return successDevEuis_;
}

long GetLoraNodesTaskResult::getTotalCount()const
{
	return totalCount_;
}

std::string GetLoraNodesTaskResult::getTaskId()const
{
	return taskId_;
}

std::string GetLoraNodesTaskResult::getErrorMessage()const
{
	return errorMessage_;
}

long GetLoraNodesTaskResult::getSuccessCount()const
{
	return successCount_;
}

std::string GetLoraNodesTaskResult::getCode()const
{
	return code_;
}

std::string GetLoraNodesTaskResult::getTaskState()const
{
	return taskState_;
}

bool GetLoraNodesTaskResult::getSuccess()const
{
	return success_;
}

