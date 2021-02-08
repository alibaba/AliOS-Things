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

#include <alibabacloud/iot/model/BindDriverToEdgeInstanceRequest.h>

using AlibabaCloud::Iot::Model::BindDriverToEdgeInstanceRequest;

BindDriverToEdgeInstanceRequest::BindDriverToEdgeInstanceRequest() :
	RpcServiceRequest("iot", "2018-01-20", "BindDriverToEdgeInstance")
{
	setMethod(HttpRequest::Method::Post);
}

BindDriverToEdgeInstanceRequest::~BindDriverToEdgeInstanceRequest()
{}

std::string BindDriverToEdgeInstanceRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void BindDriverToEdgeInstanceRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string BindDriverToEdgeInstanceRequest::getDriverId()const
{
	return driverId_;
}

void BindDriverToEdgeInstanceRequest::setDriverId(const std::string& driverId)
{
	driverId_ = driverId;
	setParameter("DriverId", driverId);
}

std::string BindDriverToEdgeInstanceRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void BindDriverToEdgeInstanceRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string BindDriverToEdgeInstanceRequest::getDriverVersion()const
{
	return driverVersion_;
}

void BindDriverToEdgeInstanceRequest::setDriverVersion(const std::string& driverVersion)
{
	driverVersion_ = driverVersion;
	setParameter("DriverVersion", driverVersion);
}

std::string BindDriverToEdgeInstanceRequest::getOrderId()const
{
	return orderId_;
}

void BindDriverToEdgeInstanceRequest::setOrderId(const std::string& orderId)
{
	orderId_ = orderId;
	setParameter("OrderId", orderId);
}

std::string BindDriverToEdgeInstanceRequest::getInstanceId()const
{
	return instanceId_;
}

void BindDriverToEdgeInstanceRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setParameter("InstanceId", instanceId);
}

std::string BindDriverToEdgeInstanceRequest::getApiProduct()const
{
	return apiProduct_;
}

void BindDriverToEdgeInstanceRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string BindDriverToEdgeInstanceRequest::getApiRevision()const
{
	return apiRevision_;
}

void BindDriverToEdgeInstanceRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

