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

#include <alibabacloud/visionai-poc/model/CheckMultiagentResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Visionai_poc;
using namespace AlibabaCloud::Visionai_poc::Model;

CheckMultiagentResult::CheckMultiagentResult() :
	ServiceResult()
{}

CheckMultiagentResult::CheckMultiagentResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

CheckMultiagentResult::~CheckMultiagentResult()
{}

void CheckMultiagentResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto responseNode = value["Response"];
	if(!responseNode["Data"].isNull())
		response_.data = responseNode["Data"].asString();
	if(!responseNode["Success"].isNull())
		response_.success = responseNode["Success"].asString() == "true";
	if(!responseNode["Url"].isNull())
		response_.url = responseNode["Url"].asString();
	if(!responseNode["RequestId"].isNull())
		response_.requestId = responseNode["RequestId"].asString();
	if(!responseNode["ErrorCode"].isNull())
		response_.errorCode = responseNode["ErrorCode"].asString();
	if(!responseNode["ErrorMessage"].isNull())
		response_.errorMessage = responseNode["ErrorMessage"].asString();
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();

}

CheckMultiagentResult::Response CheckMultiagentResult::getResponse()const
{
	return response_;
}

std::string CheckMultiagentResult::getMessage()const
{
	return message_;
}

std::string CheckMultiagentResult::getCode()const
{
	return code_;
}

bool CheckMultiagentResult::getSuccess()const
{
	return success_;
}

