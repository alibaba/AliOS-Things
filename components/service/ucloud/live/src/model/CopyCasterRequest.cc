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

#include <alibabacloud/live/model/CopyCasterRequest.h>

using AlibabaCloud::Live::Model::CopyCasterRequest;

CopyCasterRequest::CopyCasterRequest() :
	RpcServiceRequest("live", "2016-11-01", "CopyCaster")
{
	setMethod(HttpRequest::Method::Post);
}

CopyCasterRequest::~CopyCasterRequest()
{}

std::string CopyCasterRequest::getClientToken()const
{
	return clientToken_;
}

void CopyCasterRequest::setClientToken(const std::string& clientToken)
{
	clientToken_ = clientToken;
	setParameter("ClientToken", clientToken);
}

std::string CopyCasterRequest::getCasterName()const
{
	return casterName_;
}

void CopyCasterRequest::setCasterName(const std::string& casterName)
{
	casterName_ = casterName;
	setParameter("CasterName", casterName);
}

std::string CopyCasterRequest::getSrcCasterId()const
{
	return srcCasterId_;
}

void CopyCasterRequest::setSrcCasterId(const std::string& srcCasterId)
{
	srcCasterId_ = srcCasterId;
	setParameter("SrcCasterId", srcCasterId);
}

long CopyCasterRequest::getOwnerId()const
{
	return ownerId_;
}

void CopyCasterRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

