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

#ifndef ALIBABACLOUD_CR_MODEL_GETUSERSOURCEREPOREFLISTREQUEST_H_
#define ALIBABACLOUD_CR_MODEL_GETUSERSOURCEREPOREFLISTREQUEST_H_

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
			class ALIBABACLOUD_CR_EXPORT GetUserSourceRepoRefListRequest : public RoaServiceRequest
			{

			public:
				GetUserSourceRepoRefListRequest();
				~GetUserSourceRepoRefListRequest();

				long getSourceAccountId()const;
				void setSourceAccountId(long sourceAccountId);
				std::string getSourceRepoName()const;
				void setSourceRepoName(const std::string& sourceRepoName);
				std::string getSourceRepoNamespace()const;
				void setSourceRepoNamespace(const std::string& sourceRepoNamespace);

            private:
				long sourceAccountId_;
				std::string sourceRepoName_;
				std::string sourceRepoNamespace_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CR_MODEL_GETUSERSOURCEREPOREFLISTREQUEST_H_