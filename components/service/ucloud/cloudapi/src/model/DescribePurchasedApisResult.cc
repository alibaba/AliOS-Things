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

#include <alibabacloud/cloudapi/model/DescribePurchasedApisResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

DescribePurchasedApisResult::DescribePurchasedApisResult() :
	ServiceResult()
{}

DescribePurchasedApisResult::DescribePurchasedApisResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribePurchasedApisResult::~DescribePurchasedApisResult()
{}

void DescribePurchasedApisResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allPurchasedApisNode = value["PurchasedApis"]["PurchasedApi"];
	for (auto valuePurchasedApisPurchasedApi : allPurchasedApisNode)
	{
		PurchasedApi purchasedApisObject;
		if(!valuePurchasedApisPurchasedApi["RegionId"].isNull())
			purchasedApisObject.regionId = valuePurchasedApisPurchasedApi["RegionId"].asString();
		if(!valuePurchasedApisPurchasedApi["GroupId"].isNull())
			purchasedApisObject.groupId = valuePurchasedApisPurchasedApi["GroupId"].asString();
		if(!valuePurchasedApisPurchasedApi["GroupName"].isNull())
			purchasedApisObject.groupName = valuePurchasedApisPurchasedApi["GroupName"].asString();
		if(!valuePurchasedApisPurchasedApi["ApiId"].isNull())
			purchasedApisObject.apiId = valuePurchasedApisPurchasedApi["ApiId"].asString();
		if(!valuePurchasedApisPurchasedApi["ApiName"].isNull())
			purchasedApisObject.apiName = valuePurchasedApisPurchasedApi["ApiName"].asString();
		if(!valuePurchasedApisPurchasedApi["StageName"].isNull())
			purchasedApisObject.stageName = valuePurchasedApisPurchasedApi["StageName"].asString();
		if(!valuePurchasedApisPurchasedApi["Description"].isNull())
			purchasedApisObject.description = valuePurchasedApisPurchasedApi["Description"].asString();
		if(!valuePurchasedApisPurchasedApi["PurchasedTime"].isNull())
			purchasedApisObject.purchasedTime = valuePurchasedApisPurchasedApi["PurchasedTime"].asString();
		purchasedApis_.push_back(purchasedApisObject);
	}
	if(!value["TotalCount"].isNull())
		totalCount_ = std::stoi(value["TotalCount"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["PageNumber"].isNull())
		pageNumber_ = std::stoi(value["PageNumber"].asString());

}

int DescribePurchasedApisResult::getTotalCount()const
{
	return totalCount_;
}

int DescribePurchasedApisResult::getPageSize()const
{
	return pageSize_;
}

int DescribePurchasedApisResult::getPageNumber()const
{
	return pageNumber_;
}

std::vector<DescribePurchasedApisResult::PurchasedApi> DescribePurchasedApisResult::getPurchasedApis()const
{
	return purchasedApis_;
}

