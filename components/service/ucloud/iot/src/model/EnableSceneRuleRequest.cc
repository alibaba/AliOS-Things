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

#include <alibabacloud/iot/model/EnableSceneRuleRequest.h>

using AlibabaCloud::Iot::Model::EnableSceneRuleRequest;

EnableSceneRuleRequest::EnableSceneRuleRequest() :
	RpcServiceRequest("iot", "2018-01-20", "EnableSceneRule")
{
	setMethod(HttpRequest::Method::Post);
}

EnableSceneRuleRequest::~EnableSceneRuleRequest()
{}

std::string EnableSceneRuleRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void EnableSceneRuleRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string EnableSceneRuleRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void EnableSceneRuleRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string EnableSceneRuleRequest::getApiProduct()const
{
	return apiProduct_;
}

void EnableSceneRuleRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string EnableSceneRuleRequest::getApiRevision()const
{
	return apiRevision_;
}

void EnableSceneRuleRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string EnableSceneRuleRequest::getRuleId()const
{
	return ruleId_;
}

void EnableSceneRuleRequest::setRuleId(const std::string& ruleId)
{
	ruleId_ = ruleId;
	setParameter("RuleId", ruleId);
}

