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

#include <alibabacloud/iot/model/RRpcResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

RRpcResult::RRpcResult() :
	ServiceResult()
{}

RRpcResult::RRpcResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

RRpcResult::~RRpcResult()
{}

void RRpcResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();
	if(!value["RrpcCode"].isNull())
		rrpcCode_ = value["RrpcCode"].asString();
	if(!value["PayloadBase64Byte"].isNull())
		payloadBase64Byte_ = value["PayloadBase64Byte"].asString();
	if(!value["MessageId"].isNull())
		messageId_ = std::stoull(value["MessageId"].asString());

}

std::string RRpcResult::getPayloadBase64Byte()const
{
	return payloadBase64Byte_;
}

std::string RRpcResult::getRrpcCode()const
{
	return rrpcCode_;
}

std::string RRpcResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string RRpcResult::getCode()const
{
	return code_;
}

bool RRpcResult::getSuccess()const
{
	return success_;
}

long RRpcResult::getMessageId()const
{
	return messageId_;
}

