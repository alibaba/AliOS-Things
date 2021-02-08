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

#include <alibabacloud/iot/model/QueryConsumerGroupStatusRequest.h>

using AlibabaCloud::Iot::Model::QueryConsumerGroupStatusRequest;

QueryConsumerGroupStatusRequest::QueryConsumerGroupStatusRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QueryConsumerGroupStatus")
{
	setMethod(HttpRequest::Method::Post);
}

QueryConsumerGroupStatusRequest::~QueryConsumerGroupStatusRequest()
{}

std::string QueryConsumerGroupStatusRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QueryConsumerGroupStatusRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string QueryConsumerGroupStatusRequest::getGroupId()const
{
	return groupId_;
}

void QueryConsumerGroupStatusRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string QueryConsumerGroupStatusRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryConsumerGroupStatusRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryConsumerGroupStatusRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryConsumerGroupStatusRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

