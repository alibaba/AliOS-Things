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

#include <alibabacloud/live/model/SetLiveStreamDelayConfigRequest.h>

using AlibabaCloud::Live::Model::SetLiveStreamDelayConfigRequest;

SetLiveStreamDelayConfigRequest::SetLiveStreamDelayConfigRequest() :
	RpcServiceRequest("live", "2016-11-01", "SetLiveStreamDelayConfig")
{
	setMethod(HttpRequest::Method::Post);
}

SetLiveStreamDelayConfigRequest::~SetLiveStreamDelayConfigRequest()
{}

std::string SetLiveStreamDelayConfigRequest::getFlvLevel()const
{
	return flvLevel_;
}

void SetLiveStreamDelayConfigRequest::setFlvLevel(const std::string& flvLevel)
{
	flvLevel_ = flvLevel;
	setParameter("FlvLevel", flvLevel);
}

std::string SetLiveStreamDelayConfigRequest::getHlsLevel()const
{
	return hlsLevel_;
}

void SetLiveStreamDelayConfigRequest::setHlsLevel(const std::string& hlsLevel)
{
	hlsLevel_ = hlsLevel;
	setParameter("HlsLevel", hlsLevel);
}

int SetLiveStreamDelayConfigRequest::getRtmpDelay()const
{
	return rtmpDelay_;
}

void SetLiveStreamDelayConfigRequest::setRtmpDelay(int rtmpDelay)
{
	rtmpDelay_ = rtmpDelay;
	setParameter("RtmpDelay", std::to_string(rtmpDelay));
}

std::string SetLiveStreamDelayConfigRequest::getDomainName()const
{
	return domainName_;
}

void SetLiveStreamDelayConfigRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

long SetLiveStreamDelayConfigRequest::getOwnerId()const
{
	return ownerId_;
}

void SetLiveStreamDelayConfigRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

int SetLiveStreamDelayConfigRequest::getFlvDelay()const
{
	return flvDelay_;
}

void SetLiveStreamDelayConfigRequest::setFlvDelay(int flvDelay)
{
	flvDelay_ = flvDelay;
	setParameter("FlvDelay", std::to_string(flvDelay));
}

std::string SetLiveStreamDelayConfigRequest::getRtmpLevel()const
{
	return rtmpLevel_;
}

void SetLiveStreamDelayConfigRequest::setRtmpLevel(const std::string& rtmpLevel)
{
	rtmpLevel_ = rtmpLevel;
	setParameter("RtmpLevel", rtmpLevel);
}

int SetLiveStreamDelayConfigRequest::getHlsDelay()const
{
	return hlsDelay_;
}

void SetLiveStreamDelayConfigRequest::setHlsDelay(int hlsDelay)
{
	hlsDelay_ = hlsDelay;
	setParameter("HlsDelay", std::to_string(hlsDelay));
}

