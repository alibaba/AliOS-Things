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

#include <alibabacloud/cloudapi/model/DeleteAllTrafficSpecialControlRequest.h>

using AlibabaCloud::CloudAPI::Model::DeleteAllTrafficSpecialControlRequest;

DeleteAllTrafficSpecialControlRequest::DeleteAllTrafficSpecialControlRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DeleteAllTrafficSpecialControl")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteAllTrafficSpecialControlRequest::~DeleteAllTrafficSpecialControlRequest()
{}

std::string DeleteAllTrafficSpecialControlRequest::getTrafficControlId()const
{
	return trafficControlId_;
}

void DeleteAllTrafficSpecialControlRequest::setTrafficControlId(const std::string& trafficControlId)
{
	trafficControlId_ = trafficControlId;
	setParameter("TrafficControlId", trafficControlId);
}

std::string DeleteAllTrafficSpecialControlRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DeleteAllTrafficSpecialControlRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DeleteAllTrafficSpecialControlRequest::getSecurityToken()const
{
	return securityToken_;
}

void DeleteAllTrafficSpecialControlRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

