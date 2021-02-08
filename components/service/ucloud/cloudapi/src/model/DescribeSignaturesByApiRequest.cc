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

#include <alibabacloud/cloudapi/model/DescribeSignaturesByApiRequest.h>

using AlibabaCloud::CloudAPI::Model::DescribeSignaturesByApiRequest;

DescribeSignaturesByApiRequest::DescribeSignaturesByApiRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DescribeSignaturesByApi")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeSignaturesByApiRequest::~DescribeSignaturesByApiRequest()
{}

std::string DescribeSignaturesByApiRequest::getStageName()const
{
	return stageName_;
}

void DescribeSignaturesByApiRequest::setStageName(const std::string& stageName)
{
	stageName_ = stageName;
	setParameter("StageName", stageName);
}

std::string DescribeSignaturesByApiRequest::getGroupId()const
{
	return groupId_;
}

void DescribeSignaturesByApiRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string DescribeSignaturesByApiRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DescribeSignaturesByApiRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DescribeSignaturesByApiRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeSignaturesByApiRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string DescribeSignaturesByApiRequest::getApiId()const
{
	return apiId_;
}

void DescribeSignaturesByApiRequest::setApiId(const std::string& apiId)
{
	apiId_ = apiId;
	setParameter("ApiId", apiId);
}

