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

#include <alibabacloud/iot/model/DeleteEdgeDriverVersionRequest.h>

using AlibabaCloud::Iot::Model::DeleteEdgeDriverVersionRequest;

DeleteEdgeDriverVersionRequest::DeleteEdgeDriverVersionRequest() :
	RpcServiceRequest("iot", "2018-01-20", "DeleteEdgeDriverVersion")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteEdgeDriverVersionRequest::~DeleteEdgeDriverVersionRequest()
{}

std::string DeleteEdgeDriverVersionRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DeleteEdgeDriverVersionRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DeleteEdgeDriverVersionRequest::getDriverId()const
{
	return driverId_;
}

void DeleteEdgeDriverVersionRequest::setDriverId(const std::string& driverId)
{
	driverId_ = driverId;
	setParameter("DriverId", driverId);
}

std::string DeleteEdgeDriverVersionRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void DeleteEdgeDriverVersionRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string DeleteEdgeDriverVersionRequest::getDriverVersion()const
{
	return driverVersion_;
}

void DeleteEdgeDriverVersionRequest::setDriverVersion(const std::string& driverVersion)
{
	driverVersion_ = driverVersion;
	setParameter("DriverVersion", driverVersion);
}

std::string DeleteEdgeDriverVersionRequest::getApiProduct()const
{
	return apiProduct_;
}

void DeleteEdgeDriverVersionRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string DeleteEdgeDriverVersionRequest::getApiRevision()const
{
	return apiRevision_;
}

void DeleteEdgeDriverVersionRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

