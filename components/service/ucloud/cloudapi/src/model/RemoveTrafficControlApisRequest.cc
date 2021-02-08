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

#include <alibabacloud/cloudapi/model/RemoveTrafficControlApisRequest.h>

using AlibabaCloud::CloudAPI::Model::RemoveTrafficControlApisRequest;

RemoveTrafficControlApisRequest::RemoveTrafficControlApisRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "RemoveTrafficControlApis")
{
	setMethod(HttpRequest::Method::Post);
}

RemoveTrafficControlApisRequest::~RemoveTrafficControlApisRequest()
{}

std::string RemoveTrafficControlApisRequest::getTrafficControlId()const
{
	return trafficControlId_;
}

void RemoveTrafficControlApisRequest::setTrafficControlId(const std::string& trafficControlId)
{
	trafficControlId_ = trafficControlId;
	setParameter("TrafficControlId", trafficControlId);
}

std::string RemoveTrafficControlApisRequest::getStageName()const
{
	return stageName_;
}

void RemoveTrafficControlApisRequest::setStageName(const std::string& stageName)
{
	stageName_ = stageName;
	setParameter("StageName", stageName);
}

std::string RemoveTrafficControlApisRequest::getGroupId()const
{
	return groupId_;
}

void RemoveTrafficControlApisRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string RemoveTrafficControlApisRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void RemoveTrafficControlApisRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string RemoveTrafficControlApisRequest::getSecurityToken()const
{
	return securityToken_;
}

void RemoveTrafficControlApisRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string RemoveTrafficControlApisRequest::getApiIds()const
{
	return apiIds_;
}

void RemoveTrafficControlApisRequest::setApiIds(const std::string& apiIds)
{
	apiIds_ = apiIds;
	setParameter("ApiIds", apiIds);
}

