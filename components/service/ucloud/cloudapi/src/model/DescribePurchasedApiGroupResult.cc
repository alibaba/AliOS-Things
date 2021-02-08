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

#include <alibabacloud/cloudapi/model/DescribePurchasedApiGroupResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

DescribePurchasedApiGroupResult::DescribePurchasedApiGroupResult() :
	ServiceResult()
{}

DescribePurchasedApiGroupResult::DescribePurchasedApiGroupResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribePurchasedApiGroupResult::~DescribePurchasedApiGroupResult()
{}

void DescribePurchasedApiGroupResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allDomainsNode = value["Domains"]["DomainItem"];
	for (auto valueDomainsDomainItem : allDomainsNode)
	{
		DomainItem domainsObject;
		if(!valueDomainsDomainItem["DomainName"].isNull())
			domainsObject.domainName = valueDomainsDomainItem["DomainName"].asString();
		domains_.push_back(domainsObject);
	}
	if(!value["GroupId"].isNull())
		groupId_ = value["GroupId"].asString();
	if(!value["GroupName"].isNull())
		groupName_ = value["GroupName"].asString();
	if(!value["Description"].isNull())
		description_ = value["Description"].asString();
	if(!value["PurchasedTime"].isNull())
		purchasedTime_ = value["PurchasedTime"].asString();
	if(!value["RegionId"].isNull())
		regionId_ = value["RegionId"].asString();
	if(!value["Status"].isNull())
		status_ = value["Status"].asString();

}

std::vector<DescribePurchasedApiGroupResult::DomainItem> DescribePurchasedApiGroupResult::getDomains()const
{
	return domains_;
}

std::string DescribePurchasedApiGroupResult::getGroupName()const
{
	return groupName_;
}

std::string DescribePurchasedApiGroupResult::getStatus()const
{
	return status_;
}

std::string DescribePurchasedApiGroupResult::getDescription()const
{
	return description_;
}

std::string DescribePurchasedApiGroupResult::getRegionId()const
{
	return regionId_;
}

std::string DescribePurchasedApiGroupResult::getGroupId()const
{
	return groupId_;
}

std::string DescribePurchasedApiGroupResult::getPurchasedTime()const
{
	return purchasedTime_;
}

