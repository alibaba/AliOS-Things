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

#include <alibabacloud/iot/model/ReleaseEdgeDriverVersionRequest.h>

using AlibabaCloud::Iot::Model::ReleaseEdgeDriverVersionRequest;

ReleaseEdgeDriverVersionRequest::ReleaseEdgeDriverVersionRequest() :
	RpcServiceRequest("iot", "2018-01-20", "ReleaseEdgeDriverVersion")
{
	setMethod(HttpRequest::Method::Post);
}

ReleaseEdgeDriverVersionRequest::~ReleaseEdgeDriverVersionRequest()
{}

std::string ReleaseEdgeDriverVersionRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void ReleaseEdgeDriverVersionRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string ReleaseEdgeDriverVersionRequest::getDriverId()const
{
	return driverId_;
}

void ReleaseEdgeDriverVersionRequest::setDriverId(const std::string& driverId)
{
	driverId_ = driverId;
	setParameter("DriverId", driverId);
}

std::string ReleaseEdgeDriverVersionRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void ReleaseEdgeDriverVersionRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string ReleaseEdgeDriverVersionRequest::getDriverVersion()const
{
	return driverVersion_;
}

void ReleaseEdgeDriverVersionRequest::setDriverVersion(const std::string& driverVersion)
{
	driverVersion_ = driverVersion;
	setParameter("DriverVersion", driverVersion);
}

std::string ReleaseEdgeDriverVersionRequest::getApiProduct()const
{
	return apiProduct_;
}

void ReleaseEdgeDriverVersionRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string ReleaseEdgeDriverVersionRequest::getApiRevision()const
{
	return apiRevision_;
}

void ReleaseEdgeDriverVersionRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

