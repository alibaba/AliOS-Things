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

#include <alibabacloud/imm/model/RefreshOfficePreviewTokenRequest.h>

using AlibabaCloud::Imm::Model::RefreshOfficePreviewTokenRequest;

RefreshOfficePreviewTokenRequest::RefreshOfficePreviewTokenRequest() :
	RpcServiceRequest("imm", "2017-09-06", "RefreshOfficePreviewToken")
{
	setMethod(HttpRequest::Method::Post);
}

RefreshOfficePreviewTokenRequest::~RefreshOfficePreviewTokenRequest()
{}

std::string RefreshOfficePreviewTokenRequest::getProject()const
{
	return project_;
}

void RefreshOfficePreviewTokenRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string RefreshOfficePreviewTokenRequest::getAccessToken()const
{
	return accessToken_;
}

void RefreshOfficePreviewTokenRequest::setAccessToken(const std::string& accessToken)
{
	accessToken_ = accessToken;
	setParameter("AccessToken", accessToken);
}

std::string RefreshOfficePreviewTokenRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void RefreshOfficePreviewTokenRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string RefreshOfficePreviewTokenRequest::getRefreshToken()const
{
	return refreshToken_;
}

void RefreshOfficePreviewTokenRequest::setRefreshToken(const std::string& refreshToken)
{
	refreshToken_ = refreshToken;
	setParameter("RefreshToken", refreshToken);
}

