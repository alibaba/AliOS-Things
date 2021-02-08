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

#include <alibabacloud/iot/model/BatchSetEdgeInstanceDeviceChannelRequest.h>

using AlibabaCloud::Iot::Model::BatchSetEdgeInstanceDeviceChannelRequest;

BatchSetEdgeInstanceDeviceChannelRequest::BatchSetEdgeInstanceDeviceChannelRequest() :
	RpcServiceRequest("iot", "2018-01-20", "BatchSetEdgeInstanceDeviceChannel")
{
	setMethod(HttpRequest::Method::Post);
}

BatchSetEdgeInstanceDeviceChannelRequest::~BatchSetEdgeInstanceDeviceChannelRequest()
{}

std::string BatchSetEdgeInstanceDeviceChannelRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void BatchSetEdgeInstanceDeviceChannelRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string BatchSetEdgeInstanceDeviceChannelRequest::getDriverId()const
{
	return driverId_;
}

void BatchSetEdgeInstanceDeviceChannelRequest::setDriverId(const std::string& driverId)
{
	driverId_ = driverId;
	setParameter("DriverId", driverId);
}

std::vector<std::string> BatchSetEdgeInstanceDeviceChannelRequest::getIotIds()const
{
	return iotIds_;
}

void BatchSetEdgeInstanceDeviceChannelRequest::setIotIds(const std::vector<std::string>& iotIds)
{
	iotIds_ = iotIds;
	for(int dep1 = 0; dep1!= iotIds.size(); dep1++) {
		setParameter("IotIds."+ std::to_string(dep1), iotIds.at(dep1));
	}
}

std::string BatchSetEdgeInstanceDeviceChannelRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void BatchSetEdgeInstanceDeviceChannelRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string BatchSetEdgeInstanceDeviceChannelRequest::getInstanceId()const
{
	return instanceId_;
}

void BatchSetEdgeInstanceDeviceChannelRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setParameter("InstanceId", instanceId);
}

std::string BatchSetEdgeInstanceDeviceChannelRequest::getApiProduct()const
{
	return apiProduct_;
}

void BatchSetEdgeInstanceDeviceChannelRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string BatchSetEdgeInstanceDeviceChannelRequest::getApiRevision()const
{
	return apiRevision_;
}

void BatchSetEdgeInstanceDeviceChannelRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string BatchSetEdgeInstanceDeviceChannelRequest::getChannelId()const
{
	return channelId_;
}

void BatchSetEdgeInstanceDeviceChannelRequest::setChannelId(const std::string& channelId)
{
	channelId_ = channelId;
	setParameter("ChannelId", channelId);
}

