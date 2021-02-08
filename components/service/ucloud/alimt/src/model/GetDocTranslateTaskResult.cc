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

#include <alibabacloud/alimt/model/GetDocTranslateTaskResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Alimt;
using namespace AlibabaCloud::Alimt::Model;

GetDocTranslateTaskResult::GetDocTranslateTaskResult() :
	ServiceResult()
{}

GetDocTranslateTaskResult::GetDocTranslateTaskResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetDocTranslateTaskResult::~GetDocTranslateTaskResult()
{}

void GetDocTranslateTaskResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	if(!value["TaskId"].isNull())
		taskId_ = value["TaskId"].asString();
	if(!value["Status"].isNull())
		status_ = value["Status"].asString();
	if(!value["TranslateFileUrl"].isNull())
		translateFileUrl_ = value["TranslateFileUrl"].asString();
	if(!value["TranslateErrorCode"].isNull())
		translateErrorCode_ = value["TranslateErrorCode"].asString();
	if(!value["TranslateErrorMessage"].isNull())
		translateErrorMessage_ = value["TranslateErrorMessage"].asString();
	if(!value["PageCount"].isNull())
		pageCount_ = std::stoi(value["PageCount"].asString());

}

std::string GetDocTranslateTaskResult::getStatus()const
{
	return status_;
}

std::string GetDocTranslateTaskResult::getTranslateErrorMessage()const
{
	return translateErrorMessage_;
}

std::string GetDocTranslateTaskResult::getTaskId()const
{
	return taskId_;
}

int GetDocTranslateTaskResult::getPageCount()const
{
	return pageCount_;
}

std::string GetDocTranslateTaskResult::getTranslateFileUrl()const
{
	return translateFileUrl_;
}

std::string GetDocTranslateTaskResult::getTranslateErrorCode()const
{
	return translateErrorCode_;
}

