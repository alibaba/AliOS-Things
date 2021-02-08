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

#ifndef ALIBABACLOUD_IMM_MODEL_GETOFFICEPREVIEWURLREQUEST_H_
#define ALIBABACLOUD_IMM_MODEL_GETOFFICEPREVIEWURLREQUEST_H_

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
			class ALIBABACLOUD_IMM_EXPORT GetOfficePreviewURLRequest : public RpcServiceRequest
			{

			public:
				GetOfficePreviewURLRequest();
				~GetOfficePreviewURLRequest();

				std::string getSrcType()const;
				void setSrcType(const std::string& srcType);
				std::string getProject()const;
				void setProject(const std::string& project);
				int getWatermarkVertical()const;
				void setWatermarkVertical(int watermarkVertical);
				std::string getAccessKeyId()const;
				void setAccessKeyId(const std::string& accessKeyId);
				int getWatermarkType()const;
				void setWatermarkType(int watermarkType);
				float getWatermarkRotate()const;
				void setWatermarkRotate(float watermarkRotate);
				std::string getWatermarkValue()const;
				void setWatermarkValue(const std::string& watermarkValue);
				std::string getWatermarkFont()const;
				void setWatermarkFont(const std::string& watermarkFont);
				int getWatermarkHorizontal()const;
				void setWatermarkHorizontal(int watermarkHorizontal);
				std::string getSrcUri()const;
				void setSrcUri(const std::string& srcUri);
				std::string getWatermarkFillStyle()const;
				void setWatermarkFillStyle(const std::string& watermarkFillStyle);

            private:
				std::string srcType_;
				std::string project_;
				int watermarkVertical_;
				std::string accessKeyId_;
				int watermarkType_;
				float watermarkRotate_;
				std::string watermarkValue_;
				std::string watermarkFont_;
				int watermarkHorizontal_;
				std::string srcUri_;
				std::string watermarkFillStyle_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMM_MODEL_GETOFFICEPREVIEWURLREQUEST_H_