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

#include <alibabacloud/live/model/DescribeLiveDomainCertificateInfoResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveDomainCertificateInfoResult::DescribeLiveDomainCertificateInfoResult() :
	ServiceResult()
{}

DescribeLiveDomainCertificateInfoResult::DescribeLiveDomainCertificateInfoResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveDomainCertificateInfoResult::~DescribeLiveDomainCertificateInfoResult()
{}

void DescribeLiveDomainCertificateInfoResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allCertInfosNode = value["CertInfos"]["CertInfo"];
	for (auto valueCertInfosCertInfo : allCertInfosNode)
	{
		CertInfo certInfosObject;
		if(!valueCertInfosCertInfo["DomainName"].isNull())
			certInfosObject.domainName = valueCertInfosCertInfo["DomainName"].asString();
		if(!valueCertInfosCertInfo["CertName"].isNull())
			certInfosObject.certName = valueCertInfosCertInfo["CertName"].asString();
		if(!valueCertInfosCertInfo["CertDomainName"].isNull())
			certInfosObject.certDomainName = valueCertInfosCertInfo["CertDomainName"].asString();
		if(!valueCertInfosCertInfo["CertExpireTime"].isNull())
			certInfosObject.certExpireTime = valueCertInfosCertInfo["CertExpireTime"].asString();
		if(!valueCertInfosCertInfo["CertLife"].isNull())
			certInfosObject.certLife = valueCertInfosCertInfo["CertLife"].asString();
		if(!valueCertInfosCertInfo["CertOrg"].isNull())
			certInfosObject.certOrg = valueCertInfosCertInfo["CertOrg"].asString();
		if(!valueCertInfosCertInfo["CertType"].isNull())
			certInfosObject.certType = valueCertInfosCertInfo["CertType"].asString();
		if(!valueCertInfosCertInfo["SSLProtocol"].isNull())
			certInfosObject.sSLProtocol = valueCertInfosCertInfo["SSLProtocol"].asString();
		if(!valueCertInfosCertInfo["Status"].isNull())
			certInfosObject.status = valueCertInfosCertInfo["Status"].asString();
		if(!valueCertInfosCertInfo["SSLPub"].isNull())
			certInfosObject.sSLPub = valueCertInfosCertInfo["SSLPub"].asString();
		certInfos_.push_back(certInfosObject);
	}

}

std::vector<DescribeLiveDomainCertificateInfoResult::CertInfo> DescribeLiveDomainCertificateInfoResult::getCertInfos()const
{
	return certInfos_;
}

