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

#include <alibabacloud/iot/model/GetSceneRuleResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

GetSceneRuleResult::GetSceneRuleResult() :
	ServiceResult()
{}

GetSceneRuleResult::GetSceneRuleResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetSceneRuleResult::~GetSceneRuleResult()
{}

void GetSceneRuleResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["GmtCreate"].isNull())
		data_.gmtCreate = std::stol(dataNode["GmtCreate"].asString());
	if(!dataNode["GmtModified"].isNull())
		data_.gmtModified = std::stol(dataNode["GmtModified"].asString());
	if(!dataNode["RuleName"].isNull())
		data_.ruleName = dataNode["RuleName"].asString();
	if(!dataNode["RuleContent"].isNull())
		data_.ruleContent = dataNode["RuleContent"].asString();
	if(!dataNode["RuleDescription"].isNull())
		data_.ruleDescription = dataNode["RuleDescription"].asString();
	if(!dataNode["RuleStatus"].isNull())
		data_.ruleStatus = std::stoi(dataNode["RuleStatus"].asString());
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();

}

GetSceneRuleResult::Data GetSceneRuleResult::getData()const
{
	return data_;
}

std::string GetSceneRuleResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string GetSceneRuleResult::getCode()const
{
	return code_;
}

bool GetSceneRuleResult::getSuccess()const
{
	return success_;
}

