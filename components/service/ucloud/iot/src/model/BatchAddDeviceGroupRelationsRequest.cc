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

#include <alibabacloud/iot/model/BatchAddDeviceGroupRelationsRequest.h>

using AlibabaCloud::Iot::Model::BatchAddDeviceGroupRelationsRequest;

BatchAddDeviceGroupRelationsRequest::BatchAddDeviceGroupRelationsRequest() :
	RpcServiceRequest("iot", "2018-01-20", "BatchAddDeviceGroupRelations")
{
	setMethod(HttpRequest::Method::Post);
}

BatchAddDeviceGroupRelationsRequest::~BatchAddDeviceGroupRelationsRequest()
{}

std::string BatchAddDeviceGroupRelationsRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void BatchAddDeviceGroupRelationsRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string BatchAddDeviceGroupRelationsRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void BatchAddDeviceGroupRelationsRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string BatchAddDeviceGroupRelationsRequest::getGroupId()const
{
	return groupId_;
}

void BatchAddDeviceGroupRelationsRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string BatchAddDeviceGroupRelationsRequest::getApiProduct()const
{
	return apiProduct_;
}

void BatchAddDeviceGroupRelationsRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string BatchAddDeviceGroupRelationsRequest::getApiRevision()const
{
	return apiRevision_;
}

void BatchAddDeviceGroupRelationsRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::vector<BatchAddDeviceGroupRelationsRequest::Device> BatchAddDeviceGroupRelationsRequest::getDevice()const
{
	return device_;
}

void BatchAddDeviceGroupRelationsRequest::setDevice(const std::vector<Device>& device)
{
	device_ = device;
	for(int dep1 = 0; dep1!= device.size(); dep1++) {
		auto deviceObj = device.at(dep1);
		std::string deviceObjStr = "Device." + std::to_string(dep1 + 1);
		setParameter(deviceObjStr + ".DeviceName", deviceObj.deviceName);
		setParameter(deviceObjStr + ".ProductKey", deviceObj.productKey);
	}
}

