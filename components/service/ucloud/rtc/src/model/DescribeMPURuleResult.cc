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

#include <alibabacloud/rtc/model/DescribeMPURuleResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Rtc;
using namespace AlibabaCloud::Rtc::Model;

DescribeMPURuleResult::DescribeMPURuleResult() :
	ServiceResult()
{}

DescribeMPURuleResult::DescribeMPURuleResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeMPURuleResult::~DescribeMPURuleResult()
{}

void DescribeMPURuleResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allRulesNode = value["Rules"]["Rule"];
	for (auto valueRulesRule : allRulesNode)
	{
		Rule rulesObject;
		if(!valueRulesRule["RuleId"].isNull())
			rulesObject.ruleId = std::stol(valueRulesRule["RuleId"].asString());
		if(!valueRulesRule["ChannelPrefix"].isNull())
			rulesObject.channelPrefix = valueRulesRule["ChannelPrefix"].asString();
		if(!valueRulesRule["MediaEncode"].isNull())
			rulesObject.mediaEncode = std::stoi(valueRulesRule["MediaEncode"].asString());
		if(!valueRulesRule["BackgroudColor"].isNull())
			rulesObject.backgroudColor = std::stoi(valueRulesRule["BackgroudColor"].asString());
		if(!valueRulesRule["CropMode"].isNull())
			rulesObject.cropMode = std::stoi(valueRulesRule["CropMode"].asString());
		if(!valueRulesRule["TaskProfile"].isNull())
			rulesObject.taskProfile = valueRulesRule["TaskProfile"].asString();
		if(!valueRulesRule["PlayDomain"].isNull())
			rulesObject.playDomain = valueRulesRule["PlayDomain"].asString();
		if(!valueRulesRule["CallBack"].isNull())
			rulesObject.callBack = valueRulesRule["CallBack"].asString();
		if(!valueRulesRule["IsEnable"].isNull())
			rulesObject.isEnable = std::stoi(valueRulesRule["IsEnable"].asString());
		auto allLayoutIds = value["LayoutIds"]["LayoutId"];
		for (auto value : allLayoutIds)
			rulesObject.layoutIds.push_back(value.asString());
		rules_.push_back(rulesObject);
	}

}

std::vector<DescribeMPURuleResult::Rule> DescribeMPURuleResult::getRules()const
{
	return rules_;
}

