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

#include <alibabacloud/live/model/SetLiveStreamOptimizedFeatureConfigRequest.h>

using AlibabaCloud::Live::Model::SetLiveStreamOptimizedFeatureConfigRequest;

SetLiveStreamOptimizedFeatureConfigRequest::SetLiveStreamOptimizedFeatureConfigRequest() :
	RpcServiceRequest("live", "2016-11-01", "SetLiveStreamOptimizedFeatureConfig")
{
	setMethod(HttpRequest::Method::Post);
}

SetLiveStreamOptimizedFeatureConfigRequest::~SetLiveStreamOptimizedFeatureConfigRequest()
{}

std::string SetLiveStreamOptimizedFeatureConfigRequest::getConfigStatus()const
{
	return configStatus_;
}

void SetLiveStreamOptimizedFeatureConfigRequest::setConfigStatus(const std::string& configStatus)
{
	configStatus_ = configStatus;
	setParameter("ConfigStatus", configStatus);
}

std::string SetLiveStreamOptimizedFeatureConfigRequest::getConfigName()const
{
	return configName_;
}

void SetLiveStreamOptimizedFeatureConfigRequest::setConfigName(const std::string& configName)
{
	configName_ = configName;
	setParameter("ConfigName", configName);
}

std::string SetLiveStreamOptimizedFeatureConfigRequest::getDomainName()const
{
	return domainName_;
}

void SetLiveStreamOptimizedFeatureConfigRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

std::string SetLiveStreamOptimizedFeatureConfigRequest::getConfigValue()const
{
	return configValue_;
}

void SetLiveStreamOptimizedFeatureConfigRequest::setConfigValue(const std::string& configValue)
{
	configValue_ = configValue;
	setParameter("ConfigValue", configValue);
}

long SetLiveStreamOptimizedFeatureConfigRequest::getOwnerId()const
{
	return ownerId_;
}

void SetLiveStreamOptimizedFeatureConfigRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

