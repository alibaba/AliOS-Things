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

#include <alibabacloud/iot/model/BindGatewayToEdgeInstanceRequest.h>

using AlibabaCloud::Iot::Model::BindGatewayToEdgeInstanceRequest;

BindGatewayToEdgeInstanceRequest::BindGatewayToEdgeInstanceRequest() :
	RpcServiceRequest("iot", "2018-01-20", "BindGatewayToEdgeInstance")
{
	setMethod(HttpRequest::Method::Post);
}

BindGatewayToEdgeInstanceRequest::~BindGatewayToEdgeInstanceRequest()
{}

std::string BindGatewayToEdgeInstanceRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void BindGatewayToEdgeInstanceRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string BindGatewayToEdgeInstanceRequest::getIotId()const
{
	return iotId_;
}

void BindGatewayToEdgeInstanceRequest::setIotId(const std::string& iotId)
{
	iotId_ = iotId;
	setParameter("IotId", iotId);
}

std::string BindGatewayToEdgeInstanceRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void BindGatewayToEdgeInstanceRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string BindGatewayToEdgeInstanceRequest::getProductKey()const
{
	return productKey_;
}

void BindGatewayToEdgeInstanceRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string BindGatewayToEdgeInstanceRequest::getInstanceId()const
{
	return instanceId_;
}

void BindGatewayToEdgeInstanceRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setParameter("InstanceId", instanceId);
}

std::string BindGatewayToEdgeInstanceRequest::getApiProduct()const
{
	return apiProduct_;
}

void BindGatewayToEdgeInstanceRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string BindGatewayToEdgeInstanceRequest::getApiRevision()const
{
	return apiRevision_;
}

void BindGatewayToEdgeInstanceRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string BindGatewayToEdgeInstanceRequest::getDeviceName()const
{
	return deviceName_;
}

void BindGatewayToEdgeInstanceRequest::setDeviceName(const std::string& deviceName)
{
	deviceName_ = deviceName;
	setParameter("DeviceName", deviceName);
}

