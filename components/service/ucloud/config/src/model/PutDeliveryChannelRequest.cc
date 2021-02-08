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

#include <alibabacloud/config/model/PutDeliveryChannelRequest.h>

using AlibabaCloud::Config::Model::PutDeliveryChannelRequest;

PutDeliveryChannelRequest::PutDeliveryChannelRequest() :
	RpcServiceRequest("config", "2019-01-08", "PutDeliveryChannel")
{
	setMethod(HttpRequest::Method::Post);
}

PutDeliveryChannelRequest::~PutDeliveryChannelRequest()
{}

std::string PutDeliveryChannelRequest::getClientToken()const
{
	return clientToken_;
}

void PutDeliveryChannelRequest::setClientToken(const std::string& clientToken)
{
	clientToken_ = clientToken;
	setBodyParameter("ClientToken", clientToken);
}

std::string PutDeliveryChannelRequest::getDescription()const
{
	return description_;
}

void PutDeliveryChannelRequest::setDescription(const std::string& description)
{
	description_ = description;
	setBodyParameter("Description", description);
}

std::string PutDeliveryChannelRequest::getDeliveryChannelTargetArn()const
{
	return deliveryChannelTargetArn_;
}

void PutDeliveryChannelRequest::setDeliveryChannelTargetArn(const std::string& deliveryChannelTargetArn)
{
	deliveryChannelTargetArn_ = deliveryChannelTargetArn;
	setBodyParameter("DeliveryChannelTargetArn", deliveryChannelTargetArn);
}

std::string PutDeliveryChannelRequest::getDeliveryChannelCondition()const
{
	return deliveryChannelCondition_;
}

void PutDeliveryChannelRequest::setDeliveryChannelCondition(const std::string& deliveryChannelCondition)
{
	deliveryChannelCondition_ = deliveryChannelCondition;
	setBodyParameter("DeliveryChannelCondition", deliveryChannelCondition);
}

std::string PutDeliveryChannelRequest::getDeliveryChannelAssumeRoleArn()const
{
	return deliveryChannelAssumeRoleArn_;
}

void PutDeliveryChannelRequest::setDeliveryChannelAssumeRoleArn(const std::string& deliveryChannelAssumeRoleArn)
{
	deliveryChannelAssumeRoleArn_ = deliveryChannelAssumeRoleArn;
	setBodyParameter("DeliveryChannelAssumeRoleArn", deliveryChannelAssumeRoleArn);
}

std::string PutDeliveryChannelRequest::getDeliveryChannelName()const
{
	return deliveryChannelName_;
}

void PutDeliveryChannelRequest::setDeliveryChannelName(const std::string& deliveryChannelName)
{
	deliveryChannelName_ = deliveryChannelName;
	setBodyParameter("DeliveryChannelName", deliveryChannelName);
}

std::string PutDeliveryChannelRequest::getDeliveryChannelId()const
{
	return deliveryChannelId_;
}

void PutDeliveryChannelRequest::setDeliveryChannelId(const std::string& deliveryChannelId)
{
	deliveryChannelId_ = deliveryChannelId;
	setBodyParameter("DeliveryChannelId", deliveryChannelId);
}

std::string PutDeliveryChannelRequest::getDeliveryChannelType()const
{
	return deliveryChannelType_;
}

void PutDeliveryChannelRequest::setDeliveryChannelType(const std::string& deliveryChannelType)
{
	deliveryChannelType_ = deliveryChannelType;
	setBodyParameter("DeliveryChannelType", deliveryChannelType);
}

int PutDeliveryChannelRequest::getStatus()const
{
	return status_;
}

void PutDeliveryChannelRequest::setStatus(int status)
{
	status_ = status;
	setBodyParameter("Status", std::to_string(status));
}

