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

#include <alibabacloud/live/model/RealTimeRecordCommandRequest.h>

using AlibabaCloud::Live::Model::RealTimeRecordCommandRequest;

RealTimeRecordCommandRequest::RealTimeRecordCommandRequest() :
	RpcServiceRequest("live", "2016-11-01", "RealTimeRecordCommand")
{
	setMethod(HttpRequest::Method::Post);
}

RealTimeRecordCommandRequest::~RealTimeRecordCommandRequest()
{}

std::string RealTimeRecordCommandRequest::getAppName()const
{
	return appName_;
}

void RealTimeRecordCommandRequest::setAppName(const std::string& appName)
{
	appName_ = appName;
	setParameter("AppName", appName);
}

std::string RealTimeRecordCommandRequest::getStreamName()const
{
	return streamName_;
}

void RealTimeRecordCommandRequest::setStreamName(const std::string& streamName)
{
	streamName_ = streamName;
	setParameter("StreamName", streamName);
}

std::string RealTimeRecordCommandRequest::getDomainName()const
{
	return domainName_;
}

void RealTimeRecordCommandRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

long RealTimeRecordCommandRequest::getOwnerId()const
{
	return ownerId_;
}

void RealTimeRecordCommandRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string RealTimeRecordCommandRequest::getCommand()const
{
	return command_;
}

void RealTimeRecordCommandRequest::setCommand(const std::string& command)
{
	command_ = command;
	setParameter("Command", command);
}

