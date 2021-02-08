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

#include <alibabacloud/iot/model/BatchDeleteDeviceGroupRelationsRequest.h>

using AlibabaCloud::Iot::Model::BatchDeleteDeviceGroupRelationsRequest;

BatchDeleteDeviceGroupRelationsRequest::BatchDeleteDeviceGroupRelationsRequest() :
	RpcServiceRequest("iot", "2018-01-20", "BatchDeleteDeviceGroupRelations")
{
	setMethod(HttpRequest::Method::Post);
}

BatchDeleteDeviceGroupRelationsRequest::~BatchDeleteDeviceGroupRelationsRequest()
{}

std::string BatchDeleteDeviceGroupRelationsRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void BatchDeleteDeviceGroupRelationsRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string BatchDeleteDeviceGroupRelationsRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void BatchDeleteDeviceGroupRelationsRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string BatchDeleteDeviceGroupRelationsRequest::getGroupId()const
{
	return groupId_;
}

void BatchDeleteDeviceGroupRelationsRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string BatchDeleteDeviceGroupRelationsRequest::getApiProduct()const
{
	return apiProduct_;
}

void BatchDeleteDeviceGroupRelationsRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string BatchDeleteDeviceGroupRelationsRequest::getApiRevision()const
{
	return apiRevision_;
}

void BatchDeleteDeviceGroupRelationsRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::vector<BatchDeleteDeviceGroupRelationsRequest::Device> BatchDeleteDeviceGroupRelationsRequest::getDevice()const
{
	return device_;
}

void BatchDeleteDeviceGroupRelationsRequest::setDevice(const std::vector<Device>& device)
{
	device_ = device;
	for(int dep1 = 0; dep1!= device.size(); dep1++) {
		auto deviceObj = device.at(dep1);
		std::string deviceObjStr = "Device." + std::to_string(dep1 + 1);
		setParameter(deviceObjStr + ".DeviceName", deviceObj.deviceName);
		setParameter(deviceObjStr + ".ProductKey", deviceObj.productKey);
	}
}

