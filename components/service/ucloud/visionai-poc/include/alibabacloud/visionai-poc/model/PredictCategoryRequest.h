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

#ifndef ALIBABACLOUD_VISIONAI_POC_MODEL_PREDICTCATEGORYREQUEST_H_
#define ALIBABACLOUD_VISIONAI_POC_MODEL_PREDICTCATEGORYREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/visionai-poc/Visionai_pocExport.h>

namespace AlibabaCloud
{
	namespace Visionai_poc
	{
		namespace Model
		{
			class ALIBABACLOUD_VISIONAI_POC_EXPORT PredictCategoryRequest : public RpcServiceRequest
			{

			public:
				PredictCategoryRequest();
				~PredictCategoryRequest();

				std::string getMethod()const;
				void setMethod(const std::string& method);
				std::string getImageUrl()const;
				void setImageUrl(const std::string& imageUrl);
				std::string getTitle()const;
				void setTitle(const std::string& title);
				std::string getUrl()const;
				void setUrl(const std::string& url);

            private:
				std::string method_;
				std::string imageUrl_;
				std::string title_;
				std::string url_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_VISIONAI_POC_MODEL_PREDICTCATEGORYREQUEST_H_