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

#include <alibabacloud/iot/model/QueryEdgeInstanceSceneRuleResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

QueryEdgeInstanceSceneRuleResult::QueryEdgeInstanceSceneRuleResult() :
	ServiceResult()
{}

QueryEdgeInstanceSceneRuleResult::QueryEdgeInstanceSceneRuleResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QueryEdgeInstanceSceneRuleResult::~QueryEdgeInstanceSceneRuleResult()
{}

void QueryEdgeInstanceSceneRuleResult::parse(const std::string &payload)
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
		if(!dataNodeRuleListRule["RuleName"].isNull())
			ruleObject.ruleName = dataNodeRuleListRule["RuleName"].asString();
		if(!dataNodeRuleListRule["GmtCreate"].isNull())
			ruleObject.gmtCreate = std::stol(dataNodeRuleListRule["GmtCreate"].asString());
		if(!dataNodeRuleListRule["Status"].isNull())
			ruleObject.status = std::stoi(dataNodeRuleListRule["Status"].asString());
		if(!dataNodeRuleListRule["IsExisted"].isNull())
			ruleObject.isExisted = std::stoi(dataNodeRuleListRule["IsExisted"].asString());
		data_.ruleList.push_back(ruleObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

QueryEdgeInstanceSceneRuleResult::Data QueryEdgeInstanceSceneRuleResult::getData()const
{
	return data_;
}

std::string QueryEdgeInstanceSceneRuleResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string QueryEdgeInstanceSceneRuleResult::getCode()const
{
	return code_;
}

bool QueryEdgeInstanceSceneRuleResult::getSuccess()const
{
	return success_;
}

