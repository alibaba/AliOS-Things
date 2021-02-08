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

#include <alibabacloud/aliyuncvc/model/CreateLiveRequest.h>

using AlibabaCloud::Aliyuncvc::Model::CreateLiveRequest;

CreateLiveRequest::CreateLiveRequest() :
	RpcServiceRequest("aliyuncvc", "2019-10-30", "CreateLive")
{
	setMethod(HttpRequest::Method::Post);
}

CreateLiveRequest::~CreateLiveRequest()
{}

std::string CreateLiveRequest::getUserId()const
{
	return userId_;
}

void CreateLiveRequest::setUserId(const std::string& userId)
{
	userId_ = userId;
	setBodyParameter("UserId", userId);
}

bool CreateLiveRequest::getOpenPasswordFlag()const
{
	return openPasswordFlag_;
}

void CreateLiveRequest::setOpenPasswordFlag(bool openPasswordFlag)
{
	openPasswordFlag_ = openPasswordFlag;
	setBodyParameter("OpenPasswordFlag", openPasswordFlag ? "true" : "false");
}

std::string CreateLiveRequest::getPassword()const
{
	return password_;
}

void CreateLiveRequest::setPassword(const std::string& password)
{
	password_ = password;
	setBodyParameter("Password", password);
}

std::string CreateLiveRequest::getLiveName()const
{
	return liveName_;
}

void CreateLiveRequest::setLiveName(const std::string& liveName)
{
	liveName_ = liveName;
	setBodyParameter("LiveName", liveName);
}

