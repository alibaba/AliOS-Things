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

#include <alibabacloud/cloudapi/model/ModifyApiRequest.h>

using AlibabaCloud::CloudAPI::Model::ModifyApiRequest;

ModifyApiRequest::ModifyApiRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "ModifyApi")
{
	setMethod(HttpRequest::Method::Post);
}

ModifyApiRequest::~ModifyApiRequest()
{}

std::string ModifyApiRequest::getWebSocketApiType()const
{
	return webSocketApiType_;
}

void ModifyApiRequest::setWebSocketApiType(const std::string& webSocketApiType)
{
	webSocketApiType_ = webSocketApiType;
	setParameter("WebSocketApiType", webSocketApiType);
}

std::string ModifyApiRequest::getErrorCodeSamples()const
{
	return errorCodeSamples_;
}

void ModifyApiRequest::setErrorCodeSamples(const std::string& errorCodeSamples)
{
	errorCodeSamples_ = errorCodeSamples;
	setParameter("ErrorCodeSamples", errorCodeSamples);
}

std::string ModifyApiRequest::getAppCodeAuthType()const
{
	return appCodeAuthType_;
}

void ModifyApiRequest::setAppCodeAuthType(const std::string& appCodeAuthType)
{
	appCodeAuthType_ = appCodeAuthType;
	setParameter("AppCodeAuthType", appCodeAuthType);
}

std::string ModifyApiRequest::getDescription()const
{
	return description_;
}

void ModifyApiRequest::setDescription(const std::string& description)
{
	description_ = description;
	setParameter("Description", description);
}

bool ModifyApiRequest::getDisableInternet()const
{
	return disableInternet_;
}

void ModifyApiRequest::setDisableInternet(bool disableInternet)
{
	disableInternet_ = disableInternet;
	setParameter("DisableInternet", disableInternet ? "true" : "false");
}

std::string ModifyApiRequest::getConstantParameters()const
{
	return constantParameters_;
}

void ModifyApiRequest::setConstantParameters(const std::string& constantParameters)
{
	constantParameters_ = constantParameters;
	setParameter("ConstantParameters", constantParameters);
}

std::string ModifyApiRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void ModifyApiRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string ModifyApiRequest::getAuthType()const
{
	return authType_;
}

void ModifyApiRequest::setAuthType(const std::string& authType)
{
	authType_ = authType;
	setParameter("AuthType", authType);
}

std::string ModifyApiRequest::getAllowSignatureMethod()const
{
	return allowSignatureMethod_;
}

void ModifyApiRequest::setAllowSignatureMethod(const std::string& allowSignatureMethod)
{
	allowSignatureMethod_ = allowSignatureMethod;
	setParameter("AllowSignatureMethod", allowSignatureMethod);
}

std::string ModifyApiRequest::getServiceParameters()const
{
	return serviceParameters_;
}

void ModifyApiRequest::setServiceParameters(const std::string& serviceParameters)
{
	serviceParameters_ = serviceParameters;
	setParameter("ServiceParameters", serviceParameters);
}

std::string ModifyApiRequest::getFailResultSample()const
{
	return failResultSample_;
}

void ModifyApiRequest::setFailResultSample(const std::string& failResultSample)
{
	failResultSample_ = failResultSample;
	setParameter("FailResultSample", failResultSample);
}

std::string ModifyApiRequest::getResourceOwnerToken()const
{
	return resourceOwnerToken_;
}

void ModifyApiRequest::setResourceOwnerToken(const std::string& resourceOwnerToken)
{
	resourceOwnerToken_ = resourceOwnerToken;
	setParameter("ResourceOwnerToken", resourceOwnerToken);
}

std::string ModifyApiRequest::getSystemParameters()const
{
	return systemParameters_;
}

void ModifyApiRequest::setSystemParameters(const std::string& systemParameters)
{
	systemParameters_ = systemParameters;
	setParameter("SystemParameters", systemParameters);
}

