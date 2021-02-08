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

#include <alibabacloud/iot/model/DeleteRuleActionRequest.h>

using AlibabaCloud::Iot::Model::DeleteRuleActionRequest;

DeleteRuleActionRequest::DeleteRuleActionRequest() :
	RpcServiceRequest("iot", "2018-01-20", "DeleteRuleAction")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteRuleActionRequest::~DeleteRuleActionRequest()
{}

std::string DeleteRuleActionRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DeleteRuleActionRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DeleteRuleActionRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void DeleteRuleActionRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

long DeleteRuleActionRequest::getActionId()const
{
	return actionId_;
}

void DeleteRuleActionRequest::setActionId(long actionId)
{
	actionId_ = actionId;
	setParameter("ActionId", std::to_string(actionId));
}

std::string DeleteRuleActionRequest::getApiProduct()const
{
	return apiProduct_;
}

void DeleteRuleActionRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string DeleteRuleActionRequest::getApiRevision()const
{
	return apiRevision_;
}

void DeleteRuleActionRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

