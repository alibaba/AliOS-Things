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

#include <alibabacloud/cloudapi/model/DescribeApiDocResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

DescribeApiDocResult::DescribeApiDocResult() :
	ServiceResult()
{}

DescribeApiDocResult::DescribeApiDocResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeApiDocResult::~DescribeApiDocResult()
{}

void DescribeApiDocResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allErrorCodeSamplesNode = value["ErrorCodeSamples"]["ErrorCodeSample"];
	for (auto valueErrorCodeSamplesErrorCodeSample : allErrorCodeSamplesNode)
	{
		ErrorCodeSample errorCodeSamplesObject;
		if(!valueErrorCodeSamplesErrorCodeSample["Code"].isNull())
			errorCodeSamplesObject.code = valueErrorCodeSamplesErrorCodeSample["Code"].asString();
		if(!valueErrorCodeSamplesErrorCodeSample["Message"].isNull())
			errorCodeSamplesObject.message = valueErrorCodeSamplesErrorCodeSample["Message"].asString();
		if(!valueErrorCodeSamplesErrorCodeSample["Description"].isNull())
			errorCodeSamplesObject.description = valueErrorCodeSamplesErrorCodeSample["Description"].asString();
		errorCodeSamples_.push_back(errorCodeSamplesObject);
	}
	auto allResultDescriptionsNode = value["ResultDescriptions"]["ResultDescription"];
	for (auto valueResultDescriptionsResultDescription : allResultDescriptionsNode)
	{
		ResultDescription resultDescriptionsObject;
		if(!valueResultDescriptionsResultDescription["Id"].isNull())
			resultDescriptionsObject.id = valueResultDescriptionsResultDescription["Id"].asString();
		if(!valueResultDescriptionsResultDescription["Pid"].isNull())
			resultDescriptionsObject.pid = valueResultDescriptionsResultDescription["Pid"].asString();
		if(!valueResultDescriptionsResultDescription["HasChild"].isNull())
			resultDescriptionsObject.hasChild = valueResultDescriptionsResultDescription["HasChild"].asString() == "true";
		if(!valueResultDescriptionsResultDescription["Key"].isNull())
			resultDescriptionsObject.key = valueResultDescriptionsResultDescription["Key"].asString();
		if(!valueResultDescriptionsResultDescription["Name"].isNull())
			resultDescriptionsObject.name = valueResultDescriptionsResultDescription["Name"].asString();
		if(!valueResultDescriptionsResultDescription["Mandatory"].isNull())
			resultDescriptionsObject.mandatory = valueResultDescriptionsResultDescription["Mandatory"].asString() == "true";
		if(!valueResultDescriptionsResultDescription["Type"].isNull())
			resultDescriptionsObject.type = valueResultDescriptionsResultDescription["Type"].asString();
		if(!valueResultDescriptionsResultDescription["Description"].isNull())
			resultDescriptionsObject.description = valueResultDescriptionsResultDescription["Description"].asString();
		resultDescriptions_.push_back(resultDescriptionsObject);
	}
	auto allRequestParametersNode = value["RequestParameters"]["RequestParameter"];
	for (auto valueRequestParametersRequestParameter : allRequestParametersNode)
	{
		RequestParameter requestParametersObject;
		if(!valueRequestParametersRequestParameter["ApiParameterName"].isNull())
			requestParametersObject.apiParameterName = valueRequestParametersRequestParameter["ApiParameterName"].asString();
		if(!valueRequestParametersRequestParameter["Location"].isNull())
			requestParametersObject.location = valueRequestParametersRequestParameter["Location"].asString();
		if(!valueRequestParametersRequestParameter["ParameterType"].isNull())
			requestParametersObject.parameterType = valueRequestParametersRequestParameter["ParameterType"].asString();
		if(!valueRequestParametersRequestParameter["Required"].isNull())
			requestParametersObject.required = valueRequestParametersRequestParameter["Required"].asString();
		if(!valueRequestParametersRequestParameter["DefaultValue"].isNull())
			requestParametersObject.defaultValue = valueRequestParametersRequestParameter["DefaultValue"].asString();
		if(!valueRequestParametersRequestParameter["DemoValue"].isNull())
			requestParametersObject.demoValue = valueRequestParametersRequestParameter["DemoValue"].asString();
		if(!valueRequestParametersRequestParameter["MaxValue"].isNull())
			requestParametersObject.maxValue = std::stol(valueRequestParametersRequestParameter["MaxValue"].asString());
		if(!valueRequestParametersRequestParameter["MinValue"].isNull())
			requestParametersObject.minValue = std::stol(valueRequestParametersRequestParameter["MinValue"].asString());
		if(!valueRequestParametersRequestParameter["MaxLength"].isNull())
			requestParametersObject.maxLength = std::stol(valueRequestParametersRequestParameter["MaxLength"].asString());
		if(!valueRequestParametersRequestParameter["MinLength"].isNull())
			requestParametersObject.minLength = std::stol(valueRequestParametersRequestParameter["MinLength"].asString());
		if(!valueRequestParametersRequestParameter["RegularExpression"].isNull())
			requestParametersObject.regularExpression = valueRequestParametersRequestParameter["RegularExpression"].asString();
		if(!valueRequestParametersRequestParameter["JsonScheme"].isNull())
			requestParametersObject.jsonScheme = valueRequestParametersRequestParameter["JsonScheme"].asString();
		if(!valueRequestParametersRequestParameter["EnumValue"].isNull())
			requestParametersObject.enumValue = valueRequestParametersRequestParameter["EnumValue"].asString();
		if(!valueRequestParametersRequestParameter["DocShow"].isNull())
			requestParametersObject.docShow = valueRequestParametersRequestParameter["DocShow"].asString();
		if(!valueRequestParametersRequestParameter["DocOrder"].isNull())
			requestParametersObject.docOrder = std::stoi(valueRequestParametersRequestParameter["DocOrder"].asString());
		if(!valueRequestParametersRequestParameter["Description"].isNull())
			requestParametersObject.description = valueRequestParametersRequestParameter["Description"].asString();
		if(!valueRequestParametersRequestParameter["ArrayItemsType"].isNull())
			requestParametersObject.arrayItemsType = valueRequestParametersRequestParameter["ArrayItemsType"].asString();
		requestParameters_.push_back(requestParametersObject);
	}
	auto requestConfigNode = value["RequestConfig"];
	if(!requestConfigNode["RequestProtocol"].isNull())
		requestConfig_.requestProtocol = requestConfigNode["RequestProtocol"].asString();
	if(!requestConfigNode["RequestHttpMethod"].isNull())
		requestConfig_.requestHttpMethod = requestConfigNode["RequestHttpMethod"].asString();
	if(!requestConfigNode["RequestPath"].isNull())
		requestConfig_.requestPath = requestConfigNode["RequestPath"].asString();
	if(!requestConfigNode["BodyFormat"].isNull())
		requestConfig_.bodyFormat = requestConfigNode["BodyFormat"].asString();
	if(!requestConfigNode["PostBodyDescription"].isNull())
		requestConfig_.postBodyDescription = requestConfigNode["PostBodyDescription"].asString();
	if(!requestConfigNode["RequestMode"].isNull())
		requestConfig_.requestMode = requestConfigNode["RequestMode"].asString();
	if(!value["RegionId"].isNull())
		regionId_ = value["RegionId"].asString();
	if(!value["GroupId"].isNull())
		groupId_ = value["GroupId"].asString();
	if(!value["GroupName"].isNull())
		groupName_ = value["GroupName"].asString();
	if(!value["StageName"].isNull())
		stageName_ = value["StageName"].asString();
	if(!value["ApiId"].isNull())
		apiId_ = value["ApiId"].asString();
	if(!value["ApiName"].isNull())
		apiName_ = value["ApiName"].asString();
	if(!value["Description"].isNull())
		description_ = value["Description"].asString();
	if(!value["Visibility"].isNull())
		visibility_ = value["Visibility"].asString();
	if(!value["AuthType"].isNull())
		authType_ = value["AuthType"].asString();
	if(!value["ResultType"].isNull())
		resultType_ = value["ResultType"].asString();
	if(!value["ResultSample"].isNull())
		resultSample_ = value["ResultSample"].asString();
	if(!value["FailResultSample"].isNull())
		failResultSample_ = value["FailResultSample"].asString();
	if(!value["DeployedTime"].isNull())
		deployedTime_ = value["DeployedTime"].asString();
	if(!value["ForceNonceCheck"].isNull())
		forceNonceCheck_ = value["ForceNonceCheck"].asString() == "true";
	if(!value["DisableInternet"].isNull())
		disableInternet_ = value["DisableInternet"].asString() == "true";

}

