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

#include <alibabacloud/live/model/StopLiveIndexRequest.h>

using AlibabaCloud::Live::Model::StopLiveIndexRequest;

StopLiveIndexRequest::StopLiveIndexRequest() :
	RpcServiceRequest("live", "2016-11-01", "StopLiveIndex")
{
	setMethod(HttpRequest::Method::Post);
}

StopLiveIndexRequest::~StopLiveIndexRequest()
{}

std::string StopLiveIndexRequest::getAppName()const
{
	return appName_;
}

void StopLiveIndexRequest::setAppName(const std::string& appName)
{
	appName_ = appName;
	setParameter("AppName", appName);
}

std::string StopLiveIndexRequest::getStreamName()const
{
	return streamName_;
}

void StopLiveIndexRequest::setStreamName(const std::string& streamName)
{
	streamName_ = streamName;
	setParameter("StreamName", streamName);
}

std::string StopLiveIndexRequest::getTaskId()const
{
	return taskId_;
}

void StopLiveIndexRequest::setTaskId(const std::string& taskId)
{
	taskId_ = taskId;
	setParameter("TaskId", taskId);
}

std::string StopLiveIndexRequest::getDomainName()const
{
	return domainName_;
}

void StopLiveIndexRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

long StopLiveIndexRequest::getOwnerId()const
{
	return ownerId_;
}

void StopLiveIndexRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

