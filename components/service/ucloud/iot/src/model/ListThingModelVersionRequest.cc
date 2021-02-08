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

#include <alibabacloud/iot/model/ListThingModelVersionRequest.h>

using AlibabaCloud::Iot::Model::ListThingModelVersionRequest;

ListThingModelVersionRequest::ListThingModelVersionRequest() :
	RpcServiceRequest("iot", "2018-01-20", "ListThingModelVersion")
{
	setMethod(HttpRequest::Method::Post);
}

ListThingModelVersionRequest::~ListThingModelVersionRequest()
{}

std::string ListThingModelVersionRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void ListThingModelVersionRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string ListThingModelVersionRequest::getResourceGroupId()const
{
	return resourceGroupId_;
}

void ListThingModelVersionRequest::setResourceGroupId(const std::string& resourceGroupId)
{
	resourceGroupId_ = resourceGroupId;
	setParameter("ResourceGroupId", resourceGroupId);
}

std::string ListThingModelVersionRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void ListThingModelVersionRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string ListThingModelVersionRequest::getProductKey()const
{
	return productKey_;
}

void ListThingModelVersionRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string ListThingModelVersionRequest::getApiProduct()const
{
	return apiProduct_;
}

void ListThingModelVersionRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string ListThingModelVersionRequest::getApiRevision()const
{
	return apiRevision_;
}

void ListThingModelVersionRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

