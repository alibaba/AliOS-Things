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

#include <alibabacloud/imm/model/PutProjectRequest.h>

using AlibabaCloud::Imm::Model::PutProjectRequest;

PutProjectRequest::PutProjectRequest() :
	RpcServiceRequest("imm", "2017-09-06", "PutProject")
{
	setMethod(HttpRequest::Method::Post);
}

PutProjectRequest::~PutProjectRequest()
{}

std::string PutProjectRequest::getProject()const
{
	return project_;
}

void PutProjectRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string PutProjectRequest::getType()const
{
	return type_;
}

void PutProjectRequest::setType(const std::string& type)
{
	type_ = type;
	setParameter("Type", type);
}

std::string PutProjectRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void PutProjectRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

int PutProjectRequest::getCU()const
{
	return cU_;
}

void PutProjectRequest::setCU(int cU)
{
	cU_ = cU;
	setParameter("CU", std::to_string(cU));
}

std::string PutProjectRequest::getServiceRole()const
{
	return serviceRole_;
}

void PutProjectRequest::setServiceRole(const std::string& serviceRole)
{
	serviceRole_ = serviceRole;
	setParameter("ServiceRole", serviceRole);
}

std::string PutProjectRequest::getBillingType()const
{
	return billingType_;
}

void PutProjectRequest::setBillingType(const std::string& billingType)
{
	billingType_ = billingType;
	setParameter("BillingType", billingType);
}

