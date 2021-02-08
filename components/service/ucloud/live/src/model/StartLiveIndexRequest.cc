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

#include <alibabacloud/live/model/StartLiveIndexRequest.h>

using AlibabaCloud::Live::Model::StartLiveIndexRequest;

StartLiveIndexRequest::StartLiveIndexRequest() :
	RpcServiceRequest("live", "2016-11-01", "StartLiveIndex")
{
	setMethod(HttpRequest::Method::Post);
}

StartLiveIndexRequest::~StartLiveIndexRequest()
{}

std::string StartLiveIndexRequest::getTokenId()const
{
	return tokenId_;
}

void StartLiveIndexRequest::setTokenId(const std::string& tokenId)
{
	tokenId_ = tokenId;
	setParameter("TokenId", tokenId);
}

std::string StartLiveIndexRequest::getOssEndpoint()const
{
	return ossEndpoint_;
}

void StartLiveIndexRequest::setOssEndpoint(const std::string& ossEndpoint)
{
	ossEndpoint_ = ossEndpoint;
	setParameter("OssEndpoint", ossEndpoint);
}

std::string StartLiveIndexRequest::getAppName()const
{
	return appName_;
}

void StartLiveIndexRequest::setAppName(const std::string& appName)
{
	appName_ = appName;
	setParameter("AppName", appName);
}

std::string StartLiveIndexRequest::getOssRamRole()const
{
	return ossRamRole_;
}

void StartLiveIndexRequest::setOssRamRole(const std::string& ossRamRole)
{
	ossRamRole_ = ossRamRole;
	setParameter("OssRamRole", ossRamRole);
}

std::string StartLiveIndexRequest::getStreamName()const
{
	return streamName_;
}

void StartLiveIndexRequest::setStreamName(const std::string& streamName)
{
	streamName_ = streamName;
	setParameter("StreamName", streamName);
}

std::string StartLiveIndexRequest::getOssUserId()const
{
	return ossUserId_;
}

void StartLiveIndexRequest::setOssUserId(const std::string& ossUserId)
{
	ossUserId_ = ossUserId;
	setParameter("OssUserId", ossUserId);
}

std::string StartLiveIndexRequest::getOssBucket()const
{
	return ossBucket_;
}

void StartLiveIndexRequest::setOssBucket(const std::string& ossBucket)
{
	ossBucket_ = ossBucket;
	setParameter("OssBucket", ossBucket);
}

std::string StartLiveIndexRequest::getDomainName()const
{
	return domainName_;
}

void StartLiveIndexRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

std::string StartLiveIndexRequest::getInputUrl()const
{
	return inputUrl_;
}

void StartLiveIndexRequest::setInputUrl(const std::string& inputUrl)
{
	inputUrl_ = inputUrl;
	setParameter("InputUrl", inputUrl);
}

long StartLiveIndexRequest::getOwnerId()const
{
	return ownerId_;
}

void StartLiveIndexRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

int StartLiveIndexRequest::getInterval()const
{
	return interval_;
}

void StartLiveIndexRequest::setInterval(int interval)
{
	interval_ = interval;
	setParameter("Interval", std::to_string(interval));
}

