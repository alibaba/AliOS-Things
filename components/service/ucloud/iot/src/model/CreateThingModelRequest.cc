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

#include <alibabacloud/iot/model/CreateThingModelRequest.h>

using AlibabaCloud::Iot::Model::CreateThingModelRequest;

CreateThingModelRequest::CreateThingModelRequest() :
	RpcServiceRequest("iot", "2018-01-20", "CreateThingModel")
{
	setMethod(HttpRequest::Method::Post);
}

CreateThingModelRequest::~CreateThingModelRequest()
{}

std::string CreateThingModelRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void CreateThingModelRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string CreateThingModelRequest::getProductKey()const
{
	return productKey_;
}

void CreateThingModelRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string CreateThingModelRequest::getApiProduct()const
{
	return apiProduct_;
}

void CreateThingModelRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string CreateThingModelRequest::getThingModelJson()const
{
	return thingModelJson_;
}

void CreateThingModelRequest::setThingModelJson(const std::string& thingModelJson)
{
	thingModelJson_ = thingModelJson;
	setParameter("ThingModelJson", thingModelJson);
}

std::string CreateThingModelRequest::getApiRevision()const
{
	return apiRevision_;
}

void CreateThingModelRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

