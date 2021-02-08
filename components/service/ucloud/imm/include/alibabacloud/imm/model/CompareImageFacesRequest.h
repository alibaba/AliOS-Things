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

#ifndef ALIBABACLOUD_IMM_MODEL_COMPAREIMAGEFACESREQUEST_H_
#define ALIBABACLOUD_IMM_MODEL_COMPAREIMAGEFACESREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/imm/ImmExport.h>

namespace AlibabaCloud
{
	namespace Imm
	{
		namespace Model
		{
			class ALIBABACLOUD_IMM_EXPORT CompareImageFacesRequest : public RpcServiceRequest
			{

			public:
				CompareImageFacesRequest();
				~CompareImageFacesRequest();

				std::string getProject()const;
				void setProject(const std::string& project);
				std::string getFaceIdA()const;
				void setFaceIdA(const std::string& faceIdA);
				std::string getFaceIdB()const;
				void setFaceIdB(const std::string& faceIdB);
				std::string getAccessKeyId()const;
				void setAccessKeyId(const std::string& accessKeyId);
				std::string getImageUriB()const;
				void setImageUriB(const std::string& imageUriB);
				std::string getImageUriA()const;
				void setImageUriA(const std::string& imageUriA);
				std::string getSetId()const;
				void setSetId(const std::string& setId);

            private:
				std::string project_;
				std::string faceIdA_;
				std::string faceIdB_;
				std::string accessKeyId_;
				std::string imageUriB_;
				std::string imageUriA_;
				std::string setId_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMM_MODEL_COMPAREIMAGEFACESREQUEST_H_