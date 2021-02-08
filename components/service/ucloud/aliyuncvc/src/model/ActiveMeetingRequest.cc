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

#include <alibabacloud/aliyuncvc/model/ActiveMeetingRequest.h>

using AlibabaCloud::Aliyuncvc::Model::ActiveMeetingRequest;

ActiveMeetingRequest::ActiveMeetingRequest() :
	RpcServiceRequest("aliyuncvc", "2019-10-30", "ActiveMeeting")
{
	setMethod(HttpRequest::Method::Post);
}

ActiveMeetingRequest::~ActiveMeetingRequest()
{}

std::string ActiveMeetingRequest::getMeetingUUID()const
{
	return meetingUUID_;
}

void ActiveMeetingRequest::setMeetingUUID(const std::string& meetingUUID)
{
	meetingUUID_ = meetingUUID;
	setParameter("MeetingUUID", meetingUUID);
}

std::string ActiveMeetingRequest::getMeetingCode()const
{
	return meetingCode_;
}

void ActiveMeetingRequest::setMeetingCode(const std::string& meetingCode)
{
	meetingCode_ = meetingCode;
	setParameter("MeetingCode", meetingCode);
}

