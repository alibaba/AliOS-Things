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

#include <alibabacloud/imm/model/UpdateImageResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imm;
using namespace AlibabaCloud::Imm::Model;

UpdateImageResult::UpdateImageResult() :
	ServiceResult()
{}

UpdateImageResult::UpdateImageResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

UpdateImageResult::~UpdateImageResult()
{}

void UpdateImageResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	if(!value["SetId"].isNull())
		setId_ = value["SetId"].asString();
	if(!value["ImageUri"].isNull())
		imageUri_ = value["ImageUri"].asString();
	if(!value["RemarksA"].isNull())
		remarksA_ = value["RemarksA"].asString();
	if(!value["RemarksB"].isNull())
		remarksB_ = value["RemarksB"].asString();
	if(!value["CreateTime"].isNull())
		createTime_ = value["CreateTime"].asString();
	if(!value["ModifyTime"].isNull())
		modifyTime_ = value["ModifyTime"].asString();
	if(!value["RemarksC"].isNull())
		remarksC_ = value["RemarksC"].asString();
	if(!value["RemarksD"].isNull())
		remarksD_ = value["RemarksD"].asString();
	if(!value["ExternalId"].isNull())
		externalId_ = value["ExternalId"].asString();
	if(!value["RemarksArrayA"].isNull())
		remarksArrayA_ = value["RemarksArrayA"].asString();
	if(!value["RemarksArrayB"].isNull())
		remarksArrayB_ = value["RemarksArrayB"].asString();

}

std::string UpdateImageResult::getRemarksArrayB()const
{
	return remarksArrayB_;
}

std::string UpdateImageResult::getModifyTime()const
{
	return modifyTime_;
}

std::string UpdateImageResult::getRemarksC()const
{
	return remarksC_;
}

std::string UpdateImageResult::getRemarksD()const
{
	return remarksD_;
}

std::string UpdateImageResult::getCreateTime()const
{
	return createTime_;
}

std::string UpdateImageResult::getExternalId()const
{
	return externalId_;
}

std::string UpdateImageResult::getRemarksArrayA()const
{
	return remarksArrayA_;
}

std::string UpdateImageResult::getSetId()const
{
	return setId_;
}

std::string UpdateImageResult::getImageUri()const
{
	return imageUri_;
}

std::string UpdateImageResult::getRemarksA()const
{
	return remarksA_;
}

std::string UpdateImageResult::getRemarksB()const
{
	return remarksB_;
}

