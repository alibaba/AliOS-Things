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

#include <alibabacloud/config/model/PutConfigRuleRequest.h>

using AlibabaCloud::Config::Model::PutConfigRuleRequest;

PutConfigRuleRequest::PutConfigRuleRequest() :
	RpcServiceRequest("config", "2019-01-08", "PutConfigRule")
{
	setMethod(HttpRequest::Method::Post);
}

PutConfigRuleRequest::~PutConfigRuleRequest()
{}

std::string PutConfigRuleRequest::getConfigRuleId()const
{
	return configRuleId_;
}

void PutConfigRuleRequest::setConfigRuleId(const std::string& configRuleId)
{
	configRuleId_ = configRuleId;
	setBodyParameter("ConfigRuleId", configRuleId);
}

bool PutConfigRuleRequest::getMultiAccount()const
{
	return multiAccount_;
}

void PutConfigRuleRequest::setMultiAccount(bool multiAccount)
{
	multiAccount_ = multiAccount;
	setParameter("MultiAccount", multiAccount ? "true" : "false");
}

std::string PutConfigRuleRequest::getClientToken()const
{
	return clientToken_;
}

void PutConfigRuleRequest::setClientToken(const std::string& clientToken)
{
	clientToken_ = clientToken;
	setBodyParameter("ClientToken", clientToken);
}

std::string PutConfigRuleRequest::getDescription()const
{
	return description_;
}

void PutConfigRuleRequest::setDescription(const std::string& description)
{
	description_ = description;
	setBodyParameter("Description", description);
}

std::string PutConfigRuleRequest::getSourceIdentifier()const
{
	return sourceIdentifier_;
}

void PutConfigRuleRequest::setSourceIdentifier(const std::string& sourceIdentifier)
{
	sourceIdentifier_ = sourceIdentifier;
	setBodyParameter("SourceIdentifier", sourceIdentifier);
}

std::string PutConfigRuleRequest::getSourceMaximumExecutionFrequency()const
{
	return sourceMaximumExecutionFrequency_;
}

void PutConfigRuleRequest::setSourceMaximumExecutionFrequency(const std::string& sourceMaximumExecutionFrequency)
{
	sourceMaximumExecutionFrequency_ = sourceMaximumExecutionFrequency;
	setBodyParameter("SourceMaximumExecutionFrequency", sourceMaximumExecutionFrequency);
}

std::string PutConfigRuleRequest::getScopeComplianceResourceTypes()const
{
	return scopeComplianceResourceTypes_;
}

void PutConfigRuleRequest::setScopeComplianceResourceTypes(const std::string& scopeComplianceResourceTypes)
{
	scopeComplianceResourceTypes_ = scopeComplianceResourceTypes;
	setBodyParameter("ScopeComplianceResourceTypes", scopeComplianceResourceTypes);
}

std::string PutConfigRuleRequest::getSourceDetailMessageType()const
{
	return sourceDetailMessageType_;
}

void PutConfigRuleRequest::setSourceDetailMessageType(const std::string& sourceDetailMessageType)
{
	sourceDetailMessageType_ = sourceDetailMessageType;
	setBodyParameter("SourceDetailMessageType", sourceDetailMessageType);
}

int PutConfigRuleRequest::getRiskLevel()const
{
	return riskLevel_;
}

void PutConfigRuleRequest::setRiskLevel(int riskLevel)
{
	riskLevel_ = riskLevel;
	setBodyParameter("RiskLevel", std::to_string(riskLevel));
}

std::string PutConfigRuleRequest::getSourceOwner()const
{
	return sourceOwner_;
}

void PutConfigRuleRequest::setSourceOwner(const std::string& sourceOwner)
{
	sourceOwner_ = sourceOwner;
	setBodyParameter("SourceOwner", sourceOwner);
}

std::string PutConfigRuleRequest::getInputParameters()const
{
	return inputParameters_;
}

void PutConfigRuleRequest::setInputParameters(const std::string& inputParameters)
{
	inputParameters_ = inputParameters;
	setBodyParameter("InputParameters", inputParameters);
}

std::string PutConfigRuleRequest::getConfigRuleName()const
{
	return configRuleName_;
}

void PutConfigRuleRequest::setConfigRuleName(const std::string& configRuleName)
{
	configRuleName_ = configRuleName;
	setBodyParameter("ConfigRuleName", configRuleName);
}

std::string PutConfigRuleRequest::getScopeComplianceResourceId()const
{
	return scopeComplianceResourceId_;
}

void PutConfigRuleRequest::setScopeComplianceResourceId(const std::string& scopeComplianceResourceId)
{
	scopeComplianceResourceId_ = scopeComplianceResourceId;
	setBodyParameter("ScopeComplianceResourceId", scopeComplianceResourceId);
}

long PutConfigRuleRequest::getMemberId()const
{
	return memberId_;
}

void PutConfigRuleRequest::setMemberId(long memberId)
{
	memberId_ = memberId;
	setParameter("MemberId", std::to_string(memberId));
}

