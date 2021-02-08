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

#ifndef ALIBABACLOUD_CLOUDAPI_MODEL_MODIFYAPIREQUEST_H_
#define ALIBABACLOUD_CLOUDAPI_MODEL_MODIFYAPIREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/cloudapi/CloudAPIExport.h>

namespace AlibabaCloud
{
	namespace CloudAPI
	{
		namespace Model
		{
			class ALIBABACLOUD_CLOUDAPI_EXPORT ModifyApiRequest : public RpcServiceRequest
			{

			public:
				ModifyApiRequest();
				~ModifyApiRequest();

				std::string getWebSocketApiType()const;
				void setWebSocketApiType(const std::string& webSocketApiType);
				std::string getErrorCodeSamples()const;
				void setErrorCodeSamples(const std::string& errorCodeSamples);
				std::string getAppCodeAuthType()const;
				void setAppCodeAuthType(const std::string& appCodeAuthType);
				std::string getDescription()const;
				void setDescription(const std::string& description);
				bool getDisableInternet()const;
				void setDisableInternet(bool disableInternet);
				std::string getConstantParameters()const;
				void setConstantParameters(const std::string& constantParameters);
				std::string getAccessKeyId()const;
				void setAccessKeyId(const std::string& accessKeyId);
				std::string getAuthType()const;
				void setAuthType(const std::string& authType);
				std::string getAllowSignatureMethod()const;
				void setAllowSignatureMethod(const std::string& allowSignatureMethod);
				std::string getServiceParameters()const;
				void setServiceParameters(const std::string& serviceParameters);
				std::string getFailResultSample()const;
				void setFailResultSample(const std::string& failResultSample);
				std::string getResourceOwnerToken()const;
				void setResourceOwnerToken(const std::string& resourceOwnerToken);
				std::string getSystemParameters()const;
				void setSystemParameters(const std::string& systemParameters);
				std::string getServiceParametersMap()const;
				void setServiceParametersMap(const std::string& serviceParametersMap);
				std::string getSecurityToken()const;
				void setSecurityToken(const std::string& securityToken);
				std::string getOpenIdConnectConfig()const;
				void setOpenIdConnectConfig(const std::string& openIdConnectConfig);
				std::string getRequestParameters()const;
				void setRequestParameters(const std::string& requestParameters);
				std::string getResultDescriptions()const;
				void setResultDescriptions(const std::string& resultDescriptions);
				std::string getVisibility()const;
				void setVisibility(const std::string& visibility);
				std::string getGroupId()const;
				void setGroupId(const std::string& groupId);
				std::string getServiceConfig()const;
				void setServiceConfig(const std::string& serviceConfig);
				std::string getResultType()const;
				void setResultType(const std::string& resultType);
				std::string getApiName()const;
				void setApiName(const std::string& apiName);
				std::string getResultSample()const;
				void setResultSample(const std::string& resultSample);
				bool getForceNonceCheck()const;
				void setForceNonceCheck(bool forceNonceCheck);
				std::string getRequestConfig()const;
				void setRequestConfig(const std::string& requestConfig);
				std::string getResultBodyModel()const;
				void setResultBodyModel(const std::string& resultBodyModel);
				std::string getApiId()const;
				void setApiId(const std::string& apiId);

            private:
				std::string webSocketApiType_;
				std::string errorCodeSamples_;
				std::string appCodeAuthType_;
				std::string description_;
				bool disableInternet_;
				std::string constantParameters_;
				std::string accessKeyId_;
				std::string authType_;
				std::string allowSignatureMethod_;
				std::string serviceParameters_;
				std::string failResultSample_;
				std::string resourceOwnerToken_;
				std::string systemParameters_;
				std::string serviceParametersMap_;
				std::string securityToken_;
				std::string openIdConnectConfig_;
				std::string requestParameters_;
				std::string resultDescriptions_;
				std::string visibility_;
				std::string groupId_;
				std::string serviceConfig_;
				std::string resultType_;
				std::string apiName_;
				std::string resultSample_;
				bool forceNonceCheck_;
				std::string requestConfig_;
				std::string resultBodyModel_;
				std::string apiId_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CLOUDAPI_MODEL_MODIFYAPIREQUEST_H_