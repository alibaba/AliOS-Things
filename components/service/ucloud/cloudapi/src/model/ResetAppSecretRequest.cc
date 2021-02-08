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

#include <alibabacloud/cloudapi/model/ResetAppSecretRequest.h>

using AlibabaCloud::CloudAPI::Model::ResetAppSecretRequest;

ResetAppSecretRequest::ResetAppSecretRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "ResetAppSecret")
{
	setMethod(HttpRequest::Method::Post);
}

ResetAppSecretRequest::~ResetAppSecretRequest()
{}

std::string ResetAppSecretRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void ResetAppSecretRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string ResetAppSecretRequest::getSecurityToken()const
{
	return securityToken_;
}

void ResetAppSecretRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string ResetAppSecretRequest::getAppKey()const
{
	return appKey_;
}

void ResetAppSecretRequest::setAppKey(const std::string& appKey)
{
	appKey_ = appKey;
	setParameter("AppKey", appKey);
}

