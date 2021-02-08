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

#include <alibabacloud/cloudapi/model/DescribeApiSignaturesRequest.h>

using AlibabaCloud::CloudAPI::Model::DescribeApiSignaturesRequest;

DescribeApiSignaturesRequest::DescribeApiSignaturesRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DescribeApiSignatures")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeApiSignaturesRequest::~DescribeApiSignaturesRequest()
{}

std::string DescribeApiSignaturesRequest::getStageName()const
{
	return stageName_;
}

void DescribeApiSignaturesRequest::setStageName(const std::string& stageName)
{
	stageName_ = stageName;
	setParameter("StageName", stageName);
}

std::string DescribeApiSignaturesRequest::getGroupId()const
{
	return groupId_;
}

void DescribeApiSignaturesRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

int DescribeApiSignaturesRequest::getPageNumber()const
{
	return pageNumber_;
}

void DescribeApiSignaturesRequest::setPageNumber(int pageNumber)
{
	pageNumber_ = pageNumber;
	setParameter("PageNumber", std::to_string(pageNumber));
}

std::string DescribeApiSignaturesRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DescribeApiSignaturesRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DescribeApiSignaturesRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeApiSignaturesRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

int DescribeApiSignaturesRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeApiSignaturesRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string DescribeApiSignaturesRequest::getApiIds()const
{
	return apiIds_;
}

void DescribeApiSignaturesRequest::setApiIds(const std::string& apiIds)
{
	apiIds_ = apiIds;
	setParameter("ApiIds", apiIds);
}

