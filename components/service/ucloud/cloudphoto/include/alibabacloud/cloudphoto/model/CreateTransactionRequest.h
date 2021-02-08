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

#ifndef ALIBABACLOUD_CLOUDPHOTO_MODEL_CREATETRANSACTIONREQUEST_H_
#define ALIBABACLOUD_CLOUDPHOTO_MODEL_CREATETRANSACTIONREQUEST_H_

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
			class ALIBABACLOUD_CLOUDPHOTO_EXPORT CreateTransactionRequest : public RpcServiceRequest
			{

			public:
				CreateTransactionRequest();
				~CreateTransactionRequest();

				std::string getExt()const;
				void setExt(const std::string& ext);
				std::string getLibraryId()const;
				void setLibraryId(const std::string& libraryId);
				std::string getStoreName()const;
				void setStoreName(const std::string& storeName);
				long getSize()const;
				void setSize(long size);
				std::string getForce()const;
				void setForce(const std::string& force);
				std::string getMd5()const;
				void setMd5(const std::string& md5);

            private:
				std::string ext_;
				std::string libraryId_;
				std::string storeName_;
				long size_;
				std::string force_;
				std::string md5_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CLOUDPHOTO_MODEL_CREATETRANSACTIONREQUEST_H_