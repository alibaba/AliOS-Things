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

#ifndef ALIBABACLOUD_CR_MODEL_DELETEREPOAUTHORIZATIONREQUEST_H_
#define ALIBABACLOUD_CR_MODEL_DELETEREPOAUTHORIZATIONREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RoaServiceRequest.h>
#include <alibabacloud/cr/CrExport.h>

namespace AlibabaCloud
{
	namespace Cr
	{
		namespace Model
		{
			class ALIBABACLOUD_CR_EXPORT DeleteRepoAuthorizationRequest : public RoaServiceRequest
			{

			public:
				DeleteRepoAuthorizationRequest();
				~DeleteRepoAuthorizationRequest();

				std::string getRepoNamespace()const;
				void setRepoNamespace(const std::string& repoNamespace);
				std::string getRepoName()const;
				void setRepoName(const std::string& repoName);
				long getAuthorizeId()const;
				void setAuthorizeId(long authorizeId);

            private:
				std::string repoNamespace_;
				std::string repoName_;
				long authorizeId_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CR_MODEL_DELETEREPOAUTHORIZATIONREQUEST_H_