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

#include <alibabacloud/aliyuncvc/model/ListIsvStatisticsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Aliyuncvc;
using namespace AlibabaCloud::Aliyuncvc::Model;

ListIsvStatisticsResult::ListIsvStatisticsResult() :
	ServiceResult()
{}

ListIsvStatisticsResult::ListIsvStatisticsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListIsvStatisticsResult::~ListIsvStatisticsResult()
{}

void ListIsvStatisticsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto allStatisticsNode = dataNode["Statistics"]["Statistic"];
	for (auto dataNodeStatisticsStatistic : allStatisticsNode)
	{
		Data::Statistic statisticObject;
		if(!dataNodeStatisticsStatistic["MeetingNumber"].isNull())
			statisticObject.meetingNumber = dataNodeStatisticsStatistic["MeetingNumber"].asString();
		if(!dataNodeStatisticsStatistic["MeetingLength"].isNull())
			statisticObject.meetingLength = dataNodeStatisticsStatistic["MeetingLength"].asString();
		if(!dataNodeStatisticsStatistic["MemberNumber"].isNull())
			statisticObject.memberNumber = dataNodeStatisticsStatistic["MemberNumber"].asString();
		if(!dataNodeStatisticsStatistic["Day"].isNull())
			statisticObject.day = dataNodeStatisticsStatistic["Day"].asString();
		data_.statistics.push_back(statisticObject);
	}
	auto totalNode = dataNode["Total"];
	if(!totalNode["MeetingNumber"].isNull())
		data_.total.meetingNumber = std::stoi(totalNode["MeetingNumber"].asString());
	if(!totalNode["MeetingLength"].isNull())
		data_.total.meetingLength = std::stoi(totalNode["MeetingLength"].asString());
	if(!totalNode["MemberNumber"].isNull())
		data_.total.memberNumber = std::stoi(totalNode["MemberNumber"].asString());
	if(!value["ErrorCode"].isNull())
		errorCode_ = std::stoi(value["ErrorCode"].asString());
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";

}

std::string ListIsvStatisticsResult::getMessage()const
{
	return message_;
}

ListIsvStatisticsResult::Data ListIsvStatisticsResult::getData()const
{
	return data_;
}

int ListIsvStatisticsResult::getErrorCode()const
{
	return errorCode_;
}

bool ListIsvStatisticsResult::getSuccess()const
{
	return success_;
}

