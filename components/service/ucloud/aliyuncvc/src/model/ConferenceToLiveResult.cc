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

#include <alibabacloud/aliyuncvc/model/ConferenceToLiveResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Aliyuncvc;
using namespace AlibabaCloud::Aliyuncvc::Model;

ConferenceToLiveResult::ConferenceToLiveResult() :
	ServiceResult()
{}

ConferenceToLiveResult::ConferenceToLiveResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ConferenceToLiveResult::~ConferenceToLiveResult()
{}

void ConferenceToLiveResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto liveInfoNode = value["LiveInfo"];
	if(!liveInfoNode["LiveUUID"].isNull())
		liveInfo_.liveUUID = liveInfoNode["LiveUUID"].asString();
	if(!value["ErrorCode"].isNull())
		errorCode_ = std::stoi(value["ErrorCode"].asString());
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";

}

std::string ConferenceToLiveResult::getMessage()const
{
	return message_;
}

int ConferenceToLiveResult::getErrorCode()const
{
	return errorCode_;
}

bool ConferenceToLiveResult::getSuccess()const
{
	return success_;
}

ConferenceToLiveResult::LiveInfo ConferenceToLiveResult::getLiveInfo()const
{
	return liveInfo_;
}

