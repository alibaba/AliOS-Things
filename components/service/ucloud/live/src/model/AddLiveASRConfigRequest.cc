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

#include <alibabacloud/live/model/AddLiveASRConfigRequest.h>

using AlibabaCloud::Live::Model::AddLiveASRConfigRequest;

AddLiveASRConfigRequest::AddLiveASRConfigRequest() :
	RpcServiceRequest("live", "2016-11-01", "AddLiveASRConfig")
{
	setMethod(HttpRequest::Method::Post);
}

AddLiveASRConfigRequest::~AddLiveASRConfigRequest()
{}

std::string AddLiveASRConfigRequest::getAppName()const
{
	return appName_;
}

void AddLiveASRConfigRequest::setAppName(const std::string& appName)
{
	appName_ = appName;
	setParameter("AppName", appName);
}

std::string AddLiveASRConfigRequest::getMnsTopic()const
{
	return mnsTopic_;
}

void AddLiveASRConfigRequest::setMnsTopic(const std::string& mnsTopic)
{
	mnsTopic_ = mnsTopic;
	setParameter("MnsTopic", mnsTopic);
}

std::string AddLiveASRConfigRequest::getStreamName()const
{
	return streamName_;
}

void AddLiveASRConfigRequest::setStreamName(const std::string& streamName)
{
	streamName_ = streamName;
	setParameter("StreamName", streamName);
}

int AddLiveASRConfigRequest::getPeriod()const
{
	return period_;
}

void AddLiveASRConfigRequest::setPeriod(int period)
{
	period_ = period;
	setParameter("Period", std::to_string(period));
}

std::string AddLiveASRConfigRequest::getDomainName()const
{
	return domainName_;
}

void AddLiveASRConfigRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

std::string AddLiveASRConfigRequest::getHttpCallbackURL()const
{
	return httpCallbackURL_;
}

void AddLiveASRConfigRequest::setHttpCallbackURL(const std::string& httpCallbackURL)
{
	httpCallbackURL_ = httpCallbackURL;
	setParameter("HttpCallbackURL", httpCallbackURL);
}

long AddLiveASRConfigRequest::getOwnerId()const
{
	return ownerId_;
}

void AddLiveASRConfigRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string AddLiveASRConfigRequest::getMnsRegion()const
{
	return mnsRegion_;
}

void AddLiveASRConfigRequest::setMnsRegion(const std::string& mnsRegion)
{
	mnsRegion_ = mnsRegion;
	setParameter("MnsRegion", mnsRegion);
}

