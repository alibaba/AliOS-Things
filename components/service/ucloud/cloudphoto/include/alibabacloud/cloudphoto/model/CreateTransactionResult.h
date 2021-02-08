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

#ifndef ALIBABACLOUD_CLOUDPHOTO_MODEL_CREATETRANSACTIONRESULT_H_
#define ALIBABACLOUD_CLOUDPHOTO_MODEL_CREATETRANSACTIONRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/cloudphoto/CloudPhotoExport.h>

namespace AlibabaCloud
{
	namespace CloudPhoto
	{
		namespace Model
		{
			class ALIBABACLOUD_CLOUDPHOTO_EXPORT CreateTransactionResult : public ServiceResult
			{
			public:
				struct Transaction
				{
					struct Upload
					{
						std::string stsToken;
						std::string ossEndpoint;
						std::string bucket;
						std::string accessKeyId;
						std::string accessKeySecret;
						std::string objectKey;
						std::string fileId;
						std::string sessionId;
					};
					Upload upload;
				};


				CreateTransactionResult();
				explicit CreateTransactionResult(const std::string &payload);
				~CreateTransactionResult();
				std::string getAction()const;
				Transaction getTransaction()const;
				std::string getMessage()const;
				std::string getCode()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::string action_;
				Transaction transaction_;
				std::string message_;
				std::string code_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CLOUDPHOTO_MODEL_CREATETRANSACTIONRESULT_H_