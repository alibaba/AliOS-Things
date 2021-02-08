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

#include <alibabacloud/live/model/DescribeLiveRealtimeLogAuthorizedRequest.h>

using AlibabaCloud::Live::Model::DescribeLiveRealtimeLogAuthorizedRequest;

DescribeLiveRealtimeLogAuthorizedRequest::DescribeLiveRealtimeLogAuthorizedRequest() :
	RpcServiceRequest("live", "2016-11-01", "DescribeLiveRealtimeLogAuthorized")
{
	setMethod(HttpRequest::Method::Get);
}

DescribeLiveRealtimeLogAuthorizedRequest::~DescribeLiveRealtimeLogAuthorizedRequest()
{}

long DescribeLiveRealtimeLogAuthorizedRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeLiveRealtimeLogAuthorizedRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DescribeLiveRealtimeLogAuthorizedRequest::getLiveOpenapiReserve()const
{
	return liveOpenapiReserve_;
}

void DescribeLiveRealtimeLogAuthorizedRequest::setLiveOpenapiReserve(const std::string& liveOpenapiReserve)
{
	liveOpenapiReserve_ = liveOpenapiReserve;
	setParameter("LiveOpenapiReserve", liveOpenapiReserve);
}

