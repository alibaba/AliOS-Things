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

#include <alibabacloud/iot/model/ListRuleActionsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

ListRuleActionsResult::ListRuleActionsResult() :
	ServiceResult()
{}

ListRuleActionsResult::ListRuleActionsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListRuleActionsResult::~ListRuleActionsResult()
{}

void ListRuleActionsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allRuleActionListNode = value["RuleActionList"]["RuleActionInfo"];
	for (auto valueRuleActionListRuleActionInfo : allRuleActionListNode)
	{
		RuleActionInfo ruleActionListObject;
		if(!valueRuleActionListRuleActionInfo["Id"].isNull())
			ruleActionListObject.id = std::stol(valueRuleActionListRuleActionInfo["Id"].asString());
		if(!valueRuleActionListRuleActionInfo["RuleId"].isNull())
			ruleActionListObject.ruleId = std::stol(valueRuleActionListRuleActionInfo["RuleId"].asString());
		if(!valueRuleActionListRuleActionInfo["Type"].isNull())
			ruleActionListObject.type = valueRuleActionListRuleActionInfo["Type"].asString();
		if(!valueRuleActionListRuleActionInfo["Configuration"].isNull())
			ruleActionListObject.configuration = valueRuleActionListRuleActionInfo["Configuration"].asString();
		if(!valueRuleActionListRuleActionInfo["ErrorActionFlag"].isNull())
			ruleActionListObject.errorActionFlag = valueRuleActionListRuleActionInfo["ErrorActionFlag"].asString() == "true";
		ruleActionList_.push_back(ruleActionListObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

std::vector<ListRuleActionsResult::RuleActionInfo> ListRuleActionsResult::getRuleActionList()const
{
	return ruleActionList_;
}

std::string ListRuleActionsResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string ListRuleActionsResult::getCode()const
{
	return code_;
}

bool ListRuleActionsResult::getSuccess()const
{
	return success_;
}

