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

#ifndef ALIBABACLOUD_CLOUDAPI_MODEL_SETDOMAINCERTIFICATEREQUEST_H_
#define ALIBABACLOUD_CLOUDAPI_MODEL_SETDOMAINCERTIFICATEREQUEST_H_

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
			class ALIBABACLOUD_CLOUDAPI_EXPORT SetDomainCertificateRequest : public RpcServiceRequest
			{

			public:
				SetDomainCertificateRequest();
				~SetDomainCertificateRequest();

				std::string getCertificatePrivateKey()const;
				void setCertificatePrivateKey(const std::string& certificatePrivateKey);
				std::string getGroupId()const;
				void setGroupId(const std::string& groupId);
				std::string getDomainName()const;
				void setDomainName(const std::string& domainName);
				std::string getCertificateBody()const;
				void setCertificateBody(const std::string& certificateBody);
				std::string getAccessKeyId()const;
				void setAccessKeyId(const std::string& accessKeyId);
				std::string getCaCertificateBody()const;
				void setCaCertificateBody(const std::string& caCertificateBody);
				std::string getSecurityToken()const;
				void setSecurityToken(const std::string& securityToken);
				std::string getCertificateName()const;
				void setCertificateName(const std::string& certificateName);

            private:
				std::string certificatePrivateKey_;
				std::string groupId_;
				std::string domainName_;
				std::string certificateBody_;
				std::string accessKeyId_;
				std::string caCertificateBody_;
				std::string securityToken_;
				std::string certificateName_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CLOUDAPI_MODEL_SETDOMAINCERTIFICATEREQUEST_H_