std::string DescribeApiDocResult::getGroupName()const
{
	return groupName_;
}

DescribeApiDocResult::RequestConfig DescribeApiDocResult::getRequestConfig()const
{
	return requestConfig_;
}

std::string DescribeApiDocResult::getDescription()const
{
	return description_;
}

std::string DescribeApiDocResult::getResultSample()const
{
	return resultSample_;
}

bool DescribeApiDocResult::getDisableInternet()const
{
	return disableInternet_;
}

std::string DescribeApiDocResult::getStageName()const
{
	return stageName_;
}

std::string DescribeApiDocResult::getApiName()const
{
	return apiName_;
}

bool DescribeApiDocResult::getForceNonceCheck()const
{
	return forceNonceCheck_;
}

std::string DescribeApiDocResult::getResultType()const
{
	return resultType_;
}

std::string DescribeApiDocResult::getFailResultSample()const
{
	return failResultSample_;
}

std::vector<DescribeApiDocResult::ErrorCodeSample> DescribeApiDocResult::getErrorCodeSamples()const
{
	return errorCodeSamples_;
}

std::vector<DescribeApiDocResult::RequestParameter> DescribeApiDocResult::getRequestParameters()const
{
	return requestParameters_;
}

std::string DescribeApiDocResult::getGroupId()const
{
	return groupId_;
}

std::string DescribeApiDocResult::getDeployedTime()const
{
	return deployedTime_;
}

std::vector<DescribeApiDocResult::ResultDescription> DescribeApiDocResult::getResultDescriptions()const
{
	return resultDescriptions_;
}

std::string DescribeApiDocResult::getVisibility()const
{
	return visibility_;
}

std::string DescribeApiDocResult::getRegionId()const
{
	return regionId_;
}

std::string DescribeApiDocResult::getAuthType()const
{
	return authType_;
}

std::string DescribeApiDocResult::getApiId()const
{
	return apiId_;
}

