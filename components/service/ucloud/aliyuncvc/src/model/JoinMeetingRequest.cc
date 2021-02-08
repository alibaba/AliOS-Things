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

#include <alibabacloud/aliyuncvc/model/JoinMeetingRequest.h>

using AlibabaCloud::Aliyuncvc::Model::JoinMeetingRequest;

JoinMeetingRequest::JoinMeetingRequest() :
	RpcServiceRequest("aliyuncvc", "2019-10-30", "JoinMeeting")
{
	setMethod(HttpRequest::Method::Post);
}

JoinMeetingRequest::~JoinMeetingRequest()
{}

std::string JoinMeetingRequest::getUserId()const
{
	return userId_;
}

void JoinMeetingRequest::setUserId(const std::string& userId)
{
	userId_ = userId;
	setBodyParameter("UserId", userId);
}

std::string JoinMeetingRequest::getPassword()const
{
	return password_;
}

void JoinMeetingRequest::setPassword(const std::string& password)
{
	password_ = password;
	setBodyParameter("Password", password);
}

std::string JoinMeetingRequest::getMeetingCode()const
{
	return meetingCode_;
}

void JoinMeetingRequest::setMeetingCode(const std::string& meetingCode)
{
	meetingCode_ = meetingCode;
	setBodyParameter("MeetingCode", meetingCode);
}

