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

#include <alibabacloud/iot/model/GetDeviceStatusRequest.h>

using AlibabaCloud::Iot::Model::GetDeviceStatusRequest;

GetDeviceStatusRequest::GetDeviceStatusRequest() :
	RpcServiceRequest("iot", "2018-01-20", "GetDeviceStatus")
{
	setMethod(HttpRequest::Method::Post);
}

GetDeviceStatusRequest::~GetDeviceStatusRequest()
{}

std::string GetDeviceStatusRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void GetDeviceStatusRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string GetDeviceStatusRequest::getIotId()const
{
	return iotId_;
}

void GetDeviceStatusRequest::setIotId(const std::string& iotId)
{
	iotId_ = iotId;
	setParameter("IotId", iotId);
}

std::string GetDeviceStatusRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void GetDeviceStatusRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string GetDeviceStatusRequest::getProductKey()const
{
	return productKey_;
}

void GetDeviceStatusRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string GetDeviceStatusRequest::getApiProduct()const
{
	return apiProduct_;
}

void GetDeviceStatusRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string GetDeviceStatusRequest::getApiRevision()const
{
	return apiRevision_;
}

void GetDeviceStatusRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string GetDeviceStatusRequest::getDeviceName()const
{
	return deviceName_;
}

void GetDeviceStatusRequest::setDeviceName(const std::string& deviceName)
{
	deviceName_ = deviceName;
	setParameter("DeviceName", deviceName);
}

