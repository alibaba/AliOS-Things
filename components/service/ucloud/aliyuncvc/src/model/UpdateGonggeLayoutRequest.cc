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

#include <alibabacloud/aliyuncvc/model/UpdateGonggeLayoutRequest.h>

using AlibabaCloud::Aliyuncvc::Model::UpdateGonggeLayoutRequest;

UpdateGonggeLayoutRequest::UpdateGonggeLayoutRequest() :
	RpcServiceRequest("aliyuncvc", "2019-10-30", "UpdateGonggeLayout")
{
	setMethod(HttpRequest::Method::Post);
}

UpdateGonggeLayoutRequest::~UpdateGonggeLayoutRequest()
{}

std::string UpdateGonggeLayoutRequest::getMeetingUUID()const
{
	return meetingUUID_;
}

void UpdateGonggeLayoutRequest::setMeetingUUID(const std::string& meetingUUID)
{
	meetingUUID_ = meetingUUID;
	setBodyParameter("MeetingUUID", meetingUUID);
}

std::string UpdateGonggeLayoutRequest::getVideoCount()const
{
	return videoCount_;
}

void UpdateGonggeLayoutRequest::setVideoCount(const std::string& videoCount)
{
	videoCount_ = videoCount;
	setBodyParameter("VideoCount", videoCount);
}

std::string UpdateGonggeLayoutRequest::getValue()const
{
	return value_;
}

void UpdateGonggeLayoutRequest::setValue(const std::string& value)
{
	value_ = value;
	setBodyParameter("Value", value);
}

