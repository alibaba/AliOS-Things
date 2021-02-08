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

#ifndef ALIBABACLOUD_CR_MODEL_GETSEARCHREQUEST_H_
#define ALIBABACLOUD_CR_MODEL_GETSEARCHREQUEST_H_

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
			class ALIBABACLOUD_CR_EXPORT GetSearchRequest : public RoaServiceRequest
			{

			public:
				GetSearchRequest();
				~GetSearchRequest();

				std::string getOrigin()const;
				void setOrigin(const std::string& origin);
				int getPageSize()const;
				void setPageSize(int pageSize);
				int getPage()const;
				void setPage(int page);
				std::string getKeyword()const;
				void setKeyword(const std::string& keyword);

            private:
				std::string origin_;
				int pageSize_;
				int page_;
				std::string keyword_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CR_MODEL_GETSEARCHREQUEST_H_