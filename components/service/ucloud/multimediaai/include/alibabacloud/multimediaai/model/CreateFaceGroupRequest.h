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

#ifndef ALIBABACLOUD_MULTIMEDIAAI_MODEL_CREATEFACEGROUPREQUEST_H_
#define ALIBABACLOUD_MULTIMEDIAAI_MODEL_CREATEFACEGROUPREQUEST_H_

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
			class ALIBABACLOUD_MULTIMEDIAAI_EXPORT CreateFaceGroupRequest : public RpcServiceRequest
			{

			public:
				CreateFaceGroupRequest();
				~CreateFaceGroupRequest();

				std::string getDescription()const;
				void setDescription(const std::string& description);
				std::string getFaceGroupName()const;
				void setFaceGroupName(const std::string& faceGroupName);

            private:
				std::string description_;
				std::string faceGroupName_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_MULTIMEDIAAI_MODEL_CREATEFACEGROUPREQUEST_H_