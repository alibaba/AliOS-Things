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

#include <alibabacloud/config/model/ListConfigRulesRequest.h>

using AlibabaCloud::Config::Model::ListConfigRulesRequest;

ListConfigRulesRequest::ListConfigRulesRequest() :
	RpcServiceRequest("config", "2019-01-08", "ListConfigRules")
{
	setMethod(HttpRequest::Method::Get);
}

ListConfigRulesRequest::~ListConfigRulesRequest()
{}

bool ListConfigRulesRequest::getMultiAccount()const
{
	return multiAccount_;
}

void ListConfigRulesRequest::setMultiAccount(bool multiAccount)
{
	multiAccount_ = multiAccount;
	setParameter("MultiAccount", multiAccount ? "true" : "false");
}

std::string ListConfigRulesRequest::getMessageType()const
{
	return messageType_;
}

void ListConfigRulesRequest::setMessageType(const std::string& messageType)
{
	messageType_ = messageType;
	setParameter("MessageType", messageType);
}

std::string ListConfigRulesRequest::getConfigRuleState()const
{
	return configRuleState_;
}

void ListConfigRulesRequest::setConfigRuleState(const std::string& configRuleState)
{
	configRuleState_ = configRuleState;
	setParameter("ConfigRuleState", configRuleState);
}

int ListConfigRulesRequest::getPageNumber()const
{
	return pageNumber_;
}

void ListConfigRulesRequest::setPageNumber(int pageNumber)
{
	pageNumber_ = pageNumber;
	setParameter("PageNumber", std::to_string(pageNumber));
}

int ListConfigRulesRequest::getPageSize()const
{
	return pageSize_;
}

void ListConfigRulesRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string ListConfigRulesRequest::getComplianceType()const
{
	return complianceType_;
}

void ListConfigRulesRequest::setComplianceType(const std::string& complianceType)
{
	complianceType_ = complianceType;
	setParameter("ComplianceType", complianceType);
}

std::string ListConfigRulesRequest::getConfigRuleSceneId()const
{
	return configRuleSceneId_;
}

void ListConfigRulesRequest::setConfigRuleSceneId(const std::string& configRuleSceneId)
{
	configRuleSceneId_ = configRuleSceneId;
	setParameter("ConfigRuleSceneId", configRuleSceneId);
}

int ListConfigRulesRequest::getRiskLevel()const
{
	return riskLevel_;
}

void ListConfigRulesRequest::setRiskLevel(int riskLevel)
{
	riskLevel_ = riskLevel;
	setParameter("RiskLevel", std::to_string(riskLevel));
}

long ListConfigRulesRequest::getMemberId()const
{
	return memberId_;
}

void ListConfigRulesRequest::setMemberId(long memberId)
{
	memberId_ = memberId;
	setParameter("MemberId", std::to_string(memberId));
}

