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

#include <alibabacloud/metering/model/SyncDataResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Metering;
using namespace AlibabaCloud::Metering::Model;

SyncDataResult::SyncDataResult() :
	ServiceResult()
{}

SyncDataResult::SyncDataResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

SyncDataResult::~SyncDataResult()
{}

void SyncDataResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	if(!value["Code"].isNull())
		code_ = std::stoi(value["Code"].asString());
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Data"].isNull())
		data_ = value["Data"].asString() == "true";
	if(!value["ErrCode"].isNull())
		errCode_ = std::stoi(value["ErrCode"].asString());
	if(!value["ErrMessage"].isNull())
		errMessage_ = value["ErrMessage"].asString();

}

bool SyncDataResult::getData()const
{
	return data_;
}

std::string SyncDataResult::getErrMessage()const
{
	return errMessage_;
}

int SyncDataResult::getCode()const
{
	return code_;
}

bool SyncDataResult::getSuccess()const
{
	return success_;
}

int SyncDataResult::getErrCode()const
{
	return errCode_;
}

