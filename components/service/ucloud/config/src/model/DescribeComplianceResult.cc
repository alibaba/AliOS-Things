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

#include <alibabacloud/config/model/DescribeComplianceResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Config;
using namespace AlibabaCloud::Config::Model;

DescribeComplianceResult::DescribeComplianceResult() :
	ServiceResult()
{}

DescribeComplianceResult::DescribeComplianceResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeComplianceResult::~DescribeComplianceResult()
{}

void DescribeComplianceResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto complianceResultNode = value["ComplianceResult"];
	if(!complianceResultNode["TotalCount"].isNull())
		complianceResult_.totalCount = std::stol(complianceResultNode["TotalCount"].asString());
	auto allCompliancesNode = complianceResultNode["Compliances"]["CompliancesItem"];
	for (auto complianceResultNodeCompliancesCompliancesItem : allCompliancesNode)
	{
		ComplianceResult::CompliancesItem compliancesItemObject;
		if(!complianceResultNodeCompliancesCompliancesItem["ComplianceType"].isNull())
			compliancesItemObject.complianceType = complianceResultNodeCompliancesCompliancesItem["ComplianceType"].asString();
		if(!complianceResultNodeCompliancesCompliancesItem["Count"].isNull())
			compliancesItemObject.count = std::stoi(complianceResultNodeCompliancesCompliancesItem["Count"].asString());
		complianceResult_.compliances.push_back(compliancesItemObject);
	}

}

DescribeComplianceResult::ComplianceResult DescribeComplianceResult::getComplianceResult()const
{
	return complianceResult_;
}

