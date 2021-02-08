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

#ifndef ALIBABACLOUD_CLOUDAPI_MODEL_DESCRIBESIGNATURESRESULT_H_
#define ALIBABACLOUD_CLOUDAPI_MODEL_DESCRIBESIGNATURESRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/cloudapi/CloudAPIExport.h>

namespace AlibabaCloud
{
	namespace CloudAPI
	{
		namespace Model
		{
			class ALIBABACLOUD_CLOUDAPI_EXPORT DescribeSignaturesResult : public ServiceResult
			{
			public:
				struct SignatureInfo
				{
					std::string signatureId;
					std::string signatureSecret;
					std::string createdTime;
					std::string modifiedTime;
					std::string signatureName;
					std::string regionId;
					std::string signatureKey;
				};


				DescribeSignaturesResult();
				explicit DescribeSignaturesResult(const std::string &payload);
				~DescribeSignaturesResult();
				std::vector<SignatureInfo> getSignatureInfos()const;
				int getTotalCount()const;
				int getPageSize()const;
				int getPageNumber()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::vector<SignatureInfo> signatureInfos_;
				int totalCount_;
				int pageSize_;
				int pageNumber_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_CLOUDAPI_MODEL_DESCRIBESIGNATURESRESULT_H_