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

#include <alibabacloud/cloudapi/model/DescribeSignaturesRequest.h>

using AlibabaCloud::CloudAPI::Model::DescribeSignaturesRequest;

DescribeSignaturesRequest::DescribeSignaturesRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DescribeSignatures")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeSignaturesRequest::~DescribeSignaturesRequest()
{}

std::string DescribeSignaturesRequest::getSignatureName()const
{
	return signatureName_;
}

void DescribeSignaturesRequest::setSignatureName(const std::string& signatureName)
{
	signatureName_ = signatureName;
	setParameter("SignatureName", signatureName);
}

std::string DescribeSignaturesRequest::getSignatureId()const
{
	return signatureId_;
}

void DescribeSignaturesRequest::setSignatureId(const std::string& signatureId)
{
	signatureId_ = signatureId;
	setParameter("SignatureId", signatureId);
}

int DescribeSignaturesRequest::getPageNumber()const
{
	return pageNumber_;
}

void DescribeSignaturesRequest::setPageNumber(int pageNumber)
{
	pageNumber_ = pageNumber;
	setParameter("PageNumber", std::to_string(pageNumber));
}

std::string DescribeSignaturesRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DescribeSignaturesRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DescribeSignaturesRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeSignaturesRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

int DescribeSignaturesRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeSignaturesRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

