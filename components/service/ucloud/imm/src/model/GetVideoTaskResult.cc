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

#include <alibabacloud/imm/model/GetVideoTaskResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

GetVideoTaskResult::GetVideoTaskResult() :
	ServiceResult()
{}

GetVideoTaskResult::GetVideoTaskResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetVideoTaskResult::~GetVideoTaskResult()
{}

void GetVideoTaskResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	if(!value["TaskId"].isNull())
		taskId_ = value["TaskId"].asString();
	if(!value["TaskType"].isNull())
		taskType_ = value["TaskType"].asString();
	if(!value["Parameters"].isNull())
		parameters_ = value["Parameters"].asString();
	if(!value["Result"].isNull())
		result_ = value["Result"].asString();
	if(!value["Status"].isNull())
		status_ = value["Status"].asString();
	if(!value["StartTime"].isNull())
		startTime_ = value["StartTime"].asString();
	if(!value["EndTime"].isNull())
		endTime_ = value["EndTime"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();
	if(!value["NotifyEndpoint"].isNull())
		notifyEndpoint_ = value["NotifyEndpoint"].asString();
	if(!value["NotifyTopicName"].isNull())
		notifyTopicName_ = value["NotifyTopicName"].asString();
	if(!value["Progress"].isNull())
		progress_ = std::stoi(value["Progress"].asString());

}

std::string GetVideoTaskResult::getStatus()const
{
	return status_;
}

std::string GetVideoTaskResult::getNotifyEndpoint()const
{
	return notifyEndpoint_;
}

int GetVideoTaskResult::getProgress()const
{
	return progress_;
}

std::string GetVideoTaskResult::getTaskId()const
{
	return taskId_;
}

std::string GetVideoTaskResult::getParameters()const
{
	return parameters_;
}

std::string GetVideoTaskResult::getEndTime()const
{
	return endTime_;
}

std::string GetVideoTaskResult::getTaskType()const
{
	return taskType_;
}

std::string GetVideoTaskResult::getStartTime()const
{
	return startTime_;
}

std::string GetVideoTaskResult::getNotifyTopicName()const
{
	return notifyTopicName_;
}

std::string GetVideoTaskResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string GetVideoTaskResult::getResult()const
{
	return result_;
}

