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

#include <alibabacloud/live/model/ControlHtmlResourceRequest.h>

using AlibabaCloud::Live::Model::ControlHtmlResourceRequest;

ControlHtmlResourceRequest::ControlHtmlResourceRequest() :
	RpcServiceRequest("live", "2016-11-01", "ControlHtmlResource")
{
	setMethod(HttpRequest::Method::Post);
}

ControlHtmlResourceRequest::~ControlHtmlResourceRequest()
{}

std::string ControlHtmlResourceRequest::getHtmlUrl()const
{
	return htmlUrl_;
}

void ControlHtmlResourceRequest::setHtmlUrl(const std::string& htmlUrl)
{
	htmlUrl_ = htmlUrl;
	setParameter("HtmlUrl", htmlUrl);
}

std::string ControlHtmlResourceRequest::getCasterId()const
{
	return casterId_;
}

void ControlHtmlResourceRequest::setCasterId(const std::string& casterId)
{
	casterId_ = casterId;
	setParameter("CasterId", casterId);
}

long ControlHtmlResourceRequest::getOwnerId()const
{
	return ownerId_;
}

void ControlHtmlResourceRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string ControlHtmlResourceRequest::getOperate()const
{
	return operate_;
}

void ControlHtmlResourceRequest::setOperate(const std::string& operate)
{
	operate_ = operate;
	setParameter("Operate", operate);
}

std::string ControlHtmlResourceRequest::getHtmlResourceId()const
{
	return htmlResourceId_;
}

void ControlHtmlResourceRequest::setHtmlResourceId(const std::string& htmlResourceId)
{
	htmlResourceId_ = htmlResourceId;
	setParameter("HtmlResourceId", htmlResourceId);
}

