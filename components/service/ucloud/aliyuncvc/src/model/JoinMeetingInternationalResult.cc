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

#include <alibabacloud/aliyuncvc/model/JoinMeetingInternationalResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Aliyuncvc;
using namespace AlibabaCloud::Aliyuncvc::Model;

JoinMeetingInternationalResult::JoinMeetingInternationalResult() :
	ServiceResult()
{}

JoinMeetingInternationalResult::JoinMeetingInternationalResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

JoinMeetingInternationalResult::~JoinMeetingInternationalResult()
{}

void JoinMeetingInternationalResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto meetingInfoNode = value["MeetingInfo"];
	if(!meetingInfoNode["MeetingDomain"].isNull())
		meetingInfo_.meetingDomain = meetingInfoNode["MeetingDomain"].asString();
	if(!meetingInfoNode["MeetingToken"].isNull())
		meetingInfo_.meetingToken = meetingInfoNode["MeetingToken"].asString();
	if(!meetingInfoNode["MeetingCode"].isNull())
		meetingInfo_.meetingCode = meetingInfoNode["MeetingCode"].asString();
	if(!meetingInfoNode["MemberUUID"].isNull())
		meetingInfo_.memberUUID = meetingInfoNode["MemberUUID"].asString();
	if(!meetingInfoNode["ClientAppId"].isNull())
		meetingInfo_.clientAppId = meetingInfoNode["ClientAppId"].asString();
	if(!meetingInfoNode["MeetingUUID"].isNull())
		meetingInfo_.meetingUUID = meetingInfoNode["MeetingUUID"].asString();
	if(!meetingInfoNode["MeetingAppId"].isNull())
		meetingInfo_.meetingAppId = meetingInfoNode["MeetingAppId"].asString();
	auto slsInfoNode = meetingInfoNode["SlsInfo"];
	if(!slsInfoNode["LogServiceEndpoint"].isNull())
		meetingInfo_.slsInfo.logServiceEndpoint = slsInfoNode["LogServiceEndpoint"].asString();
	if(!slsInfoNode["Logstore"].isNull())
		meetingInfo_.slsInfo.logstore = slsInfoNode["Logstore"].asString();
	if(!slsInfoNode["Project"].isNull())
		meetingInfo_.slsInfo.project = slsInfoNode["Project"].asString();
	if(!value["ErrorCode"].isNull())
		errorCode_ = std::stoi(value["ErrorCode"].asString());
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();

}

JoinMeetingInternationalResult::MeetingInfo JoinMeetingInternationalResult::getMeetingInfo()const
{
	return meetingInfo_;
}

std::string JoinMeetingInternationalResult::getMessage()const
{
	return message_;
}

int JoinMeetingInternationalResult::getErrorCode()const
{
	return errorCode_;
}

bool JoinMeetingInternationalResult::getSuccess()const
{
	return success_;
}

