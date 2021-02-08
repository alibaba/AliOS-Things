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

#include <alibabacloud/live/model/UpdateLiveASRConfigRequest.h>

using AlibabaCloud::Live::Model::UpdateLiveASRConfigRequest;

UpdateLiveASRConfigRequest::UpdateLiveASRConfigRequest() :
	RpcServiceRequest("live", "2016-11-01", "UpdateLiveASRConfig")
{
	setMethod(HttpRequest::Method::Post);
}

UpdateLiveASRConfigRequest::~UpdateLiveASRConfigRequest()
{}

std::string UpdateLiveASRConfigRequest::getAppName()const
{
	return appName_;
}

void UpdateLiveASRConfigRequest::setAppName(const std::string& appName)
{
	appName_ = appName;
	setParameter("AppName", appName);
}

std::string UpdateLiveASRConfigRequest::getMnsTopic()const
{
	return mnsTopic_;
}

void UpdateLiveASRConfigRequest::setMnsTopic(const std::string& mnsTopic)
{
	mnsTopic_ = mnsTopic;
	setParameter("MnsTopic", mnsTopic);
}

std::string UpdateLiveASRConfigRequest::getStreamName()const
{
	return streamName_;
}

void UpdateLiveASRConfigRequest::setStreamName(const std::string& streamName)
{
	streamName_ = streamName;
	setParameter("StreamName", streamName);
}

int UpdateLiveASRConfigRequest::getPeriod()const
{
	return period_;
}

void UpdateLiveASRConfigRequest::setPeriod(int period)
{
	period_ = period;
	setParameter("Period", std::to_string(period));
}

std::string UpdateLiveASRConfigRequest::getDomainName()const
{
	return domainName_;
}

void UpdateLiveASRConfigRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

std::string UpdateLiveASRConfigRequest::getHttpCallbackURL()const
{
	return httpCallbackURL_;
}

void UpdateLiveASRConfigRequest::setHttpCallbackURL(const std::string& httpCallbackURL)
{
	httpCallbackURL_ = httpCallbackURL;
	setParameter("HttpCallbackURL", httpCallbackURL);
}

long UpdateLiveASRConfigRequest::getOwnerId()const
{
	return ownerId_;
}

void UpdateLiveASRConfigRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string UpdateLiveASRConfigRequest::getMnsRegion()const
{
	return mnsRegion_;
}

void UpdateLiveASRConfigRequest::setMnsRegion(const std::string& mnsRegion)
{
	mnsRegion_ = mnsRegion;
	setParameter("MnsRegion", mnsRegion);
}

