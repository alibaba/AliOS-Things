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

#include <alibabacloud/live/model/DescribeCasterLayoutsRequest.h>

using AlibabaCloud::Live::Model::DescribeCasterLayoutsRequest;

DescribeCasterLayoutsRequest::DescribeCasterLayoutsRequest() :
	RpcServiceRequest("live", "2016-11-01", "DescribeCasterLayouts")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeCasterLayoutsRequest::~DescribeCasterLayoutsRequest()
{}

std::string DescribeCasterLayoutsRequest::getLayoutId()const
{
	return layoutId_;
}

void DescribeCasterLayoutsRequest::setLayoutId(const std::string& layoutId)
{
	layoutId_ = layoutId;
	setParameter("LayoutId", layoutId);
}

std::string DescribeCasterLayoutsRequest::getCasterId()const
{
	return casterId_;
}

void DescribeCasterLayoutsRequest::setCasterId(const std::string& casterId)
{
	casterId_ = casterId;
	setParameter("CasterId", casterId);
}

long DescribeCasterLayoutsRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeCasterLayoutsRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

