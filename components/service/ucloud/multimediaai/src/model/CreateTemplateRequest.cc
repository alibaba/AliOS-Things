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

#include <alibabacloud/multimediaai/model/CreateTemplateRequest.h>

using AlibabaCloud::Multimediaai::Model::CreateTemplateRequest;

CreateTemplateRequest::CreateTemplateRequest() :
	RpcServiceRequest("multimediaai", "2019-08-10", "CreateTemplate")
{
	setMethod(HttpRequest::Method::Post);
}

CreateTemplateRequest::~CreateTemplateRequest()
{}

int CreateTemplateRequest::getType()const
{
	return type_;
}

void CreateTemplateRequest::setType(int type)
{
	type_ = type;
	setParameter("Type", std::to_string(type));
}

std::string CreateTemplateRequest::getContent()const
{
	return content_;
}

void CreateTemplateRequest::setContent(const std::string& content)
{
	content_ = content;
	setParameter("Content", content);
}

std::string CreateTemplateRequest::getTemplateName()const
{
	return templateName_;
}

void CreateTemplateRequest::setTemplateName(const std::string& templateName)
{
	templateName_ = templateName;
	setParameter("TemplateName", templateName);
}

bool CreateTemplateRequest::getIsDefault()const
{
	return isDefault_;
}

void CreateTemplateRequest::setIsDefault(bool isDefault)
{
	isDefault_ = isDefault;
	setParameter("IsDefault", isDefault ? "true" : "false");
}

