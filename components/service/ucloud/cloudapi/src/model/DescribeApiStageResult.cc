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

#include <alibabacloud/cloudapi/model/DescribeApiStageResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

DescribeApiStageResult::DescribeApiStageResult() :
	ServiceResult()
{}

DescribeApiStageResult::DescribeApiStageResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeApiStageResult::~DescribeApiStageResult()
{}

void DescribeApiStageResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allVariablesNode = value["Variables"]["VariableItem"];
	for (auto valueVariablesVariableItem : allVariablesNode)
	{
		VariableItem variablesObject;
		if(!valueVariablesVariableItem["VariableName"].isNull())
			variablesObject.variableName = valueVariablesVariableItem["VariableName"].asString();
		if(!valueVariablesVariableItem["VariableValue"].isNull())
			variablesObject.variableValue = valueVariablesVariableItem["VariableValue"].asString();
		if(!valueVariablesVariableItem["SupportRoute"].isNull())
			variablesObject.supportRoute = valueVariablesVariableItem["SupportRoute"].asString() == "true";
		auto stageRouteModelNode = value["StageRouteModel"];
		if(!stageRouteModelNode["ParameterCatalog"].isNull())
			variablesObject.stageRouteModel.parameterCatalog = stageRouteModelNode["ParameterCatalog"].asString();
		if(!stageRouteModelNode["ServiceParameterName"].isNull())
			variablesObject.stageRouteModel.serviceParameterName = stageRouteModelNode["ServiceParameterName"].asString();
		if(!stageRouteModelNode["Location"].isNull())
			variablesObject.stageRouteModel.location = stageRouteModelNode["Location"].asString();
		if(!stageRouteModelNode["ParameterType"].isNull())
			variablesObject.stageRouteModel.parameterType = stageRouteModelNode["ParameterType"].asString();
		if(!stageRouteModelNode["RouteMatchSymbol"].isNull())
			variablesObject.stageRouteModel.routeMatchSymbol = stageRouteModelNode["RouteMatchSymbol"].asString();
		auto allRouteRulesNode = stageRouteModelNode["RouteRules"]["RouteRuleItem"];
		for (auto stageRouteModelNodeRouteRulesRouteRuleItem : allRouteRulesNode)
		{
			VariableItem::StageRouteModel::RouteRuleItem routeRuleItemObject;
			if(!stageRouteModelNodeRouteRulesRouteRuleItem["MaxValue"].isNull())
				routeRuleItemObject.maxValue = std::stol(stageRouteModelNodeRouteRulesRouteRuleItem["MaxValue"].asString());
			if(!stageRouteModelNodeRouteRulesRouteRuleItem["MinValue"].isNull())
				routeRuleItemObject.minValue = std::stol(stageRouteModelNodeRouteRulesRouteRuleItem["MinValue"].asString());
			if(!stageRouteModelNodeRouteRulesRouteRuleItem["ConditionValue"].isNull())
				routeRuleItemObject.conditionValue = stageRouteModelNodeRouteRulesRouteRuleItem["ConditionValue"].asString();
			if(!stageRouteModelNodeRouteRulesRouteRuleItem["ResultValue"].isNull())
				routeRuleItemObject.resultValue = stageRouteModelNodeRouteRulesRouteRuleItem["ResultValue"].asString();
			variablesObject.stageRouteModel.routeRules.push_back(routeRuleItemObject);
		}
		variables_.push_back(variablesObject);
	}
	if(!value["GroupId"].isNull())
		groupId_ = value["GroupId"].asString();
	if(!value["StageId"].isNull())
		stageId_ = value["StageId"].asString();
	if(!value["StageName"].isNull())
		stageName_ = value["StageName"].asString();
	if(!value["Description"].isNull())
		description_ = value["Description"].asString();
	if(!value["CreatedTime"].isNull())
		createdTime_ = value["CreatedTime"].asString();
	if(!value["ModifiedTime"].isNull())
		modifiedTime_ = value["ModifiedTime"].asString();

}

std::vector<DescribeApiStageResult::VariableItem> DescribeApiStageResult::getVariables()const
{
	return variables_;
}

std::string DescribeApiStageResult::getDescription()const
{
	return description_;
}

std::string DescribeApiStageResult::getCreatedTime()const
{
	return createdTime_;
}

std::string DescribeApiStageResult::getModifiedTime()const
{
	return modifiedTime_;
}

std::string DescribeApiStageResult::getStageName()const
{
	return stageName_;
}

std::string DescribeApiStageResult::getStageId()const
{
	return stageId_;
}

std::string DescribeApiStageResult::getGroupId()const
{
	return groupId_;
}

