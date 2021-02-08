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

#include <alibabacloud/iot/model/ResetThingRequest.h>

using AlibabaCloud::Iot::Model::ResetThingRequest;

ResetThingRequest::ResetThingRequest() :
	RpcServiceRequest("iot", "2018-01-20", "ResetThing")
{
	setMethod(HttpRequest::Method::Post);
}

ResetThingRequest::~ResetThingRequest()
{}

std::string ResetThingRequest::getIotId()const
{
	return iotId_;
}

void ResetThingRequest::setIotId(const std::string& iotId)
{
	iotId_ = iotId;
	setParameter("IotId", iotId);
}

std::string ResetThingRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void ResetThingRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string ResetThingRequest::getProductKey()const
{
	return productKey_;
}

void ResetThingRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string ResetThingRequest::getApiProduct()const
{
	return apiProduct_;
}

void ResetThingRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string ResetThingRequest::getApiRevision()const
{
	return apiRevision_;
}

void ResetThingRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string ResetThingRequest::getDeviceName()const
{
	return deviceName_;
}

void ResetThingRequest::setDeviceName(const std::string& deviceName)
{
	deviceName_ = deviceName;
	setParameter("DeviceName", deviceName);
}

