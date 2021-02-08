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

#include <alibabacloud/iot/model/BatchDeleteEdgeInstanceChannelRequest.h>

using AlibabaCloud::Iot::Model::BatchDeleteEdgeInstanceChannelRequest;

BatchDeleteEdgeInstanceChannelRequest::BatchDeleteEdgeInstanceChannelRequest() :
	RpcServiceRequest("iot", "2018-01-20", "BatchDeleteEdgeInstanceChannel")
{
	setMethod(HttpRequest::Method::Post);
}

BatchDeleteEdgeInstanceChannelRequest::~BatchDeleteEdgeInstanceChannelRequest()
{}

std::string BatchDeleteEdgeInstanceChannelRequest::getDriverId()const
{
	return driverId_;
}

void BatchDeleteEdgeInstanceChannelRequest::setDriverId(const std::string& driverId)
{
	driverId_ = driverId;
	setParameter("DriverId", driverId);
}

std::string BatchDeleteEdgeInstanceChannelRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void BatchDeleteEdgeInstanceChannelRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string BatchDeleteEdgeInstanceChannelRequest::getInstanceId()const
{
	return instanceId_;
}

void BatchDeleteEdgeInstanceChannelRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setParameter("InstanceId", instanceId);
}

std::string BatchDeleteEdgeInstanceChannelRequest::getApiProduct()const
{
	return apiProduct_;
}

void BatchDeleteEdgeInstanceChannelRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string BatchDeleteEdgeInstanceChannelRequest::getApiRevision()const
{
	return apiRevision_;
}

void BatchDeleteEdgeInstanceChannelRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::vector<std::string> BatchDeleteEdgeInstanceChannelRequest::getChannelIds()const
{
	return channelIds_;
}

void BatchDeleteEdgeInstanceChannelRequest::setChannelIds(const std::vector<std::string>& channelIds)
{
	channelIds_ = channelIds;
	for(int dep1 = 0; dep1!= channelIds.size(); dep1++) {
		setParameter("ChannelIds."+ std::to_string(dep1), channelIds.at(dep1));
	}
}

