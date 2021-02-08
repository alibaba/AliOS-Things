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

#include <alibabacloud/cloudapi/model/SetDomainResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

SetDomainResult::SetDomainResult() :
	ServiceResult()
{}

SetDomainResult::SetDomainResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

SetDomainResult::~SetDomainResult()
{}

void SetDomainResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	if(!value["GroupId"].isNull())
		groupId_ = value["GroupId"].asString();
	if(!value["DomainName"].isNull())
		domainName_ = value["DomainName"].asString();
	if(!value["SubDomain"].isNull())
		subDomain_ = value["SubDomain"].asString();
	if(!value["DomainBindingStatus"].isNull())
		domainBindingStatus_ = value["DomainBindingStatus"].asString();
	if(!value["DomainLegalStatus"].isNull())
		domainLegalStatus_ = value["DomainLegalStatus"].asString();
	if(!value["DomainWebSocketStatus"].isNull())
		domainWebSocketStatus_ = value["DomainWebSocketStatus"].asString();
	if(!value["DomainRemark"].isNull())
		domainRemark_ = value["DomainRemark"].asString();

}

std::string SetDomainResult::getSubDomain()const
{
	return subDomain_;
}

std::string SetDomainResult::getDomainRemark()const
{
	return domainRemark_;
}

std::string SetDomainResult::getDomainBindingStatus()const
{
	return domainBindingStatus_;
}

std::string SetDomainResult::getDomainLegalStatus()const
{
	return domainLegalStatus_;
}

std::string SetDomainResult::getDomainName()const
{
	return domainName_;
}

std::string SetDomainResult::getDomainWebSocketStatus()const
{
	return domainWebSocketStatus_;
}

std::string SetDomainResult::getGroupId()const
{
	return groupId_;
}

