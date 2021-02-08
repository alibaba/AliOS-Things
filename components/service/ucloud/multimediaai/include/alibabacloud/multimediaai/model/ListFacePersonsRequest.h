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

#ifndef ALIBABACLOUD_MULTIMEDIAAI_MODEL_LISTFACEPERSONSREQUEST_H_
#define ALIBABACLOUD_MULTIMEDIAAI_MODEL_LISTFACEPERSONSREQUEST_H_

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
			class ALIBABACLOUD_MULTIMEDIAAI_EXPORT ListFacePersonsRequest : public RpcServiceRequest
			{

			public:
				ListFacePersonsRequest();
				~ListFacePersonsRequest();

				long getFaceGroupId()const;
				void setFaceGroupId(long faceGroupId);
				std::string getFacePersonName()const;
				void setFacePersonName(const std::string& facePersonName);
				int getPageNumber()const;
				void setPageNumber(int pageNumber);
				int getPageSize()const;
				void setPageSize(int pageSize);

            private:
				long faceGroupId_;
				std::string facePersonName_;
				int pageNumber_;
				int pageSize_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_MULTIMEDIAAI_MODEL_LISTFACEPERSONSREQUEST_H_