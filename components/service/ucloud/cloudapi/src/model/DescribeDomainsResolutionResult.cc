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

#include <alibabacloud/cloudapi/model/DescribeDomainsResolutionResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

DescribeDomainsResolutionResult::DescribeDomainsResolutionResult() :
	ServiceResult()
{}

DescribeDomainsResolutionResult::DescribeDomainsResolutionResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeDomainsResolutionResult::~DescribeDomainsResolutionResult()
{}

void DescribeDomainsResolutionResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allDomainResolutionsNode = value["DomainResolutions"]["DomainResolution"];
	for (auto valueDomainResolutionsDomainResolution : allDomainResolutionsNode)
	{
		DomainResolution domainResolutionsObject;
		if(!valueDomainResolutionsDomainResolution["DomainName"].isNull())
			domainResolutionsObject.domainName = valueDomainResolutionsDomainResolution["DomainName"].asString();
		if(!valueDomainResolutionsDomainResolution["DomainResolutionStatus"].isNull())
			domainResolutionsObject.domainResolutionStatus = valueDomainResolutionsDomainResolution["DomainResolutionStatus"].asString();
		domainResolutions_.push_back(domainResolutionsObject);
	}
	if(!value["GroupId"].isNull())
		groupId_ = value["GroupId"].asString();

}

std::vector<DescribeDomainsResolutionResult::DomainResolution> DescribeDomainsResolutionResult::getDomainResolutions()const
{
	return domainResolutions_;
}

std::string DescribeDomainsResolutionResult::getGroupId()const
{
	return groupId_;
}

