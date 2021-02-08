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

#include <alibabacloud/imm/model/UpdateDocIndexMetaRequest.h>

using AlibabaCloud::Imm::Model::UpdateDocIndexMetaRequest;

UpdateDocIndexMetaRequest::UpdateDocIndexMetaRequest() :
	RpcServiceRequest("imm", "2017-09-06", "UpdateDocIndexMeta")
{
	setMethod(HttpRequest::Method::Post);
}

UpdateDocIndexMetaRequest::~UpdateDocIndexMetaRequest()
{}

std::string UpdateDocIndexMetaRequest::getProject()const
{
	return project_;
}

void UpdateDocIndexMetaRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string UpdateDocIndexMetaRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void UpdateDocIndexMetaRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string UpdateDocIndexMetaRequest::getUniqueId()const
{
	return uniqueId_;
}

void UpdateDocIndexMetaRequest::setUniqueId(const std::string& uniqueId)
{
	uniqueId_ = uniqueId;
	setParameter("UniqueId", uniqueId);
}

std::string UpdateDocIndexMetaRequest::getCustomKey1()const
{
	return customKey1_;
}

void UpdateDocIndexMetaRequest::setCustomKey1(const std::string& customKey1)
{
	customKey1_ = customKey1;
	setParameter("CustomKey1", customKey1);
}

std::string UpdateDocIndexMetaRequest::getSet()const
{
	return set_;
}

void UpdateDocIndexMetaRequest::setSet(const std::string& set)
{
	set_ = set;
	setParameter("Set", set);
}

std::string UpdateDocIndexMetaRequest::getCustomKey5()const
{
	return customKey5_;
}

void UpdateDocIndexMetaRequest::setCustomKey5(const std::string& customKey5)
{
	customKey5_ = customKey5;
	setParameter("CustomKey5", customKey5);
}

std::string UpdateDocIndexMetaRequest::getCustomKey4()const
{
	return customKey4_;
}

void UpdateDocIndexMetaRequest::setCustomKey4(const std::string& customKey4)
{
	customKey4_ = customKey4;
	setParameter("CustomKey4", customKey4);
}

std::string UpdateDocIndexMetaRequest::getCustomKey3()const
{
	return customKey3_;
}

void UpdateDocIndexMetaRequest::setCustomKey3(const std::string& customKey3)
{
	customKey3_ = customKey3;
	setParameter("CustomKey3", customKey3);
}

std::string UpdateDocIndexMetaRequest::getCustomKey2()const
{
	return customKey2_;
}

void UpdateDocIndexMetaRequest::setCustomKey2(const std::string& customKey2)
{
	customKey2_ = customKey2;
	setParameter("CustomKey2", customKey2);
}

std::string UpdateDocIndexMetaRequest::getCustomKey6()const
{
	return customKey6_;
}

void UpdateDocIndexMetaRequest::setCustomKey6(const std::string& customKey6)
{
	customKey6_ = customKey6;
	setParameter("CustomKey6", customKey6);
}

std::string UpdateDocIndexMetaRequest::getName()const
{
	return name_;
}

void UpdateDocIndexMetaRequest::setName(const std::string& name)
{
	name_ = name;
	setParameter("Name", name);
}

