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

#include <alibabacloud/live/model/CreateCasterRequest.h>

using AlibabaCloud::Live::Model::CreateCasterRequest;

CreateCasterRequest::CreateCasterRequest() :
	RpcServiceRequest("live", "2016-11-01", "CreateCaster")
{
	setMethod(HttpRequest::Method::Post);
}

CreateCasterRequest::~CreateCasterRequest()
{}

std::string CreateCasterRequest::getClientToken()const
{
	return clientToken_;
}

void CreateCasterRequest::setClientToken(const std::string& clientToken)
{
	clientToken_ = clientToken;
	setParameter("ClientToken", clientToken);
}

std::string CreateCasterRequest::getCasterName()const
{
	return casterName_;
}

void CreateCasterRequest::setCasterName(const std::string& casterName)
{
	casterName_ = casterName;
	setParameter("CasterName", casterName);
}

std::string CreateCasterRequest::getCasterTemplate()const
{
	return casterTemplate_;
}

void CreateCasterRequest::setCasterTemplate(const std::string& casterTemplate)
{
	casterTemplate_ = casterTemplate;
	setParameter("CasterTemplate", casterTemplate);
}

std::string CreateCasterRequest::getExpireTime()const
{
	return expireTime_;
}

void CreateCasterRequest::setExpireTime(const std::string& expireTime)
{
	expireTime_ = expireTime;
	setParameter("ExpireTime", expireTime);
}

int CreateCasterRequest::getNormType()const
{
	return normType_;
}

void CreateCasterRequest::setNormType(int normType)
{
	normType_ = normType;
	setParameter("NormType", std::to_string(normType));
}

long CreateCasterRequest::getOwnerId()const
{
	return ownerId_;
}

void CreateCasterRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string CreateCasterRequest::getPurchaseTime()const
{
	return purchaseTime_;
}

void CreateCasterRequest::setPurchaseTime(const std::string& purchaseTime)
{
	purchaseTime_ = purchaseTime;
	setParameter("PurchaseTime", purchaseTime);
}

std::string CreateCasterRequest::getChargeType()const
{
	return chargeType_;
}

void CreateCasterRequest::setChargeType(const std::string& chargeType)
{
	chargeType_ = chargeType;
	setParameter("ChargeType", chargeType);
}

