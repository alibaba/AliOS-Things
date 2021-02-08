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

#include <alibabacloud/live/model/DescribeLiveDomainRealtimeLogDeliveryResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveDomainRealtimeLogDeliveryResult::DescribeLiveDomainRealtimeLogDeliveryResult() :
	ServiceResult()
{}

DescribeLiveDomainRealtimeLogDeliveryResult::DescribeLiveDomainRealtimeLogDeliveryResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveDomainRealtimeLogDeliveryResult::~DescribeLiveDomainRealtimeLogDeliveryResult()
{}

void DescribeLiveDomainRealtimeLogDeliveryResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	if(!value["Project"].isNull())
		project_ = value["Project"].asString();
	if(!value["Region"].isNull())
		region_ = value["Region"].asString();
	if(!value["Logstore"].isNull())
		logstore_ = value["Logstore"].asString();
	if(!value["Status"].isNull())
		status_ = value["Status"].asString();

}

std::string DescribeLiveDomainRealtimeLogDeliveryResult::getStatus()const
{
	return status_;
}

std::string DescribeLiveDomainRealtimeLogDeliveryResult::getProject()const
{
	return project_;
}

std::string DescribeLiveDomainRealtimeLogDeliveryResult::getLogstore()const
{
	return logstore_;
}

std::string DescribeLiveDomainRealtimeLogDeliveryResult::getRegion()const
{
	return region_;
}

