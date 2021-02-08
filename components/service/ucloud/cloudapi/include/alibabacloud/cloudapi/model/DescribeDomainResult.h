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

#ifndef ALIBABACLOUD_CLOUDAPI_MODEL_DESCRIBEDOMAINRESULT_H_
#define ALIBABACLOUD_CLOUDAPI_MODEL_DESCRIBEDOMAINRESULT_H_

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
			class ALIBABACLOUD_CLOUDAPI_EXPORT DescribeDomainResult : public ServiceResult
			{
			public:


				DescribeDomainResult();
				explicit DescribeDomainResult(const std::string &payload);
				~DescribeDomainResult();
				std::string getSubDomain()const;
				std::string getCertificateBody()const;
				std::string getDomainCNAMEStatus()const;
				std::string getDomainRemark()const;
				std::string getDomainBindingStatus()const;
				std::string getDomainLegalStatus()const;
				std::string getDomainName()const;
				std::string getCertificateId()const;
				std::string getDomainWebSocketStatus()const;
				std::string getCertificatePrivateKey()const;
				std::string getCertificateName()const;
				std::string getGroupId()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::string subDomain_;
				std::string certificateBody_;
				std::string domainCNAMEStatus_;
				std::string domainRemark_;
				std::string domainBindingStatus_;
				std::string domainLegalStatus_;
				std::string domainName_;
				std::string certificateId_;
				std::string domainWebSocketStatus_;
				std::string certificatePrivateKey_;
				std::string certificateName_;
				std::string groupId_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CLOUDAPI_MODEL_DESCRIBEDOMAINRESULT_H_