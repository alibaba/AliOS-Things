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

#include <alibabacloud/cloudapi/model/ImportSwaggerResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

ImportSwaggerResult::ImportSwaggerResult() :
	ServiceResult()
{}

ImportSwaggerResult::ImportSwaggerResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ImportSwaggerResult::~ImportSwaggerResult()
{}

void ImportSwaggerResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allSuccessNode = value["Success"]["ApiImportSwaggerSuccess"];
	for (auto valueSuccessApiImportSwaggerSuccess : allSuccessNode)
	{
		ApiImportSwaggerSuccess successObject;
		if(!valueSuccessApiImportSwaggerSuccess["Path"].isNull())
			successObject.path = valueSuccessApiImportSwaggerSuccess["Path"].asString();
		if(!valueSuccessApiImportSwaggerSuccess["HttpMethod"].isNull())
			successObject.httpMethod = valueSuccessApiImportSwaggerSuccess["HttpMethod"].asString();
		if(!valueSuccessApiImportSwaggerSuccess["ApiUid"].isNull())
			successObject.apiUid = valueSuccessApiImportSwaggerSuccess["ApiUid"].asString();
		if(!valueSuccessApiImportSwaggerSuccess["ApiOperation"].isNull())
			successObject.apiOperation = valueSuccessApiImportSwaggerSuccess["ApiOperation"].asString();
		success_.push_back(successObject);
	}
	auto allFailedNode = value["Failed"]["ApiImportSwaggerFailed"];
	for (auto valueFailedApiImportSwaggerFailed : allFailedNode)
	{
		ApiImportSwaggerFailed failedObject;
		if(!valueFailedApiImportSwaggerFailed["Path"].isNull())
			failedObject.path = valueFailedApiImportSwaggerFailed["Path"].asString();
		if(!valueFailedApiImportSwaggerFailed["HttpMethod"].isNull())
			failedObject.httpMethod = valueFailedApiImportSwaggerFailed["HttpMethod"].asString();
		if(!valueFailedApiImportSwaggerFailed["ErrorMsg"].isNull())
			failedObject.errorMsg = valueFailedApiImportSwaggerFailed["ErrorMsg"].asString();
		failed_.push_back(failedObject);
	}
	auto allModelFailedNode = value["ModelFailed"]["ApiImportModelFailed"];
	for (auto valueModelFailedApiImportModelFailed : allModelFailedNode)
	{
		ApiImportModelFailed modelFailedObject;
		if(!valueModelFailedApiImportModelFailed["GroupId"].isNull())
			modelFailedObject.groupId = valueModelFailedApiImportModelFailed["GroupId"].asString();
		if(!valueModelFailedApiImportModelFailed["ModelName"].isNull())
			modelFailedObject.modelName = valueModelFailedApiImportModelFailed["ModelName"].asString();
		if(!valueModelFailedApiImportModelFailed["ErrorMsg"].isNull())
			modelFailedObject.errorMsg = valueModelFailedApiImportModelFailed["ErrorMsg"].asString();
		modelFailed_.push_back(modelFailedObject);
	}
	auto allModelSuccessNode = value["ModelSuccess"]["ApiImportModelSuccess"];
	for (auto valueModelSuccessApiImportModelSuccess : allModelSuccessNode)
	{
		ApiImportModelSuccess modelSuccessObject;
		if(!valueModelSuccessApiImportModelSuccess["ModelUid"].isNull())
			modelSuccessObject.modelUid = valueModelSuccessApiImportModelSuccess["ModelUid"].asString();
		if(!valueModelSuccessApiImportModelSuccess["ModelOperation"].isNull())
			modelSuccessObject.modelOperation = valueModelSuccessApiImportModelSuccess["ModelOperation"].asString();
		if(!valueModelSuccessApiImportModelSuccess["GroupId"].isNull())
			modelSuccessObject.groupId = valueModelSuccessApiImportModelSuccess["GroupId"].asString();
		if(!valueModelSuccessApiImportModelSuccess["ModelName"].isNull())
			modelSuccessObject.modelName = valueModelSuccessApiImportModelSuccess["ModelName"].asString();
		modelSuccess_.push_back(modelSuccessObject);
	}

}

std::vector<ImportSwaggerResult::ApiImportSwaggerFailed> ImportSwaggerResult::getFailed()const
{
	return failed_;
}

std::vector<ImportSwaggerResult::ApiImportModelFailed> ImportSwaggerResult::getModelFailed()const
{
	return modelFailed_;
}

std::vector<ImportSwaggerResult::ApiImportSwaggerSuccess> ImportSwaggerResult::getSuccess()const
{
	return success_;
}

std::vector<ImportSwaggerResult::ApiImportModelSuccess> ImportSwaggerResult::getModelSuccess()const
{
	return modelSuccess_;
}

