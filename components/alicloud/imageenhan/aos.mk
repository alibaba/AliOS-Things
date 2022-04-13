# component name
NAME := imageenhan
# component information
$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := add aliyun open api include ai vision and so on.
# source files and the folder of internal include files

#image recognize AI detection
$(NAME)_SOURCES +=  ./src/imageenhan.cc \
                    ./src/ImageenhanClient.cc \
                    ./src/model/AssessCompositionRequest.cc \
                    ./src/model/AssessCompositionResult.cc \
                    ./src/model/AssessExposureRequest.cc \
                    ./src/model/AssessExposureResult.cc \
                    ./src/model/AssessSharpnessRequest.cc \
                    ./src/model/AssessSharpnessResult.cc \
                    ./src/model/ChangeImageSizeRequest.cc \
                    ./src/model/ChangeImageSizeResult.cc \
                    ./src/model/ColorizeImageRequest.cc \
                    ./src/model/ColorizeImageResult.cc \
                    ./src/model/EnhanceImageColorRequest.cc \
                    ./src/model/EnhanceImageColorResult.cc \
                    ./src/model/ErasePersonRequest.cc \
                    ./src/model/ErasePersonResult.cc \
                    ./src/model/ExtendImageStyleRequest.cc \
                    ./src/model/ExtendImageStyleResult.cc \
                    ./src/model/GenerateDynamicImageRequest.cc \
                    ./src/model/GenerateDynamicImageResult.cc \
                    ./src/model/GetAsyncJobResultRequest.cc \
                    ./src/model/GetAsyncJobResultResult.cc \
                    ./src/model/ImageBlindCharacterWatermarkRequest.cc \
                    ./src/model/ImageBlindCharacterWatermarkResult.cc \
                    ./src/model/ImageBlindPicWatermarkRequest.cc \
                    ./src/model/ImageBlindPicWatermarkResult.cc \
                    ./src/model/ImitatePhotoStyleRequest.cc \
                    ./src/model/ImitatePhotoStyleResult.cc \
                    ./src/model/IntelligentCompositionRequest.cc \
                    ./src/model/IntelligentCompositionResult.cc \
                    ./src/model/MakeSuperResolutionImageRequest.cc \
                    ./src/model/MakeSuperResolutionImageResult.cc \
                    ./src/model/RecolorHDImageRequest.cc \
                    ./src/model/RecolorHDImageResult.cc \
                    ./src/model/RecolorImageRequest.cc \
                    ./src/model/RecolorImageResult.cc \
                    ./src/model/RemoveImageSubtitlesRequest.cc \
                    ./src/model/RemoveImageSubtitlesResult.cc \
                    ./src/model/RemoveImageWatermarkRequest.cc \
                    ./src/model/RemoveImageWatermarkResult.cc

$(NAME)_INCLUDES += ./include
GLOBAL_INCLUDES += ./include

GLOBAL_DEFINES += UCLOUD_AI_IMAGEENHAN_CONFIG
