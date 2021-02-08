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

#include <alibabacloud/cloudapi/model/DescribeSignaturesResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

DescribeSignaturesResult::DescribeSignaturesResult() :
	ServiceResult()
{}

DescribeSignaturesResult::DescribeSignaturesResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeSignaturesResult::~DescribeSignaturesResult()
{}

void DescribeSignaturesResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allSignatureInfosNode = value["SignatureInfos"]["SignatureInfo"];
	for (auto valueSignatureInfosSignatureInfo : allSignatureInfosNode)
	{
		SignatureInfo signatureInfosObject;
		if(!valueSignatureInfosSignatureInfo["RegionId"].isNull())
			signatureInfosObject.regionId = valueSignatureInfosSignatureInfo["RegionId"].asString();
		if(!valueSignatureInfosSignatureInfo["SignatureId"].isNull())
			signatureInfosObject.signatureId = valueSignatureInfosSignatureInfo["SignatureId"].asString();
		if(!valueSignatureInfosSignatureInfo["SignatureName"].isNull())
			signatureInfosObject.signatureName = valueSignatureInfosSignatureInfo["SignatureName"].asString();
		if(!valueSignatureInfosSignatureInfo["SignatureKey"].isNull())
			signatureInfosObject.signatureKey = valueSignatureInfosSignatureInfo["SignatureKey"].asString();
		if(!valueSignatureInfosSignatureInfo["SignatureSecret"].isNull())
			signatureInfosObject.signatureSecret = valueSignatureInfosSignatureInfo["SignatureSecret"].asString();
		if(!valueSignatureInfosSignatureInfo["CreatedTime"].isNull())
			signatureInfosObject.createdTime = valueSignatureInfosSignatureInfo["CreatedTime"].asString();
		if(!valueSignatureInfosSignatureInfo["ModifiedTime"].isNull())
			signatureInfosObject.modifiedTime = valueSignatureInfosSignatureInfo["ModifiedTime"].asString();
		signatureInfos_.push_back(signatureInfosObject);
	}
	if(!value["TotalCount"].isNull())
		totalCount_ = std::stoi(value["TotalCount"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["PageNumber"].isNull())
		pageNumber_ = std::stoi(value["PageNumber"].asString());

}

std::vector<DescribeSignaturesResult::SignatureInfo> DescribeSignaturesResult::getSignatureInfos()const
{
	return signatureInfos_;
}

int DescribeSignaturesResult::getTotalCount()const
{
	return totalCount_;
}

int DescribeSignaturesResult::getPageSize()const
{
	return pageSize_;
}

int DescribeSignaturesResult::getPageNumber()const
{
	return pageNumber_;
}

