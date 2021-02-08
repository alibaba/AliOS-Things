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

#include <alibabacloud/iot/model/GetRuleActionRequest.h>

using AlibabaCloud::Iot::Model::GetRuleActionRequest;

GetRuleActionRequest::GetRuleActionRequest() :
	RpcServiceRequest("iot", "2018-01-20", "GetRuleAction")
{
	setMethod(HttpRequest::Method::Post);
}

GetRuleActionRequest::~GetRuleActionRequest()
{}

std::string GetRuleActionRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void GetRuleActionRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string GetRuleActionRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void GetRuleActionRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

long GetRuleActionRequest::getActionId()const
{
	return actionId_;
}

void GetRuleActionRequest::setActionId(long actionId)
{
	actionId_ = actionId;
	setParameter("ActionId", std::to_string(actionId));
}

std::string GetRuleActionRequest::getApiProduct()const
{
	return apiProduct_;
}

void GetRuleActionRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string GetRuleActionRequest::getApiRevision()const
{
	return apiRevision_;
}

void GetRuleActionRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

