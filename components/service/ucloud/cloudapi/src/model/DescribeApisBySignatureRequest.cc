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

#include <alibabacloud/cloudapi/model/DescribeApisBySignatureRequest.h>

using AlibabaCloud::CloudAPI::Model::DescribeApisBySignatureRequest;

DescribeApisBySignatureRequest::DescribeApisBySignatureRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DescribeApisBySignature")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeApisBySignatureRequest::~DescribeApisBySignatureRequest()
{}

std::string DescribeApisBySignatureRequest::getSignatureId()const
{
	return signatureId_;
}

void DescribeApisBySignatureRequest::setSignatureId(const std::string& signatureId)
{
	signatureId_ = signatureId;
	setParameter("SignatureId", signatureId);
}

int DescribeApisBySignatureRequest::getPageNumber()const
{
	return pageNumber_;
}

void DescribeApisBySignatureRequest::setPageNumber(int pageNumber)
{
	pageNumber_ = pageNumber;
	setParameter("PageNumber", std::to_string(pageNumber));
}

std::string DescribeApisBySignatureRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DescribeApisBySignatureRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DescribeApisBySignatureRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeApisBySignatureRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

int DescribeApisBySignatureRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeApisBySignatureRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

