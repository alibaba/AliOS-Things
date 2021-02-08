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

#include <alibabacloud/iot/model/UnbindDriverFromEdgeInstanceRequest.h>

using AlibabaCloud::Iot::Model::UnbindDriverFromEdgeInstanceRequest;

UnbindDriverFromEdgeInstanceRequest::UnbindDriverFromEdgeInstanceRequest() :
	RpcServiceRequest("iot", "2018-01-20", "UnbindDriverFromEdgeInstance")
{
	setMethod(HttpRequest::Method::Post);
}

UnbindDriverFromEdgeInstanceRequest::~UnbindDriverFromEdgeInstanceRequest()
{}

std::string UnbindDriverFromEdgeInstanceRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void UnbindDriverFromEdgeInstanceRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string UnbindDriverFromEdgeInstanceRequest::getDriverId()const
{
	return driverId_;
}

void UnbindDriverFromEdgeInstanceRequest::setDriverId(const std::string& driverId)
{
	driverId_ = driverId;
	setParameter("DriverId", driverId);
}

std::string UnbindDriverFromEdgeInstanceRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void UnbindDriverFromEdgeInstanceRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string UnbindDriverFromEdgeInstanceRequest::getInstanceId()const
{
	return instanceId_;
}

void UnbindDriverFromEdgeInstanceRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setParameter("InstanceId", instanceId);
}

std::string UnbindDriverFromEdgeInstanceRequest::getApiProduct()const
{
	return apiProduct_;
}

void UnbindDriverFromEdgeInstanceRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string UnbindDriverFromEdgeInstanceRequest::getApiRevision()const
{
	return apiRevision_;
}

void UnbindDriverFromEdgeInstanceRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

