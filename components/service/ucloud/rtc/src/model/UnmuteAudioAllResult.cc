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

#include <alibabacloud/rtc/model/UnmuteAudioAllResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Rtc;
using namespace AlibabaCloud::Rtc::Model;

UnmuteAudioAllResult::UnmuteAudioAllResult() :
	ServiceResult()
{}

UnmuteAudioAllResult::UnmuteAudioAllResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

UnmuteAudioAllResult::~UnmuteAudioAllResult()
{}

void UnmuteAudioAllResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allParticipantsNode = value["Participants"]["Participant"];
	for (auto valueParticipantsParticipant : allParticipantsNode)
	{
		Participant participantsObject;
		if(!valueParticipantsParticipant["Id"].isNull())
			participantsObject.id = valueParticipantsParticipant["Id"].asString();
		if(!valueParticipantsParticipant["Code"].isNull())
			participantsObject.code = valueParticipantsParticipant["Code"].asString();
		if(!valueParticipantsParticipant["Message"].isNull())
			participantsObject.message = valueParticipantsParticipant["Message"].asString();
		participants_.push_back(participantsObject);
	}
	if(!value["ConferenceId"].isNull())
		conferenceId_ = value["ConferenceId"].asString();

}

std::string UnmuteAudioAllResult::getConferenceId()const
{
	return conferenceId_;
}

std::vector<UnmuteAudioAllResult::Participant> UnmuteAudioAllResult::getParticipants()const
{
	return participants_;
}

