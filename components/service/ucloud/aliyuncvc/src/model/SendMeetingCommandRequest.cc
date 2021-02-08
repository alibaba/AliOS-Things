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

#include <alibabacloud/aliyuncvc/model/SendMeetingCommandRequest.h>

using AlibabaCloud::Aliyuncvc::Model::SendMeetingCommandRequest;

SendMeetingCommandRequest::SendMeetingCommandRequest() :
	RpcServiceRequest("aliyuncvc", "2019-10-30", "SendMeetingCommand")
{
	setMethod(HttpRequest::Method::Post);
}

SendMeetingCommandRequest::~SendMeetingCommandRequest()
{}

std::string SendMeetingCommandRequest::getOperatorMemberUUID()const
{
	return operatorMemberUUID_;
}

void SendMeetingCommandRequest::setOperatorMemberUUID(const std::string& operatorMemberUUID)
{
	operatorMemberUUID_ = operatorMemberUUID;
	setBodyParameter("OperatorMemberUUID", operatorMemberUUID);
}

std::string SendMeetingCommandRequest::getMemberUUID()const
{
	return memberUUID_;
}

void SendMeetingCommandRequest::setMemberUUID(const std::string& memberUUID)
{
	memberUUID_ = memberUUID;
	setParameter("MemberUUID", memberUUID);
}

int SendMeetingCommandRequest::getSendType()const
{
	return sendType_;
}

void SendMeetingCommandRequest::setSendType(int sendType)
{
	sendType_ = sendType;
	setBodyParameter("SendType", std::to_string(sendType));
}

std::string SendMeetingCommandRequest::getCommand()const
{
	return command_;
}

void SendMeetingCommandRequest::setCommand(const std::string& command)
{
	command_ = command;
	setBodyParameter("Command", command);
}

std::string SendMeetingCommandRequest::getMeetingUUID()const
{
	return meetingUUID_;
}

void SendMeetingCommandRequest::setMeetingUUID(const std::string& meetingUUID)
{
	meetingUUID_ = meetingUUID;
	setParameter("MeetingUUID", meetingUUID);
}

