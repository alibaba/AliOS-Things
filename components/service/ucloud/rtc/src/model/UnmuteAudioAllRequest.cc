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

#include <alibabacloud/rtc/model/UnmuteAudioAllRequest.h>

using AlibabaCloud::Rtc::Model::UnmuteAudioAllRequest;

UnmuteAudioAllRequest::UnmuteAudioAllRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "UnmuteAudioAll")
{
	setMethod(HttpRequest::Method::Post);
}

UnmuteAudioAllRequest::~UnmuteAudioAllRequest()
{}

std::string UnmuteAudioAllRequest::getConferenceId()const
{
	return conferenceId_;
}

void UnmuteAudioAllRequest::setConferenceId(const std::string& conferenceId)
{
	conferenceId_ = conferenceId;
	setParameter("ConferenceId", conferenceId);
}

std::string UnmuteAudioAllRequest::getShowLog()const
{
	return showLog_;
}

void UnmuteAudioAllRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

long UnmuteAudioAllRequest::getOwnerId()const
{
	return ownerId_;
}

void UnmuteAudioAllRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string UnmuteAudioAllRequest::getParticipantId()const
{
	return participantId_;
}

void UnmuteAudioAllRequest::setParticipantId(const std::string& participantId)
{
	participantId_ = participantId;
	setParameter("ParticipantId", participantId);
}

std::string UnmuteAudioAllRequest::getAppId()const
{
	return appId_;
}

void UnmuteAudioAllRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

