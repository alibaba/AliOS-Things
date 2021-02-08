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

#include <alibabacloud/multimediaai/model/ListTemplatesRequest.h>

using AlibabaCloud::Multimediaai::Model::ListTemplatesRequest;

ListTemplatesRequest::ListTemplatesRequest() :
	RpcServiceRequest("multimediaai", "2019-08-10", "ListTemplates")
{
	setMethod(HttpRequest::Method::Post);
}

ListTemplatesRequest::~ListTemplatesRequest()
{}

long ListTemplatesRequest::getTemplateId()const
{
	return templateId_;
}

void ListTemplatesRequest::setTemplateId(long templateId)
{
	templateId_ = templateId;
	setParameter("TemplateId", std::to_string(templateId));
}

int ListTemplatesRequest::getType()const
{
	return type_;
}

void ListTemplatesRequest::setType(int type)
{
	type_ = type;
	setParameter("Type", std::to_string(type));
}

int ListTemplatesRequest::getPageNumber()const
{
	return pageNumber_;
}

void ListTemplatesRequest::setPageNumber(int pageNumber)
{
	pageNumber_ = pageNumber;
	setParameter("PageNumber", std::to_string(pageNumber));
}

int ListTemplatesRequest::getPageSize()const
{
	return pageSize_;
}

void ListTemplatesRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string ListTemplatesRequest::getTemplateName()const
{
	return templateName_;
}

void ListTemplatesRequest::setTemplateName(const std::string& templateName)
{
	templateName_ = templateName;
	setParameter("TemplateName", templateName);
}

