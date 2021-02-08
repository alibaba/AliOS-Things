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

#include <alibabacloud/live/model/DescribeLiveRealtimeDeliveryAccResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveRealtimeDeliveryAccResult::DescribeLiveRealtimeDeliveryAccResult() :
	ServiceResult()
{}

DescribeLiveRealtimeDeliveryAccResult::DescribeLiveRealtimeDeliveryAccResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveRealtimeDeliveryAccResult::~DescribeLiveRealtimeDeliveryAccResult()
{}

void DescribeLiveRealtimeDeliveryAccResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allRealTimeDeliveryAccDataNode = value["RealTimeDeliveryAccData"]["AccData"];
	for (auto valueRealTimeDeliveryAccDataAccData : allRealTimeDeliveryAccDataNode)
	{
		AccData realTimeDeliveryAccDataObject;
		if(!valueRealTimeDeliveryAccDataAccData["TimeStamp"].isNull())
			realTimeDeliveryAccDataObject.timeStamp = valueRealTimeDeliveryAccDataAccData["TimeStamp"].asString();
		if(!valueRealTimeDeliveryAccDataAccData["SuccessNum"].isNull())
			realTimeDeliveryAccDataObject.successNum = std::stoi(valueRealTimeDeliveryAccDataAccData["SuccessNum"].asString());
		if(!valueRealTimeDeliveryAccDataAccData["FailedNum"].isNull())
			realTimeDeliveryAccDataObject.failedNum = std::stoi(valueRealTimeDeliveryAccDataAccData["FailedNum"].asString());
		realTimeDeliveryAccData_.push_back(realTimeDeliveryAccDataObject);
	}

}

std::vector<DescribeLiveRealtimeDeliveryAccResult::AccData> DescribeLiveRealtimeDeliveryAccResult::getRealTimeDeliveryAccData()const
{
	return realTimeDeliveryAccData_;
}

