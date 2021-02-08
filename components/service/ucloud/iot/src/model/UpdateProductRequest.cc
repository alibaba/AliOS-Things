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

#include <alibabacloud/iot/model/UpdateProductRequest.h>

using AlibabaCloud::Iot::Model::UpdateProductRequest;

UpdateProductRequest::UpdateProductRequest() :
	RpcServiceRequest("iot", "2018-01-20", "UpdateProduct")
{
	setMethod(HttpRequest::Method::Post);
}

UpdateProductRequest::~UpdateProductRequest()
{}

std::string UpdateProductRequest::getDescription()const
{
	return description_;
}

void UpdateProductRequest::setDescription(const std::string& description)
{
	description_ = description;
	setParameter("Description", description);
}

std::string UpdateProductRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void UpdateProductRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string UpdateProductRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void UpdateProductRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string UpdateProductRequest::getProductName()const
{
	return productName_;
}

void UpdateProductRequest::setProductName(const std::string& productName)
{
	productName_ = productName;
	setParameter("ProductName", productName);
}

std::string UpdateProductRequest::getProductKey()const
{
	return productKey_;
}

void UpdateProductRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string UpdateProductRequest::getApiProduct()const
{
	return apiProduct_;
}

void UpdateProductRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string UpdateProductRequest::getApiRevision()const
{
	return apiRevision_;
}

void UpdateProductRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

