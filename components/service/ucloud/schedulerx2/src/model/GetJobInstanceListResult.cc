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

#include <alibabacloud/schedulerx2/model/GetJobInstanceListResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Schedulerx2;
using namespace AlibabaCloud::Schedulerx2::Model;

GetJobInstanceListResult::GetJobInstanceListResult() :
	ServiceResult()
{}

GetJobInstanceListResult::GetJobInstanceListResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetJobInstanceListResult::~GetJobInstanceListResult()
{}

void GetJobInstanceListResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto allJobInstanceDetailsNode = dataNode["JobInstanceDetails"]["JobInstanceDetailsItem"];
	for (auto dataNodeJobInstanceDetailsJobInstanceDetailsItem : allJobInstanceDetailsNode)
	{
		Data::JobInstanceDetailsItem jobInstanceDetailsItemObject;
		if(!dataNodeJobInstanceDetailsJobInstanceDetailsItem["InstanceId"].isNull())
			jobInstanceDetailsItemObject.instanceId = std::stol(dataNodeJobInstanceDetailsJobInstanceDetailsItem["InstanceId"].asString());
		if(!dataNodeJobInstanceDetailsJobInstanceDetailsItem["JobId"].isNull())
			jobInstanceDetailsItemObject.jobId = std::stol(dataNodeJobInstanceDetailsJobInstanceDetailsItem["JobId"].asString());
		if(!dataNodeJobInstanceDetailsJobInstanceDetailsItem["Status"].isNull())
			jobInstanceDetailsItemObject.status = std::stoi(dataNodeJobInstanceDetailsJobInstanceDetailsItem["Status"].asString());
		if(!dataNodeJobInstanceDetailsJobInstanceDetailsItem["StartTime"].isNull())
			jobInstanceDetailsItemObject.startTime = dataNodeJobInstanceDetailsJobInstanceDetailsItem["StartTime"].asString();
		if(!dataNodeJobInstanceDetailsJobInstanceDetailsItem["EndTime"].isNull())
			jobInstanceDetailsItemObject.endTime = dataNodeJobInstanceDetailsJobInstanceDetailsItem["EndTime"].asString();
		if(!dataNodeJobInstanceDetailsJobInstanceDetailsItem["ScheduleTime"].isNull())
			jobInstanceDetailsItemObject.scheduleTime = dataNodeJobInstanceDetailsJobInstanceDetailsItem["ScheduleTime"].asString();
		if(!dataNodeJobInstanceDetailsJobInstanceDetailsItem["DataTime"].isNull())
			jobInstanceDetailsItemObject.dataTime = dataNodeJobInstanceDetailsJobInstanceDetailsItem["DataTime"].asString();
		if(!dataNodeJobInstanceDetailsJobInstanceDetailsItem["Executor"].isNull())
			jobInstanceDetailsItemObject.executor = dataNodeJobInstanceDetailsJobInstanceDetailsItem["Executor"].asString();
		if(!dataNodeJobInstanceDetailsJobInstanceDetailsItem["WorkAddr"].isNull())
			jobInstanceDetailsItemObject.workAddr = dataNodeJobInstanceDetailsJobInstanceDetailsItem["WorkAddr"].asString();
		if(!dataNodeJobInstanceDetailsJobInstanceDetailsItem["Result"].isNull())
			jobInstanceDetailsItemObject.result = dataNodeJobInstanceDetailsJobInstanceDetailsItem["Result"].asString();
		if(!dataNodeJobInstanceDetailsJobInstanceDetailsItem["Progress"].isNull())
			jobInstanceDetailsItemObject.progress = dataNodeJobInstanceDetailsJobInstanceDetailsItem["Progress"].asString();
		if(!dataNodeJobInstanceDetailsJobInstanceDetailsItem["TimeType"].isNull())
			jobInstanceDetailsItemObject.timeType = std::stoi(dataNodeJobInstanceDetailsJobInstanceDetailsItem["TimeType"].asString());
		if(!dataNodeJobInstanceDetailsJobInstanceDetailsItem["TriggerType"].isNull())
			jobInstanceDetailsItemObject.triggerType = std::stoi(dataNodeJobInstanceDetailsJobInstanceDetailsItem["TriggerType"].asString());
		data_.jobInstanceDetails.push_back(jobInstanceDetailsItemObject);
	}
	if(!value["Code"].isNull())
		code_ = std::stoi(value["Code"].asString());
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";

}

std::string GetJobInstanceListResult::getMessage()const
{
	return message_;
}

GetJobInstanceListResult::Data GetJobInstanceListResult::getData()const
{
	return data_;
}

int GetJobInstanceListResult::getCode()const
{
	return code_;
}

bool GetJobInstanceListResult::getSuccess()const
{
	return success_;
}

