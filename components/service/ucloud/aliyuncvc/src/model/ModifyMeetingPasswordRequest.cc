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

#include <alibabacloud/aliyuncvc/model/ModifyMeetingPasswordRequest.h>

using AlibabaCloud::Aliyuncvc::Model::ModifyMeetingPasswordRequest;

ModifyMeetingPasswordRequest::ModifyMeetingPasswordRequest() :
	RpcServiceRequest("aliyuncvc", "2019-10-30", "ModifyMeetingPassword")
{
	setMethod(HttpRequest::Method::Post);
}

ModifyMeetingPasswordRequest::~ModifyMeetingPasswordRequest()
{}

std::string ModifyMeetingPasswordRequest::getUserId()const
{
	return userId_;
}

void ModifyMeetingPasswordRequest::setUserId(const std::string& userId)
{
	userId_ = userId;
	setBodyParameter("UserId", userId);
}

bool ModifyMeetingPasswordRequest::getOpenPasswordFlag()const
{
	return openPasswordFlag_;
}

void ModifyMeetingPasswordRequest::setOpenPasswordFlag(bool openPasswordFlag)
{
	openPasswordFlag_ = openPasswordFlag;
	setBodyParameter("OpenPasswordFlag", openPasswordFlag ? "true" : "false");
}

std::string ModifyMeetingPasswordRequest::getMeetingUUID()const
{
	return meetingUUID_;
}

void ModifyMeetingPasswordRequest::setMeetingUUID(const std::string& meetingUUID)
{
	meetingUUID_ = meetingUUID;
	setBodyParameter("MeetingUUID", meetingUUID);
}

std::string ModifyMeetingPasswordRequest::getPassword()const
{
	return password_;
}

void ModifyMeetingPasswordRequest::setPassword(const std::string& password)
{
	password_ = password;
	setBodyParameter("Password", password);
}

