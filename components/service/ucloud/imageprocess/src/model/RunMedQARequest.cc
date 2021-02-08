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

#include <alibabacloud/imageprocess/model/RunMedQARequest.h>

using AlibabaCloud::Imageprocess::Model::RunMedQARequest;

RunMedQARequest::RunMedQARequest() :
	RpcServiceRequest("imageprocess", "2020-03-20", "RunMedQA")
{
	setMethod(HttpRequest::Method::Post);
}

RunMedQARequest::~RunMedQARequest()
{}

std::string RunMedQARequest::getQuestion()const
{
	return question_;
}

void RunMedQARequest::setQuestion(const std::string& question)
{
	question_ = question;
	setBodyParameter("Question", question);
}

std::string RunMedQARequest::getOrgId()const
{
	return orgId_;
}

void RunMedQARequest::setOrgId(const std::string& orgId)
{
	orgId_ = orgId;
	setBodyParameter("OrgId", orgId);
}

std::string RunMedQARequest::getOrgName()const
{
	return orgName_;
}

void RunMedQARequest::setOrgName(const std::string& orgName)
{
	orgName_ = orgName;
	setBodyParameter("OrgName", orgName);
}

