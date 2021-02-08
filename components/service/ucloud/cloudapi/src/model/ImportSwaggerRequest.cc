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

#include <alibabacloud/cloudapi/model/ImportSwaggerRequest.h>

using AlibabaCloud::CloudAPI::Model::ImportSwaggerRequest;

ImportSwaggerRequest::ImportSwaggerRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "ImportSwagger")
{
	setMethod(HttpRequest::Method::Post);
}

ImportSwaggerRequest::~ImportSwaggerRequest()
{}

std::string ImportSwaggerRequest::getDataFormat()const
{
	return dataFormat_;
}

void ImportSwaggerRequest::setDataFormat(const std::string& dataFormat)
{
	dataFormat_ = dataFormat;
	setParameter("DataFormat", dataFormat);
}

std::string ImportSwaggerRequest::getData()const
{
	return data_;
}

void ImportSwaggerRequest::setData(const std::string& data)
{
	data_ = data;
	setBodyParameter("Data", data);
}

std::string ImportSwaggerRequest::getGroupId()const
{
	return groupId_;
}

void ImportSwaggerRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

bool ImportSwaggerRequest::getOverwrite()const
{
	return overwrite_;
}

void ImportSwaggerRequest::setOverwrite(bool overwrite)
{
	overwrite_ = overwrite;
	setParameter("Overwrite", overwrite ? "true" : "false");
}

