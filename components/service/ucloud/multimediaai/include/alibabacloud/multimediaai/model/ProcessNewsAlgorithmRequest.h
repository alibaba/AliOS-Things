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

#ifndef ALIBABACLOUD_MULTIMEDIAAI_MODEL_PROCESSNEWSALGORITHMREQUEST_H_
#define ALIBABACLOUD_MULTIMEDIAAI_MODEL_PROCESSNEWSALGORITHMREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/multimediaai/MultimediaaiExport.h>

namespace AlibabaCloud
{
	namespace Multimediaai
	{
		namespace Model
		{
			class ALIBABACLOUD_MULTIMEDIAAI_EXPORT ProcessNewsAlgorithmRequest : public RpcServiceRequest
			{

			public:
				ProcessNewsAlgorithmRequest();
				~ProcessNewsAlgorithmRequest();

				std::string getData()const;
				void setData(const std::string& data);
				std::string getAppKey()const;
				void setAppKey(const std::string& appKey);

            private:
				std::string data_;
				std::string appKey_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_MULTIMEDIAAI_MODEL_PROCESSNEWSALGORITHMREQUEST_H_