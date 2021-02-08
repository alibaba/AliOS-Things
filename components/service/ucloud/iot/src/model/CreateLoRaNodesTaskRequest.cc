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

#include <alibabacloud/iot/model/CreateLoRaNodesTaskRequest.h>

using AlibabaCloud::Iot::Model::CreateLoRaNodesTaskRequest;

CreateLoRaNodesTaskRequest::CreateLoRaNodesTaskRequest() :
	RpcServiceRequest("iot", "2018-01-20", "CreateLoRaNodesTask")
{
	setMethod(HttpRequest::Method::Post);
}

CreateLoRaNodesTaskRequest::~CreateLoRaNodesTaskRequest()
{}

std::string CreateLoRaNodesTaskRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CreateLoRaNodesTaskRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CreateLoRaNodesTaskRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void CreateLoRaNodesTaskRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::vector<CreateLoRaNodesTaskRequest::DeviceInfo> CreateLoRaNodesTaskRequest::getDeviceInfo()const
{
	return deviceInfo_;
}

void CreateLoRaNodesTaskRequest::setDeviceInfo(const std::vector<DeviceInfo>& deviceInfo)
{
	deviceInfo_ = deviceInfo;
	for(int dep1 = 0; dep1!= deviceInfo.size(); dep1++) {
		auto deviceInfoObj = deviceInfo.at(dep1);
		std::string deviceInfoObjStr = "DeviceInfo." + std::to_string(dep1 + 1);
		setParameter(deviceInfoObjStr + ".PinCode", deviceInfoObj.pinCode);
		setParameter(deviceInfoObjStr + ".DevEui", deviceInfoObj.devEui);
	}
}

std::string CreateLoRaNodesTaskRequest::getProductKey()const
{
	return productKey_;
}

void CreateLoRaNodesTaskRequest::setProductKey(const std::string& productKey)
{
	productKey_ = productKey;
	setParameter("ProductKey", productKey);
}

std::string CreateLoRaNodesTaskRequest::getApiProduct()const
{
	return apiProduct_;
}

void CreateLoRaNodesTaskRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string CreateLoRaNodesTaskRequest::getApiRevision()const
{
	return apiRevision_;
}

void CreateLoRaNodesTaskRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

