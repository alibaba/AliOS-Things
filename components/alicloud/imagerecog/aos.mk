# component name
NAME := imagerecog
# component information
$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := add aliyun open api include ai vision and so on.
# source files and the folder of internal include files

#image recognize AI detection
$(NAME)_SOURCES +=  ./src/imagerecog.cc \
                    ./src/ImagerecogClient.cc \
                    ./src/model/ClassifyingRubbishRequest.cc \
                    ./src/model/ClassifyingRubbishResult.cc \
                    ./src/model/DetectFruitsRequest.cc \
                    ./src/model/DetectFruitsResult.cc \
                    ./src/model/DetectImageElementsRequest.cc \
                    ./src/model/DetectImageElementsResult.cc \
                    ./src/model/RecognizeImageColorRequest.cc \
                    ./src/model/RecognizeImageColorResult.cc \
                    ./src/model/RecognizeImageStyleRequest.cc \
                    ./src/model/RecognizeImageStyleResult.cc \
                    ./src/model/RecognizeLogoRequest.cc \
                    ./src/model/RecognizeLogoResult.cc \
                    ./src/model/RecognizeSceneRequest.cc \
                    ./src/model/RecognizeSceneResult.cc \
                    ./src/model/RecognizeVehicleTypeRequest.cc \
                    ./src/model/RecognizeVehicleTypeResult.cc \
                    ./src/model/TaggingImageRequest.cc \
                    ./src/model/TaggingImageResult.cc

$(NAME)_INCLUDES += ./include
GLOBAL_INCLUDES += ./include

GLOBAL_DEFINES += UCLOUD_AI_IMAGERECOG_CONFIG
