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

#include <alibabacloud/aliyuncvc/model/GetAccountInfoResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Aliyuncvc;
using namespace AlibabaCloud::Aliyuncvc::Model;

GetAccountInfoResult::GetAccountInfoResult() :
	ServiceResult()
{}

GetAccountInfoResult::GetAccountInfoResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetAccountInfoResult::~GetAccountInfoResult()
{}

void GetAccountInfoResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto accountInfoNode = value["AccountInfo"];
	if(!accountInfoNode["AccountConcurrentMax"].isNull())
		accountInfo_.accountConcurrentMax = std::stoi(accountInfoNode["AccountConcurrentMax"].asString());
	if(!accountInfoNode["AccountApplicationMax"].isNull())
		accountInfo_.accountApplicationMax = std::stoi(accountInfoNode["AccountApplicationMax"].asString());
	if(!accountInfoNode["AccountApplicationNumber"].isNull())
		accountInfo_.accountApplicationNumber = std::stoi(accountInfoNode["AccountApplicationNumber"].asString());
	if(!value["ErrorCode"].isNull())
		errorCode_ = std::stoi(value["ErrorCode"].asString());
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";

}

std::string GetAccountInfoResult::getMessage()const
{
	return message_;
}

int GetAccountInfoResult::getErrorCode()const
{
	return errorCode_;
}

GetAccountInfoResult::AccountInfo GetAccountInfoResult::getAccountInfo()const
{
	return accountInfo_;
}

bool GetAccountInfoResult::getSuccess()const
{
	return success_;
}

