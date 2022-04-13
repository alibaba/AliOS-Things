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

#ifndef ALIBABACLOUD_OCR_MODEL_TRIMDOCUMENTREQUEST_H_
#define ALIBABACLOUD_OCR_MODEL_TRIMDOCUMENTREQUEST_H_

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
			class ALIBABACLOUD_OCR_EXPORT TrimDocumentRequest : public RpcServiceRequest
			{

			public:
				TrimDocumentRequest();
				~TrimDocumentRequest();

				std::string getFileType()const;
				void setFileType(const std::string& fileType);
				bool getAsync()const;
				void setAsync(bool async);
				std::string getFileURL()const;
				void setFileURL(const std::string& fileURL);
				std::string getOutputType()const;
				void setOutputType(const std::string& outputType);

            private:
				std::string fileType_;
				bool async_;
				std::string fileURL_;
				std::string outputType_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_OCR_MODEL_TRIMDOCUMENTREQUEST_H_