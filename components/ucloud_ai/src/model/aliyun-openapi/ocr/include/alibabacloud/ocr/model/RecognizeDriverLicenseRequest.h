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

#ifndef ALIBABACLOUD_OCR_MODEL_RECOGNIZEDRIVERLICENSEREQUEST_H_
#define ALIBABACLOUD_OCR_MODEL_RECOGNIZEDRIVERLICENSEREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/ocr/OcrExport.h>

namespace AlibabaCloud
{
	namespace Ocr
	{
		namespace Model
		{
			class ALIBABACLOUD_OCR_EXPORT RecognizeDriverLicenseRequest : public RpcServiceRequest
			{

			public:
				RecognizeDriverLicenseRequest();
				~RecognizeDriverLicenseRequest();

				int getImageType()const;
				void setImageType(int imageType);
				std::string getSide()const;
				void setSide(const std::string& side);
				std::string getImageURL()const;
				void setImageURL(const std::string& imageURL);

            private:
				int imageType_;
				std::string side_;
				std::string imageURL_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_OCR_MODEL_RECOGNIZEDRIVERLICENSEREQUEST_H_