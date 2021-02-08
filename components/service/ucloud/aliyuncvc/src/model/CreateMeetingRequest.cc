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

#include <alibabacloud/aliyuncvc/model/CreateMeetingRequest.h>

using AlibabaCloud::Aliyuncvc::Model::CreateMeetingRequest;

CreateMeetingRequest::CreateMeetingRequest() :
	RpcServiceRequest("aliyuncvc", "2019-10-30", "CreateMeeting")
{
	setMethod(HttpRequest::Method::Post);
}

CreateMeetingRequest::~CreateMeetingRequest()
{}

std::string CreateMeetingRequest::getMeetingName()const
{
	return meetingName_;
}

void CreateMeetingRequest::setMeetingName(const std::string& meetingName)
{
	meetingName_ = meetingName;
	setBodyParameter("MeetingName", meetingName);
}

std::string CreateMeetingRequest::getUserId()const
{
	return userId_;
}

void CreateMeetingRequest::setUserId(const std::string& userId)
{
	userId_ = userId;
	setBodyParameter("UserId", userId);
}

bool CreateMeetingRequest::getOpenPasswordFlag()const
{
	return openPasswordFlag_;
}

void CreateMeetingRequest::setOpenPasswordFlag(bool openPasswordFlag)
{
	openPasswordFlag_ = openPasswordFlag;
	setBodyParameter("OpenPasswordFlag", openPasswordFlag ? "true" : "false");
}

std::string CreateMeetingRequest::getPassword()const
{
	return password_;
}

void CreateMeetingRequest::setPassword(const std::string& password)
{
	password_ = password;
	setBodyParameter("Password", password);
}

bool CreateMeetingRequest::getMasterEnableFlag()const
{
	return masterEnableFlag_;
}

void CreateMeetingRequest::setMasterEnableFlag(bool masterEnableFlag)
{
	masterEnableFlag_ = masterEnableFlag;
	setBodyParameter("MasterEnableFlag", masterEnableFlag ? "true" : "false");
}

std::string CreateMeetingRequest::getMeetingMode()const
{
	return meetingMode_;
}

void CreateMeetingRequest::setMeetingMode(const std::string& meetingMode)
{
	meetingMode_ = meetingMode;
	setBodyParameter("MeetingMode", meetingMode);
}

