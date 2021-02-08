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

#include <alibabacloud/live/model/AddCasterEpisodeGroupContentRequest.h>

using AlibabaCloud::Live::Model::AddCasterEpisodeGroupContentRequest;

AddCasterEpisodeGroupContentRequest::AddCasterEpisodeGroupContentRequest() :
	RpcServiceRequest("live", "2016-11-01", "AddCasterEpisodeGroupContent")
{
	setMethod(HttpRequest::Method::Post);
}

AddCasterEpisodeGroupContentRequest::~AddCasterEpisodeGroupContentRequest()
{}

std::string AddCasterEpisodeGroupContentRequest::getClientToken()const
{
	return clientToken_;
}

void AddCasterEpisodeGroupContentRequest::setClientToken(const std::string& clientToken)
{
	clientToken_ = clientToken;
	setParameter("ClientToken", clientToken);
}

std::string AddCasterEpisodeGroupContentRequest::getContent()const
{
	return content_;
}

void AddCasterEpisodeGroupContentRequest::setContent(const std::string& content)
{
	content_ = content;
	setParameter("Content", content);
}

long AddCasterEpisodeGroupContentRequest::getOwnerId()const
{
	return ownerId_;
}

void AddCasterEpisodeGroupContentRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

