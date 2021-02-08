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

#include <alibabacloud/config/model/GetDiscoveredResourceCountsRequest.h>

using AlibabaCloud::Config::Model::GetDiscoveredResourceCountsRequest;

GetDiscoveredResourceCountsRequest::GetDiscoveredResourceCountsRequest() :
	RpcServiceRequest("config", "2019-01-08", "GetDiscoveredResourceCounts")
{
	setMethod(HttpRequest::Method::Get);
}

GetDiscoveredResourceCountsRequest::~GetDiscoveredResourceCountsRequest()
{}

bool GetDiscoveredResourceCountsRequest::getMultiAccount()const
{
	return multiAccount_;
}

void GetDiscoveredResourceCountsRequest::setMultiAccount(bool multiAccount)
{
	multiAccount_ = multiAccount;
	setParameter("MultiAccount", multiAccount ? "true" : "false");
}

std::string GetDiscoveredResourceCountsRequest::getGroupByKey()const
{
	return groupByKey_;
}

void GetDiscoveredResourceCountsRequest::setGroupByKey(const std::string& groupByKey)
{
	groupByKey_ = groupByKey;
	setParameter("GroupByKey", groupByKey);
}

long GetDiscoveredResourceCountsRequest::getMemberId()const
{
	return memberId_;
}

void GetDiscoveredResourceCountsRequest::setMemberId(long memberId)
{
	memberId_ = memberId;
	setParameter("MemberId", std::to_string(memberId));
}

