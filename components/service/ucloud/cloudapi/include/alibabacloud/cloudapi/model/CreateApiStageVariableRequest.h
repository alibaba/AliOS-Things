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

#ifndef ALIBABACLOUD_CLOUDAPI_MODEL_CREATEAPISTAGEVARIABLEREQUEST_H_
#define ALIBABACLOUD_CLOUDAPI_MODEL_CREATEAPISTAGEVARIABLEREQUEST_H_

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
			class ALIBABACLOUD_CLOUDAPI_EXPORT CreateApiStageVariableRequest : public RpcServiceRequest
			{

			public:
				CreateApiStageVariableRequest();
				~CreateApiStageVariableRequest();

				bool getSupportRoute()const;
				void setSupportRoute(bool supportRoute);
				std::string getVariableValue()const;
				void setVariableValue(const std::string& variableValue);
				std::string getGroupId()const;
				void setGroupId(const std::string& groupId);
				std::string getStageRouteModel()const;
				void setStageRouteModel(const std::string& stageRouteModel);
				std::string getAccessKeyId()const;
				void setAccessKeyId(const std::string& accessKeyId);
				std::string getSecurityToken()const;
				void setSecurityToken(const std::string& securityToken);
				std::string getVariableName()const;
				void setVariableName(const std::string& variableName);
				std::string getStageId()const;
				void setStageId(const std::string& stageId);

            private:
				bool supportRoute_;
				std::string variableValue_;
				std::string groupId_;
				std::string stageRouteModel_;
				std::string accessKeyId_;
				std::string securityToken_;
				std::string variableName_;
				std::string stageId_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CLOUDAPI_MODEL_CREATEAPISTAGEVARIABLEREQUEST_H_