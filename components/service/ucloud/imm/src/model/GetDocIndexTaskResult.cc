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

#include <alibabacloud/imm/model/GetDocIndexTaskResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

GetDocIndexTaskResult::GetDocIndexTaskResult() :
	ServiceResult()
{}

GetDocIndexTaskResult::GetDocIndexTaskResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetDocIndexTaskResult::~GetDocIndexTaskResult()
{}

void GetDocIndexTaskResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto failDetailNode = value["FailDetail"];
	if(!failDetailNode["Code"].isNull())
		failDetail_.code = failDetailNode["Code"].asString();
	if(!value["Status"].isNull())
		status_ = value["Status"].asString();
	if(!value["TaskId"].isNull())
		taskId_ = value["TaskId"].asString();
	if(!value["CreateTime"].isNull())
		createTime_ = value["CreateTime"].asString();
	if(!value["FinishTime"].isNull())
		finishTime_ = value["FinishTime"].asString();

}

std::string GetDocIndexTaskResult::getStatus()const
{
	return status_;
}

GetDocIndexTaskResult::FailDetail GetDocIndexTaskResult::getFailDetail()const
{
	return failDetail_;
}

std::string GetDocIndexTaskResult::getTaskId()const
{
	return taskId_;
}

std::string GetDocIndexTaskResult::getFinishTime()const
{
	return finishTime_;
}

std::string GetDocIndexTaskResult::getCreateTime()const
{
	return createTime_;
}

