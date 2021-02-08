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

#include <alibabacloud/cloudphoto/model/RegisterTagRequest.h>

using AlibabaCloud::CloudPhoto::Model::RegisterTagRequest;

RegisterTagRequest::RegisterTagRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "RegisterTag")
{
	setMethod(HttpRequest::Method::Post);
}

RegisterTagRequest::~RegisterTagRequest()
{}

std::string RegisterTagRequest::getStoreName()const
{
	return storeName_;
}

void RegisterTagRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}

std::string RegisterTagRequest::getText()const
{
	return text_;
}

void RegisterTagRequest::setText(const std::string& text)
{
	text_ = text;
	setParameter("Text", text);
}

std::string RegisterTagRequest::getTagKey()const
{
	return tagKey_;
}

void RegisterTagRequest::setTagKey(const std::string& tagKey)
{
	tagKey_ = tagKey;
	setParameter("TagKey", tagKey);
}

std::string RegisterTagRequest::getLang()const
{
	return lang_;
}

void RegisterTagRequest::setLang(const std::string& lang)
{
	lang_ = lang;
	setParameter("Lang", lang);
}

