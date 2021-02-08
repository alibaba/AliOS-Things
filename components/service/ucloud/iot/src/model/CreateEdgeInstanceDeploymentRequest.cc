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

#include <alibabacloud/iot/model/CreateEdgeInstanceDeploymentRequest.h>

using AlibabaCloud::Iot::Model::CreateEdgeInstanceDeploymentRequest;

CreateEdgeInstanceDeploymentRequest::CreateEdgeInstanceDeploymentRequest() :
	RpcServiceRequest("iot", "2018-01-20", "CreateEdgeInstanceDeployment")
{
	setMethod(HttpRequest::Method::Post);
}

CreateEdgeInstanceDeploymentRequest::~CreateEdgeInstanceDeploymentRequest()
{}

std::string CreateEdgeInstanceDeploymentRequest::getType()const
{
	return type_;
}

void CreateEdgeInstanceDeploymentRequest::setType(const std::string& type)
{
	type_ = type;
	setParameter("Type", type);
}

std::string CreateEdgeInstanceDeploymentRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CreateEdgeInstanceDeploymentRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CreateEdgeInstanceDeploymentRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void CreateEdgeInstanceDeploymentRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string CreateEdgeInstanceDeploymentRequest::getInstanceId()const
{
	return instanceId_;
}

void CreateEdgeInstanceDeploymentRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setParameter("InstanceId", instanceId);
}

std::string CreateEdgeInstanceDeploymentRequest::getApiProduct()const
{
	return apiProduct_;
}

void CreateEdgeInstanceDeploymentRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string CreateEdgeInstanceDeploymentRequest::getApiRevision()const
{
	return apiRevision_;
}

void CreateEdgeInstanceDeploymentRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

