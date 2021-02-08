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

#include <alibabacloud/iot/model/PublishThingModelRequest.h>

using AlibabaCloud::Iot::Model::PublishThingModelRequest;

PublishThingModelRequest::PublishThingModelRequest() :
	RpcServiceRequest("iot", "2018-01-20", "PublishThingModel")
{
	setMethod(HttpRequest::Method::Post);
}

PublishThingModelRequest::~PublishThingModelRequest()
{}

std::string PublishThingModelRequest::getDescription()const
{
	return description_;
}

void PublishThingModelRequest::setDescription(const std::string& description)
{
	description_ = description;
	setParameter("Description", description);
}

std::string PublishThingModelRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void PublishThingModelRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string PublishThingModelRequest::getResourceGroupId()const
{
	return resourceGroupId_;
}

void PublishThingModelRequest::setResourceGroupId(const std::string& resourceGroupId)
{
	resourceGroupId_ = resourceGroupId;
	setParameter("ResourceGroupId", resourceGroupId);
}

std::string PublishThingModelRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void PublishThingModelRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string PublishThingModelRequest::getProductKey()const
{
	return productKey_;
}

void PublishThingModelRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string PublishThingModelRequest::getApiProduct()const
{
	return apiProduct_;
}

void PublishThingModelRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string PublishThingModelRequest::getApiRevision()const
{
	return apiRevision_;
}

void PublishThingModelRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string PublishThingModelRequest::getModelVersion()const
{
	return modelVersion_;
}

void PublishThingModelRequest::setModelVersion(const std::string& modelVersion)
{
	modelVersion_ = modelVersion;
	setParameter("ModelVersion", modelVersion);
}

