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

#include <alibabacloud/schedulerx2/model/GetJobInstanceResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Schedulerx2;
using namespace AlibabaCloud::Schedulerx2::Model;

GetJobInstanceResult::GetJobInstanceResult() :
	ServiceResult()
{}

GetJobInstanceResult::GetJobInstanceResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetJobInstanceResult::~GetJobInstanceResult()
{}

void GetJobInstanceResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto jobInstanceDetailNode = dataNode["JobInstanceDetail"];
	if(!jobInstanceDetailNode["InstanceId"].isNull())
		data_.jobInstanceDetail.instanceId = std::stol(jobInstanceDetailNode["InstanceId"].asString());
	if(!jobInstanceDetailNode["JobId"].isNull())
		data_.jobInstanceDetail.jobId = std::stol(jobInstanceDetailNode["JobId"].asString());
	if(!jobInstanceDetailNode["Status"].isNull())
		data_.jobInstanceDetail.status = std::stoi(jobInstanceDetailNode["Status"].asString());
	if(!jobInstanceDetailNode["StartTime"].isNull())
		data_.jobInstanceDetail.startTime = jobInstanceDetailNode["StartTime"].asString();
	if(!jobInstanceDetailNode["EndTime"].isNull())
		data_.jobInstanceDetail.endTime = jobInstanceDetailNode["EndTime"].asString();
	if(!jobInstanceDetailNode["ScheduleTime"].isNull())
		data_.jobInstanceDetail.scheduleTime = jobInstanceDetailNode["ScheduleTime"].asString();
	if(!jobInstanceDetailNode["DataTime"].isNull())
		data_.jobInstanceDetail.dataTime = jobInstanceDetailNode["DataTime"].asString();
	if(!jobInstanceDetailNode["Executor"].isNull())
		data_.jobInstanceDetail.executor = jobInstanceDetailNode["Executor"].asString();
	if(!jobInstanceDetailNode["WorkAddr"].isNull())
		data_.jobInstanceDetail.workAddr = jobInstanceDetailNode["WorkAddr"].asString();
	if(!jobInstanceDetailNode["Result"].isNull())
		data_.jobInstanceDetail.result = jobInstanceDetailNode["Result"].asString();
	if(!jobInstanceDetailNode["Progress"].isNull())
		data_.jobInstanceDetail.progress = jobInstanceDetailNode["Progress"].asString();
	if(!jobInstanceDetailNode["TimeType"].isNull())
		data_.jobInstanceDetail.timeType = std::stoi(jobInstanceDetailNode["TimeType"].asString());
	if(!jobInstanceDetailNode["TriggerType"].isNull())
		data_.jobInstanceDetail.triggerType = std::stoi(jobInstanceDetailNode["TriggerType"].asString());
	if(!value["Code"].isNull())
		code_ = std::stoi(value["Code"].asString());
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";

}

std::string GetJobInstanceResult::getMessage()const
{
	return message_;
}

GetJobInstanceResult::Data GetJobInstanceResult::getData()const
{
	return data_;
}

int GetJobInstanceResult::getCode()const
{
	return code_;
}

bool GetJobInstanceResult::getSuccess()const
{
	return success_;
}

