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

#ifndef ALIBABACLOUD_CLOUDAPI_MODEL_DESCRIBEAPIRESULT_H_
#define ALIBABACLOUD_CLOUDAPI_MODEL_DESCRIBEAPIRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/cloudapi/CloudAPIExport.h>

namespace AlibabaCloud
{
	namespace CloudAPI
	{
		namespace Model
		{
			class ALIBABACLOUD_CLOUDAPI_EXPORT DescribeApiResult : public ServiceResult
			{
			public:
				struct RequestConfig
				{
					std::string requestPath;
					std::string requestMode;
					std::string bodyModel;
					std::string requestProtocol;
					std::string requestHttpMethod;
					std::string postBodyDescription;
					std::string bodyFormat;
				};
				struct ServiceConfig
				{
					struct VpcConfig
					{
						std::string vpcId;
						std::string instanceId;
						int port;
						std::string name;
					};
					struct FunctionComputeConfig
					{
						std::string functionName;
						std::string serviceName;
						std::string regionId;
						std::string roleArn;
					};
					struct MockHeader
					{
						std::string headerValue;
						std::string headerName;
					};
					std::string serviceAddress;
					FunctionComputeConfig functionComputeConfig;
					std::string contentTypeValue;
					std::string mockResult;
					VpcConfig vpcConfig;
					std::string serviceVpcEnable;
					int mockStatusCode;
					std::vector<MockHeader> mockHeaders;
					std::string serviceHttpMethod;
					std::string servicePath;
					std::string aoneAppName;
					std::string mock;
					int serviceTimeout;
					std::string serviceProtocol;
					std::string contentTypeCatagory;
				};
				struct OpenIdConnectConfig
				{
					std::string openIdApiType;
					std::string publicKey;
					std::string idTokenParamName;
					std::string publicKeyId;
				};
				struct ErrorCodeSample
				{
					std::string description;
					std::string message;
					std::string model;
					std::string code;
				};
				struct ResultDescription
				{
					std::string type;
					std::string description;
					std::string pid;
					std::string id;
					bool hasChild;
					std::string key;
					std::string name;
					bool mandatory;
				};
				struct SystemParameter
				{
					std::string demoValue;
					std::string description;
					std::string parameterName;
					std::string serviceParameterName;
					std::string location;
				};
				struct CustomSystemParameter
				{
					std::string demoValue;
					std::string description;
					std::string parameterName;
					std::string serviceParameterName;
					std::string location;
				};
				struct ConstantParameter
				{
					std::string description;
					std::string constantValue;
					std::string serviceParameterName;
					std::string location;
				};
				struct RequestParameter
				{
					std::string arrayItemsType;
					std::string regularExpression;
					std::string parameterType;
					std::string jsonScheme;
					std::string description;
					std::string apiParameterName;
					std::string enumValue;
					long minLength;
					long maxValue;
					long maxLength;
					std::string demoValue;
					std::string defaultValue;
					std::string required;
					int docOrder;
					long minValue;
					std::string docShow;
					std::string location;
				};
				struct ServiceParameter
				{
					std::string parameterType;
					std::string serviceParameterName;
					std::string location;
				};
				struct ServiceParameterMap
				{
					std::string requestParameterName;
					std::string serviceParameterName;
				};
				struct DeployedInfo
				{
					std::string stageName;
					std::string effectiveVersion;
					std::string deployedStatus;
				};


				DescribeApiResult();
				explicit DescribeApiResult(const std::string &payload);
				~DescribeApiResult();
				std::string getGroupName()const;
				std::string getCreatedTime()const;
				std::string getDescription()const;
				std::vector<DeployedInfo> getDeployedInfos()const;
				bool getForceNonceCheck()const;
				std::string getAllowSignatureMethod()const;
				std::vector<ErrorCodeSample> getErrorCodeSamples()const;
				std::string getMock()const;
				std::vector<RequestParameter> getRequestParameters()const;
				std::vector<ServiceParameterMap> getServiceParametersMap()const;
				std::string getAppCodeAuthType()const;
				std::string getResultBodyModel()const;
				std::string getModifiedTime()const;
				ServiceConfig getServiceConfig()const;
				std::vector<ConstantParameter> getConstantParameters()const;
				std::string getWebSocketApiType()const;
				std::vector<ResultDescription> getResultDescriptions()const;
				OpenIdConnectConfig getOpenIdConnectConfig()const;
				std::string getAuthType()const;
				RequestConfig getRequestConfig()const;
				std::string getResultSample()const;
				std::string getMockResult()const;
				bool getDisableInternet()const;
				std::string getApiName()const;
				std::string getResultType()const;
				std::string getFailResultSample()const;
				std::vector<CustomSystemParameter> getCustomSystemParameters()const;
				std::string getGroupId()const;
				std::vector<SystemParameter> getSystemParameters()const;
				std::string getVisibility()const;
				std::vector<ServiceParameter> getServiceParameters()const;
				std::string getRegionId()const;
				std::string getApiId()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::string groupName_;
				std::string createdTime_;
				std::string description_;
				std::vector<DeployedInfo> deployedInfos_;
				bool forceNonceCheck_;
				std::string allowSignatureMethod_;
				std::vector<ErrorCodeSample> errorCodeSamples_;
				std::string mock_;
				std::vector<RequestParameter> requestParameters_;
				std::vector<ServiceParameterMap> serviceParametersMap_;
				std::string appCodeAuthType_;
				std::string resultBodyModel_;
				std::string modifiedTime_;
				ServiceConfig serviceConfig_;
				std::vector<ConstantParameter> constantParameters_;
				std::string webSocketApiType_;
				std::vector<ResultDescription> resultDescriptions_;
				OpenIdConnectConfig openIdConnectConfig_;
				std::string authType_;
				RequestConfig requestConfig_;
				std::string resultSample_;
				std::string mockResult_;
				bool disableInternet_;
				std::string apiName_;
				std::string resultType_;
				std::string failResultSample_;
				std::vector<CustomSystemParameter> customSystemParameters_;
				std::string groupId_;
				std::vector<SystemParameter> systemParameters_;
				std::string visibility_;
				std::vector<ServiceParameter> serviceParameters_;
				std::string regionId_;
				std::string apiId_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CLOUDAPI_MODEL_DESCRIBEAPIRESULT_H_