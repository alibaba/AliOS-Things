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

#include <alibabacloud/config/model/DescribeConfigRuleResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Config;
using namespace AlibabaCloud::Config::Model;

DescribeConfigRuleResult::DescribeConfigRuleResult() :
	ServiceResult()
{}

DescribeConfigRuleResult::DescribeConfigRuleResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeConfigRuleResult::~DescribeConfigRuleResult()
{}

void DescribeConfigRuleResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto configRuleNode = value["ConfigRule"];
	if(!configRuleNode["ConfigRuleArn"].isNull())
		configRule_.configRuleArn = configRuleNode["ConfigRuleArn"].asString();
	if(!configRuleNode["ConfigRuleId"].isNull())
		configRule_.configRuleId = configRuleNode["ConfigRuleId"].asString();
	if(!configRuleNode["ConfigRuleName"].isNull())
		configRule_.configRuleName = configRuleNode["ConfigRuleName"].asString();
	if(!configRuleNode["ConfigRuleState"].isNull())
		configRule_.configRuleState = configRuleNode["ConfigRuleState"].asString();
	if(!configRuleNode["CreateTimestamp"].isNull())
		configRule_.createTimestamp = std::stol(configRuleNode["CreateTimestamp"].asString());
	if(!configRuleNode["Description"].isNull())
		configRule_.description = configRuleNode["Description"].asString();
	if(!configRuleNode["InputParameters"].isNull())
		configRule_.inputParameters = configRuleNode["InputParameters"].asString();
	if(!configRuleNode["ModifiedTimestamp"].isNull())
		configRule_.modifiedTimestamp = std::stol(configRuleNode["ModifiedTimestamp"].asString());
	if(!configRuleNode["RiskLevel"].isNull())
		configRule_.riskLevel = std::stoi(configRuleNode["RiskLevel"].asString());
	if(!configRuleNode["MaximumExecutionFrequency"].isNull())
		configRule_.maximumExecutionFrequency = configRuleNode["MaximumExecutionFrequency"].asString();
	auto createByNode = configRuleNode["CreateBy"];
	if(!createByNode["CreatorType"].isNull())
		configRule_.createBy.creatorType = createByNode["CreatorType"].asString();
	if(!createByNode["CreatorId"].isNull())
		configRule_.createBy.creatorId = createByNode["CreatorId"].asString();
	if(!createByNode["CreatorName"].isNull())
		configRule_.createBy.creatorName = createByNode["CreatorName"].asString();
	if(!createByNode["ConfigRuleSceneId"].isNull())
		configRule_.createBy.configRuleSceneId = createByNode["ConfigRuleSceneId"].asString();
	if(!createByNode["ConfigRuleSceneName"].isNull())
		configRule_.createBy.configRuleSceneName = createByNode["ConfigRuleSceneName"].asString();
	auto configRuleEvaluationStatusNode = configRuleNode["ConfigRuleEvaluationStatus"];
	if(!configRuleEvaluationStatusNode["FirstActivatedTimestamp"].isNull())
		configRule_.configRuleEvaluationStatus.firstActivatedTimestamp = std::stol(configRuleEvaluationStatusNode["FirstActivatedTimestamp"].asString());
	if(!configRuleEvaluationStatusNode["FirstEvaluationStarted"].isNull())
		configRule_.configRuleEvaluationStatus.firstEvaluationStarted = configRuleEvaluationStatusNode["FirstEvaluationStarted"].asString() == "true";
	if(!configRuleEvaluationStatusNode["LastErrorCode"].isNull())
		configRule_.configRuleEvaluationStatus.lastErrorCode = configRuleEvaluationStatusNode["LastErrorCode"].asString();
	if(!configRuleEvaluationStatusNode["LastErrorMessage"].isNull())
		configRule_.configRuleEvaluationStatus.lastErrorMessage = configRuleEvaluationStatusNode["LastErrorMessage"].asString();
	if(!configRuleEvaluationStatusNode["LastFailedEvaluationTimestamp"].isNull())
		configRule_.configRuleEvaluationStatus.lastFailedEvaluationTimestamp = std::stol(configRuleEvaluationStatusNode["LastFailedEvaluationTimestamp"].asString());
	if(!configRuleEvaluationStatusNode["LastFailedInvocationTimestamp"].isNull())
		configRule_.configRuleEvaluationStatus.lastFailedInvocationTimestamp = std::stol(configRuleEvaluationStatusNode["LastFailedInvocationTimestamp"].asString());
	if(!configRuleEvaluationStatusNode["LastSuccessfulEvaluationTimestamp"].isNull())
		configRule_.configRuleEvaluationStatus.lastSuccessfulEvaluationTimestamp = std::stol(configRuleEvaluationStatusNode["LastSuccessfulEvaluationTimestamp"].asString());
	if(!configRuleEvaluationStatusNode["LastSuccessfulInvocationTimestamp"].isNull())
		configRule_.configRuleEvaluationStatus.lastSuccessfulInvocationTimestamp = std::stol(configRuleEvaluationStatusNode["LastSuccessfulInvocationTimestamp"].asString());
	auto managedRuleNode = configRuleNode["ManagedRule"];
	if(!managedRuleNode["ManagedRuleName"].isNull())
		configRule_.managedRule.managedRuleName = managedRuleNode["ManagedRuleName"].asString();
	if(!managedRuleNode["Description"].isNull())
		configRule_.managedRule.description = managedRuleNode["Description"].asString();
	if(!managedRuleNode["Identifier"].isNull())
		configRule_.managedRule.identifier = managedRuleNode["Identifier"].asString();
	if(!managedRuleNode["CompulsoryInputParameterDetails"].isNull())
		configRule_.managedRule.compulsoryInputParameterDetails = managedRuleNode["CompulsoryInputParameterDetails"].asString();
	if(!managedRuleNode["OptionalInputParameterDetails"].isNull())
		configRule_.managedRule.optionalInputParameterDetails = managedRuleNode["OptionalInputParameterDetails"].asString();
	if(!managedRuleNode["HelpUrl"].isNull())
		configRule_.managedRule.helpUrl = managedRuleNode["HelpUrl"].asString();
	auto allSourceDetailsNode = managedRuleNode["SourceDetails"]["SourceDetailsItem"];
	for (auto managedRuleNodeSourceDetailsSourceDetailsItem : allSourceDetailsNode)
	{
		ConfigRule::ManagedRule::SourceDetailsItem sourceDetailsItemObject;
		if(!managedRuleNodeSourceDetailsSourceDetailsItem["EventSource"].isNull())
			sourceDetailsItemObject.eventSource = managedRuleNodeSourceDetailsSourceDetailsItem["EventSource"].asString();
		if(!managedRuleNodeSourceDetailsSourceDetailsItem["MaximumExecutionFrequency"].isNull())
			sourceDetailsItemObject.maximumExecutionFrequency = managedRuleNodeSourceDetailsSourceDetailsItem["MaximumExecutionFrequency"].asString();
		if(!managedRuleNodeSourceDetailsSourceDetailsItem["MessageType"].isNull())
			sourceDetailsItemObject.messageType = managedRuleNodeSourceDetailsSourceDetailsItem["MessageType"].asString();
		configRule_.managedRule.sourceDetails.push_back(sourceDetailsItemObject);
	}
		auto allLabels = managedRuleNode["Labels"]["Labels"];
		for (auto value : allLabels)
			configRule_.managedRule.labels.push_back(value.asString());
	auto sourceNode = configRuleNode["Source"];
	if(!sourceNode["Identifier"].isNull())
		configRule_.source.identifier = sourceNode["Identifier"].asString();
	if(!sourceNode["Owner"].isNull())
		configRule_.source.owner = sourceNode["Owner"].asString();
	auto allSourceConditionsNode = sourceNode["SourceConditions"]["SourceConditionsItem"];
	for (auto sourceNodeSourceConditionsSourceConditionsItem : allSourceConditionsNode)
	{
		ConfigRule::Source::SourceConditionsItem sourceConditionsItemObject;
		if(!sourceNodeSourceConditionsSourceConditionsItem["DesiredValue"].isNull())
			sourceConditionsItemObject.desiredValue = sourceNodeSourceConditionsSourceConditionsItem["DesiredValue"].asString();
		if(!sourceNodeSourceConditionsSourceConditionsItem["Name"].isNull())
			sourceConditionsItemObject.name = sourceNodeSourceConditionsSourceConditionsItem["Name"].asString();
		if(!sourceNodeSourceConditionsSourceConditionsItem["Operator"].isNull())
			sourceConditionsItemObject._operator = sourceNodeSourceConditionsSourceConditionsItem["Operator"].asString();
		if(!sourceNodeSourceConditionsSourceConditionsItem["Required"].isNull())
			sourceConditionsItemObject.required = sourceNodeSourceConditionsSourceConditionsItem["Required"].asString() == "true";
		if(!sourceNodeSourceConditionsSourceConditionsItem["SelectPath"].isNull())
			sourceConditionsItemObject.selectPath = sourceNodeSourceConditionsSourceConditionsItem["SelectPath"].asString();
		if(!sourceNodeSourceConditionsSourceConditionsItem["Tips"].isNull())
			sourceConditionsItemObject.tips = sourceNodeSourceConditionsSourceConditionsItem["Tips"].asString();
		configRule_.source.sourceConditions.push_back(sourceConditionsItemObject);
	}
	auto allSourceDetails1Node = sourceNode["SourceDetails"]["SourceDetailsItem"];
	for (auto sourceNodeSourceDetailsSourceDetailsItem : allSourceDetails1Node)
	{
		ConfigRule::Source::SourceDetailsItem2 sourceDetailsItem2Object;
		if(!sourceNodeSourceDetailsSourceDetailsItem["EventSource"].isNull())
			sourceDetailsItem2Object.eventSource = sourceNodeSourceDetailsSourceDetailsItem["EventSource"].asString();
		if(!sourceNodeSourceDetailsSourceDetailsItem["MaximumExecutionFrequency"].isNull())
			sourceDetailsItem2Object.maximumExecutionFrequency = sourceNodeSourceDetailsSourceDetailsItem["MaximumExecutionFrequency"].asString();
		if(!sourceNodeSourceDetailsSourceDetailsItem["MessageType"].isNull())
			sourceDetailsItem2Object.messageType = sourceNodeSourceDetailsSourceDetailsItem["MessageType"].asString();
		configRule_.source.sourceDetails1.push_back(sourceDetailsItem2Object);
	}
	auto scopeNode = configRuleNode["Scope"];
	if(!scopeNode["ComplianceResourceId"].isNull())
		configRule_.scope.complianceResourceId = scopeNode["ComplianceResourceId"].asString();
		auto allComplianceResourceTypes = scopeNode["ComplianceResourceTypes"]["ComplianceResourceTypes"];
		for (auto value : allComplianceResourceTypes)
			configRule_.scope.complianceResourceTypes.push_back(value.asString());

}

DescribeConfigRuleResult::ConfigRule DescribeConfigRuleResult::getConfigRule()const
{
	return configRule_;
}

