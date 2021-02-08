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

#ifndef ALIBABACLOUD_CLOUDPHOTO_MODEL_FETCHMOMENTPHOTOSREQUEST_H_
#define ALIBABACLOUD_CLOUDPHOTO_MODEL_FETCHMOMENTPHOTOSREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/cloudphoto/CloudPhotoExport.h>

namespace AlibabaCloud
{
	namespace CloudPhoto
	{
		namespace Model
		{
			class ALIBABACLOUD_CLOUDPHOTO_EXPORT FetchMomentPhotosRequest : public RpcServiceRequest
			{

			public:
				FetchMomentPhotosRequest();
				~FetchMomentPhotosRequest();

				std::string getLibraryId()const;
				void setLibraryId(const std::string& libraryId);
				std::string getOrderBy()const;
				void setOrderBy(const std::string& orderBy);
				std::string getStoreName()const;
				void setStoreName(const std::string& storeName);
				long getMomentId()const;
				void setMomentId(long momentId);
				int getSize()const;
				void setSize(int size);
				int getPage()const;
				void setPage(int page);
				std::string getOrder()const;
				void setOrder(const std::string& order);

            private:
				std::string libraryId_;
				std::string orderBy_;
				std::string storeName_;
				long momentId_;
				int size_;
				int page_;
				std::string order_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CLOUDPHOTO_MODEL_FETCHMOMENTPHOTOSREQUEST_H_