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

#include <alibabacloud/iot/model/GetRuleActionResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

GetRuleActionResult::GetRuleActionResult() :
	ServiceResult()
{}

GetRuleActionResult::GetRuleActionResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetRuleActionResult::~GetRuleActionResult()
{}

void GetRuleActionResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto ruleActionInfoNode = value["RuleActionInfo"];
	if(!ruleActionInfoNode["Id"].isNull())
		ruleActionInfo_.id = std::stol(ruleActionInfoNode["Id"].asString());
	if(!ruleActionInfoNode["RuleId"].isNull())
		ruleActionInfo_.ruleId = std::stol(ruleActionInfoNode["RuleId"].asString());
	if(!ruleActionInfoNode["Type"].isNull())
		ruleActionInfo_.type = ruleActionInfoNode["Type"].asString();
	if(!ruleActionInfoNode["Configuration"].isNull())
		ruleActionInfo_.configuration = ruleActionInfoNode["Configuration"].asString();
	if(!ruleActionInfoNode["ErrorActionFlag"].isNull())
		ruleActionInfo_.errorActionFlag = ruleActionInfoNode["ErrorActionFlag"].asString() == "true";
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

GetRuleActionResult::RuleActionInfo GetRuleActionResult::getRuleActionInfo()const
{
	return ruleActionInfo_;
}

std::string GetRuleActionResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string GetRuleActionResult::getCode()const
{
	return code_;
}

bool GetRuleActionResult::getSuccess()const
{
	return success_;
}

