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

#include <alibabacloud/iot/model/GetDataAPIServiceDetailResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

GetDataAPIServiceDetailResult::GetDataAPIServiceDetailResult() :
	ServiceResult()
{}

GetDataAPIServiceDetailResult::GetDataAPIServiceDetailResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetDataAPIServiceDetailResult::~GetDataAPIServiceDetailResult()
{}

void GetDataAPIServiceDetailResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["ApiSrn"].isNull())
		data_.apiSrn = dataNode["ApiSrn"].asString();
	if(!dataNode["Status"].isNull())
		data_.status = std::stoi(dataNode["Status"].asString());
	if(!dataNode["DisplayName"].isNull())
		data_.displayName = dataNode["DisplayName"].asString();
	if(!dataNode["ApiPath"].isNull())
		data_.apiPath = dataNode["ApiPath"].asString();
	if(!dataNode["CreateTime"].isNull())
		data_.createTime = std::stol(dataNode["CreateTime"].asString());
	if(!dataNode["LastUpdateTime"].isNull())
		data_.lastUpdateTime = std::stol(dataNode["LastUpdateTime"].asString());
	if(!dataNode["DateFormat"].isNull())
		data_.dateFormat = dataNode["DateFormat"].asString();
	if(!dataNode["RequestMethod"].isNull())
		data_.requestMethod = dataNode["RequestMethod"].asString();
	if(!dataNode["RequestProtocol"].isNull())
		data_.requestProtocol = dataNode["RequestProtocol"].asString();
	if(!dataNode["Description"].isNull())
		data_.description = dataNode["Description"].asString();
	auto sqlTemplateDTONode = dataNode["SqlTemplateDTO"];
	if(!sqlTemplateDTONode["OriginSql"].isNull())
		data_.sqlTemplateDTO.originSql = sqlTemplateDTONode["OriginSql"].asString();
	if(!sqlTemplateDTONode["TemplateSql"].isNull())
		data_.sqlTemplateDTO.templateSql = sqlTemplateDTONode["TemplateSql"].asString();
	auto allRequestParamsNode = sqlTemplateDTONode["RequestParams"]["RequestParamsItem"];
	for (auto sqlTemplateDTONodeRequestParamsRequestParamsItem : allRequestParamsNode)
	{
		Data::SqlTemplateDTO::RequestParamsItem requestParamsItemObject;
		if(!sqlTemplateDTONodeRequestParamsRequestParamsItem["Name"].isNull())
			requestParamsItemObject.name = sqlTemplateDTONodeRequestParamsRequestParamsItem["Name"].asString();
		if(!sqlTemplateDTONodeRequestParamsRequestParamsItem["Type"].isNull())
			requestParamsItemObject.type = sqlTemplateDTONodeRequestParamsRequestParamsItem["Type"].asString();
		if(!sqlTemplateDTONodeRequestParamsRequestParamsItem["Desc"].isNull())
			requestParamsItemObject.desc = sqlTemplateDTONodeRequestParamsRequestParamsItem["Desc"].asString();
		if(!sqlTemplateDTONodeRequestParamsRequestParamsItem["Example"].isNull())
			requestParamsItemObject.example = sqlTemplateDTONodeRequestParamsRequestParamsItem["Example"].asString();
		if(!sqlTemplateDTONodeRequestParamsRequestParamsItem["Required"].isNull())
			requestParamsItemObject.required = sqlTemplateDTONodeRequestParamsRequestParamsItem["Required"].asString() == "true";
		data_.sqlTemplateDTO.requestParams.push_back(requestParamsItemObject);
	}
	auto allResponseParamsNode = sqlTemplateDTONode["ResponseParams"]["ResponseParamsItem"];
	for (auto sqlTemplateDTONodeResponseParamsResponseParamsItem : allResponseParamsNode)
	{
		Data::SqlTemplateDTO::ResponseParamsItem responseParamsItemObject;
		if(!sqlTemplateDTONodeResponseParamsResponseParamsItem["Name"].isNull())
			responseParamsItemObject.name = sqlTemplateDTONodeResponseParamsResponseParamsItem["Name"].asString();
		if(!sqlTemplateDTONodeResponseParamsResponseParamsItem["Type"].isNull())
			responseParamsItemObject.type = sqlTemplateDTONodeResponseParamsResponseParamsItem["Type"].asString();
		if(!sqlTemplateDTONodeResponseParamsResponseParamsItem["Desc"].isNull())
			responseParamsItemObject.desc = sqlTemplateDTONodeResponseParamsResponseParamsItem["Desc"].asString();
		if(!sqlTemplateDTONodeResponseParamsResponseParamsItem["Example"].isNull())
			responseParamsItemObject.example = sqlTemplateDTONodeResponseParamsResponseParamsItem["Example"].asString();
		if(!sqlTemplateDTONodeResponseParamsResponseParamsItem["Required"].isNull())
			responseParamsItemObject.required = sqlTemplateDTONodeResponseParamsResponseParamsItem["Required"].asString() == "true";
		data_.sqlTemplateDTO.responseParams.push_back(responseParamsItemObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

GetDataAPIServiceDetailResult::Data GetDataAPIServiceDetailResult::getData()const
{
	return data_;
}

std::string GetDataAPIServiceDetailResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string GetDataAPIServiceDetailResult::getCode()const
{
	return code_;
}

bool GetDataAPIServiceDetailResult::getSuccess()const
{
	return success_;
}

