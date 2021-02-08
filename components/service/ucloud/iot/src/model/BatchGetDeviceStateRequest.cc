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

#include <alibabacloud/iot/model/BatchGetDeviceStateRequest.h>

using AlibabaCloud::Iot::Model::BatchGetDeviceStateRequest;

BatchGetDeviceStateRequest::BatchGetDeviceStateRequest() :
	RpcServiceRequest("iot", "2018-01-20", "BatchGetDeviceState")
{
	setMethod(HttpRequest::Method::Post);
}

BatchGetDeviceStateRequest::~BatchGetDeviceStateRequest()
{}

std::string BatchGetDeviceStateRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void BatchGetDeviceStateRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::vector<std::string> BatchGetDeviceStateRequest::getIotId()const
{
	return iotId_;
}

void BatchGetDeviceStateRequest::setIotId(const std::vector<std::string>& iotId)
{
	iotId_ = iotId;
	for(int dep1 = 0; dep1!= iotId.size(); dep1++) {
		setParameter("IotId."+ std::to_string(dep1), iotId.at(dep1));
	}
}

std::string BatchGetDeviceStateRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void BatchGetDeviceStateRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string BatchGetDeviceStateRequest::getProductKey()const
{
	return productKey_;
}

void BatchGetDeviceStateRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string BatchGetDeviceStateRequest::getApiProduct()const
{
	return apiProduct_;
}

void BatchGetDeviceStateRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string BatchGetDeviceStateRequest::getApiRevision()const
{
	return apiRevision_;
}

void BatchGetDeviceStateRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::vector<std::string> BatchGetDeviceStateRequest::getDeviceName()const
{
	return deviceName_;
}

void BatchGetDeviceStateRequest::setDeviceName(const std::vector<std::string>& deviceName)
{
	deviceName_ = deviceName;
	for(int dep1 = 0; dep1!= deviceName.size(); dep1++) {
		setParameter("DeviceName."+ std::to_string(dep1), deviceName.at(dep1));
	}
}

