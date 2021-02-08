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

#ifndef ALIBABACLOUD_CLOUDAPI_MODEL_SETTRAFFICCONTROLAPISREQUEST_H_
#define ALIBABACLOUD_CLOUDAPI_MODEL_SETTRAFFICCONTROLAPISREQUEST_H_

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
			class ALIBABACLOUD_CLOUDAPI_EXPORT SetTrafficControlApisRequest : public RpcServiceRequest
			{

			public:
				SetTrafficControlApisRequest();
				~SetTrafficControlApisRequest();

				std::string getTrafficControlId()const;
				void setTrafficControlId(const std::string& trafficControlId);
				std::string getStageName()const;
				void setStageName(const std::string& stageName);
				std::string getGroupId()const;
				void setGroupId(const std::string& groupId);
				std::string getAccessKeyId()const;
				void setAccessKeyId(const std::string& accessKeyId);
				std::string getSecurityToken()const;
				void setSecurityToken(const std::string& securityToken);
				std::string getApiIds()const;
				void setApiIds(const std::string& apiIds);

            private:
				std::string trafficControlId_;
				std::string stageName_;
				std::string groupId_;
				std::string accessKeyId_;
				std::string securityToken_;
				std::string apiIds_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CLOUDAPI_MODEL_SETTRAFFICCONTROLAPISREQUEST_H_