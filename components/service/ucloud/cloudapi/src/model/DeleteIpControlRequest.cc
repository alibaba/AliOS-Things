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

#include <alibabacloud/cloudapi/model/DeleteIpControlRequest.h>

using AlibabaCloud::CloudAPI::Model::DeleteIpControlRequest;

DeleteIpControlRequest::DeleteIpControlRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DeleteIpControl")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteIpControlRequest::~DeleteIpControlRequest()
{}

std::string DeleteIpControlRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DeleteIpControlRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DeleteIpControlRequest::getIpControlId()const
{
	return ipControlId_;
}

void DeleteIpControlRequest::setIpControlId(const std::string& ipControlId)
{
	ipControlId_ = ipControlId;
	setParameter("IpControlId", ipControlId);
}

std::string DeleteIpControlRequest::getSecurityToken()const
{
	return securityToken_;
}

void DeleteIpControlRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

