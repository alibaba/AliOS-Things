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

#include <alibabacloud/rtc/model/CreateConferenceRequest.h>

using AlibabaCloud::Rtc::Model::CreateConferenceRequest;

CreateConferenceRequest::CreateConferenceRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "CreateConference")
{
	setMethod(HttpRequest::Method::Post);
}

CreateConferenceRequest::~CreateConferenceRequest()
{}

std::string CreateConferenceRequest::getClientToken()const
{
	return clientToken_;
}

void CreateConferenceRequest::setClientToken(const std::string& clientToken)
{
	clientToken_ = clientToken;
	setParameter("ClientToken", clientToken);
}

std::string CreateConferenceRequest::getStartTime()const
{
	return startTime_;
}

void CreateConferenceRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string CreateConferenceRequest::getType()const
{
	return type_;
}

void CreateConferenceRequest::setType(const std::string& type)
{
	type_ = type;
	setParameter("Type", type);
}

std::string CreateConferenceRequest::getConferenceName()const
{
	return conferenceName_;
}

void CreateConferenceRequest::setConferenceName(const std::string& conferenceName)
{
	conferenceName_ = conferenceName;
	setParameter("ConferenceName", conferenceName);
}

std::string CreateConferenceRequest::getShowLog()const
{
	return showLog_;
}

void CreateConferenceRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

long CreateConferenceRequest::getOwnerId()const
{
	return ownerId_;
}

void CreateConferenceRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string CreateConferenceRequest::getAppId()const
{
	return appId_;
}

void CreateConferenceRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

int CreateConferenceRequest::getRemindNotice()const
{
	return remindNotice_;
}

void CreateConferenceRequest::setRemindNotice(int remindNotice)
{
	remindNotice_ = remindNotice;
	setParameter("RemindNotice", std::to_string(remindNotice));
}

