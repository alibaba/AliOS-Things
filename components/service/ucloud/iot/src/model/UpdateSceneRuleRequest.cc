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

#include <alibabacloud/iot/model/UpdateSceneRuleRequest.h>

using AlibabaCloud::Iot::Model::UpdateSceneRuleRequest;

UpdateSceneRuleRequest::UpdateSceneRuleRequest() :
	RpcServiceRequest("iot", "2018-01-20", "UpdateSceneRule")
{
	setMethod(HttpRequest::Method::Post);
}

UpdateSceneRuleRequest::~UpdateSceneRuleRequest()
{}

std::string UpdateSceneRuleRequest::getRuleName()const
{
	return ruleName_;
}

void UpdateSceneRuleRequest::setRuleName(const std::string& ruleName)
{
	ruleName_ = ruleName;
	setParameter("RuleName", ruleName);
}

std::string UpdateSceneRuleRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void UpdateSceneRuleRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string UpdateSceneRuleRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void UpdateSceneRuleRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string UpdateSceneRuleRequest::getRuleDescription()const
{
	return ruleDescription_;
}

void UpdateSceneRuleRequest::setRuleDescription(const std::string& ruleDescription)
{
	ruleDescription_ = ruleDescription;
	setParameter("RuleDescription", ruleDescription);
}

std::string UpdateSceneRuleRequest::getRuleContent()const
{
	return ruleContent_;
}

void UpdateSceneRuleRequest::setRuleContent(const std::string& ruleContent)
{
	ruleContent_ = ruleContent;
	setParameter("RuleContent", ruleContent);
}

std::string UpdateSceneRuleRequest::getApiProduct()const
{
	return apiProduct_;
}

void UpdateSceneRuleRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string UpdateSceneRuleRequest::getApiRevision()const
{
	return apiRevision_;
}

void UpdateSceneRuleRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string UpdateSceneRuleRequest::getRuleId()const
{
	return ruleId_;
}

void UpdateSceneRuleRequest::setRuleId(const std::string& ruleId)
{
	ruleId_ = ruleId;
	setParameter("RuleId", ruleId);
}

