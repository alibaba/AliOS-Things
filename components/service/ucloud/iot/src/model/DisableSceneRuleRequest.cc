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

#include <alibabacloud/iot/model/DisableSceneRuleRequest.h>

using AlibabaCloud::Iot::Model::DisableSceneRuleRequest;

DisableSceneRuleRequest::DisableSceneRuleRequest() :
	RpcServiceRequest("iot", "2018-01-20", "DisableSceneRule")
{
	setMethod(HttpRequest::Method::Post);
}

DisableSceneRuleRequest::~DisableSceneRuleRequest()
{}

std::string DisableSceneRuleRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DisableSceneRuleRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DisableSceneRuleRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void DisableSceneRuleRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string DisableSceneRuleRequest::getApiProduct()const
{
	return apiProduct_;
}

void DisableSceneRuleRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string DisableSceneRuleRequest::getApiRevision()const
{
	return apiRevision_;
}

void DisableSceneRuleRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string DisableSceneRuleRequest::getRuleId()const
{
	return ruleId_;
}

void DisableSceneRuleRequest::setRuleId(const std::string& ruleId)
{
	ruleId_ = ruleId;
	setParameter("RuleId", ruleId);
}

