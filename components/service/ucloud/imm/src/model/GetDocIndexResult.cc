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

#include <alibabacloud/imm/model/GetDocIndexResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

GetDocIndexResult::GetDocIndexResult() :
	ServiceResult()
{}

GetDocIndexResult::GetDocIndexResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetDocIndexResult::~GetDocIndexResult()
{}

void GetDocIndexResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	if(!value["IndexCreatedTime"].isNull())
		indexCreatedTime_ = value["IndexCreatedTime"].asString();
	if(!value["IndexModifiedTime"].isNull())
		indexModifiedTime_ = value["IndexModifiedTime"].asString();
	if(!value["UniqueId"].isNull())
		uniqueId_ = value["UniqueId"].asString();
	if(!value["SrcUri"].isNull())
		srcUri_ = value["SrcUri"].asString();
	if(!value["Name"].isNull())
		name_ = value["Name"].asString();
	if(!value["ContentType"].isNull())
		contentType_ = value["ContentType"].asString();
	if(!value["LastModified"].isNull())
		lastModified_ = value["LastModified"].asString();
	if(!value["Size"].isNull())
		size_ = std::stol(value["Size"].asString());
	if(!value["PageNum"].isNull())
		pageNum_ = std::stol(value["PageNum"].asString());
	if(!value["CustomKey1"].isNull())
		customKey1_ = value["CustomKey1"].asString();
	if(!value["CustomKey2"].isNull())
		customKey2_ = value["CustomKey2"].asString();
	if(!value["CustomKey3"].isNull())
		customKey3_ = value["CustomKey3"].asString();
	if(!value["CustomKey4"].isNull())
		customKey4_ = value["CustomKey4"].asString();
	if(!value["CustomKey5"].isNull())
		customKey5_ = value["CustomKey5"].asString();
	if(!value["CustomKey6"].isNull())
		customKey6_ = value["CustomKey6"].asString();

}

std::string GetDocIndexResult::getCustomKey4()const
{
	return customKey4_;
}

std::string GetDocIndexResult::getUniqueId()const
{
	return uniqueId_;
}

std::string GetDocIndexResult::getCustomKey5()const
{
	return customKey5_;
}

std::string GetDocIndexResult::getCustomKey2()const
{
	return customKey2_;
}

std::string GetDocIndexResult::getCustomKey3()const
{
	return customKey3_;
}

std::string GetDocIndexResult::getContentType()const
{
	return contentType_;
}

long GetDocIndexResult::getPageNum()const
{
	return pageNum_;
}

std::string GetDocIndexResult::getCustomKey1()const
{
	return customKey1_;
}

long GetDocIndexResult::getSize()const
{
	return size_;
}

std::string GetDocIndexResult::getIndexModifiedTime()const
{
	return indexModifiedTime_;
}

std::string GetDocIndexResult::getIndexCreatedTime()const
{
	return indexCreatedTime_;
}

std::string GetDocIndexResult::getCustomKey6()const
{
	return customKey6_;
}

std::string GetDocIndexResult::getName()const
{
	return name_;
}

std::string GetDocIndexResult::getLastModified()const
{
	return lastModified_;
}

std::string GetDocIndexResult::getSrcUri()const
{
	return srcUri_;
}

