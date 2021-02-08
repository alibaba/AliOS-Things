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

#include <alibabacloud/iot/model/UpdateEdgeInstanceChannelRequest.h>

using AlibabaCloud::Iot::Model::UpdateEdgeInstanceChannelRequest;

UpdateEdgeInstanceChannelRequest::UpdateEdgeInstanceChannelRequest() :
	RpcServiceRequest("iot", "2018-01-20", "UpdateEdgeInstanceChannel")
{
	setMethod(HttpRequest::Method::Post);
}

UpdateEdgeInstanceChannelRequest::~UpdateEdgeInstanceChannelRequest()
{}

std::vector<UpdateEdgeInstanceChannelRequest::Configs> UpdateEdgeInstanceChannelRequest::getConfigs()const
{
	return configs_;
}

void UpdateEdgeInstanceChannelRequest::setConfigs(const std::vector<Configs>& configs)
{
	configs_ = configs;
	for(int dep1 = 0; dep1!= configs.size(); dep1++) {
		auto configsObj = configs.at(dep1);
		std::string configsObjStr = "Configs." + std::to_string(dep1 + 1);
		setParameter(configsObjStr + ".Format", configsObj.format);
		setParameter(configsObjStr + ".Content", configsObj.content);
		setParameter(configsObjStr + ".Key", configsObj.key);
	}
}

std::string UpdateEdgeInstanceChannelRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void UpdateEdgeInstanceChannelRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string UpdateEdgeInstanceChannelRequest::getDriverId()const
{
	return driverId_;
}

void UpdateEdgeInstanceChannelRequest::setDriverId(const std::string& driverId)
{
	driverId_ = driverId;
	setParameter("DriverId", driverId);
}

std::string UpdateEdgeInstanceChannelRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void UpdateEdgeInstanceChannelRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string UpdateEdgeInstanceChannelRequest::getChannelName()const
{
	return channelName_;
}

void UpdateEdgeInstanceChannelRequest::setChannelName(const std::string& channelName)
{
	channelName_ = channelName;
	setParameter("ChannelName", channelName);
}

std::string UpdateEdgeInstanceChannelRequest::getInstanceId()const
{
	return instanceId_;
}

void UpdateEdgeInstanceChannelRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setParameter("InstanceId", instanceId);
}

std::string UpdateEdgeInstanceChannelRequest::getApiProduct()const
{
	return apiProduct_;
}

void UpdateEdgeInstanceChannelRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string UpdateEdgeInstanceChannelRequest::getApiRevision()const
{
	return apiRevision_;
}

void UpdateEdgeInstanceChannelRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string UpdateEdgeInstanceChannelRequest::getChannelId()const
{
	return channelId_;
}

void UpdateEdgeInstanceChannelRequest::setChannelId(const std::string& channelId)
{
	channelId_ = channelId;
	setParameter("ChannelId", channelId);
}

