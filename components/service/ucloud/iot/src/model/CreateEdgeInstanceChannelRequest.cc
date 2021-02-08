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

#include <alibabacloud/iot/model/CreateEdgeInstanceChannelRequest.h>

using AlibabaCloud::Iot::Model::CreateEdgeInstanceChannelRequest;

CreateEdgeInstanceChannelRequest::CreateEdgeInstanceChannelRequest() :
	RpcServiceRequest("iot", "2018-01-20", "CreateEdgeInstanceChannel")
{
	setMethod(HttpRequest::Method::Post);
}

CreateEdgeInstanceChannelRequest::~CreateEdgeInstanceChannelRequest()
{}

std::vector<CreateEdgeInstanceChannelRequest::Configs> CreateEdgeInstanceChannelRequest::getConfigs()const
{
	return configs_;
}

void CreateEdgeInstanceChannelRequest::setConfigs(const std::vector<Configs>& configs)
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

std::string CreateEdgeInstanceChannelRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CreateEdgeInstanceChannelRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CreateEdgeInstanceChannelRequest::getDriverId()const
{
	return driverId_;
}

void CreateEdgeInstanceChannelRequest::setDriverId(const std::string& driverId)
{
	driverId_ = driverId;
	setParameter("DriverId", driverId);
}

std::string CreateEdgeInstanceChannelRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void CreateEdgeInstanceChannelRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string CreateEdgeInstanceChannelRequest::getChannelName()const
{
	return channelName_;
}

void CreateEdgeInstanceChannelRequest::setChannelName(const std::string& channelName)
{
	channelName_ = channelName;
	setParameter("ChannelName", channelName);
}

std::string CreateEdgeInstanceChannelRequest::getInstanceId()const
{
	return instanceId_;
}

void CreateEdgeInstanceChannelRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setParameter("InstanceId", instanceId);
}

std::string CreateEdgeInstanceChannelRequest::getApiProduct()const
{
	return apiProduct_;
}

void CreateEdgeInstanceChannelRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string CreateEdgeInstanceChannelRequest::getApiRevision()const
{
	return apiRevision_;
}

void CreateEdgeInstanceChannelRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

