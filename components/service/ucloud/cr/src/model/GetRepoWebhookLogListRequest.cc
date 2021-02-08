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

#include <alibabacloud/cr/model/GetRepoWebhookLogListRequest.h>

using AlibabaCloud::Cr::Model::GetRepoWebhookLogListRequest;

GetRepoWebhookLogListRequest::GetRepoWebhookLogListRequest() :
	RoaServiceRequest("cr", "2016-06-07")
{}

GetRepoWebhookLogListRequest::~GetRepoWebhookLogListRequest()
{}

long GetRepoWebhookLogListRequest::getWebhookId()const
{
	return webhookId_;
}

void GetRepoWebhookLogListRequest::setWebhookId(long webhookId)
{
	webhookId_ = webhookId;
	setCoreParameter("WebhookId", std::to_string(webhookId));
}

std::string GetRepoWebhookLogListRequest::getRepoNamespace()const
{
	return repoNamespace_;
}

void GetRepoWebhookLogListRequest::setRepoNamespace(const std::string& repoNamespace)
{
	repoNamespace_ = repoNamespace;
	setCoreParameter("RepoNamespace", repoNamespace);
}

std::string GetRepoWebhookLogListRequest::getRepoName()const
{
	return repoName_;
}

void GetRepoWebhookLogListRequest::setRepoName(const std::string& repoName)
{
	repoName_ = repoName;
	setCoreParameter("RepoName", repoName);
}

