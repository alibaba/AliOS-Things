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

#include <alibabacloud/aliyuncvc/model/ListEvaluationsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Aliyuncvc;
using namespace AlibabaCloud::Aliyuncvc::Model;

ListEvaluationsResult::ListEvaluationsResult() :
	ServiceResult()
{}

ListEvaluationsResult::ListEvaluationsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListEvaluationsResult::~ListEvaluationsResult()
{}

void ListEvaluationsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	if(!value["UserEvaluation"].isNull())
		userEvaluation_ = value["UserEvaluation"].asString();
	if(!value["ErrorCode"].isNull())
		errorCode_ = std::stoi(value["ErrorCode"].asString());
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";

}

std::string ListEvaluationsResult::getMessage()const
{
	return message_;
}

int ListEvaluationsResult::getErrorCode()const
{
	return errorCode_;
}

std::string ListEvaluationsResult::getUserEvaluation()const
{
	return userEvaluation_;
}

bool ListEvaluationsResult::getSuccess()const
{
	return success_;
}

