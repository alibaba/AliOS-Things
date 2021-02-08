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

#include <alibabacloud/cloudapi/model/DescribePurchasedApiGroupsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

DescribePurchasedApiGroupsResult::DescribePurchasedApiGroupsResult() :
	ServiceResult()
{}

DescribePurchasedApiGroupsResult::DescribePurchasedApiGroupsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribePurchasedApiGroupsResult::~DescribePurchasedApiGroupsResult()
{}

void DescribePurchasedApiGroupsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allPurchasedApiGroupAttributesNode = value["PurchasedApiGroupAttributes"]["PurchasedApiGroupAttribute"];
	for (auto valuePurchasedApiGroupAttributesPurchasedApiGroupAttribute : allPurchasedApiGroupAttributesNode)
	{
		PurchasedApiGroupAttribute purchasedApiGroupAttributesObject;
		if(!valuePurchasedApiGroupAttributesPurchasedApiGroupAttribute["GroupId"].isNull())
			purchasedApiGroupAttributesObject.groupId = valuePurchasedApiGroupAttributesPurchasedApiGroupAttribute["GroupId"].asString();
		if(!valuePurchasedApiGroupAttributesPurchasedApiGroupAttribute["GroupName"].isNull())
			purchasedApiGroupAttributesObject.groupName = valuePurchasedApiGroupAttributesPurchasedApiGroupAttribute["GroupName"].asString();
		if(!valuePurchasedApiGroupAttributesPurchasedApiGroupAttribute["Description"].isNull())
			purchasedApiGroupAttributesObject.description = valuePurchasedApiGroupAttributesPurchasedApiGroupAttribute["Description"].asString();
		if(!valuePurchasedApiGroupAttributesPurchasedApiGroupAttribute["PurchasedTime"].isNull())
			purchasedApiGroupAttributesObject.purchasedTime = valuePurchasedApiGroupAttributesPurchasedApiGroupAttribute["PurchasedTime"].asString();
		if(!valuePurchasedApiGroupAttributesPurchasedApiGroupAttribute["ExpireTime"].isNull())
			purchasedApiGroupAttributesObject.expireTime = valuePurchasedApiGroupAttributesPurchasedApiGroupAttribute["ExpireTime"].asString();
		if(!valuePurchasedApiGroupAttributesPurchasedApiGroupAttribute["RegionId"].isNull())
			purchasedApiGroupAttributesObject.regionId = valuePurchasedApiGroupAttributesPurchasedApiGroupAttribute["RegionId"].asString();
		if(!valuePurchasedApiGroupAttributesPurchasedApiGroupAttribute["BillingType"].isNull())
			purchasedApiGroupAttributesObject.billingType = valuePurchasedApiGroupAttributesPurchasedApiGroupAttribute["BillingType"].asString();
		if(!valuePurchasedApiGroupAttributesPurchasedApiGroupAttribute["InvokeTimesMax"].isNull())
			purchasedApiGroupAttributesObject.invokeTimesMax = std::stol(valuePurchasedApiGroupAttributesPurchasedApiGroupAttribute["InvokeTimesMax"].asString());
		if(!valuePurchasedApiGroupAttributesPurchasedApiGroupAttribute["InvokeTimesNow"].isNull())
			purchasedApiGroupAttributesObject.invokeTimesNow = std::stol(valuePurchasedApiGroupAttributesPurchasedApiGroupAttribute["InvokeTimesNow"].asString());
		if(!valuePurchasedApiGroupAttributesPurchasedApiGroupAttribute["Status"].isNull())
			purchasedApiGroupAttributesObject.status = valuePurchasedApiGroupAttributesPurchasedApiGroupAttribute["Status"].asString();
		purchasedApiGroupAttributes_.push_back(purchasedApiGroupAttributesObject);
	}
	if(!value["TotalCount"].isNull())
		totalCount_ = std::stoi(value["TotalCount"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["PageNumber"].isNull())
		pageNumber_ = std::stoi(value["PageNumber"].asString());

}

int DescribePurchasedApiGroupsResult::getTotalCount()const
{
	return totalCount_;
}

int DescribePurchasedApiGroupsResult::getPageSize()const
{
	return pageSize_;
}

int DescribePurchasedApiGroupsResult::getPageNumber()const
{
	return pageNumber_;
}

std::vector<DescribePurchasedApiGroupsResult::PurchasedApiGroupAttribute> DescribePurchasedApiGroupsResult::getPurchasedApiGroupAttributes()const
{
	return purchasedApiGroupAttributes_;
}

