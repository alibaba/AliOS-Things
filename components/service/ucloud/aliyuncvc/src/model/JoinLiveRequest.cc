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

#include <alibabacloud/aliyuncvc/model/JoinLiveRequest.h>

using AlibabaCloud::Aliyuncvc::Model::JoinLiveRequest;

JoinLiveRequest::JoinLiveRequest() :
	RpcServiceRequest("aliyuncvc", "2019-10-30", "JoinLive")
{
	setMethod(HttpRequest::Method::Post);
}

JoinLiveRequest::~JoinLiveRequest()
{}

std::string JoinLiveRequest::getLiveUUID()const
{
	return liveUUID_;
}

void JoinLiveRequest::setLiveUUID(const std::string& liveUUID)
{
	liveUUID_ = liveUUID;
	setBodyParameter("LiveUUID", liveUUID);
}

std::string JoinLiveRequest::getUserId()const
{
	return userId_;
}

void JoinLiveRequest::setUserId(const std::string& userId)
{
	userId_ = userId;
	setBodyParameter("UserId", userId);
}

std::string JoinLiveRequest::getPassword()const
{
	return password_;
}

void JoinLiveRequest::setPassword(const std::string& password)
{
	password_ = password;
	setBodyParameter("Password", password);
}

