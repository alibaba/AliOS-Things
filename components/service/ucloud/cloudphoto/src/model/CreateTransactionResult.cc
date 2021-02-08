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

#include <alibabacloud/cloudphoto/model/CreateTransactionResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudPhoto;
using namespace AlibabaCloud::CloudPhoto::Model;

CreateTransactionResult::CreateTransactionResult() :
	ServiceResult()
{}

CreateTransactionResult::CreateTransactionResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

CreateTransactionResult::~CreateTransactionResult()
{}

void CreateTransactionResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto transactionNode = value["Transaction"];
	auto uploadNode = transactionNode["Upload"];
	if(!uploadNode["Bucket"].isNull())
		transaction_.upload.bucket = uploadNode["Bucket"].asString();
	if(!uploadNode["FileId"].isNull())
		transaction_.upload.fileId = uploadNode["FileId"].asString();
	if(!uploadNode["OssEndpoint"].isNull())
		transaction_.upload.ossEndpoint = uploadNode["OssEndpoint"].asString();
	if(!uploadNode["ObjectKey"].isNull())
		transaction_.upload.objectKey = uploadNode["ObjectKey"].asString();
	if(!uploadNode["SessionId"].isNull())
		transaction_.upload.sessionId = uploadNode["SessionId"].asString();
	if(!uploadNode["AccessKeyId"].isNull())
		transaction_.upload.accessKeyId = uploadNode["AccessKeyId"].asString();
	if(!uploadNode["AccessKeySecret"].isNull())
		transaction_.upload.accessKeySecret = uploadNode["AccessKeySecret"].asString();
	if(!uploadNode["StsToken"].isNull())
		transaction_.upload.stsToken = uploadNode["StsToken"].asString();
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Action"].isNull())
		action_ = value["Action"].asString();

}

std::string CreateTransactionResult::getAction()const
{
	return action_;
}

CreateTransactionResult::Transaction CreateTransactionResult::getTransaction()const
{
	return transaction_;
}

std::string CreateTransactionResult::getMessage()const
{
	return message_;
}

std::string CreateTransactionResult::getCode()const
{
	return code_;
}

