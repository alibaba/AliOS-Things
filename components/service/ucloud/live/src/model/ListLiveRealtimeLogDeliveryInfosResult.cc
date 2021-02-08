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

#include <alibabacloud/live/model/ListLiveRealtimeLogDeliveryInfosResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

ListLiveRealtimeLogDeliveryInfosResult::ListLiveRealtimeLogDeliveryInfosResult() :
	ServiceResult()
{}

ListLiveRealtimeLogDeliveryInfosResult::ListLiveRealtimeLogDeliveryInfosResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListLiveRealtimeLogDeliveryInfosResult::~ListLiveRealtimeLogDeliveryInfosResult()
{}

void ListLiveRealtimeLogDeliveryInfosResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allContentNode = value["Content"]["RealtimeLogDeliveryInfos"];
	for (auto valueContentRealtimeLogDeliveryInfos : allContentNode)
	{
		RealtimeLogDeliveryInfos contentObject;
		if(!valueContentRealtimeLogDeliveryInfos["Project"].isNull())
			contentObject.project = valueContentRealtimeLogDeliveryInfos["Project"].asString();
		if(!valueContentRealtimeLogDeliveryInfos["Logstore"].isNull())
			contentObject.logstore = valueContentRealtimeLogDeliveryInfos["Logstore"].asString();
		if(!valueContentRealtimeLogDeliveryInfos["Region"].isNull())
			contentObject.region = valueContentRealtimeLogDeliveryInfos["Region"].asString();
		content_.push_back(contentObject);
	}

}

std::vector<ListLiveRealtimeLogDeliveryInfosResult::RealtimeLogDeliveryInfos> ListLiveRealtimeLogDeliveryInfosResult::getContent()const
{
	return content_;
}

