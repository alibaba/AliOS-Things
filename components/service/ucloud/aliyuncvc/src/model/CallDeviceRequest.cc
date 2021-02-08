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

#include <alibabacloud/aliyuncvc/model/CallDeviceRequest.h>

using AlibabaCloud::Aliyuncvc::Model::CallDeviceRequest;

CallDeviceRequest::CallDeviceRequest() :
	RpcServiceRequest("aliyuncvc", "2019-10-30", "CallDevice")
{
	setMethod(HttpRequest::Method::Post);
}

CallDeviceRequest::~CallDeviceRequest()
{}

std::string CallDeviceRequest::getInviteName()const
{
	return inviteName_;
}

void CallDeviceRequest::setInviteName(const std::string& inviteName)
{
	inviteName_ = inviteName;
	setBodyParameter("InviteName", inviteName);
}

std::string CallDeviceRequest::getOperateUserId()const
{
	return operateUserId_;
}

void CallDeviceRequest::setOperateUserId(const std::string& operateUserId)
{
	operateUserId_ = operateUserId;
	setBodyParameter("OperateUserId", operateUserId);
}

bool CallDeviceRequest::getJoinMeetingFlag()const
{
	return joinMeetingFlag_;
}

void CallDeviceRequest::setJoinMeetingFlag(bool joinMeetingFlag)
{
	joinMeetingFlag_ = joinMeetingFlag;
	setParameter("JoinMeetingFlag", joinMeetingFlag ? "true" : "false");
}

std::string CallDeviceRequest::getMeetingCode()const
{
	return meetingCode_;
}

void CallDeviceRequest::setMeetingCode(const std::string& meetingCode)
{
	meetingCode_ = meetingCode;
	setBodyParameter("MeetingCode", meetingCode);
}

std::string CallDeviceRequest::getSN()const
{
	return sN_;
}

void CallDeviceRequest::setSN(const std::string& sN)
{
	sN_ = sN;
	setBodyParameter("SN", sN);
}

