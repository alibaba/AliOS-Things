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

#include <alibabacloud/iot/model/BatchSetEdgeInstanceDeviceConfigRequest.h>

using AlibabaCloud::Iot::Model::BatchSetEdgeInstanceDeviceConfigRequest;

BatchSetEdgeInstanceDeviceConfigRequest::BatchSetEdgeInstanceDeviceConfigRequest() :
	RpcServiceRequest("iot", "2018-01-20", "BatchSetEdgeInstanceDeviceConfig")
{
	setMethod(HttpRequest::Method::Post);
}

BatchSetEdgeInstanceDeviceConfigRequest::~BatchSetEdgeInstanceDeviceConfigRequest()
{}

std::string BatchSetEdgeInstanceDeviceConfigRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void BatchSetEdgeInstanceDeviceConfigRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::vector<BatchSetEdgeInstanceDeviceConfigRequest::DeviceConfigs> BatchSetEdgeInstanceDeviceConfigRequest::getDeviceConfigs()const
{
	return deviceConfigs_;
}

void BatchSetEdgeInstanceDeviceConfigRequest::setDeviceConfigs(const std::vector<DeviceConfigs>& deviceConfigs)
{
	deviceConfigs_ = deviceConfigs;
	for(int dep1 = 0; dep1!= deviceConfigs.size(); dep1++) {
		auto deviceConfigsObj = deviceConfigs.at(dep1);
		std::string deviceConfigsObjStr = "DeviceConfigs." + std::to_string(dep1 + 1);
		setParameter(deviceConfigsObjStr + ".IotId", deviceConfigsObj.iotId);
		setParameter(deviceConfigsObjStr + ".Content", deviceConfigsObj.content);
	}
}

std::string BatchSetEdgeInstanceDeviceConfigRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void BatchSetEdgeInstanceDeviceConfigRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string BatchSetEdgeInstanceDeviceConfigRequest::getInstanceId()const
{
	return instanceId_;
}

void BatchSetEdgeInstanceDeviceConfigRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setParameter("InstanceId", instanceId);
}

std::string BatchSetEdgeInstanceDeviceConfigRequest::getApiProduct()const
{
	return apiProduct_;
}

void BatchSetEdgeInstanceDeviceConfigRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string BatchSetEdgeInstanceDeviceConfigRequest::getApiRevision()const
{
	return apiRevision_;
}

void BatchSetEdgeInstanceDeviceConfigRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

