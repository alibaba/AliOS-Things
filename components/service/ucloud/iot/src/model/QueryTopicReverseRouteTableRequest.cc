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

#include <alibabacloud/iot/model/QueryTopicReverseRouteTableRequest.h>

using AlibabaCloud::Iot::Model::QueryTopicReverseRouteTableRequest;

QueryTopicReverseRouteTableRequest::QueryTopicReverseRouteTableRequest() :
	RpcServiceRequest("iot", "2018-01-20", "QueryTopicReverseRouteTable")
{
	setMethod(HttpRequest::Method::Post);
}

QueryTopicReverseRouteTableRequest::~QueryTopicReverseRouteTableRequest()
{}

std::string QueryTopicReverseRouteTableRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void QueryTopicReverseRouteTableRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string QueryTopicReverseRouteTableRequest::getRegionId()const
{
	return regionId_;
}

void QueryTopicReverseRouteTableRequest::setRegionId(const std::string& regionId)
{
	regionId_ = regionId;
	setParameter("RegionId", regionId);
}

std::string QueryTopicReverseRouteTableRequest::getIotInstanceId()const
{
	return iotInstanceId_;
}

void QueryTopicReverseRouteTableRequest::setIotInstanceId(const std::string& iotInstanceId)
{
	iotInstanceId_ = iotInstanceId;
	setParameter("IotInstanceId", iotInstanceId);
}

std::string QueryTopicReverseRouteTableRequest::getApiProduct()const
{
	return apiProduct_;
}

void QueryTopicReverseRouteTableRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string QueryTopicReverseRouteTableRequest::getTopic()const
{
	return topic_;
}

void QueryTopicReverseRouteTableRequest::setTopic(const std::string& topic)
{
	topic_ = topic;
	setParameter("Topic", topic);
}

std::string QueryTopicReverseRouteTableRequest::getApiRevision()const
{
	return apiRevision_;
}

void QueryTopicReverseRouteTableRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}

