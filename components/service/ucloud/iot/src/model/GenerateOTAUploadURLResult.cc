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

#include <alibabacloud/iot/model/GenerateOTAUploadURLResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

GenerateOTAUploadURLResult::GenerateOTAUploadURLResult() :
	ServiceResult()
{}

GenerateOTAUploadURLResult::GenerateOTAUploadURLResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GenerateOTAUploadURLResult::~GenerateOTAUploadURLResult()
{}

void GenerateOTAUploadURLResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["Key"].isNull())
		data_.key = dataNode["Key"].asString();
	if(!dataNode["Host"].isNull())
		data_.host = dataNode["Host"].asString();
	if(!dataNode["Policy"].isNull())
		data_.policy = dataNode["Policy"].asString();
	if(!dataNode["OSSAccessKeyId"].isNull())
		data_.oSSAccessKeyId = dataNode["OSSAccessKeyId"].asString();
	if(!dataNode["Signature"].isNull())
		data_.signature = dataNode["Signature"].asString();
	if(!dataNode["FirmwareUrl"].isNull())
		data_.firmwareUrl = dataNode["FirmwareUrl"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

GenerateOTAUploadURLResult::Data GenerateOTAUploadURLResult::getData()const
{
	return data_;
}

std::string GenerateOTAUploadURLResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string GenerateOTAUploadURLResult::getCode()const
{
	return code_;
}

bool GenerateOTAUploadURLResult::getSuccess()const
{
	return success_;
}

