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

#include <alibabacloud/alinlp/model/GetWeChEcomRequest.h>

using AlibabaCloud::Alinlp::Model::GetWeChEcomRequest;

GetWeChEcomRequest::GetWeChEcomRequest() :
	RpcServiceRequest("alinlp", "2020-06-29", "GetWeChEcom")
{
	setMethod(HttpRequest::Method::Post);
}

GetWeChEcomRequest::~GetWeChEcomRequest()
{}

std::string GetWeChEcomRequest::getWordVectorDelimiter()const
{
	return wordVectorDelimiter_;
}

void GetWeChEcomRequest::setWordVectorDelimiter(const std::string& wordVectorDelimiter)
{
	wordVectorDelimiter_ = wordVectorDelimiter;
	setBodyParameter("WordVectorDelimiter", wordVectorDelimiter);
}

std::string GetWeChEcomRequest::getType()const
{
	return type_;
}

void GetWeChEcomRequest::setType(const std::string& type)
{
	type_ = type;
	setBodyParameter("Type", type);
}

std::string GetWeChEcomRequest::getUuid()const
{
	return uuid_;
}

void GetWeChEcomRequest::setUuid(const std::string& uuid)
{
	uuid_ = uuid;
	setBodyParameter("Uuid", uuid);
}

std::string GetWeChEcomRequest::getDelimiter()const
{
	return delimiter_;
}

void GetWeChEcomRequest::setDelimiter(const std::string& delimiter)
{
	delimiter_ = delimiter;
	setBodyParameter("Delimiter", delimiter);
}

std::string GetWeChEcomRequest::getText()const
{
	return text_;
}

void GetWeChEcomRequest::setText(const std::string& text)
{
	text_ = text;
	setBodyParameter("Text", text);
}

std::string GetWeChEcomRequest::getToken()const
{
	return token_;
}

void GetWeChEcomRequest::setToken(const std::string& token)
{
	token_ = token;
	setBodyParameter("Token", token);
}

std::string GetWeChEcomRequest::getServiceCode()const
{
	return serviceCode_;
}

void GetWeChEcomRequest::setServiceCode(const std::string& serviceCode)
{
	serviceCode_ = serviceCode;
	setBodyParameter("ServiceCode", serviceCode);
}

std::string GetWeChEcomRequest::getSize()const
{
	return size_;
}

void GetWeChEcomRequest::setSize(const std::string& size)
{
	size_ = size;
	setBodyParameter("Size", size);
}

std::string GetWeChEcomRequest::getWordDelimiter()const
{
	return wordDelimiter_;
}

void GetWeChEcomRequest::setWordDelimiter(const std::string& wordDelimiter)
{
	wordDelimiter_ = wordDelimiter;
	setBodyParameter("WordDelimiter", wordDelimiter);
}

std::string GetWeChEcomRequest::getOperation()const
{
	return operation_;
}

void GetWeChEcomRequest::setOperation(const std::string& operation)
{
	operation_ = operation;
	setBodyParameter("Operation", operation);
}

