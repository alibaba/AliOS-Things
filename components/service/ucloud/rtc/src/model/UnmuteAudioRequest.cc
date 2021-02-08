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

#include <alibabacloud/rtc/model/UnmuteAudioRequest.h>

using AlibabaCloud::Rtc::Model::UnmuteAudioRequest;

UnmuteAudioRequest::UnmuteAudioRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "UnmuteAudio")
{
	setMethod(HttpRequest::Method::Post);
}

UnmuteAudioRequest::~UnmuteAudioRequest()
{}

std::vector<std::string> UnmuteAudioRequest::getParticipantIds()const
{
	return participantIds_;
}

void UnmuteAudioRequest::setParticipantIds(const std::vector<std::string>& participantIds)
{
	participantIds_ = participantIds;
	for(int dep1 = 0; dep1!= participantIds.size(); dep1++) {
		setParameter("ParticipantIds."+ std::to_string(dep1), participantIds.at(dep1));
	}
}

std::string UnmuteAudioRequest::getConferenceId()const
{
	return conferenceId_;
}

void UnmuteAudioRequest::setConferenceId(const std::string& conferenceId)
{
	conferenceId_ = conferenceId;
	setParameter("ConferenceId", conferenceId);
}

std::string UnmuteAudioRequest::getShowLog()const
{
	return showLog_;
}

void UnmuteAudioRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

long UnmuteAudioRequest::getOwnerId()const
{
	return ownerId_;
}

void UnmuteAudioRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string UnmuteAudioRequest::getAppId()const
{
	return appId_;
}

void UnmuteAudioRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

