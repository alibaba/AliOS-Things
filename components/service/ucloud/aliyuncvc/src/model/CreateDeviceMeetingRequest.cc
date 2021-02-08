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

#include <alibabacloud/aliyuncvc/model/CreateDeviceMeetingRequest.h>

using AlibabaCloud::Aliyuncvc::Model::CreateDeviceMeetingRequest;

CreateDeviceMeetingRequest::CreateDeviceMeetingRequest() :
	RpcServiceRequest("aliyuncvc", "2019-10-30", "CreateDeviceMeeting")
{
	setMethod(HttpRequest::Method::Post);
}

CreateDeviceMeetingRequest::~CreateDeviceMeetingRequest()
{}

std::string CreateDeviceMeetingRequest::getMeetingName()const
{
	return meetingName_;
}

void CreateDeviceMeetingRequest::setMeetingName(const std::string& meetingName)
{
	meetingName_ = meetingName;
	setBodyParameter("MeetingName", meetingName);
}

bool CreateDeviceMeetingRequest::getOpenPasswordtag()const
{
	return openPasswordtag_;
}

void CreateDeviceMeetingRequest::setOpenPasswordtag(bool openPasswordtag)
{
	openPasswordtag_ = openPasswordtag;
	setBodyParameter("OpenPasswordtag", openPasswordtag ? "true" : "false");
}

std::string CreateDeviceMeetingRequest::getToken()const
{
	return token_;
}

void CreateDeviceMeetingRequest::setToken(const std::string& token)
{
	token_ = token;
	setBodyParameter("Token", token);
}

std::string CreateDeviceMeetingRequest::getPassword()const
{
	return password_;
}

void CreateDeviceMeetingRequest::setPassword(const std::string& password)
{
	password_ = password;
	setBodyParameter("Password", password);
}

std::string CreateDeviceMeetingRequest::getSN()const
{
	return sN_;
}

void CreateDeviceMeetingRequest::setSN(const std::string& sN)
{
	sN_ = sN;
	setBodyParameter("SN", sN);
}

