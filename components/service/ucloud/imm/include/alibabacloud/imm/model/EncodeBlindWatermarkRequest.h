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

#ifndef ALIBABACLOUD_IMM_MODEL_ENCODEBLINDWATERMARKREQUEST_H_
#define ALIBABACLOUD_IMM_MODEL_ENCODEBLINDWATERMARKREQUEST_H_

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
			class ALIBABACLOUD_IMM_EXPORT EncodeBlindWatermarkRequest : public RpcServiceRequest
			{

			public:
				EncodeBlindWatermarkRequest();
				~EncodeBlindWatermarkRequest();

				std::string getImageQuality()const;
				void setImageQuality(const std::string& imageQuality);
				std::string getWatermarkUri()const;
				void setWatermarkUri(const std::string& watermarkUri);
				std::string getProject()const;
				void setProject(const std::string& project);
				std::string getContent()const;
				void setContent(const std::string& content);
				std::string getAccessKeyId()const;
				void setAccessKeyId(const std::string& accessKeyId);
				std::string getWatermarkType()const;
				void setWatermarkType(const std::string& watermarkType);
				std::string getTargetUri()const;
				void setTargetUri(const std::string& targetUri);
				std::string getModel()const;
				void setModel(const std::string& model);
				std::string getTargetImageType()const;
				void setTargetImageType(const std::string& targetImageType);
				std::string getImageUri()const;
				void setImageUri(const std::string& imageUri);

            private:
				std::string imageQuality_;
				std::string watermarkUri_;
				std::string project_;
				std::string content_;
				std::string accessKeyId_;
				std::string watermarkType_;
				std::string targetUri_;
				std::string model_;
				std::string targetImageType_;
				std::string imageUri_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMM_MODEL_ENCODEBLINDWATERMARKREQUEST_H_