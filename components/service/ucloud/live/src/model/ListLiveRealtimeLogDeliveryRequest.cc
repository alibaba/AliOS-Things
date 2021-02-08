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

#include <alibabacloud/live/model/ListLiveRealtimeLogDeliveryRequest.h>

using AlibabaCloud::Live::Model::ListLiveRealtimeLogDeliveryRequest;

ListLiveRealtimeLogDeliveryRequest::ListLiveRealtimeLogDeliveryRequest() :
	RpcServiceRequest("live", "2016-11-01", "ListLiveRealtimeLogDelivery")
{
	setMethod(HttpRequest::Method::Get);
}

ListLiveRealtimeLogDeliveryRequest::~ListLiveRealtimeLogDeliveryRequest()
{}

long ListLiveRealtimeLogDeliveryRequest::getOwnerId()const
{
	return ownerId_;
}

void ListLiveRealtimeLogDeliveryRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string ListLiveRealtimeLogDeliveryRequest::getLiveOpenapiReserve()const
{
	return liveOpenapiReserve_;
}

void ListLiveRealtimeLogDeliveryRequest::setLiveOpenapiReserve(const std::string& liveOpenapiReserve)
{
	liveOpenapiReserve_ = liveOpenapiReserve;
	setParameter("LiveOpenapiReserve", liveOpenapiReserve);
}

