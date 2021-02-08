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

#include <alibabacloud/iot/model/QuerySceneRuleResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

QuerySceneRuleResult::QuerySceneRuleResult() :
	ServiceResult()
{}

QuerySceneRuleResult::QuerySceneRuleResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QuerySceneRuleResult::~QuerySceneRuleResult()
{}

void QuerySceneRuleResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["Total"].isNull())
		data_.total = std::stoi(dataNode["Total"].asString());
	if(!dataNode["PageSize"].isNull())
		data_.pageSize = std::stoi(dataNode["PageSize"].asString());
	if(!dataNode["CurrentPage"].isNull())
		data_.currentPage = std::stoi(dataNode["CurrentPage"].asString());
	auto allRuleListNode = dataNode["RuleList"]["Rule"];
	for (auto dataNodeRuleListRule : allRuleListNode)
	{
		Data::Rule ruleObject;
		if(!dataNodeRuleListRule["RuleId"].isNull())
			ruleObject.ruleId = dataNodeRuleListRule["RuleId"].asString();
		if(!dataNodeRuleListRule["GmtCreate"].isNull())
			ruleObject.gmtCreate = std::stol(dataNodeRuleListRule["GmtCreate"].asString());
		if(!dataNodeRuleListRule["GmtModified"].isNull())
			ruleObject.gmtModified = std::stol(dataNodeRuleListRule["GmtModified"].asString());
		if(!dataNodeRuleListRule["RuleName"].isNull())
			ruleObject.ruleName = dataNodeRuleListRule["RuleName"].asString();
		if(!dataNodeRuleListRule["RuleDescription"].isNull())
			ruleObject.ruleDescription = dataNodeRuleListRule["RuleDescription"].asString();
		if(!dataNodeRuleListRule["RuleStatus"].isNull())
			ruleObject.ruleStatus = std::stoi(dataNodeRuleListRule["RuleStatus"].asString());
		data_.ruleList.push_back(ruleObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

QuerySceneRuleResult::Data QuerySceneRuleResult::getData()const
{
	return data_;
}

std::string QuerySceneRuleResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string QuerySceneRuleResult::getCode()const
{
	return code_;
}

bool QuerySceneRuleResult::getSuccess()const
{
	return success_;
}