std::string ModifyApiRequest::getServiceParametersMap()const
{
	return serviceParametersMap_;
}

void ModifyApiRequest::setServiceParametersMap(const std::string& serviceParametersMap)
{
	serviceParametersMap_ = serviceParametersMap;
	setParameter("ServiceParametersMap", serviceParametersMap);
}

std::string ModifyApiRequest::getSecurityToken()const
{
	return securityToken_;
}

void ModifyApiRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string ModifyApiRequest::getOpenIdConnectConfig()const
{
	return openIdConnectConfig_;
}

void ModifyApiRequest::setOpenIdConnectConfig(const std::string& openIdConnectConfig)
{
	openIdConnectConfig_ = openIdConnectConfig;
	setParameter("OpenIdConnectConfig", openIdConnectConfig);
}

std::string ModifyApiRequest::getRequestParameters()const
{
	return requestParameters_;
}

void ModifyApiRequest::setRequestParameters(const std::string& requestParameters)
{
	requestParameters_ = requestParameters;
	setParameter("RequestParameters", requestParameters);
}

std::string ModifyApiRequest::getResultDescriptions()const
{
	return resultDescriptions_;
}

void ModifyApiRequest::setResultDescriptions(const std::string& resultDescriptions)
{
	resultDescriptions_ = resultDescriptions;
	setParameter("ResultDescriptions", resultDescriptions);
}

std::string ModifyApiRequest::getVisibility()const
{
	return visibility_;
}

void ModifyApiRequest::setVisibility(const std::string& visibility)
{
	visibility_ = visibility;
	setParameter("Visibility", visibility);
}

std::string ModifyApiRequest::getGroupId()const
{
	return groupId_;
}

void ModifyApiRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string ModifyApiRequest::getServiceConfig()const
{
	return serviceConfig_;
}

void ModifyApiRequest::setServiceConfig(const std::string& serviceConfig)
{
	serviceConfig_ = serviceConfig;
	setParameter("ServiceConfig", serviceConfig);
}

std::string ModifyApiRequest::getResultType()const
{
	return resultType_;
}

void ModifyApiRequest::setResultType(const std::string& resultType)
{
	resultType_ = resultType;
	setParameter("ResultType", resultType);
}

std::string ModifyApiRequest::getApiName()const
{
	return apiName_;
}

void ModifyApiRequest::setApiName(const std::string& apiName)
{
	apiName_ = apiName;
	setParameter("ApiName", apiName);
}

std::string ModifyApiRequest::getResultSample()const
{
	return resultSample_;
}

void ModifyApiRequest::setResultSample(const std::string& resultSample)
{
	resultSample_ = resultSample;
	setParameter("ResultSample", resultSample);
}

bool ModifyApiRequest::getForceNonceCheck()const
{
	return forceNonceCheck_;
}

void ModifyApiRequest::setForceNonceCheck(bool forceNonceCheck)
{
	forceNonceCheck_ = forceNonceCheck;
	setParameter("ForceNonceCheck", forceNonceCheck ? "true" : "false");
}

std::string ModifyApiRequest::getRequestConfig()const
{
	return requestConfig_;
}

void ModifyApiRequest::setRequestConfig(const std::string& requestConfig)
{
	requestConfig_ = requestConfig;
	setParameter("RequestConfig", requestConfig);
}

std::string ModifyApiRequest::getResultBodyModel()const
{
	return resultBodyModel_;
}

void ModifyApiRequest::setResultBodyModel(const std::string& resultBodyModel)
{
	resultBodyModel_ = resultBodyModel;
	setParameter("ResultBodyModel", resultBodyModel);
}

std::string ModifyApiRequest::getApiId()const
{
	return apiId_;
}

void ModifyApiRequest::setApiId(const std::string& apiId)
{
	apiId_ = apiId;
	setParameter("ApiId", apiId);
}

