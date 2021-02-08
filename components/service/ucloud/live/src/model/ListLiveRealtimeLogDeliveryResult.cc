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

#include <alibabacloud/live/model/ListLiveRealtimeLogDeliveryResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

ListLiveRealtimeLogDeliveryResult::ListLiveRealtimeLogDeliveryResult() :
	ServiceResult()
{}

ListLiveRealtimeLogDeliveryResult::ListLiveRealtimeLogDeliveryResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListLiveRealtimeLogDeliveryResult::~ListLiveRealtimeLogDeliveryResult()
{}

void ListLiveRealtimeLogDeliveryResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allContentNode = value["Content"]["RealtimeLogDeliveryInfo"];
	for (auto valueContentRealtimeLogDeliveryInfo : allContentNode)
	{
		RealtimeLogDeliveryInfo contentObject;
		if(!valueContentRealtimeLogDeliveryInfo["Project"].isNull())
			contentObject.project = valueContentRealtimeLogDeliveryInfo["Project"].asString();
		if(!valueContentRealtimeLogDeliveryInfo["Logstore"].isNull())
			contentObject.logstore = valueContentRealtimeLogDeliveryInfo["Logstore"].asString();
		if(!valueContentRealtimeLogDeliveryInfo["Region"].isNull())
			contentObject.region = valueContentRealtimeLogDeliveryInfo["Region"].asString();
		if(!valueContentRealtimeLogDeliveryInfo["DomainName"].isNull())
			contentObject.domainName = valueContentRealtimeLogDeliveryInfo["DomainName"].asString();
		if(!valueContentRealtimeLogDeliveryInfo["DmId"].isNull())
			contentObject.dmId = std::stoi(valueContentRealtimeLogDeliveryInfo["DmId"].asString());
		if(!valueContentRealtimeLogDeliveryInfo["Status"].isNull())
			contentObject.status = valueContentRealtimeLogDeliveryInfo["Status"].asString();
		content_.push_back(contentObject);
	}

}

std::vector<ListLiveRealtimeLogDeliveryResult::RealtimeLogDeliveryInfo> ListLiveRealtimeLogDeliveryResult::getContent()const
{
	return content_;
}

