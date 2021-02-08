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

#include <alibabacloud/live/model/SetLiveLazyPullStreamInfoConfigRequest.h>

using AlibabaCloud::Live::Model::SetLiveLazyPullStreamInfoConfigRequest;

SetLiveLazyPullStreamInfoConfigRequest::SetLiveLazyPullStreamInfoConfigRequest() :
	RpcServiceRequest("live", "2016-11-01", "SetLiveLazyPullStreamInfoConfig")
{
	setMethod(HttpRequest::Method::Post);
}

SetLiveLazyPullStreamInfoConfigRequest::~SetLiveLazyPullStreamInfoConfigRequest()
{}

std::string SetLiveLazyPullStreamInfoConfigRequest::getPullArgs()const
{
	return pullArgs_;
}

void SetLiveLazyPullStreamInfoConfigRequest::setPullArgs(const std::string& pullArgs)
{
	pullArgs_ = pullArgs;
	setParameter("PullArgs", pullArgs);
}

std::string SetLiveLazyPullStreamInfoConfigRequest::getAppName()const
{
	return appName_;
}

void SetLiveLazyPullStreamInfoConfigRequest::setAppName(const std::string& appName)
{
	appName_ = appName;
	setParameter("AppName", appName);
}

std::string SetLiveLazyPullStreamInfoConfigRequest::getLiveapiRequestFrom()const
{
	return liveapiRequestFrom_;
}

void SetLiveLazyPullStreamInfoConfigRequest::setLiveapiRequestFrom(const std::string& liveapiRequestFrom)
{
	liveapiRequestFrom_ = liveapiRequestFrom;
	setParameter("LiveapiRequestFrom", liveapiRequestFrom);
}

std::string SetLiveLazyPullStreamInfoConfigRequest::getPullAuthKey()const
{
	return pullAuthKey_;
}

void SetLiveLazyPullStreamInfoConfigRequest::setPullAuthKey(const std::string& pullAuthKey)
{
	pullAuthKey_ = pullAuthKey;
	setParameter("PullAuthKey", pullAuthKey);
}

std::string SetLiveLazyPullStreamInfoConfigRequest::getPullAuthType()const
{
	return pullAuthType_;
}

void SetLiveLazyPullStreamInfoConfigRequest::setPullAuthType(const std::string& pullAuthType)
{
	pullAuthType_ = pullAuthType;
	setParameter("PullAuthType", pullAuthType);
}

std::string SetLiveLazyPullStreamInfoConfigRequest::getDomainName()const
{
	return domainName_;
}

void SetLiveLazyPullStreamInfoConfigRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

std::string SetLiveLazyPullStreamInfoConfigRequest::getPullDomainName()const
{
	return pullDomainName_;
}

void SetLiveLazyPullStreamInfoConfigRequest::setPullDomainName(const std::string& pullDomainName)
{
	pullDomainName_ = pullDomainName;
	setParameter("PullDomainName", pullDomainName);
}

long SetLiveLazyPullStreamInfoConfigRequest::getOwnerId()const
{
	return ownerId_;
}

void SetLiveLazyPullStreamInfoConfigRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string SetLiveLazyPullStreamInfoConfigRequest::getPullAppName()const
{
	return pullAppName_;
}

void SetLiveLazyPullStreamInfoConfigRequest::setPullAppName(const std::string& pullAppName)
{
	pullAppName_ = pullAppName;
	setParameter("PullAppName", pullAppName);
}

std::string SetLiveLazyPullStreamInfoConfigRequest::getPullProtocol()const
{
	return pullProtocol_;
}

void SetLiveLazyPullStreamInfoConfigRequest::setPullProtocol(const std::string& pullProtocol)
{
	pullProtocol_ = pullProtocol;
	setParameter("PullProtocol", pullProtocol);
}

