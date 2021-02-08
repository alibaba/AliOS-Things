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

#include <alibabacloud/aliyuncvc/model/JoinDeviceMeetingRequest.h>

using AlibabaCloud::Aliyuncvc::Model::JoinDeviceMeetingRequest;

JoinDeviceMeetingRequest::JoinDeviceMeetingRequest() :
	RpcServiceRequest("aliyuncvc", "2019-10-30", "JoinDeviceMeeting")
{
	setMethod(HttpRequest::Method::Post);
}

JoinDeviceMeetingRequest::~JoinDeviceMeetingRequest()
{}

std::string JoinDeviceMeetingRequest::getToken()const
{
	return token_;
}

void JoinDeviceMeetingRequest::setToken(const std::string& token)
{
	token_ = token;
	setBodyParameter("Token", token);
}

std::string JoinDeviceMeetingRequest::getPassword()const
{
	return password_;
}

void JoinDeviceMeetingRequest::setPassword(const std::string& password)
{
	password_ = password;
	setBodyParameter("Password", password);
}

std::string JoinDeviceMeetingRequest::getMeetingCode()const
{
	return meetingCode_;
}

void JoinDeviceMeetingRequest::setMeetingCode(const std::string& meetingCode)
{
	meetingCode_ = meetingCode;
	setBodyParameter("MeetingCode", meetingCode);
}

std::string JoinDeviceMeetingRequest::getSN()const
{
	return sN_;
}

void JoinDeviceMeetingRequest::setSN(const std::string& sN)
{
	sN_ = sN;
	setBodyParameter("SN", sN);
}

