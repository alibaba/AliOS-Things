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

#include <alibabacloud/cloudapi/model/ResetAppCodeRequest.h>

using AlibabaCloud::CloudAPI::Model::ResetAppCodeRequest;

ResetAppCodeRequest::ResetAppCodeRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "ResetAppCode")
{
	setMethod(HttpRequest::Method::Post);
}

ResetAppCodeRequest::~ResetAppCodeRequest()
{}

std::string ResetAppCodeRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void ResetAppCodeRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string ResetAppCodeRequest::getSecurityToken()const
{
	return securityToken_;
}

void ResetAppCodeRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string ResetAppCodeRequest::getAppCode()const
{
	return appCode_;
}

void ResetAppCodeRequest::setAppCode(const std::string& appCode)
{
	appCode_ = appCode;
	setParameter("AppCode", appCode);
}

