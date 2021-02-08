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

#include <alibabacloud/rtc/model/ModifyConferenceRequest.h>

using AlibabaCloud::Rtc::Model::ModifyConferenceRequest;

ModifyConferenceRequest::ModifyConferenceRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "ModifyConference")
{
	setMethod(HttpRequest::Method::Post);
}

ModifyConferenceRequest::~ModifyConferenceRequest()
{}

std::string ModifyConferenceRequest::getStartTime()const
{
	return startTime_;
}

void ModifyConferenceRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string ModifyConferenceRequest::getType()const
{
	return type_;
}

void ModifyConferenceRequest::setType(const std::string& type)
{
	type_ = type;
	setParameter("Type", type);
}

std::string ModifyConferenceRequest::getConferenceId()const
{
	return conferenceId_;
}

void ModifyConferenceRequest::setConferenceId(const std::string& conferenceId)
{
	conferenceId_ = conferenceId;
	setParameter("ConferenceId", conferenceId);
}

std::string ModifyConferenceRequest::getConferenceName()const
{
	return conferenceName_;
}

void ModifyConferenceRequest::setConferenceName(const std::string& conferenceName)
{
	conferenceName_ = conferenceName;
	setParameter("ConferenceName", conferenceName);
}

std::string ModifyConferenceRequest::getShowLog()const
{
	return showLog_;
}

void ModifyConferenceRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

long ModifyConferenceRequest::getOwnerId()const
{
	return ownerId_;
}

void ModifyConferenceRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string ModifyConferenceRequest::getAppId()const
{
	return appId_;
}

void ModifyConferenceRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

int ModifyConferenceRequest::getRemindNotice()const
{
	return remindNotice_;
}

void ModifyConferenceRequest::setRemindNotice(int remindNotice)
{
	remindNotice_ = remindNotice;
	setParameter("RemindNotice", std::to_string(remindNotice));
}

