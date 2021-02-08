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

#include <alibabacloud/live/model/DescribeCastersResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeCastersResult::DescribeCastersResult() :
	ServiceResult()
{}

DescribeCastersResult::DescribeCastersResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeCastersResult::~DescribeCastersResult()
{}

void DescribeCastersResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allCasterListNode = value["CasterList"]["Caster"];
	for (auto valueCasterListCaster : allCasterListNode)
	{
		Caster casterListObject;
		if(!valueCasterListCaster["Status"].isNull())
			casterListObject.status = std::stoi(valueCasterListCaster["Status"].asString());
		if(!valueCasterListCaster["NormType"].isNull())
			casterListObject.normType = std::stoi(valueCasterListCaster["NormType"].asString());
		if(!valueCasterListCaster["CasterId"].isNull())
			casterListObject.casterId = valueCasterListCaster["CasterId"].asString();
		if(!valueCasterListCaster["CasterName"].isNull())
			casterListObject.casterName = valueCasterListCaster["CasterName"].asString();
		if(!valueCasterListCaster["CreateTime"].isNull())
			casterListObject.createTime = valueCasterListCaster["CreateTime"].asString();
		if(!valueCasterListCaster["StartTime"].isNull())
			casterListObject.startTime = valueCasterListCaster["StartTime"].asString();
		if(!valueCasterListCaster["PurchaseTime"].isNull())
			casterListObject.purchaseTime = valueCasterListCaster["PurchaseTime"].asString();
		if(!valueCasterListCaster["ExpireTime"].isNull())
			casterListObject.expireTime = valueCasterListCaster["ExpireTime"].asString();
		if(!valueCasterListCaster["ChargeType"].isNull())
			casterListObject.chargeType = valueCasterListCaster["ChargeType"].asString();
		if(!valueCasterListCaster["CasterTemplate"].isNull())
			casterListObject.casterTemplate = valueCasterListCaster["CasterTemplate"].asString();
		if(!valueCasterListCaster["ChannelEnable"].isNull())
			casterListObject.channelEnable = std::stoi(valueCasterListCaster["ChannelEnable"].asString());
		casterList_.push_back(casterListObject);
	}
	if(!value["Total"].isNull())
		total_ = std::stoi(value["Total"].asString());

}

int DescribeCastersResult::getTotal()const
{
	return total_;
}

std::vector<DescribeCastersResult::Caster> DescribeCastersResult::getCasterList()const
{
	return casterList_;
}

