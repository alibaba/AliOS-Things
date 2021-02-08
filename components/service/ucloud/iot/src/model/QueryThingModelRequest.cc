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

#include <alibabacloud/iot/model/QueryThingModelRequest.h>

using AlibabaCloud::Iot::Model::QueryThingModelRequest;

QueryThingModelRequest::QueryThingModelRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QueryThingModel")
{
	setMethod(HttpRequest::Method::Post);
}

QueryThingModelRequest::~QueryThingModelRequest()
{}

std::string QueryThingModelRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void QueryThingModelRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string QueryThingModelRequest::getResourceGroupId()const
{
	return resourceGroupId_;
}

void QueryThingModelRequest::setResourceGroupId(const std::string& resourceGroupId)
{
	resourceGroupId_ = resourceGroupId;
	setParameter("ResourceGroupId", resourceGroupId);
}

std::string QueryThingModelRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QueryThingModelRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string QueryThingModelRequest::getProductKey()const
{
	return productKey_;
}

void QueryThingModelRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string QueryThingModelRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryThingModelRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryThingModelRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryThingModelRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string QueryThingModelRequest::getModelVersion()const
{
	return modelVersion_;
}

void QueryThingModelRequest::setModelVersion(const std::string& modelVersion)
{
	modelVersion_ = modelVersion;
	setParameter("ModelVersion", modelVersion);
}

