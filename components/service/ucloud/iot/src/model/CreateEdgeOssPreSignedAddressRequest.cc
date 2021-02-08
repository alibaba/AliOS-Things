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

#include <alibabacloud/iot/model/CreateEdgeOssPreSignedAddressRequest.h>

using AlibabaCloud::Iot::Model::CreateEdgeOssPreSignedAddressRequest;

CreateEdgeOssPreSignedAddressRequest::CreateEdgeOssPreSignedAddressRequest() :
	RpcServiceRequest("iot", "2018-01-20", "CreateEdgeOssPreSignedAddress")
{
	setMethod(HttpRequest::Method::Post);
}

CreateEdgeOssPreSignedAddressRequest::~CreateEdgeOssPreSignedAddressRequest()
{}

std::string CreateEdgeOssPreSignedAddressRequest::getType()const
{
	return type_;
}

void CreateEdgeOssPreSignedAddressRequest::setType(const std::string& type)
{
	type_ = type;
	setParameter("Type", type);
}

std::string CreateEdgeOssPreSignedAddressRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void CreateEdgeOssPreSignedAddressRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string CreateEdgeOssPreSignedAddressRequest::getResourceVersion()const
{
	return resourceVersion_;
}

void CreateEdgeOssPreSignedAddressRequest::setResourceVersion(const std::string& resourceVersion)
{
	resourceVersion_ = resourceVersion;
	setParameter("ResourceVersion", resourceVersion);
}

std::string CreateEdgeOssPreSignedAddressRequest::getResourceId()const
{
	return resourceId_;
}

void CreateEdgeOssPreSignedAddressRequest::setResourceId(const std::string& resourceId)
{
	resourceId_ = resourceId;
	setParameter("ResourceId", resourceId);
}

std::string CreateEdgeOssPreSignedAddressRequest::getFileName()const
{
	return fileName_;
}

void CreateEdgeOssPreSignedAddressRequest::setFileName(const std::string& fileName)
{
	fileName_ = fileName;
	setParameter("FileName", fileName);
}

std::string CreateEdgeOssPreSignedAddressRequest::getInstanceId()const
{
	return instanceId_;
}

void CreateEdgeOssPreSignedAddressRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setParameter("InstanceId", instanceId);
}

std::string CreateEdgeOssPreSignedAddressRequest::getApiProduct()const
{
	return apiProduct_;
}

void CreateEdgeOssPreSignedAddressRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string CreateEdgeOssPreSignedAddressRequest::getApiRevision()const
{
	return apiRevision_;
}

void CreateEdgeOssPreSignedAddressRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

