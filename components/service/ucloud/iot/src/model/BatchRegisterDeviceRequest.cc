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

#include <alibabacloud/iot/model/BatchRegisterDeviceRequest.h>

using AlibabaCloud::Iot::Model::BatchRegisterDeviceRequest;

BatchRegisterDeviceRequest::BatchRegisterDeviceRequest() :
	RpcServiceRequest("iot", "2018-01-20", "BatchRegisterDevice")
{
	setMethod(HttpRequest::Method::Post);
}

BatchRegisterDeviceRequest::~BatchRegisterDeviceRequest()
{}

std::string BatchRegisterDeviceRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void BatchRegisterDeviceRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string BatchRegisterDeviceRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void BatchRegisterDeviceRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

int BatchRegisterDeviceRequest::getCount()const
{
	return count_;
}

void BatchRegisterDeviceRequest::setCount(int count)
{
	count_ = count;
	setParameter("Count", std::to_string(count));
}

std::string BatchRegisterDeviceRequest::getProductKey()const
{
	return productKey_;
}

void BatchRegisterDeviceRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string BatchRegisterDeviceRequest::getApiProduct()const
{
	return apiProduct_;
}

void BatchRegisterDeviceRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string BatchRegisterDeviceRequest::getApiRevision()const
{
	return apiRevision_;
}

void BatchRegisterDeviceRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

