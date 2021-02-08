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

#include <alibabacloud/live/model/ListLiveRealtimeLogDeliveryDomainsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

ListLiveRealtimeLogDeliveryDomainsResult::ListLiveRealtimeLogDeliveryDomainsResult() :
	ServiceResult()
{}

ListLiveRealtimeLogDeliveryDomainsResult::ListLiveRealtimeLogDeliveryDomainsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListLiveRealtimeLogDeliveryDomainsResult::~ListLiveRealtimeLogDeliveryDomainsResult()
{}

void ListLiveRealtimeLogDeliveryDomainsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allContentNode = value["Content"]["Domains"];
	for (auto valueContentDomains : allContentNode)
	{
		Domains contentObject;
		if(!valueContentDomains["DomainName"].isNull())
			contentObject.domainName = valueContentDomains["DomainName"].asString();
		if(!valueContentDomains["Status"].isNull())
			contentObject.status = valueContentDomains["Status"].asString();
		content_.push_back(contentObject);
	}

}

std::vector<ListLiveRealtimeLogDeliveryDomainsResult::Domains> ListLiveRealtimeLogDeliveryDomainsResult::getContent()const
{
	return content_;
}

