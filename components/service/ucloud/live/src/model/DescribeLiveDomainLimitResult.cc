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

#include <alibabacloud/live/model/DescribeLiveDomainLimitResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveDomainLimitResult::DescribeLiveDomainLimitResult() :
	ServiceResult()
{}

DescribeLiveDomainLimitResult::DescribeLiveDomainLimitResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveDomainLimitResult::~DescribeLiveDomainLimitResult()
{}

void DescribeLiveDomainLimitResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allLiveDomainLimitListNode = value["LiveDomainLimitList"]["LiveDomainLimit"];
	for (auto valueLiveDomainLimitListLiveDomainLimit : allLiveDomainLimitListNode)
	{
		LiveDomainLimit liveDomainLimitListObject;
		if(!valueLiveDomainLimitListLiveDomainLimit["DomainName"].isNull())
			liveDomainLimitListObject.domainName = valueLiveDomainLimitListLiveDomainLimit["DomainName"].asString();
		if(!valueLiveDomainLimitListLiveDomainLimit["LimitNum"].isNull())
			liveDomainLimitListObject.limitNum = std::stoi(valueLiveDomainLimitListLiveDomainLimit["LimitNum"].asString());
		if(!valueLiveDomainLimitListLiveDomainLimit["LimitTranscodeNum"].isNull())
			liveDomainLimitListObject.limitTranscodeNum = std::stoi(valueLiveDomainLimitListLiveDomainLimit["LimitTranscodeNum"].asString());
		liveDomainLimitList_.push_back(liveDomainLimitListObject);
	}

}

std::vector<DescribeLiveDomainLimitResult::LiveDomainLimit> DescribeLiveDomainLimitResult::getLiveDomainLimitList()const
{
	return liveDomainLimitList_;
}

