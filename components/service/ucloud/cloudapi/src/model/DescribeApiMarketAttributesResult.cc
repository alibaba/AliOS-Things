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

#include <alibabacloud/cloudapi/model/DescribeApiMarketAttributesResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

DescribeApiMarketAttributesResult::DescribeApiMarketAttributesResult() :
	ServiceResult()
{}

DescribeApiMarketAttributesResult::DescribeApiMarketAttributesResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeApiMarketAttributesResult::~DescribeApiMarketAttributesResult()
{}

void DescribeApiMarketAttributesResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	if(!value["ApiId"].isNull())
		apiId_ = value["ApiId"].asString();
	if(!value["NeedCharging"].isNull())
		needCharging_ = value["NeedCharging"].asString();
	if(!value["MarketChargingMode"].isNull())
		marketChargingMode_ = value["MarketChargingMode"].asString();

}

std::string DescribeApiMarketAttributesResult::getNeedCharging()const
{
	return needCharging_;
}

std::string DescribeApiMarketAttributesResult::getMarketChargingMode()const
{
	return marketChargingMode_;
}

std::string DescribeApiMarketAttributesResult::getApiId()const
{
	return apiId_;
}

