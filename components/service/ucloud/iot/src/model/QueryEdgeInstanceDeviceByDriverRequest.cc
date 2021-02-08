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

#include <alibabacloud/iot/model/QueryEdgeInstanceDeviceByDriverRequest.h>

using AlibabaCloud::Iot::Model::QueryEdgeInstanceDeviceByDriverRequest;

QueryEdgeInstanceDeviceByDriverRequest::QueryEdgeInstanceDeviceByDriverRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QueryEdgeInstanceDeviceByDriver")
{
	setMethod(HttpRequest::Method::Post);
}

QueryEdgeInstanceDeviceByDriverRequest::~QueryEdgeInstanceDeviceByDriverRequest()
{}

std::string QueryEdgeInstanceDeviceByDriverRequest::getDriverId()const
{
	return driverId_;
}

void QueryEdgeInstanceDeviceByDriverRequest::setDriverId(const std::string& driverId)
{
	driverId_ = driverId;
	setParameter("DriverId", driverId);
}

std::string QueryEdgeInstanceDeviceByDriverRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QueryEdgeInstanceDeviceByDriverRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

int QueryEdgeInstanceDeviceByDriverRequest::getPageSize()const
{
	return pageSize_;
}

void QueryEdgeInstanceDeviceByDriverRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

int QueryEdgeInstanceDeviceByDriverRequest::getCurrentPage()const
{
	return currentPage_;
}

void QueryEdgeInstanceDeviceByDriverRequest::setCurrentPage(int currentPage)
{
	currentPage_ = currentPage;
	setParameter("CurrentPage", std::to_string(currentPage));
}

std::string QueryEdgeInstanceDeviceByDriverRequest::getInstanceId()const
{
	return instanceId_;
}

void QueryEdgeInstanceDeviceByDriverRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setParameter("InstanceId", instanceId);
}

std::string QueryEdgeInstanceDeviceByDriverRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryEdgeInstanceDeviceByDriverRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryEdgeInstanceDeviceByDriverRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryEdgeInstanceDeviceByDriverRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

std::string QueryEdgeInstanceDeviceByDriverRequest::getChannelId()const
{
	return channelId_;
}

void QueryEdgeInstanceDeviceByDriverRequest::setChannelId(const std::string& channelId)
{
	channelId_ = channelId;
	setParameter("ChannelId", channelId);
}

