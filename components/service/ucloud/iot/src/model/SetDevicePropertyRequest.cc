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

#include <alibabacloud/iot/model/SetDevicePropertyRequest.h>

using AlibabaCloud::Iot::Model::SetDevicePropertyRequest;

SetDevicePropertyRequest::SetDevicePropertyRequest() :
	RpcServiceRequest("iot", "2018-01-20", "SetDeviceProperty")
{
	setMethod(HttpRequest::Method::Post);
}

SetDevicePropertyRequest::~SetDevicePropertyRequest()
{}

std::string SetDevicePropertyRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void SetDevicePropertyRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string SetDevicePropertyRequest::getIotId()const
{
	return iotId_;
}

void SetDevicePropertyRequest::setIotId(const std::string& iotId)
{
	iotId_ = iotId;
	setParameter("IotId", iotId);
}

std::string SetDevicePropertyRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void SetDevicePropertyRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string SetDevicePropertyRequest::getProductKey()const
{
	return productKey_;
}

void SetDevicePropertyRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string SetDevicePropertyRequest::getApiProduct()const
{
	return apiProduct_;
}

void SetDevicePropertyRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string SetDevicePropertyRequest::getApiRevision()const
{
	return apiRevision_;
}

void SetDevicePropertyRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string SetDevicePropertyRequest::getDeviceName()const
{
	return deviceName_;
}

void SetDevicePropertyRequest::setDeviceName(const std::string& deviceName)
{
	deviceName_ = deviceName;
	setParameter("DeviceName", deviceName);
}

std::string SetDevicePropertyRequest::getItems()const
{
	return items_;
}

void SetDevicePropertyRequest::setItems(const std::string& items)
{
	items_ = items;
	setParameter("Items", items);
}

