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

#include <alibabacloud/iot/model/CreateRuleActionRequest.h>

using AlibabaCloud::Iot::Model::CreateRuleActionRequest;

CreateRuleActionRequest::CreateRuleActionRequest() :
	RpcServiceRequest("iot", "2018-01-20", "CreateRuleAction")
{
	setMethod(HttpRequest::Method::Post);
}

CreateRuleActionRequest::~CreateRuleActionRequest()
{}

std::string CreateRuleActionRequest::getConfiguration()const
{
	return configuration_;
}

void CreateRuleActionRequest::setConfiguration(const std::string& configuration)
{
	configuration_ = configuration;
	setParameter("Configuration", configuration);
}

std::string CreateRuleActionRequest::getType()const
{
	return type_;
}

void CreateRuleActionRequest::setType(const std::string& type)
{
	type_ = type;
	setParameter("Type", type);
}

std::string CreateRuleActionRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CreateRuleActionRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CreateRuleActionRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void CreateRuleActionRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

bool CreateRuleActionRequest::getErrorActionFlag()const
{
	return errorActionFlag_;
}

void CreateRuleActionRequest::setErrorActionFlag(bool errorActionFlag)
{
	errorActionFlag_ = errorActionFlag;
	setParameter("ErrorActionFlag", errorActionFlag ? "true" : "false");
}

std::string CreateRuleActionRequest::getApiProduct()const
{
	return apiProduct_;
}

void CreateRuleActionRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string CreateRuleActionRequest::getApiRevision()const
{
	return apiRevision_;
}

void CreateRuleActionRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

long CreateRuleActionRequest::getRuleId()const
{
	return ruleId_;
}

void CreateRuleActionRequest::setRuleId(long ruleId)
{
	ruleId_ = ruleId;
	setParameter("RuleId", std::to_string(ruleId));
}

