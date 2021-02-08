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

#include <alibabacloud/live/model/DeleteLiveRealtimeLogDeliveryRequest.h>

using AlibabaCloud::Live::Model::DeleteLiveRealtimeLogDeliveryRequest;

DeleteLiveRealtimeLogDeliveryRequest::DeleteLiveRealtimeLogDeliveryRequest() :
	RpcServiceRequest("live", "2016-11-01", "DeleteLiveRealtimeLogDelivery")
{
	setMethod(HttpRequest::Method::Get);
}

DeleteLiveRealtimeLogDeliveryRequest::~DeleteLiveRealtimeLogDeliveryRequest()
{}

std::string DeleteLiveRealtimeLogDeliveryRequest::getProject()const
{
	return project_;
}

void DeleteLiveRealtimeLogDeliveryRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string DeleteLiveRealtimeLogDeliveryRequest::getDomainName()const
{
	return domainName_;
}

void DeleteLiveRealtimeLogDeliveryRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

long DeleteLiveRealtimeLogDeliveryRequest::getOwnerId()const
{
	return ownerId_;
}

void DeleteLiveRealtimeLogDeliveryRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DeleteLiveRealtimeLogDeliveryRequest::getRegion()const
{
	return region_;
}

void DeleteLiveRealtimeLogDeliveryRequest::setRegion(const std::string& region)
{
	region_ = region;
	setParameter("Region", region);
}

std::string DeleteLiveRealtimeLogDeliveryRequest::getLogstore()const
{
	return logstore_;
}

void DeleteLiveRealtimeLogDeliveryRequest::setLogstore(const std::string& logstore)
{
	logstore_ = logstore;
	setParameter("Logstore", logstore);
}

