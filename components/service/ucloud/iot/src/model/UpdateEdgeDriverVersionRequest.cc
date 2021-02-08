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

#include <alibabacloud/iot/model/UpdateEdgeDriverVersionRequest.h>

using AlibabaCloud::Iot::Model::UpdateEdgeDriverVersionRequest;

UpdateEdgeDriverVersionRequest::UpdateEdgeDriverVersionRequest() :
	RpcServiceRequest("iot", "2018-01-20", "UpdateEdgeDriverVersion")
{
	setMethod(HttpRequest::Method::Post);
}

UpdateEdgeDriverVersionRequest::~UpdateEdgeDriverVersionRequest()
{}

std::string UpdateEdgeDriverVersionRequest::getConfigCheckRule()const
{
	return configCheckRule_;
}

void UpdateEdgeDriverVersionRequest::setConfigCheckRule(const std::string& configCheckRule)
{
	configCheckRule_ = configCheckRule;
	setParameter("ConfigCheckRule", configCheckRule);
}

std::string UpdateEdgeDriverVersionRequest::getArgument()const
{
	return argument_;
}

void UpdateEdgeDriverVersionRequest::setArgument(const std::string& argument)
{
	argument_ = argument;
	setParameter("Argument", argument);
}

std::string UpdateEdgeDriverVersionRequest::getEdgeVersion()const
{
	return edgeVersion_;
}

void UpdateEdgeDriverVersionRequest::setEdgeVersion(const std::string& edgeVersion)
{
	edgeVersion_ = edgeVersion;
	setParameter("EdgeVersion", edgeVersion);
}

std::string UpdateEdgeDriverVersionRequest::getDescription()const
{
	return description_;
}

void UpdateEdgeDriverVersionRequest::setDescription(const std::string& description)
{
	description_ = description;
	setParameter("Description", description);
}

std::string UpdateEdgeDriverVersionRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void UpdateEdgeDriverVersionRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string UpdateEdgeDriverVersionRequest::getDriverId()const
{
	return driverId_;
}

void UpdateEdgeDriverVersionRequest::setDriverId(const std::string& driverId)
{
	driverId_ = driverId;
	setParameter("DriverId", driverId);
}

std::string UpdateEdgeDriverVersionRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void UpdateEdgeDriverVersionRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string UpdateEdgeDriverVersionRequest::getContainerConfig()const
{
	return containerConfig_;
}

void UpdateEdgeDriverVersionRequest::setContainerConfig(const std::string& containerConfig)
{
	containerConfig_ = containerConfig;
	setParameter("ContainerConfig", containerConfig);
}

std::string UpdateEdgeDriverVersionRequest::getDriverVersion()const
{
	return driverVersion_;
}

void UpdateEdgeDriverVersionRequest::setDriverVersion(const std::string& driverVersion)
{
	driverVersion_ = driverVersion;
	setParameter("DriverVersion", driverVersion);
}

std::string UpdateEdgeDriverVersionRequest::getDriverConfig()const
{
	return driverConfig_;
}

void UpdateEdgeDriverVersionRequest::setDriverConfig(const std::string& driverConfig)
{
	driverConfig_ = driverConfig;
	setParameter("DriverConfig", driverConfig);
}

std::string UpdateEdgeDriverVersionRequest::getSourceConfig()const
{
	return sourceConfig_;
}

void UpdateEdgeDriverVersionRequest::setSourceConfig(const std::string& sourceConfig)
{
	sourceConfig_ = sourceConfig;
	setParameter("SourceConfig", sourceConfig);
}

std::string UpdateEdgeDriverVersionRequest::getApiProduct()const
{
	return apiProduct_;
}

void UpdateEdgeDriverVersionRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string UpdateEdgeDriverVersionRequest::getApiRevision()const
{
	return apiRevision_;
}

void UpdateEdgeDriverVersionRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

