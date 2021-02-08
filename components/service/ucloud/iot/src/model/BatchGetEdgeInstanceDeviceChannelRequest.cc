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

#include <alibabacloud/iot/model/BatchGetEdgeInstanceDeviceChannelRequest.h>

using AlibabaCloud::Iot::Model::BatchGetEdgeInstanceDeviceChannelRequest;

BatchGetEdgeInstanceDeviceChannelRequest::BatchGetEdgeInstanceDeviceChannelRequest() :
	RpcServiceRequest("iot", "2018-01-20", "BatchGetEdgeInstanceDeviceChannel")
{
	setMethod(HttpRequest::Method::Post);
}

BatchGetEdgeInstanceDeviceChannelRequest::~BatchGetEdgeInstanceDeviceChannelRequest()
{}

std::string BatchGetEdgeInstanceDeviceChannelRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void BatchGetEdgeInstanceDeviceChannelRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string BatchGetEdgeInstanceDeviceChannelRequest::getDriverId()const
{
	return driverId_;
}

void BatchGetEdgeInstanceDeviceChannelRequest::setDriverId(const std::string& driverId)
{
	driverId_ = driverId;
	setParameter("DriverId", driverId);
}

std::vector<std::string> BatchGetEdgeInstanceDeviceChannelRequest::getIotIds()const
{
	return iotIds_;
}

void BatchGetEdgeInstanceDeviceChannelRequest::setIotIds(const std::vector<std::string>& iotIds)
{
	iotIds_ = iotIds;
	for(int dep1 = 0; dep1!= iotIds.size(); dep1++) {
		setParameter("IotIds."+ std::to_string(dep1), iotIds.at(dep1));
	}
}

std::string BatchGetEdgeInstanceDeviceChannelRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void BatchGetEdgeInstanceDeviceChannelRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string BatchGetEdgeInstanceDeviceChannelRequest::getInstanceId()const
{
	return instanceId_;
}

void BatchGetEdgeInstanceDeviceChannelRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setParameter("InstanceId", instanceId);
}

std::string BatchGetEdgeInstanceDeviceChannelRequest::getApiProduct()const
{
	return apiProduct_;
}

void BatchGetEdgeInstanceDeviceChannelRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string BatchGetEdgeInstanceDeviceChannelRequest::getApiRevision()const
{
	return apiRevision_;
}

void BatchGetEdgeInstanceDeviceChannelRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

