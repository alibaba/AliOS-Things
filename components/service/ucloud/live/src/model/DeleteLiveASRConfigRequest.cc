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

#include <alibabacloud/live/model/DeleteLiveASRConfigRequest.h>

using AlibabaCloud::Live::Model::DeleteLiveASRConfigRequest;

DeleteLiveASRConfigRequest::DeleteLiveASRConfigRequest() :
	RpcServiceRequest("live", "2016-11-01", "DeleteLiveASRConfig")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteLiveASRConfigRequest::~DeleteLiveASRConfigRequest()
{}

std::string DeleteLiveASRConfigRequest::getAppName()const
{
	return appName_;
}

void DeleteLiveASRConfigRequest::setAppName(const std::string& appName)
{
	appName_ = appName;
	setParameter("AppName", appName);
}

std::string DeleteLiveASRConfigRequest::getStreamName()const
{
	return streamName_;
}

void DeleteLiveASRConfigRequest::setStreamName(const std::string& streamName)
{
	streamName_ = streamName;
	setParameter("StreamName", streamName);
}

std::string DeleteLiveASRConfigRequest::getDomainName()const
{
	return domainName_;
}

void DeleteLiveASRConfigRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

long DeleteLiveASRConfigRequest::getOwnerId()const
{
	return ownerId_;
}

void DeleteLiveASRConfigRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

