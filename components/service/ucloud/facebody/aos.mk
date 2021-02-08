# component name
NAME := facebody
# component information
$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := add aliyun open api include ai vision and so on.
# source files and the folder of internal include files

#face and body AI detection
$(NAME)_SOURCES += ./src/facebody.cc \
                ./src/FacebodyClient.cc \
                ./src/model/AddFaceRequest.cc \
                ./src/model/AddFaceResult.cc \
                ./src/model/AddFaceEntityRequest.cc \
                ./src/model/AddFaceEntityResult.cc \
                ./src/model/BlurFaceRequest.cc \
                ./src/model/BlurFaceResult.cc \
                ./src/model/BodyPostureRequest.cc \
                ./src/model/BodyPostureResult.cc \
                ./src/model/CompareFaceRequest.cc \
                ./src/model/CompareFaceResult.cc \
                ./src/model/CountCrowdRequest.cc \
                ./src/model/CountCrowdResult.cc \
                ./src/model/CreateFaceDbRequest.cc \
                ./src/model/CreateFaceDbResult.cc \
                ./src/model/DeleteFaceRequest.cc \
                ./src/model/DeleteFaceResult.cc \
                ./src/model/DeleteFaceDbRequest.cc \
                ./src/model/DeleteFaceDbResult.cc \
                ./src/model/DeleteFaceEntityRequest.cc \
                ./src/model/DeleteFaceEntityResult.cc \
                ./src/model/DetectBodyCountRequest.cc \
                ./src/model/DetectBodyCountResult.cc \
                ./src/model/DetectCelebrityRequest.cc \
                ./src/model/DetectCelebrityResult.cc \
                ./src/model/DetectChefCapRequest.cc \
                ./src/model/DetectChefCapResult.cc \
                ./src/model/DetectFaceRequest.cc \
                ./src/model/DetectFaceResult.cc \
                ./src/model/DetectIPCPedestrianRequest.cc \
                ./src/model/DetectIPCPedestrianResult.cc \
                ./src/model/DetectLivingFaceRequest.cc \
                ./src/model/DetectLivingFaceResult.cc \
                ./src/model/DetectMaskRequest.cc \
                ./src/model/DetectMaskResult.cc \
                ./src/model/DetectPedestrianRequest.cc \
                ./src/model/DetectPedestrianResult.cc \
                ./src/model/DetectVideoLivingFaceRequest.cc \
                ./src/model/DetectVideoLivingFaceResult.cc \
                ./src/model/EnhanceFaceRequest.cc \
                ./src/model/EnhanceFaceResult.cc \
                ./src/model/ExtractPedestrianFeatureAttrRequest.cc \
                ./src/model/ExtractPedestrianFeatureAttrResult.cc \
                ./src/model/ExtractPedestrianFeatureAttributeRequest.cc \
                ./src/model/ExtractPedestrianFeatureAttributeResult.cc \
                ./src/model/FaceBeautyRequest.cc \
                ./src/model/FaceBeautyResult.cc \
                ./src/model/FaceFilterRequest.cc \
                ./src/model/FaceFilterResult.cc \
                ./src/model/FaceMakeupRequest.cc \
                ./src/model/FaceMakeupResult.cc \
                ./src/model/FaceTidyupRequest.cc \
                ./src/model/FaceTidyupResult.cc \
                ./src/model/GenerateHumanAnimeStyleRequest.cc \
                ./src/model/GenerateHumanAnimeStyleResult.cc \
                ./src/model/GetFaceEntityRequest.cc \
                ./src/model/GetFaceEntityResult.cc \
                ./src/model/HandPostureRequest.cc \
                ./src/model/HandPostureResult.cc \
                ./src/model/ListFaceDbsRequest.cc \
                ./src/model/ListFaceDbsResult.cc \
                ./src/model/ListFaceEntitiesRequest.cc \
                ./src/model/ListFaceEntitiesResult.cc \
                ./src/model/PedestrianDetectAttributeRequest.cc \
                ./src/model/PedestrianDetectAttributeResult.cc \
                ./src/model/RecognizeActionRequest.cc \
                ./src/model/RecognizeActionResult.cc \
                ./src/model/RecognizeExpressionRequest.cc \
                ./src/model/RecognizeExpressionResult.cc \
                ./src/model/RecognizeFaceRequest.cc \
                ./src/model/RecognizeFaceResult.cc \
                ./src/model/RecognizePublicFaceRequest.cc \
                ./src/model/RecognizePublicFaceResult.cc \
                ./src/model/SearchFaceRequest.cc \
                ./src/model/SearchFaceResult.cc \
                ./src/model/SwapFacialFeaturesRequest.cc \
                ./src/model/SwapFacialFeaturesResult.cc \
                ./src/model/UpdateFaceEntityRequest.cc \
                ./src/model/UpdateFaceEntityResult.cc \
                ./src/model/VerifyFaceMaskRequest.cc \
                ./src/model/VerifyFaceMaskResult.cc
$(NAME)_INCLUDES += ./include
GLOBAL_INCLUDES += ./include

GLOBAL_DEFINES += UCLOUD_AI_FACEBODY_CONFIG
GLOBAL_DEFINES += USE_CRYPTO_MBEDTLS USE_AOS_TIME_POSIX_API
