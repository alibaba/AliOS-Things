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

#include <alibabacloud/aliyuncvc/model/InviteUserRequest.h>

using AlibabaCloud::Aliyuncvc::Model::InviteUserRequest;

InviteUserRequest::InviteUserRequest() :
	RpcServiceRequest("aliyuncvc", "2019-10-30", "InviteUser")
{
	setMethod(HttpRequest::Method::Post);
}

InviteUserRequest::~InviteUserRequest()
{}

std::string InviteUserRequest::getMeetingUUID()const
{
	return meetingUUID_;
}

void InviteUserRequest::setMeetingUUID(const std::string& meetingUUID)
{
	meetingUUID_ = meetingUUID;
	setBodyParameter("MeetingUUID", meetingUUID);
}

std::string InviteUserRequest::getUserIds()const
{
	return userIds_;
}

void InviteUserRequest::setUserIds(const std::string& userIds)
{
	userIds_ = userIds;
	setBodyParameter("UserIds", userIds);
}

