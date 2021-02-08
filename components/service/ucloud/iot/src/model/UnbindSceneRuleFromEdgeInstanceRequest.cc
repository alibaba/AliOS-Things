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

#include <alibabacloud/iot/model/UnbindSceneRuleFromEdgeInstanceRequest.h>

using AlibabaCloud::Iot::Model::UnbindSceneRuleFromEdgeInstanceRequest;

UnbindSceneRuleFromEdgeInstanceRequest::UnbindSceneRuleFromEdgeInstanceRequest() :
	RpcServiceRequest("iot", "2018-01-20", "UnbindSceneRuleFromEdgeInstance")
{
	setMethod(HttpRequest::Method::Post);
}

UnbindSceneRuleFromEdgeInstanceRequest::~UnbindSceneRuleFromEdgeInstanceRequest()
{}

std::string UnbindSceneRuleFromEdgeInstanceRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void UnbindSceneRuleFromEdgeInstanceRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string UnbindSceneRuleFromEdgeInstanceRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void UnbindSceneRuleFromEdgeInstanceRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string UnbindSceneRuleFromEdgeInstanceRequest::getInstanceId()const
{
	return instanceId_;
}

void UnbindSceneRuleFromEdgeInstanceRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setParameter("InstanceId", instanceId);
}

std::string UnbindSceneRuleFromEdgeInstanceRequest::getApiProduct()const
{
	return apiProduct_;
}

void UnbindSceneRuleFromEdgeInstanceRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string UnbindSceneRuleFromEdgeInstanceRequest::getApiRevision()const
{
	return apiRevision_;
}

void UnbindSceneRuleFromEdgeInstanceRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string UnbindSceneRuleFromEdgeInstanceRequest::getRuleId()const
{
	return ruleId_;
}

void UnbindSceneRuleFromEdgeInstanceRequest::setRuleId(const std::string& ruleId)
{
	ruleId_ = ruleId;
	setParameter("RuleId", ruleId);
}

