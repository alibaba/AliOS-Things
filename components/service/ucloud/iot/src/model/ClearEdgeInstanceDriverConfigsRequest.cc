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

#include <alibabacloud/iot/model/ClearEdgeInstanceDriverConfigsRequest.h>

using AlibabaCloud::Iot::Model::ClearEdgeInstanceDriverConfigsRequest;

ClearEdgeInstanceDriverConfigsRequest::ClearEdgeInstanceDriverConfigsRequest() :
	RpcServiceRequest("iot", "2018-01-20", "ClearEdgeInstanceDriverConfigs")
{
	setMethod(HttpRequest::Method::Post);
}

ClearEdgeInstanceDriverConfigsRequest::~ClearEdgeInstanceDriverConfigsRequest()
{}

std::string ClearEdgeInstanceDriverConfigsRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void ClearEdgeInstanceDriverConfigsRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string ClearEdgeInstanceDriverConfigsRequest::getDriverId()const
{
	return driverId_;
}

void ClearEdgeInstanceDriverConfigsRequest::setDriverId(const std::string& driverId)
{
	driverId_ = driverId;
	setParameter("DriverId", driverId);
}

std::string ClearEdgeInstanceDriverConfigsRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void ClearEdgeInstanceDriverConfigsRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string ClearEdgeInstanceDriverConfigsRequest::getInstanceId()const
{
	return instanceId_;
}

void ClearEdgeInstanceDriverConfigsRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setParameter("InstanceId", instanceId);
}

std::string ClearEdgeInstanceDriverConfigsRequest::getApiProduct()const
{
	return apiProduct_;
}

void ClearEdgeInstanceDriverConfigsRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string ClearEdgeInstanceDriverConfigsRequest::getApiRevision()const
{
	return apiRevision_;
}

void ClearEdgeInstanceDriverConfigsRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

