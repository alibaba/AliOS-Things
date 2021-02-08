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

#include <alibabacloud/live/model/DescribeLiveTopDomainsByFlowResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveTopDomainsByFlowResult::DescribeLiveTopDomainsByFlowResult() :
	ServiceResult()
{}

DescribeLiveTopDomainsByFlowResult::DescribeLiveTopDomainsByFlowResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveTopDomainsByFlowResult::~DescribeLiveTopDomainsByFlowResult()
{}

void DescribeLiveTopDomainsByFlowResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allTopDomainsNode = value["TopDomains"]["TopDomain"];
	for (auto valueTopDomainsTopDomain : allTopDomainsNode)
	{
		TopDomain topDomainsObject;
		if(!valueTopDomainsTopDomain["DomainName"].isNull())
			topDomainsObject.domainName = valueTopDomainsTopDomain["DomainName"].asString();
		if(!valueTopDomainsTopDomain["Rank"].isNull())
			topDomainsObject.rank = std::stol(valueTopDomainsTopDomain["Rank"].asString());
		if(!valueTopDomainsTopDomain["TotalTraffic"].isNull())
			topDomainsObject.totalTraffic = valueTopDomainsTopDomain["TotalTraffic"].asString();
		if(!valueTopDomainsTopDomain["TrafficPercent"].isNull())
			topDomainsObject.trafficPercent = valueTopDomainsTopDomain["TrafficPercent"].asString();
		if(!valueTopDomainsTopDomain["MaxBps"].isNull())
			topDomainsObject.maxBps = std::stol(valueTopDomainsTopDomain["MaxBps"].asString());
		if(!valueTopDomainsTopDomain["MaxBpsTime"].isNull())
			topDomainsObject.maxBpsTime = valueTopDomainsTopDomain["MaxBpsTime"].asString();
		if(!valueTopDomainsTopDomain["TotalAccess"].isNull())
			topDomainsObject.totalAccess = std::stol(valueTopDomainsTopDomain["TotalAccess"].asString());
		topDomains_.push_back(topDomainsObject);
	}
	if(!value["StartTime"].isNull())
		startTime_ = value["StartTime"].asString();
	if(!value["EndTime"].isNull())
		endTime_ = value["EndTime"].asString();
	if(!value["DomainCount"].isNull())
		domainCount_ = std::stol(value["DomainCount"].asString());
	if(!value["DomainOnlineCount"].isNull())
		domainOnlineCount_ = std::stol(value["DomainOnlineCount"].asString());

}

std::vector<DescribeLiveTopDomainsByFlowResult::TopDomain> DescribeLiveTopDomainsByFlowResult::getTopDomains()const
{
	return topDomains_;
}

std::string DescribeLiveTopDomainsByFlowResult::getEndTime()const
{
	return endTime_;
}

long DescribeLiveTopDomainsByFlowResult::getDomainOnlineCount()const
{
	return domainOnlineCount_;
}

std::string DescribeLiveTopDomainsByFlowResult::getStartTime()const
{
	return startTime_;
}

long DescribeLiveTopDomainsByFlowResult::getDomainCount()const
{
	return domainCount_;
}

