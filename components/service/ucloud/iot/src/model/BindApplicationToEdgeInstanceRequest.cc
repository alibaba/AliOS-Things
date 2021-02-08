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

#include <alibabacloud/iot/model/BindApplicationToEdgeInstanceRequest.h>

using AlibabaCloud::Iot::Model::BindApplicationToEdgeInstanceRequest;

BindApplicationToEdgeInstanceRequest::BindApplicationToEdgeInstanceRequest() :
	RpcServiceRequest("iot", "2018-01-20", "BindApplicationToEdgeInstance")
{
	setMethod(HttpRequest::Method::Post);
}

BindApplicationToEdgeInstanceRequest::~BindApplicationToEdgeInstanceRequest()
{}

std::string BindApplicationToEdgeInstanceRequest::getApplicationVersion()const
{
	return applicationVersion_;
}

void BindApplicationToEdgeInstanceRequest::setApplicationVersion(const std::string& applicationVersion)
{
	applicationVersion_ = applicationVersion;
	setParameter("ApplicationVersion", applicationVersion);
}

std::string BindApplicationToEdgeInstanceRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void BindApplicationToEdgeInstanceRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string BindApplicationToEdgeInstanceRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void BindApplicationToEdgeInstanceRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string BindApplicationToEdgeInstanceRequest::getApplicationId()const
{
	return applicationId_;
}

void BindApplicationToEdgeInstanceRequest::setApplicationId(const std::string& applicationId)
{
	applicationId_ = applicationId;
	setParameter("ApplicationId", applicationId);
}

std::string BindApplicationToEdgeInstanceRequest::getInstanceId()const
{
	return instanceId_;
}

void BindApplicationToEdgeInstanceRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setParameter("InstanceId", instanceId);
}

std::string BindApplicationToEdgeInstanceRequest::getApiProduct()const
{
	return apiProduct_;
}

void BindApplicationToEdgeInstanceRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string BindApplicationToEdgeInstanceRequest::getApiRevision()const
{
	return apiRevision_;
}

void BindApplicationToEdgeInstanceRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

