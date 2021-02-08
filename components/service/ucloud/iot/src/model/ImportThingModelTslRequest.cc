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

#include <alibabacloud/iot/model/ImportThingModelTslRequest.h>

using AlibabaCloud::Iot::Model::ImportThingModelTslRequest;

ImportThingModelTslRequest::ImportThingModelTslRequest() :
	RpcServiceRequest("iot", "2018-01-20", "ImportThingModelTsl")
{
	setMethod(HttpRequest::Method::Post);
}

ImportThingModelTslRequest::~ImportThingModelTslRequest()
{}

std::string ImportThingModelTslRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void ImportThingModelTslRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string ImportThingModelTslRequest::getResourceGroupId()const
{
	return resourceGroupId_;
}

void ImportThingModelTslRequest::setResourceGroupId(const std::string& resourceGroupId)
{
	resourceGroupId_ = resourceGroupId;
	setParameter("ResourceGroupId", resourceGroupId);
}

std::string ImportThingModelTslRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void ImportThingModelTslRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string ImportThingModelTslRequest::getProductKey()const
{
	return productKey_;
}

void ImportThingModelTslRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string ImportThingModelTslRequest::getApiProduct()const
{
	return apiProduct_;
}

void ImportThingModelTslRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string ImportThingModelTslRequest::getApiRevision()const
{
	return apiRevision_;
}

void ImportThingModelTslRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string ImportThingModelTslRequest::getTslUrl()const
{
	return tslUrl_;
}

void ImportThingModelTslRequest::setTslUrl(const std::string& tslUrl)
{
	tslUrl_ = tslUrl;
	setParameter("TslUrl", tslUrl);
}

std::string ImportThingModelTslRequest::getTslStr()const
{
	return tslStr_;
}

void ImportThingModelTslRequest::setTslStr(const std::string& tslStr)
{
	tslStr_ = tslStr;
	setParameter("TslStr", tslStr);
}

