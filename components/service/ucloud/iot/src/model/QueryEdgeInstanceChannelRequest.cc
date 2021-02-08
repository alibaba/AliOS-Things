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

#include <alibabacloud/iot/model/QueryEdgeInstanceChannelRequest.h>

using AlibabaCloud::Iot::Model::QueryEdgeInstanceChannelRequest;

QueryEdgeInstanceChannelRequest::QueryEdgeInstanceChannelRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QueryEdgeInstanceChannel")
{
	setMethod(HttpRequest::Method::Post);
}

QueryEdgeInstanceChannelRequest::~QueryEdgeInstanceChannelRequest()
{}

std::string QueryEdgeInstanceChannelRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void QueryEdgeInstanceChannelRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string QueryEdgeInstanceChannelRequest::getDriverId()const
{
	return driverId_;
}

void QueryEdgeInstanceChannelRequest::setDriverId(const std::string& driverId)
{
	driverId_ = driverId;
	setParameter("DriverId", driverId);
}

std::string QueryEdgeInstanceChannelRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QueryEdgeInstanceChannelRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

int QueryEdgeInstanceChannelRequest::getPageSize()const
{
	return pageSize_;
}

void QueryEdgeInstanceChannelRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

int QueryEdgeInstanceChannelRequest::getCurrentPage()const
{
	return currentPage_;
}

void QueryEdgeInstanceChannelRequest::setCurrentPage(int currentPage)
{
	currentPage_ = currentPage;
	setParameter("CurrentPage", std::to_string(currentPage));
}

std::string QueryEdgeInstanceChannelRequest::getChannelName()const
{
	return channelName_;
}

void QueryEdgeInstanceChannelRequest::setChannelName(const std::string& channelName)
{
	channelName_ = channelName;
	setParameter("ChannelName", channelName);
}

std::string QueryEdgeInstanceChannelRequest::getInstanceId()const
{
	return instanceId_;
}

void QueryEdgeInstanceChannelRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setParameter("InstanceId", instanceId);
}

std::string QueryEdgeInstanceChannelRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryEdgeInstanceChannelRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryEdgeInstanceChannelRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryEdgeInstanceChannelRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

