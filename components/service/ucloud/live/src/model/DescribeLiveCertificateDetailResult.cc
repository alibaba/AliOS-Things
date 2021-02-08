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

#include <alibabacloud/live/model/DescribeLiveCertificateDetailResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveCertificateDetailResult::DescribeLiveCertificateDetailResult() :
	ServiceResult()
{}

DescribeLiveCertificateDetailResult::DescribeLiveCertificateDetailResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveCertificateDetailResult::~DescribeLiveCertificateDetailResult()
{}

void DescribeLiveCertificateDetailResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	if(!value["Cert"].isNull())
		cert_ = value["Cert"].asString();
	if(!value["Key"].isNull())
		key_ = value["Key"].asString();
	if(!value["CertId"].isNull())
		certId_ = std::stol(value["CertId"].asString());
	if(!value["CertName"].isNull())
		certName_ = value["CertName"].asString();

}

long DescribeLiveCertificateDetailResult::getCertId()const
{
	return certId_;
}

std::string DescribeLiveCertificateDetailResult::getCert()const
{
	return cert_;
}

std::string DescribeLiveCertificateDetailResult::getCertName()const
{
	return certName_;
}

std::string DescribeLiveCertificateDetailResult::getKey()const
{
	return key_;
}

