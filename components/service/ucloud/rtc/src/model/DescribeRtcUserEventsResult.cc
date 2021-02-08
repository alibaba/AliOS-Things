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

#include <alibabacloud/rtc/model/DescribeRtcUserEventsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Rtc;
using namespace AlibabaCloud::Rtc::Model;

DescribeRtcUserEventsResult::DescribeRtcUserEventsResult() :
	ServiceResult()
{}

DescribeRtcUserEventsResult::DescribeRtcUserEventsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeRtcUserEventsResult::~DescribeRtcUserEventsResult()
{}

void DescribeRtcUserEventsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allEventsNode = value["Events"]["Event"];
	for (auto valueEventsEvent : allEventsNode)
	{
		Event eventsObject;
		if(!valueEventsEvent["EventId"].isNull())
			eventsObject.eventId = valueEventsEvent["EventId"].asString();
		if(!valueEventsEvent["EventTime"].isNull())
			eventsObject.eventTime = std::stol(valueEventsEvent["EventTime"].asString());
		if(!valueEventsEvent["Category"].isNull())
			eventsObject.category = valueEventsEvent["Category"].asString();
		events_.push_back(eventsObject);
	}

}

std::vector<DescribeRtcUserEventsResult::Event> DescribeRtcUserEventsResult::getEvents()const
{
	return events_;
}

