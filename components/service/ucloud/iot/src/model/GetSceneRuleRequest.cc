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

#include <alibabacloud/iot/model/GetSceneRuleRequest.h>

using AlibabaCloud::Iot::Model::GetSceneRuleRequest;

GetSceneRuleRequest::GetSceneRuleRequest() :
	RpcServiceRequest("iot", "2018-01-20", "GetSceneRule")
{
	setMethod(HttpRequest::Method::Post);
}

GetSceneRuleRequest::~GetSceneRuleRequest()
{}

std::string GetSceneRuleRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void GetSceneRuleRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string GetSceneRuleRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void GetSceneRuleRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string GetSceneRuleRequest::getApiProduct()const
{
	return apiProduct_;
}

void GetSceneRuleRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string GetSceneRuleRequest::getApiRevision()const
{
	return apiRevision_;
}

void GetSceneRuleRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string GetSceneRuleRequest::getRuleId()const
{
	return ruleId_;
}

void GetSceneRuleRequest::setRuleId(const std::string& ruleId)
{
	ruleId_ = ruleId;
	setParameter("RuleId", ruleId);
}

