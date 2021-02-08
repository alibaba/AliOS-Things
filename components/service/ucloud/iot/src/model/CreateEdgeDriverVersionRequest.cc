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

#include <alibabacloud/iot/model/CreateEdgeDriverVersionRequest.h>

using AlibabaCloud::Iot::Model::CreateEdgeDriverVersionRequest;

CreateEdgeDriverVersionRequest::CreateEdgeDriverVersionRequest() :
	RpcServiceRequest("iot", "2018-01-20", "CreateEdgeDriverVersion")
{
	setMethod(HttpRequest::Method::Post);
}

CreateEdgeDriverVersionRequest::~CreateEdgeDriverVersionRequest()
{}

std::string CreateEdgeDriverVersionRequest::getConfigCheckRule()const
{
	return configCheckRule_;
}

void CreateEdgeDriverVersionRequest::setConfigCheckRule(const std::string& configCheckRule)
{
	configCheckRule_ = configCheckRule;
	setParameter("ConfigCheckRule", configCheckRule);
}

std::string CreateEdgeDriverVersionRequest::getArgument()const
{
	return argument_;
}

void CreateEdgeDriverVersionRequest::setArgument(const std::string& argument)
{
	argument_ = argument;
	setParameter("Argument", argument);
}

std::string CreateEdgeDriverVersionRequest::getEdgeVersion()const
{
	return edgeVersion_;
}

void CreateEdgeDriverVersionRequest::setEdgeVersion(const std::string& edgeVersion)
{
	edgeVersion_ = edgeVersion;
	setParameter("EdgeVersion", edgeVersion);
}

std::string CreateEdgeDriverVersionRequest::getDescription()const
{
	return description_;
}

void CreateEdgeDriverVersionRequest::setDescription(const std::string& description)
{
	description_ = description;
	setParameter("Description", description);
}

std::string CreateEdgeDriverVersionRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CreateEdgeDriverVersionRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CreateEdgeDriverVersionRequest::getDriverId()const
{
	return driverId_;
}

void CreateEdgeDriverVersionRequest::setDriverId(const std::string& driverId)
{
	driverId_ = driverId;
	setParameter("DriverId", driverId);
}

std::string CreateEdgeDriverVersionRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void CreateEdgeDriverVersionRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string CreateEdgeDriverVersionRequest::getContainerConfig()const
{
	return containerConfig_;
}

void CreateEdgeDriverVersionRequest::setContainerConfig(const std::string& containerConfig)
{
	containerConfig_ = containerConfig;
	setParameter("ContainerConfig", containerConfig);
}

std::string CreateEdgeDriverVersionRequest::getDriverVersion()const
{
	return driverVersion_;
}

void CreateEdgeDriverVersionRequest::setDriverVersion(const std::string& driverVersion)
{
	driverVersion_ = driverVersion;
	setParameter("DriverVersion", driverVersion);
}

std::string CreateEdgeDriverVersionRequest::getDriverConfig()const
{
	return driverConfig_;
}

void CreateEdgeDriverVersionRequest::setDriverConfig(const std::string& driverConfig)
{
	driverConfig_ = driverConfig;
	setParameter("DriverConfig", driverConfig);
}

std::string CreateEdgeDriverVersionRequest::getSourceConfig()const
{
	return sourceConfig_;
}

void CreateEdgeDriverVersionRequest::setSourceConfig(const std::string& sourceConfig)
{
	sourceConfig_ = sourceConfig;
	setParameter("SourceConfig", sourceConfig);
}

std::string CreateEdgeDriverVersionRequest::getApiProduct()const
{
	return apiProduct_;
}

void CreateEdgeDriverVersionRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string CreateEdgeDriverVersionRequest::getApiRevision()const
{
	return apiRevision_;
}

void CreateEdgeDriverVersionRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

