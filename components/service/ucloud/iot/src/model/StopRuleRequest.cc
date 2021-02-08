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

#include <alibabacloud/iot/model/StopRuleRequest.h>

using AlibabaCloud::Iot::Model::StopRuleRequest;

StopRuleRequest::StopRuleRequest() :
	RpcServiceRequest("iot", "2018-01-20", "StopRule")
{
	setMethod(HttpRequest::Method::Post);
}

StopRuleRequest::~StopRuleRequest()
{}

std::string StopRuleRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void StopRuleRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string StopRuleRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void StopRuleRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string StopRuleRequest::getApiProduct()const
{
	return apiProduct_;
}

void StopRuleRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string StopRuleRequest::getApiRevision()const
{
	return apiRevision_;
}

void StopRuleRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

long StopRuleRequest::getRuleId()const
{
	return ruleId_;
}

void StopRuleRequest::setRuleId(long ruleId)
{
	ruleId_ = ruleId;
	setParameter("RuleId", std::to_string(ruleId));
}

