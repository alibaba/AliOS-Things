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

#include <alibabacloud/live/model/DescribeHtmlResourceRequest.h>

using AlibabaCloud::Live::Model::DescribeHtmlResourceRequest;

DescribeHtmlResourceRequest::DescribeHtmlResourceRequest() :
	RpcServiceRequest("live", "2016-11-01", "DescribeHtmlResource")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeHtmlResourceRequest::~DescribeHtmlResourceRequest()
{}

std::string DescribeHtmlResourceRequest::getHtmlUrl()const
{
	return htmlUrl_;
}

void DescribeHtmlResourceRequest::setHtmlUrl(const std::string& htmlUrl)
{
	htmlUrl_ = htmlUrl;
	setParameter("HtmlUrl", htmlUrl);
}

std::string DescribeHtmlResourceRequest::getCasterId()const
{
	return casterId_;
}

void DescribeHtmlResourceRequest::setCasterId(const std::string& casterId)
{
	casterId_ = casterId;
	setParameter("CasterId", casterId);
}

long DescribeHtmlResourceRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeHtmlResourceRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DescribeHtmlResourceRequest::getHtmlResourceId()const
{
	return htmlResourceId_;
}

void DescribeHtmlResourceRequest::setHtmlResourceId(const std::string& htmlResourceId)
{
	htmlResourceId_ = htmlResourceId;
	setParameter("HtmlResourceId", htmlResourceId);
}

