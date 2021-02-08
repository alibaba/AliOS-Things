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

#include <alibabacloud/aliyuncvc/model/ActiveDeviceResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Aliyuncvc;
using namespace AlibabaCloud::Aliyuncvc::Model;

ActiveDeviceResult::ActiveDeviceResult() :
	ServiceResult()
{}

ActiveDeviceResult::ActiveDeviceResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ActiveDeviceResult::~ActiveDeviceResult()
{}

void ActiveDeviceResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	if(!value["ErrorCode"].isNull())
		errorCode_ = std::stoi(value["ErrorCode"].asString());
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Token"].isNull())
		token_ = value["Token"].asString();

}

std::string ActiveDeviceResult::getMessage()const
{
	return message_;
}

std::string ActiveDeviceResult::getToken()const
{
	return token_;
}

int ActiveDeviceResult::getErrorCode()const
{
	return errorCode_;
}

bool ActiveDeviceResult::getSuccess()const
{
	return success_;
}

