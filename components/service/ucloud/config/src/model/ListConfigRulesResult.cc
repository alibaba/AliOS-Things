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

#include <alibabacloud/config/model/ListConfigRulesResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Config;
using namespace AlibabaCloud::Config::Model;

ListConfigRulesResult::ListConfigRulesResult() :
	ServiceResult()
{}

ListConfigRulesResult::ListConfigRulesResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListConfigRulesResult::~ListConfigRulesResult()
{}

void ListConfigRulesResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto configRulesNode = value["ConfigRules"];
	if(!configRulesNode["PageNumber"].isNull())
		configRules_.pageNumber = std::stoi(configRulesNode["PageNumber"].asString());
	if(!configRulesNode["PageSize"].isNull())
		configRules_.pageSize = std::stoi(configRulesNode["PageSize"].asString());
	if(!configRulesNode["TotalCount"].isNull())
		configRules_.totalCount = std::stol(configRulesNode["TotalCount"].asString());
	auto allConfigRuleListNode = configRulesNode["ConfigRuleList"]["ConfigRule"];
	for (auto configRulesNodeConfigRuleListConfigRule : allConfigRuleListNode)
	{
		ConfigRules::ConfigRule configRuleObject;
		if(!configRulesNodeConfigRuleListConfigRule["AccountId"].isNull())
			configRuleObject.accountId = std::stol(configRulesNodeConfigRuleListConfigRule["AccountId"].asString());
		if(!configRulesNodeConfigRuleListConfigRule["ConfigRuleArn"].isNull())
			configRuleObject.configRuleArn = configRulesNodeConfigRuleListConfigRule["ConfigRuleArn"].asString();
		if(!configRulesNodeConfigRuleListConfigRule["ConfigRuleId"].isNull())
			configRuleObject.configRuleId = configRulesNodeConfigRuleListConfigRule["ConfigRuleId"].asString();
		if(!configRulesNodeConfigRuleListConfigRule["ConfigRuleName"].isNull())
			configRuleObject.configRuleName = configRulesNodeConfigRuleListConfigRule["ConfigRuleName"].asString();
		if(!configRulesNodeConfigRuleListConfigRule["ConfigRuleState"].isNull())
			configRuleObject.configRuleState = configRulesNodeConfigRuleListConfigRule["ConfigRuleState"].asString();
		if(!configRulesNodeConfigRuleListConfigRule["Description"].isNull())
			configRuleObject.description = configRulesNodeConfigRuleListConfigRule["Description"].asString();
		if(!configRulesNodeConfigRuleListConfigRule["RiskLevel"].isNull())
			configRuleObject.riskLevel = std::stoi(configRulesNodeConfigRuleListConfigRule["RiskLevel"].asString());
		if(!configRulesNodeConfigRuleListConfigRule["SourceIdentifier"].isNull())
			configRuleObject.sourceIdentifier = configRulesNodeConfigRuleListConfigRule["SourceIdentifier"].asString();
		if(!configRulesNodeConfigRuleListConfigRule["SourceOwner"].isNull())
			configRuleObject.sourceOwner = configRulesNodeConfigRuleListConfigRule["SourceOwner"].asString();
		if(!configRulesNodeConfigRuleListConfigRule["AutomationType"].isNull())
			configRuleObject.automationType = configRulesNodeConfigRuleListConfigRule["AutomationType"].asString();
		auto complianceNode = value["Compliance"];
		if(!complianceNode["ComplianceType"].isNull())
			configRuleObject.compliance.complianceType = complianceNode["ComplianceType"].asString();
		if(!complianceNode["Count"].isNull())
			configRuleObject.compliance.count = std::stoi(complianceNode["Count"].asString());
		auto createByNode = value["CreateBy"];
		if(!createByNode["CreatorId"].isNull())
			configRuleObject.createBy.creatorId = createByNode["CreatorId"].asString();
		if(!createByNode["CreatorName"].isNull())
			configRuleObject.createBy.creatorName = createByNode["CreatorName"].asString();
		if(!createByNode["CreatorType"].isNull())
			configRuleObject.createBy.creatorType = createByNode["CreatorType"].asString();
		if(!createByNode["ConfigRuleSceneId"].isNull())
			configRuleObject.createBy.configRuleSceneId = createByNode["ConfigRuleSceneId"].asString();
		if(!createByNode["ConfigRuleSceneName"].isNull())
			configRuleObject.createBy.configRuleSceneName = createByNode["ConfigRuleSceneName"].asString();
		configRules_.configRuleList.push_back(configRuleObject);
	}

}

ListConfigRulesResult::ConfigRules ListConfigRulesResult::getConfigRules()const
{
	return configRules_;
}

