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

#include <alibabacloud/iot/model/DeleteDeviceRequest.h>

using AlibabaCloud::Iot::Model::DeleteDeviceRequest;

DeleteDeviceRequest::DeleteDeviceRequest() :
	RpcServiceRequest("iot", "2018-01-20", "DeleteDevice")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteDeviceRequest::~DeleteDeviceRequest()
{}

std::string DeleteDeviceRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DeleteDeviceRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DeleteDeviceRequest::getIotId()const
{
	return iotId_;
}

void DeleteDeviceRequest::setIotId(const std::string& iotId)
{
	iotId_ = iotId;
	setParameter("IotId", iotId);
}

std::string DeleteDeviceRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void DeleteDeviceRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string DeleteDeviceRequest::getProductKey()const
{
	return productKey_;
}

void DeleteDeviceRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string DeleteDeviceRequest::getApiProduct()const
{
	return apiProduct_;
}

void DeleteDeviceRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string DeleteDeviceRequest::getApiRevision()const
{
	return apiRevision_;
}

void DeleteDeviceRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string DeleteDeviceRequest::getDeviceName()const
{
	return deviceName_;
}

void DeleteDeviceRequest::setDeviceName(const std::string& deviceName)
{
	deviceName_ = deviceName;
	setParameter("DeviceName", deviceName);
}

