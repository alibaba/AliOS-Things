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

#include <alibabacloud/aliyuncvc/model/JoinDeviceMeetingResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Aliyuncvc;
using namespace AlibabaCloud::Aliyuncvc::Model;

JoinDeviceMeetingResult::JoinDeviceMeetingResult() :
	ServiceResult()
{}

JoinDeviceMeetingResult::JoinDeviceMeetingResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

JoinDeviceMeetingResult::~JoinDeviceMeetingResult()
{}

void JoinDeviceMeetingResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto deviceNode = value["Device"];
	if(!deviceNode["MeetingDomain"].isNull())
		device_.meetingDomain = deviceNode["MeetingDomain"].asString();
	if(!deviceNode["MeetingToken"].isNull())
		device_.meetingToken = deviceNode["MeetingToken"].asString();
	if(!deviceNode["MeetingCode"].isNull())
		device_.meetingCode = deviceNode["MeetingCode"].asString();
	if(!deviceNode["MemberUUID"].isNull())
		device_.memberUUID = deviceNode["MemberUUID"].asString();
	if(!deviceNode["ClientAppId"].isNull())
		device_.clientAppId = deviceNode["ClientAppId"].asString();
	if(!deviceNode["MeetingUUID"].isNull())
		device_.meetingUUID = deviceNode["MeetingUUID"].asString();
	if(!deviceNode["MeetingAppId"].isNull())
		device_.meetingAppId = deviceNode["MeetingAppId"].asString();
	auto slsInfoNode = deviceNode["SlsInfo"];
	if(!slsInfoNode["LogServiceEndpoint"].isNull())
		device_.slsInfo.logServiceEndpoint = slsInfoNode["LogServiceEndpoint"].asString();
	if(!slsInfoNode["Project"].isNull())
		device_.slsInfo.project = slsInfoNode["Project"].asString();
	if(!slsInfoNode["Logstore"].isNull())
		device_.slsInfo.logstore = slsInfoNode["Logstore"].asString();
	if(!value["ErrorCode"].isNull())
		errorCode_ = std::stoi(value["ErrorCode"].asString());
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";

}

std::string JoinDeviceMeetingResult::getMessage()const
{
	return message_;
}

JoinDeviceMeetingResult::Device JoinDeviceMeetingResult::getDevice()const
{
	return device_;
}

int JoinDeviceMeetingResult::getErrorCode()const
{
	return errorCode_;
}

bool JoinDeviceMeetingResult::getSuccess()const
{
	return success_;
}

