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

#include <alibabacloud/imm/model/RefreshOfficeEditTokenRequest.h>

using AlibabaCloud::Imm::Model::RefreshOfficeEditTokenRequest;

RefreshOfficeEditTokenRequest::RefreshOfficeEditTokenRequest() :
	RpcServiceRequest("imm", "2017-09-06", "RefreshOfficeEditToken")
{
	setMethod(HttpRequest::Method::Post);
}

RefreshOfficeEditTokenRequest::~RefreshOfficeEditTokenRequest()
{}

std::string RefreshOfficeEditTokenRequest::getProject()const
{
	return project_;
}

void RefreshOfficeEditTokenRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string RefreshOfficeEditTokenRequest::getAccessToken()const
{
	return accessToken_;
}

void RefreshOfficeEditTokenRequest::setAccessToken(const std::string& accessToken)
{
	accessToken_ = accessToken;
	setParameter("AccessToken", accessToken);
}

std::string RefreshOfficeEditTokenRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void RefreshOfficeEditTokenRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string RefreshOfficeEditTokenRequest::getRefreshToken()const
{
	return refreshToken_;
}

void RefreshOfficeEditTokenRequest::setRefreshToken(const std::string& refreshToken)
{
	refreshToken_ = refreshToken;
	setParameter("RefreshToken", refreshToken);
}

