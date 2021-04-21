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

#ifndef ALIBABACLOUD_IMAGERECOG_MODEL_RECOGNIZELOGOREQUEST_H_
#define ALIBABACLOUD_IMAGERECOG_MODEL_RECOGNIZELOGOREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/imagerecog/ImagerecogExport.h>

namespace AlibabaCloud
{
	namespace Imagerecog
	{
		namespace Model
		{
			class ALIBABACLOUD_IMAGERECOG_EXPORT RecognizeLogoRequest : public RpcServiceRequest
			{
			public:
				struct Tasks
				{
					std::string imageURL;
				};

			public:
				RecognizeLogoRequest();
				~RecognizeLogoRequest();

				std::vector<Tasks> getTasks()const;
				void setTasks(const std::vector<Tasks>& tasks);

            private:
				std::vector<Tasks> tasks_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMAGERECOG_MODEL_RECOGNIZELOGOREQUEST_H_