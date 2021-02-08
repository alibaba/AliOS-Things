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

#include <alibabacloud/iot/model/UpdateThingModelRequest.h>

using AlibabaCloud::Iot::Model::UpdateThingModelRequest;

UpdateThingModelRequest::UpdateThingModelRequest() :
	RpcServiceRequest("iot", "2018-01-20", "UpdateThingModel")
{
	setMethod(HttpRequest::Method::Post);
}

UpdateThingModelRequest::~UpdateThingModelRequest()
{}

std::string UpdateThingModelRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void UpdateThingModelRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string UpdateThingModelRequest::getIdentifier()const
{
	return identifier_;
}

void UpdateThingModelRequest::setIdentifier(const std::string& identifier)
{
	identifier_ = identifier;
	setParameter("Identifier", identifier);
}

std::string UpdateThingModelRequest::getProductKey()const
{
	return productKey_;
}

void UpdateThingModelRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string UpdateThingModelRequest::getApiProduct()const
{
	return apiProduct_;
}

void UpdateThingModelRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string UpdateThingModelRequest::getThingModelJson()const
{
	return thingModelJson_;
}

void UpdateThingModelRequest::setThingModelJson(const std::string& thingModelJson)
{
	thingModelJson_ = thingModelJson;
	setParameter("ThingModelJson", thingModelJson);
}

std::string UpdateThingModelRequest::getApiRevision()const
{
	return apiRevision_;
}

void UpdateThingModelRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

