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

#include <alibabacloud/config/model/StartConfigRuleEvaluationRequest.h>

using AlibabaCloud::Config::Model::StartConfigRuleEvaluationRequest;

StartConfigRuleEvaluationRequest::StartConfigRuleEvaluationRequest() :
	RpcServiceRequest("config", "2019-01-08", "StartConfigRuleEvaluation")
{
	setMethod(HttpRequest::Method::Post);
}

StartConfigRuleEvaluationRequest::~StartConfigRuleEvaluationRequest()
{}

std::string StartConfigRuleEvaluationRequest::getConfigRuleId()const
{
	return configRuleId_;
}

void StartConfigRuleEvaluationRequest::setConfigRuleId(const std::string& configRuleId)
{
	configRuleId_ = configRuleId;
	setParameter("ConfigRuleId", configRuleId);
}

bool StartConfigRuleEvaluationRequest::getMultiAccount()const
{
	return multiAccount_;
}

void StartConfigRuleEvaluationRequest::setMultiAccount(bool multiAccount)
{
	multiAccount_ = multiAccount;
	setParameter("MultiAccount", multiAccount ? "true" : "false");
}

long StartConfigRuleEvaluationRequest::getMemberId()const
{
	return memberId_;
}

void StartConfigRuleEvaluationRequest::setMemberId(long memberId)
{
	memberId_ = memberId;
	setParameter("MemberId", std::to_string(memberId));
}

