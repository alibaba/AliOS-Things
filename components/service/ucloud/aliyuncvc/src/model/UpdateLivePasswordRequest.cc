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

#include <alibabacloud/aliyuncvc/model/UpdateLivePasswordRequest.h>

using AlibabaCloud::Aliyuncvc::Model::UpdateLivePasswordRequest;

UpdateLivePasswordRequest::UpdateLivePasswordRequest() :
	RpcServiceRequest("aliyuncvc", "2019-10-30", "UpdateLivePassword")
{
	setMethod(HttpRequest::Method::Post);
}

UpdateLivePasswordRequest::~UpdateLivePasswordRequest()
{}

std::string UpdateLivePasswordRequest::getLiveUUID()const
{
	return liveUUID_;
}

void UpdateLivePasswordRequest::setLiveUUID(const std::string& liveUUID)
{
	liveUUID_ = liveUUID;
	setBodyParameter("LiveUUID", liveUUID);
}

std::string UpdateLivePasswordRequest::getUserId()const
{
	return userId_;
}

void UpdateLivePasswordRequest::setUserId(const std::string& userId)
{
	userId_ = userId;
	setBodyParameter("UserId", userId);
}

bool UpdateLivePasswordRequest::getOpenPasswordFlag()const
{
	return openPasswordFlag_;
}

void UpdateLivePasswordRequest::setOpenPasswordFlag(bool openPasswordFlag)
{
	openPasswordFlag_ = openPasswordFlag;
	setBodyParameter("OpenPasswordFlag", openPasswordFlag ? "true" : "false");
}

std::string UpdateLivePasswordRequest::getPassword()const
{
	return password_;
}

void UpdateLivePasswordRequest::setPassword(const std::string& password)
{
	password_ = password;
	setBodyParameter("Password", password);
}

