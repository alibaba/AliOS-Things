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

#ifndef ALIBABACLOUD_IMM_MODEL_CREATECADCONVERSIONTASKREQUEST_H_
#define ALIBABACLOUD_IMM_MODEL_CREATECADCONVERSIONTASKREQUEST_H_

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
			class ALIBABACLOUD_IMM_EXPORT CreateCADConversionTaskRequest : public RpcServiceRequest
			{

			public:
				CreateCADConversionTaskRequest();
				~CreateCADConversionTaskRequest();

				std::string getSrcType()const;
				void setSrcType(const std::string& srcType);
				int getBaseRow()const;
				void setBaseRow(int baseRow);
				std::string getProject()const;
				void setProject(const std::string& project);
				int getZoomFactor()const;
				void setZoomFactor(int zoomFactor);
				std::string getAccessKeyId()const;
				void setAccessKeyId(const std::string& accessKeyId);
				std::string getNotifyEndpoint()const;
				void setNotifyEndpoint(const std::string& notifyEndpoint);
				int getBaseCol()const;
				void setBaseCol(int baseCol);
				std::string getNotifyTopicName()const;
				void setNotifyTopicName(const std::string& notifyTopicName);
				long getUnitWidth()const;
				void setUnitWidth(long unitWidth);
				int getZoomLevel()const;
				void setZoomLevel(int zoomLevel);
				std::string getModelId()const;
				void setModelId(const std::string& modelId);
				std::string getTgtType()const;
				void setTgtType(const std::string& tgtType);
				long getUnitHeight()const;
				void setUnitHeight(long unitHeight);
				std::string getSrcUri()const;
				void setSrcUri(const std::string& srcUri);
				bool getThumbnails()const;
				void setThumbnails(bool thumbnails);
				std::string getTgtUri()const;
				void setTgtUri(const std::string& tgtUri);

            private:
				std::string srcType_;
				int baseRow_;
				std::string project_;
				int zoomFactor_;
				std::string accessKeyId_;
				std::string notifyEndpoint_;
				int baseCol_;
				std::string notifyTopicName_;
				long unitWidth_;
				int zoomLevel_;
				std::string modelId_;
				std::string tgtType_;
				long unitHeight_;
				std::string srcUri_;
				bool thumbnails_;
				std::string tgtUri_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_IMM_MODEL_CREATECADCONVERSIONTASKREQUEST_H_