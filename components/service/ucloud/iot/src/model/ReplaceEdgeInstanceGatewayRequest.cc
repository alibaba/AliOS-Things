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

#include <alibabacloud/iot/model/ReplaceEdgeInstanceGatewayRequest.h>

using AlibabaCloud::Iot::Model::ReplaceEdgeInstanceGatewayRequest;

ReplaceEdgeInstanceGatewayRequest::ReplaceEdgeInstanceGatewayRequest() :
	RpcServiceRequest("iot", "2018-01-20", "ReplaceEdgeInstanceGateway")
{
	setMethod(HttpRequest::Method::Post);
}

ReplaceEdgeInstanceGatewayRequest::~ReplaceEdgeInstanceGatewayRequest()
{}

std::string ReplaceEdgeInstanceGatewayRequest::getNewGatewayId()const
{
	return newGatewayId_;
}

void ReplaceEdgeInstanceGatewayRequest::setNewGatewayId(const std::string& newGatewayId)
{
	newGatewayId_ = newGatewayId;
	setParameter("NewGatewayId", newGatewayId);
}

std::string ReplaceEdgeInstanceGatewayRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void ReplaceEdgeInstanceGatewayRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string ReplaceEdgeInstanceGatewayRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void ReplaceEdgeInstanceGatewayRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string ReplaceEdgeInstanceGatewayRequest::getInstanceId()const
{
	return instanceId_;
}

void ReplaceEdgeInstanceGatewayRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setParameter("InstanceId", instanceId);
}

std::string ReplaceEdgeInstanceGatewayRequest::getCurrentGatewayId()const
{
	return currentGatewayId_;
}

void ReplaceEdgeInstanceGatewayRequest::setCurrentGatewayId(const std::string& currentGatewayId)
{
	currentGatewayId_ = currentGatewayId;
	setParameter("CurrentGatewayId", currentGatewayId);
}

std::string ReplaceEdgeInstanceGatewayRequest::getApiProduct()const
{
	return apiProduct_;
}

void ReplaceEdgeInstanceGatewayRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string ReplaceEdgeInstanceGatewayRequest::getApiRevision()const
{
	return apiRevision_;
}

void ReplaceEdgeInstanceGatewayRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

