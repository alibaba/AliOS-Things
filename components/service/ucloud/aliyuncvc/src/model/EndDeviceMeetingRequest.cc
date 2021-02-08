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

#include <alibabacloud/aliyuncvc/model/EndDeviceMeetingRequest.h>

using AlibabaCloud::Aliyuncvc::Model::EndDeviceMeetingRequest;

EndDeviceMeetingRequest::EndDeviceMeetingRequest() :
	RpcServiceRequest("aliyuncvc", "2019-10-30", "EndDeviceMeeting")
{
	setMethod(HttpRequest::Method::Post);
}

EndDeviceMeetingRequest::~EndDeviceMeetingRequest()
{}

std::string EndDeviceMeetingRequest::getMeetingUUID()const
{
	return meetingUUID_;
}

void EndDeviceMeetingRequest::setMeetingUUID(const std::string& meetingUUID)
{
	meetingUUID_ = meetingUUID;
	setBodyParameter("MeetingUUID", meetingUUID);
}

std::string EndDeviceMeetingRequest::getSN()const
{
	return sN_;
}

void EndDeviceMeetingRequest::setSN(const std::string& sN)
{
	sN_ = sN;
	setBodyParameter("SN", sN);
}

