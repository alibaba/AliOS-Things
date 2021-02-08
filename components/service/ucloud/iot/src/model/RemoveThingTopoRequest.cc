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

#include <alibabacloud/iot/model/RemoveThingTopoRequest.h>

using AlibabaCloud::Iot::Model::RemoveThingTopoRequest;

RemoveThingTopoRequest::RemoveThingTopoRequest() :
	RpcServiceRequest("iot", "2018-01-20", "RemoveThingTopo")
{
	setMethod(HttpRequest::Method::Post);
}

RemoveThingTopoRequest::~RemoveThingTopoRequest()
{}

std::string RemoveThingTopoRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void RemoveThingTopoRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string RemoveThingTopoRequest::getIotId()const
{
	return iotId_;
}

void RemoveThingTopoRequest::setIotId(const std::string& iotId)
{
	iotId_ = iotId;
	setParameter("IotId", iotId);
}

std::string RemoveThingTopoRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void RemoveThingTopoRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string RemoveThingTopoRequest::getProductKey()const
{
	return productKey_;
}

void RemoveThingTopoRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string RemoveThingTopoRequest::getApiProduct()const
{
	return apiProduct_;
}

void RemoveThingTopoRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string RemoveThingTopoRequest::getApiRevision()const
{
	return apiRevision_;
}

void RemoveThingTopoRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string RemoveThingTopoRequest::getDeviceName()const
{
	return deviceName_;
}

void RemoveThingTopoRequest::setDeviceName(const std::string& deviceName)
{
	deviceName_ = deviceName;
	setParameter("DeviceName", deviceName);
}

