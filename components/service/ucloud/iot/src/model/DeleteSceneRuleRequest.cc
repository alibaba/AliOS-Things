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

#include <alibabacloud/iot/model/DeleteSceneRuleRequest.h>

using AlibabaCloud::Iot::Model::DeleteSceneRuleRequest;

DeleteSceneRuleRequest::DeleteSceneRuleRequest() :
	RpcServiceRequest("iot", "2018-01-20", "DeleteSceneRule")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteSceneRuleRequest::~DeleteSceneRuleRequest()
{}

std::string DeleteSceneRuleRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DeleteSceneRuleRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DeleteSceneRuleRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void DeleteSceneRuleRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string DeleteSceneRuleRequest::getApiProduct()const
{
	return apiProduct_;
}

void DeleteSceneRuleRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string DeleteSceneRuleRequest::getApiRevision()const
{
	return apiRevision_;
}

void DeleteSceneRuleRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string DeleteSceneRuleRequest::getRuleId()const
{
	return ruleId_;
}

void DeleteSceneRuleRequest::setRuleId(const std::string& ruleId)
{
	ruleId_ = ruleId;
	setParameter("RuleId", ruleId);
}

