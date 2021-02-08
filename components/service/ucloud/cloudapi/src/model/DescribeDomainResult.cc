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

#include <alibabacloud/cloudapi/model/DescribeDomainResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

DescribeDomainResult::DescribeDomainResult() :
	ServiceResult()
{}

DescribeDomainResult::DescribeDomainResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeDomainResult::~DescribeDomainResult()
{}

void DescribeDomainResult::parse(const std::string &payload)
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
	if(!value["CertificateId"].isNull())
		certificateId_ = value["CertificateId"].asString();
	if(!value["CertificateName"].isNull())
		certificateName_ = value["CertificateName"].asString();
	if(!value["CertificateBody"].isNull())
		certificateBody_ = value["CertificateBody"].asString();
	if(!value["CertificatePrivateKey"].isNull())
		certificatePrivateKey_ = value["CertificatePrivateKey"].asString();
	if(!value["DomainBindingStatus"].isNull())
		domainBindingStatus_ = value["DomainBindingStatus"].asString();
	if(!value["DomainCNAMEStatus"].isNull())
		domainCNAMEStatus_ = value["DomainCNAMEStatus"].asString();
	if(!value["DomainLegalStatus"].isNull())
		domainLegalStatus_ = value["DomainLegalStatus"].asString();
	if(!value["DomainWebSocketStatus"].isNull())
		domainWebSocketStatus_ = value["DomainWebSocketStatus"].asString();
	if(!value["DomainRemark"].isNull())
		domainRemark_ = value["DomainRemark"].asString();

}

std::string DescribeDomainResult::getSubDomain()const
{
	return subDomain_;
}

std::string DescribeDomainResult::getCertificateBody()const
{
	return certificateBody_;
}

std::string DescribeDomainResult::getDomainCNAMEStatus()const
{
	return domainCNAMEStatus_;
}

std::string DescribeDomainResult::getDomainRemark()const
{
	return domainRemark_;
}

std::string DescribeDomainResult::getDomainBindingStatus()const
{
	return domainBindingStatus_;
}

std::string DescribeDomainResult::getDomainLegalStatus()const
{
	return domainLegalStatus_;
}

std::string DescribeDomainResult::getDomainName()const
{
	return domainName_;
}

std::string DescribeDomainResult::getCertificateId()const
{
	return certificateId_;
}

std::string DescribeDomainResult::getDomainWebSocketStatus()const
{
	return domainWebSocketStatus_;
}

std::string DescribeDomainResult::getCertificatePrivateKey()const
{
	return certificatePrivateKey_;
}

std::string DescribeDomainResult::getCertificateName()const
{
	return certificateName_;
}

std::string DescribeDomainResult::getGroupId()const
{
	return groupId_;
}

