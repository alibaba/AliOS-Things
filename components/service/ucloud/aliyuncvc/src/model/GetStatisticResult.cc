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

#include <alibabacloud/aliyuncvc/model/GetStatisticResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Aliyuncvc;
using namespace AlibabaCloud::Aliyuncvc::Model;

GetStatisticResult::GetStatisticResult() :
	ServiceResult()
{}

GetStatisticResult::GetStatisticResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetStatisticResult::~GetStatisticResult()
{}

void GetStatisticResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto statisticInfoNode = value["StatisticInfo"];
	if(!statisticInfoNode["MeetingDuration"].isNull())
		statisticInfo_.meetingDuration = std::stol(statisticInfoNode["MeetingDuration"].asString());
	if(!statisticInfoNode["MeetingNumber"].isNull())
		statisticInfo_.meetingNumber = std::stol(statisticInfoNode["MeetingNumber"].asString());
	if(!statisticInfoNode["MemberNumber"].isNull())
		statisticInfo_.memberNumber = std::stol(statisticInfoNode["MemberNumber"].asString());
	if(!statisticInfoNode["MaxConcurrency"].isNull())
		statisticInfo_.maxConcurrency = std::stol(statisticInfoNode["MaxConcurrency"].asString());
	if(!value["ErrorCode"].isNull())
		errorCode_ = std::stoi(value["ErrorCode"].asString());
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";

}

std::string GetStatisticResult::getMessage()const
{
	return message_;
}

GetStatisticResult::StatisticInfo GetStatisticResult::getStatisticInfo()const
{
	return statisticInfo_;
}

int GetStatisticResult::getErrorCode()const
{
	return errorCode_;
}

bool GetStatisticResult::getSuccess()const
{
	return success_;
}

